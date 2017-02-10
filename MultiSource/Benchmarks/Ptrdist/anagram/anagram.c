/*
 *  Anagram program by Raymond Chen,
 *  inspired by a similar program by Brian Scearce
 *
 *  This program is Copyright 1991 by Raymond Chen.
 *              (rjc@math.princeton.edu)
 *
 *  This program may be freely distributed provided all alterations
 *  to the original are clearly indicated as such.
 */

/* There are two tricks.  First is the Basic Idea:
 *
 * When the user types in a phrase, the phrase is first preprocessed to
 * determine how many of each letter appears.  A bit field is then constructed
 * dynamically, such that each field is large enough to hold the next power
 * of two larger than the number of times the character appears.  For example,
 * if the phrase is hello, world, the bit field would be
 *
 *   00 00 00 000 000 00 00
 *    d e   h   l   o  r  w
 *
 * The phrase hello, world, itself would be encoded as
 *
 *   01 01 01 011 010 01 01
 *    d e   h   l   o  r  w
 *
 * and the word hollow would be encoded as
 *
 *   00 00 01 010 010 00 01
 *    d  e  h   l   o  r  w
 *
 * The top bit of each field is set in a special value called the sign.
 * Here, the sign would be
 *
 *   10 10 10 100 100 10 10
 *    d  e  h   l   o  r  w
 *
 * The reason for packing the values into a bit field is that the operation
 * of subtracting out the letters of a word from the current phrase can be
 * carried out in parallel.  for example, subtracting the word hello from
 * the phrase hello, world, is merely
 *
 *    d e   h   l   o  r  w
 *   01 01 01 011 010 01 01 (dehllloorw)
 * - 00 00 01 010 010 00 01 (hlloow)
 * ========================
 *   01 01 00 001 000 01 00 (delr)
 *
 * Since none of the sign bits is set, the word fits, and we can continue.
 * Suppose the next word we tried was hood.
 *
 *    d e   h   l   o  r  w
 *   01 01 00 001 000 01 00 (delr)
 * - 01 00 01 000 010 00 00 (hood)
 * ========================
 *   00 00 11 000 110 01 00
 *         ^      ^
 * A sign bit is set.  (Two, actually.)  This means that hood does not
 * fit in delr, so we skip it and try another word.  (Observe that
 * when a sign bit becomes set, it screws up the values for the letters to
 * the left of that bit, but that's not important.)
 *
 * The inner loop of an anagram program is testing to see if a
 * word fits in the collection of untried letters.  Traditional methods
 * keep an array of 26 integers, which are then compared in turn.  This
 * means that there are 26 comparisons per word.
 *
 * This method reduces the number of comparisons to MAX_QUAD, typically 2.
 * Instead of looping through an array, we merely perform the indicated
 * subtraction and test if any of the sign bits is set.
 */

/* The nuts and bolts:
 *
 * The dictionary is loaded and preprocessed.  The preprocessed dictionary
 * is a concatenation of copies of the structure:
 *
 * struct dictword {
 *     char bStructureSize;             -- size of this structure
 *     char cLetters;                   -- number of letters in the word
 *     char achWord[];                  -- the word itself (0-terminated)
 * }
 *
 * Since this is a variable-sized structure, we keep its size in the structure
 * itself for rapid stepping through the table.
 *
 * When a phrase is typed in, it is first preprocessed as described in the
 * Basic Idea.  We then go through the dictionary, testing each word.  If
 * the word fits in our phrase, we build the bit field for its frequency
 * table and add it to the list of candidates.
 */

/*
 * The Second Trick:
 *
 * Before diving into our anagram search, we then tabulate how many times
 * each letter appears in our list of candidates, and sort the table, with
 * the rarest letter first.
 *
 * We then do our anagram search.
 *
 * Like most anagram programs, this program does a depth-first search.
 * Although most anagram programs do some sort of heuristics to decide what
 * order to place words in the list_of_candidates, the search itself proceeds
 * according to a greedy algorithm.  That is, once you find a word that fits,
 * subtract it and recurse.
 *
 * This anagram program exercises some restraint and does not march down
 * every branch that shows itself.  Instead, it only goes down branches
 * that use the rarest unused letter.  This helps to find dead ends faster.
 *
 * FindAnagram(unused_letters, list_of_candidates) {
 *  l = the rarest letter as yet unused
 *  For word in list_of_candidates {
 *     if word does not fit in unused_letters, go on to the next word.
 *     if word does not contain l, defer.
 *      FindAnagram(unused_letters - word, list_of_candidates[word,...])
 *  }
 * }
 *
 *
 * The heuristic of the Second Trick can probably be improved.  I invite
 * anyone willing to improve it to do so.
 */

/* Use the accompanying unproto perl script to remove the ANSI-style
 * prototypes, for those of you who have K&R compilers.
 */

// CHECKEDC : To use bounds-safe interface, use header files in checkedc repo
// checked header files are copied from checkedc repo to checkedc-llvm-test-suite/include path
// To include checkedc header file, make include directory within checkedc-llvm-test-suite
#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <unistd.h>

/* Before compiling, make sure Quad and MASK_BITS are set properly.  For best
 * results, make Quad the largest integer size supported on your machine.
 * So if your machine has long longs, make Quad an unsigned long long.
 * (I called it Quad because on most machines, the largest integer size
 * supported is a four-byte unsigned long.)
 *
 * If you need to be able to anagram larger phrases, increase MAX_QUADS.
 * If you increase it beyond 4, you'll have to add a few more loop unrolling
 * steps to FindAnagram.
 */

typedef unsigned long Quad;             /* for building our bit mask */
#define MASK_BITS (sizeof(Quad)*8)                    /* number of bits in a Quad */

#define MAX_QUADS 2                     /* controls largest phrase */

#define MAXWORDS 26000                  /* dictionary length */
#define MAXCAND  5000                   /* candidates */
#define MAXSOL   51                     /* words in the solution */

#define ALPHABET 26                     /* letters in the alphabet */
#define ch2i(ch) ((ch)-'a')             /* convert letter to index */
#define i2ch(ch) ((ch)+'a')             /* convert index to letter */

/* IBM PC's don't like globs of memory larger than 64K without
 * special gyrations.  That's where the huges get stuck in.  And the
 * two types of allocations on an IBM PC need to be handled differently.
 *
 * HaltProcessing is called during the anagram search.	If it returns nonzero,
 * then the search is aborted.
 *
 * Cdecl is a macro expanded before the name of all functions that must
 * use C-style parameter passing.  This lets you change the default parameter
 * passing style for the other functions.
 */

/* char *malloc(); */
#   define huge
#   define far
#   define StringFormat    "%15s%c"
#   define bigmalloc       malloc
#   define smallmalloc     malloc
#   define smallmallocfail (char *)0
#   define HaltProcessing() 0           /* no easy way to interrupt on UNIX */
#   define Cdecl

/* Code to be used only when debugging lives inside Debug().
 * Code to be used only when collecting statistics lives inside Stat().
 */
#ifdef DEBUG
#define Debug(x) x
#else
#define Debug(x)
#endif

#ifdef STAT
#define Stat(x) x
#else
#define Stat(x)
#endif

/* A Word remembers the information about a candidate word. */
typedef struct {
    // CHECKEDC : checkedc array type
    Quad aqMask _Checked [MAX_QUADS];  /* the word's mask */
    // CHECKEDC : structure member variable, checkedc array pointer
    // array pointer length field is within structure
    _Array_ptr<char> pchWord : count(cchLength);                 /* the word itself */
    unsigned cchLength;                 /* letters in the word */
} Word;
typedef _Ptr<Word> PWord;
typedef _Array_ptr<_Ptr<Word>> PPWord;

PWord apwCand _Checked [MAXCAND];    /* candidates we've found so far */
unsigned cpwCand;                       /* how many of them? */


/* A Letter remembers information about each letter in the phrase to be
 * anagrammed.
 */

typedef struct {
    unsigned uFrequency;                /* how many times it appears */
    unsigned uShift;                    /* how to mask */
    unsigned uBits;                     /* the bit mask itself */
    unsigned iq;                        /* which Quad to inspect? */
} Letter;
typedef _Ptr<Letter> PLetter;

// CHECKEDC : checkedc array type
// checkedc array type of T == array pointer type of T
// bounds information for checkedc array is from array size
Letter alPhrase _Checked [ALPHABET]; /* statistics on the current phrase */
#define lPhrase(ch) alPhrase[ch2i(ch)]  /* quick access to a letter */

int cchPhraseLength;                    /* number of letters in phrase */

Quad aqMainMask _Checked [MAX_QUADS];/* the bit field for the full phrase */
Quad aqMainSign _Checked [MAX_QUADS];/* where the sign bits are */

char achPhrase _Checked [255];

int cchMinLength = 3;

/* auGlobalFrequency counts the number of times each letter appears, summed
 * over all candidate words.  This is used to decide which letter to attack
 * first.
 */
unsigned auGlobalFrequency _Checked [ALPHABET];
char achByFrequency _Checked [ALPHABET];          /* for sorting */

// CHECKEDC : externally scoped variable bounds information
// global variable bounds information is defined in declaration
// FIXME
// - add additional global variable to store size information of pointer variable
// - represent bounds information with added size variable
unsigned long pchDictionarySize;
_Array_ptr<char> pchDictionary : count(pchDictionarySize);               /* the dictionary is read here */

#define Zero(t) memset(t, 0, sizeof(t)) /* quickly zero out an integer array */

/* Fatal -- print a message before expiring */
void Fatal(char *pchMsg, unsigned u) {
    fprintf(stderr, pchMsg, u);
    exit(1);
}

/* ReadDict -- read the dictionary file into memory and preprocess it
 *
 * A word of length cch in the dictionary is encoded as follows:
 *
 *    byte 0    = cch + 3
 *    byte 1    = number of letters in the word
 *    byte 2... = the word itself, null-terminated
 *
 * Observe that cch+3 is the length of the total encoding.  These
 * byte streams are concatenated, and terminated with a 0.
 */

void ReadDict(_Ptr<char> pchFile) {
    _Ptr<FILE> fp = 0;
    // CHECKEDC : automatic checkedc pointer value initializer
    unsigned cWords = 0;
    unsigned cLetters;
    int ch;
    struct stat statBuf;

    // CHECKEDC : bounds-safe interface is required but not implemented
    if (stat((const char*)pchFile, &statBuf)) Fatal("Cannot stat dictionary\n", 0);

    // CHECKEDC : 
    // pch & pchBase are both incremented.
    // Because of that, they should not have count bounds
    unsigned long ulLen;
    pchDictionarySize = ulLen = statBuf.st_size + 2 * (unsigned long)MAXWORDS;
    // CHECKEDC : add more variable to hold bounds information
    // buffer is added to declare bounds information for both pch and pchBase
    // since those are both incremented/modified in code
    _Array_ptr<char> buffer : count(ulLen) = 0;
    _Array_ptr<char> pch : bounds(buffer, buffer+ulLen) = 0;
    _Array_ptr<char> pchBase : bounds(buffer, buffer+ulLen) = 0;
    // CHECKEDC : programmer-inserted facts
    // For static checking, a new fact is introduced by hands
    // buffer : count(ulLen)
    // pchDictionary : count(pchDictionarySize)
    // dynamic_check(pchDictionarySize == ulLen);
    pchBase = buffer = pchDictionary = malloc(pchDictionarySize);

    if(pchDictionary == NULL)
	Fatal("Unable to allocate memory for dictionary\n", 0);

    if ((fp = fopen((const char*)pchFile, "r")) == NULL)
	Fatal("Cannot open dictionary\n", 0);

    // CHECKEDC : bounds-safe interface is required (implemented, stdio_checked.h)
    // TODO : after handling issue#143, commit
    while (!feof((FILE*)fp)) {
        // CHECKEDC : pointer arithmetic (non-null check only)
        // dynamic_check(pchBase != NULL)
        pch = pchBase+2;                /* reserve for length */
        cLetters = 0;
        // CHECKEDC : bounds-safe interface is required (implemented, stdio_checked.h)
        // TODO : after handling issue#143, commit
        while ((ch = fgetc((FILE*)fp)) != '\n' && ch != EOF) {
            if (isalpha(ch)) cLetters++;
            // CHECKEDC : pointer arithmetic(non-null check only) & dereference(in-bounds check)
            // dynamic_check(pch != NULL && (pch >= buffer && pch < (buffer+ulLen)))
            *pch++ = ch;
        }
        // CHECKEDC : pointer arithmetic(non-null check only) & dereference(in-bounds check)
        // dynamic_check(pch != NULL && (pch >= buffer && pch < (buffer+ulLen)))
        *pch++ = '\0';
        // CHECKEDC : dereference(non-null check & in-bounds check)
        // dynamic_check(pchBase != NULL && (pchBase >= buffer && pchBase < (buffer+ulLen)))
        *pchBase = pch - pchBase;
        // CHECKEDC : array subscript (in-bounds check)
        // dynamic_check((pchBase+1) >= buffer && (pchBase+1) < (buffer+ulLen))
        pchBase[1] = cLetters;
        pchBase = pch;
        cWords++;
    }
    // CHECKEDC : bounds-safe interface is required (implemented, stdio_checked.h)
    // TODO : after handling issue#143, commit
    fclose((FILE*)fp);

    // CHECKEDC : pointer arithmetic(non-null check only) & dereference (in-bounds check)
    // dynamic_check(pchBase != NULL)
    // dynamic_check((pchBase+1)>=buffer && (pchBase+1) < (buffer+ulLen))
    *pchBase++ = 0;

    fprintf(stderr, "main dictionary has %u entries\n", cWords);
    if (cWords >= MAXWORDS)
	Fatal("Dictionary too large; increase MAXWORDS\n", 0);
    fprintf(stderr, "%lu bytes wasted\n", ulLen - (pchBase - pchDictionary));
}

// CHECKEDC : function call formal parameter bounds information
// function input parameter is from achPhrase[255]
// since checked array type(achPhrase) is not modifiable, it can be used to represent bounds
void BuildMask(_Array_ptr<char> pchPhrase : bounds(achPhrase, achPhrase+255)) {
    int i;
    int ch;
    unsigned iq;                        /* which Quad? */
    int cbtUsed;                        /* bits used in the current Quad */
    int cbtNeed;                        /* bits needed for current letter */
    Quad qNeed;                         /* used to build the mask */

    // CHECKEDC : bounds-safe interface is required (implemented, stdio_checked.h)
    // rewrite bzero with memset to use bounds-safe interface
    memset(alPhrase, 0, sizeof(Letter)*ALPHABET);
    memset(aqMainMask, 0, sizeof(Quad)*MAX_QUADS);
    memset(aqMainSign, 0, sizeof(Quad)*MAX_QUADS);
/*
    Zero(alPhrase);
    Zero(aqMainMask);
    Zero(aqMainSign);
*/

    /* Tabulate letter frequencies in the phrase */
    cchPhraseLength = 0;
    // CHECKEDC : pointer arithmetic (non-null check only) & dereference (in-bounds check)
    // dynamic_check(pchPhrase != NULL)
    // dynamic_check(pchPhrase >= achPhrase && pchPhrase < (achPhrase+255))
    while ((ch = *pchPhrase++) != '\0') {
        if (isalpha(ch)) {
            ch = tolower(ch);
            // CHECKEDC : checkedc array type array subscript, in-bounds check
            // dynamic_check(ch2i(ch) >= 0 && ch2i(ch) < ALPHABET)
            lPhrase(ch).uFrequency++;
            cchPhraseLength++;
        }
    }

    /* Build  masks */
    iq = 0;                             /* which quad being used */
    cbtUsed = 0;                        /* bits used so far */

    for (i = 0; i < ALPHABET; i++) {
        // CHECKEDC : checkedc array type array subscript, in-bounds check
        // dynamic_check(i >= 0 && i < ALPHABET)
        // : reasoning facts from for-stmt can remove unnecessary bounds check
        if (alPhrase[i].uFrequency == 0) {
            auGlobalFrequency[i] = ~0;  /* to make it sort last */
        } else {
            auGlobalFrequency[i] = 0;
            // CHECKEDC : checkedc array type array subscript, in-bounds check
            // dynamic_check(i >= 0 && i < ALPHABET)
            // : reasoning facts can remove unnecessary bounds check
            for (cbtNeed = 1, qNeed = 1;
                 alPhrase[i].uFrequency >= qNeed;
                 cbtNeed++, qNeed <<= 1);
            if (cbtUsed + cbtNeed > MASK_BITS) {
                if (++iq >= MAX_QUADS)
		    Fatal("MAX_QUADS not large enough\n", 0);
                cbtUsed = 0;
            }
            alPhrase[i].uBits = qNeed-1;
            if (cbtUsed)
		qNeed <<= cbtUsed;
            // CHECKEDC : checkedc array type array subscript in-bounds check
            // dynamic_check(iq >= 0 && iq < MAX_QUADS)
            // : reasoning facts can remove unnecessary runtime bounds check
            aqMainSign[iq] |= qNeed;
            aqMainMask[iq] |= (Quad)alPhrase[i].uFrequency << cbtUsed;
            alPhrase[i].uShift = cbtUsed;
            alPhrase[i].iq = iq;
            cbtUsed += cbtNeed;
        }
    }
}

// CHECKEDC
PWord NewWord(void) {
    PWord pw = malloc(sizeof(Word));
    if (pw == NULL)
        Fatal("Out of memory after %d candidates\n", cpwCand);
    return pw;
}

/* wprint -- print a word, followed by a space
 *
 * We would normally just use printf, but the string being printed is
 * is a huge pointer (on an IBM PC), so special care must be taken.
 */
void wprint(_Array_ptr<char> pch) {
    // CHECKEDC : bounds-safe interface is not given
    printf("%s ", (const char*)pch);
}

PWord NextWord(void);

/* NextWord -- get another candidate entry, creating if necessary */
PWord NextWord(void) {
    // CHECKEDC : automatic checkedc pointer variable initializer
    PWord pw = 0;
    if (cpwCand >= MAXCAND)
	Fatal("Too many candidates\n", 0);
    // CHECKEDC : checkedc array pointer array subscript, in-bounds check
    // dynamic_check((cpwCand >= 0) && (cpwCand < MAXCAND))
    // : above reasoning facts can remove redundant dynamic_check insertion
    pw = apwCand[cpwCand++];
    if (pw != NULL)
	return pw;
    // CHECKEDC : checkedc array pointer access, in-bounds check
    // dynamic_check((cpwCand-1) >= 0 && ((cpwCand-1) < MAXCAND))
    // : reaonsing facts can remove redundant bounds check insertion
    apwCand[cpwCand-1] = NewWord();
    return apwCand[cpwCand-1];
}

/* BuildWord -- build a Word structure from an ASCII word
 * If the word does not fit, then do nothing.
 */
// CHECKEDC : function call formal parameters, checkedc array pointer
// FIXME
// since pchDictionary is global variable and function has function call in function body
// it is not reasonable that pchDictionary is used to represent bounds of parameter
// - rewrite function to represent pchWord parameter bounds information
// - add additional parameter to hold bounds for pchWord
// - wordStart, wordEnd is added to represent start/end of pchWords
void BuildWord(_Array_ptr<char> pchWord : bounds(wordStart, wordEnd),
        _Array_ptr<char> wordStart, _Array_ptr<char> wordEnd) {
    // CHECKEDC : checkedc array type, array subscript
    unsigned char cchFrequency _Checked [ALPHABET];
    int i;
    // CHECKEDC
    // FIXME
    // global pointer variable is not recommended to represent bounds of local variable
    // if it has function call within its body
    // use additional bounds parameter
    _Array_ptr<char> pch : bounds(wordStart, wordEnd) = pchWord;
    PWord pw = 0;
    int cchLength = 0;

    // CHECKEDC : bounds-safe interface is required
    // rewrite bzero with memset to use bounds-safe interface
    memset(cchFrequency, 0, sizeof(unsigned char)*ALPHABET);
    /* Zero(cchFrequency); */

    /* Build frequency table */
    // CHECKEDC : pointer arithmetic (non-null check only) && dereference(in-bounds check)
    // dynamic_check(pch != NULL)
    // dynamic_check(pch >= wordStart && pch < wordEnd)
    while ((i = *pch++) != '\0') {
        if (!isalpha(i)) continue;
        i = ch2i(tolower(i));
        // CHECKEDC : checkedc array type array subscript, in-bounds check
        // dynamic_check(i >= 0 && i < ALPHABET), cchFrequency[ALPHABET], alPhrase[ALPHABET]
        // : isalpha() check if value is alphabet or not
        // : reasoning facts can't know about facts of function call, insert bounds check
        if (++cchFrequency[i] > alPhrase[i].uFrequency)
	    return;
        ++cchLength;
    }

    Debug(wprint(pchWord);)

    /* Update global count */
    for (i = 0; i < ALPHABET; i++)
        // CHECKEDC : checkedc array type array subscript
        // dynamic_check(i >= 0 && i < ALPHABET)
        // : reaonsing facts can remove unnecessary bounds check
        auGlobalFrequency[i] += cchFrequency[i];

    /* Create a Word structure and fill it in, including building the
     * bitfield of frequencies.
     */
    pw = NextWord();
    // CHECKEDC : bounds-safe interface is required
    // rewrite bzero with memset to use bounds-safe interface
    memset(pw->aqMask, 0, sizeof(Quad)*MAX_QUADS);
    /* Zero(pw->aqMask); */
    pw->pchWord = pchWord;
    pw->cchLength = cchLength;
    for (i = 0; i < ALPHABET; i++) {
        // CHECKEDC : checkedc array type array subscript
        // : runtime bounds check
        // dynamic_check(i >= 0 && i < ALPHABET)
        // dynamic_check(alPhrase[i].iq >= 0 && alPhrase[i].iq < MAX_QUADS)
        pw->aqMask[alPhrase[i].iq] |=
            (Quad)cchFrequency[i] << alPhrase[i].uShift;
    }
}

/* AddWords -- build the list of candidates */
void
AddWords(void) {
    // CHECKEDC : declared bounds at variable declaration
    // FIXME
    // Since it has function call within body, it is not proper usage
    // that global variable is directly used for bounds declaration
    // In another function call, it can modify those global variables
    // To prevent this problem, add additional pointer variable to hold bounds
    _Array_ptr<char> pchLowerBounds = pchDictionary;
    _Array_ptr<char> pchUpperBounds = pchDictionary + pchDictionarySize;
    _Array_ptr<char> pch : bounds(pchLowerBounds, pchUpperBounds) = pchDictionary;     /* walk through the dictionary */

    cpwCand = 0;

    // CHECKEDC : memory read, dereference(in-bounds check)
    // dynamic_check(pch >= pchLowerBounds && pch < pchUpperBounds)
    while (*pch) {
        // 1. find out each word in dictionary
        // 2. if word letter size is beyond phrase size, skip word
        // 3. check if frequency(word letter) < frequency(phrase letter) for all letters of word
        // 4. add words as candidates for anagram for this phrase
        if ((pch[1] >= cchMinLength && pch[1]+cchMinLength <= cchPhraseLength)
            || pch[1] == cchPhraseLength)
            // pch[0] = length of word + 3 bytes(length of words, number of letter, NULL string)
            // pch[1] = number of letters
            // pch[2..] = word itself
        {
            // FIXME
            // - add addtional variable to represent words bounds (wordStart/wordEnd)
            // - max word length is encoded in pch[0], use it
            // - rewrite function call to hold bounds start/end in function argument
            unsigned char wordLength = pch[0];
            _Array_ptr<char> wordStart = pch;
            _Array_ptr<char> wordEnd = pch+wordLength;
            BuildWord(pch+2, wordStart, wordEnd);
        }
        // CHECKEDC : dereference(in-bounds check) && pointer arithmetic (non-null check only)
        // dynamic_check(pch != NULL)
        // dynamic_check((pch + (*pch)) >= pchDictionary && (pch + (*pch)) < (pchDictionary+MAXWORDS))
        pch += *pch;
    }

    fprintf(stderr, "%d candidates\n", cpwCand);
}

void DumpCandidates(void) {
    unsigned u;

    for (u = 0; u < cpwCand; u++)
        printf(StringFormat, apwCand[u]->pchWord, (u % 4 == 3) ? '\n' : ' ');
    printf("\n");
}

PWord apwSol _Checked [MAXSOL];                   /* the answers */
int cpwLast;

Debug(
void DumpWord(Quad * pq) {
    int i;
    Quad q;
    for (i = 0; i < ALPHABET; i++) {
        if (alPhrase[i].uFrequency == 0) continue;
        q = pq[alPhrase[i].iq];
        if (alPhrase[i].uShift) q >>= alPhrase[i].uShift;
        q &= alPhrase[i].uBits;
        while (q--) putchar('a'+i);
    }
    putchar(' ');
}
)                                       /* End of debug code */

void DumpWords(void) {
static int X;
  int i;
  X = (X+1) & 1023;
  if (X != 0) return;
    for (i = 0; i < cpwLast; i++) wprint(apwSol[i]->pchWord);
    printf("\n");
}

Stat(unsigned long ulHighCount; unsigned long ulLowCount;)

jmp_buf jbAnagram;

#define OneStep(i) \
    if ((aqNext[i] = pqMask[i] - pw->aqMask[i]) & aqMainSign[i]) { \
        ppwStart++; \
        continue; \
    }

// CHECKEDC : function call formal parameters, checkedc array pointer
// : pqMask declared bounds is from aqMainMask[MAX_QUADS]
// : ppwStart declared bounds is from apwCand[MAXCAND]
void FindAnagram(_Array_ptr<Quad> pqMask : count(MAX_QUADS),
        PPWord ppwStart : bounds(apwCand, apwCand+MAXCAND), int iLetter)
{
    // CHECKEDC
    Quad aqNext _Checked [MAX_QUADS];
    register PWord pw = 0;
    Quad qMask;
    unsigned iq;
    PPWord ppwEnd : bounds(apwCand, apwCand+MAXCAND) = &apwCand[0];
    // CHECKEDC : pointer arithmetic(non-null check only)
    // dynamic_check(ppwEnd != NULL)
    ppwEnd += cpwCand;

    ;

    if (HaltProcessing()) longjmp(jbAnagram, 1);

    Debug(printf("Trying :"); DumpWord(pqMask); printf(":\n");)

    for (;;) {
        // CHECKEDC : checkedc array type array subscript in-bounds check
        // dynamic_check(iLetter >= 0 && iLetter < ALPHABET)
        // dynamic_check(achByFrequency[iLetter].iq >= 0 && achByFrequency[iLetter].iq < ALPHABET)
        iq = alPhrase[achByFrequency[iLetter]].iq;
        qMask = alPhrase[achByFrequency[iLetter]].uBits <<
                alPhrase[achByFrequency[iLetter]].uShift;
        // CHECKEDC : checkedc array type array subscript in-bounds check
        // dynamic_check(iq >= 0 && iq < MAX_QUADS)
        if (pqMask[iq] & qMask) break;
        iLetter++;
    }

    // CHECKEDC : checkedc array type array subscript, in-bounds check
    // dynamic_check(iLetter >= 0 && iLetter < ALPHABET)
    Debug(printf("Pivoting on %c\n", i2ch(achByFrequency[iLetter]));)

    while (ppwStart < ppwEnd) {          /* Half of the program execution */
        // CHECKEDC : pointer dereference
        // dynamic_check(ppwStart >= apwCand && ppwStart < (apwCand+MAXCAND))
        // : reasoning facts ppwStart < ppwEnd < (apwCand+MAXCAND)
        // : compiler can optimize redundant runtime bounds check
        pw = *ppwStart;                  /* time is spent in these three */

        Stat(if (++ulLowCount == 0) ++ulHighCount;)

            // CHECKEDC : checkedc array type array subscript
            // aqNext[i], pqMask[i], pw->aqMask[i], aqMainSign[i]
            // dynamic_check(i >= 0 && i < MAX_QUADS)
            // In below macros, i is constant value (0/1/2/3)
#if MAX_QUADS > 0
        OneStep(0);                     /* lines of code. */
#endif

#if MAX_QUADS > 1
        OneStep(1);
#endif

#if MAX_QUADS > 2
        OneStep(2);
#endif

#if MAX_QUADS > 3
        OneStep(3);
#endif

#if MAX_QUADS > 4
            @@"Add more unrolling steps here, please."@@
#endif

        /* If the pivot letter isn't present, defer this word until later */
                // CHECKEDC : checkedc array type array subscript
                // dynamic_check(iq >= 0 && iq < MAX_QUADS)
        if ((pw->aqMask[iq] & qMask) == 0) {
            // CHECKEDC : pointer arithmetic(non-null check only) && dereference(in-bounds check)
            // dynamic_check(ppwStart != NULL && ppwEnd != NULL)
            // dynamic_check(ppwStart >= apwCand && ppwStart < (apwCand+MAXCAND))
            // dynamic_check(ppwEnd >= apwCand && ppwEnd < (apwCand+MAXCAND))
            *ppwStart = *--ppwEnd;
            *ppwEnd = pw;
            continue;
        }

        /* If we get here, this means the word fits. */
            // CHECKEDC : checkedc array type array subscript, in-bounds check
            // dynamic_check(cpwLast >= 0 && cpwLast < MAXSOL)
        apwSol[cpwLast++] = pw;
        if (cchPhraseLength -= pw->cchLength) { /* recurse */
            Debug(DumpWords();)
            /* The recursive call scrambles the tail, so we have to be
             * pessimistic.
             */
                // CHECKEDC : pointer arithmetic(non-null check only)
                // dynamic_check(ppwEnd != NULL)
                // dynamic_check((ppwEnd+cpwCand) >= apwCand && (ppwEnd+cpwCand) < (apwCand+MAXCAND))
	    ppwEnd = &apwCand[0];
	    ppwEnd += cpwCand;
        // CHECKEDC : function call actual argument passing
        // : static checking rule if function call actual argument is within formal parameter
        // if bounds condition is statically verifiable, compilation pass, otherwise compilation fails
            FindAnagram(&aqNext[0],
			ppwStart, iLetter);
        } else DumpWords();             /* found one */
        cchPhraseLength += pw->cchLength;
        --cpwLast;
        ppwStart++;
        continue;
    }

    ;
}

int Cdecl CompareFrequency(_Ptr<char> pch1, _Ptr<char> pch2) {
    // CHECKEDC : checkedc array type & memory dereference
    // : _Ptr type check - non-null check
    // : _Array_ptr type check - in-bounds check
    // dynamic_check(pch1 != NULL && pch2 != NULL)
    // dynamic_check((*pch1) >= 0 && (*pch1) < ALPHABET)
    // dynamic_check((*pch2) >= 0 && (*pch2) < ALPHABET)
    if (auGlobalFrequency[*pch1] < auGlobalFrequency[*pch2])
        return -1;
	if (auGlobalFrequency[*pch1] > auGlobalFrequency[*pch2])
        return 1;
    if (*pch1 < *pch2)
        return -1;
	if (*pch1 > *pch2)
        return 1;
    return 0;
}

void SortCandidates(void) {
    int i;

    /* Sort the letters by frequency */
    // CHECKEDC : array subscript, in-bounds check
    // dynamic_check(i >= 0 && i < ALPHABET)
    // : reaonsing facts for for-stmt can remove redundant runtime bounds check
    for (i = 0; i < ALPHABET; i++) achByFrequency[i] = i;
    // CHECKEDC : bounds-safe interface is required  (implemented, stdlib_checked.h)
    qsort((void*)achByFrequency, ALPHABET, sizeof(char),
          (int (*)(const void*, const void *))CompareFrequency);

    fprintf(stderr, "Order of search will be ");
    for (i = 0; i < ALPHABET; i++)
        // CHECKEDC : checkedc array type array subscript,
        // dynamic_check(i >= 0 && i < ALPHABET)
        // : reaonsing facts for for-stmt can remove redundant runtime bounds check
	fputc(i2ch(achByFrequency[i]), stderr);
    fputc('\n', stderr);
}

int fInteractive;

// CHECKEDC : function call formal parameter & return value is checkedc array pointer 
// : formal parameter declared bounds is from achPhrase[255]
_Array_ptr<char> GetPhrase(_Array_ptr<char> pch : bounds(achPhrase, achPhrase+255), int size)
    : bounds(achPhrase, achPhrase+255) {
    if (fInteractive) printf(">");
    fflush(stdout);
    // CHECKEDC : bounds-safe interface is required (implemented, stdio_checked.h)
    // To avoid compilation error in current implementation, do explicit type casting
    if (fgets((char*)pch, size, stdin) == NULL) {
#ifdef PLUS_STATS
	PrintDerefStats(stderr);
        PrintHeapSize(stderr);
#endif /* PLUS_STATS */
	exit(0);
    }
    return(pch);
}

// CHECKEDC : moved upward since this global variable is used to represent bounds information
// of buildMask function formal parameter
//char achPhrase _Checked [255];

int Cdecl main(int cpchArgc, char **ppchArgv) {

    if (cpchArgc != 2 && cpchArgc != 3)
        Fatal("Usage: anagram dictionary [length]\n", 0);

    if (cpchArgc == 3)
	cchMinLength = atoi(ppchArgv[2]);

    fInteractive = isatty(1);

    ReadDict(ppchArgv[1]);

    // CHECKEDC : checkedc array type array subscript, in-bounds check
    // static checking for function call
    while (GetPhrase(&achPhrase[0], sizeof(achPhrase)) != NULL) {
        if (isdigit(achPhrase[0])) {
            // CHECKEDC : bounds-safe interface is required but not implemented
            cchMinLength = atoi((const char*)achPhrase);
            printf("New length: %d\n", cchMinLength);
        } else if (achPhrase[0] == '?') {
            DumpCandidates();
        } else {
            // CHECKEDC : checkedc array type is treated as checkedc array_ptr
            BuildMask(&achPhrase[0]);
            AddWords();
            if (cpwCand == 0 || cchPhraseLength == 0) continue;

            Stat(ulHighCount = ulLowCount = 0;)
            cpwLast = 0;
            SortCandidates();
            if (setjmp(jbAnagram) == 0)
                FindAnagram(&aqMainMask[0], &apwCand[0], 0);
            Stat(printf("%lu:%lu probes\n", ulHighCount, ulLowCount);)
        }
    }
    return 0;
}
