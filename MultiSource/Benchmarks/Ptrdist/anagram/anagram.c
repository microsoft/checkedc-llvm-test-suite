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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <unistd.h>
#include "hacks.h"

jmp_buf jbAnagram;

#pragma CHECKED_SCOPE ON

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
    Quad aqMask _Checked [MAX_QUADS];  /* the word's mask */
    _Array_ptr<char> pchWord : count(cchLength);                 /* the word itself */
    unsigned cchLength;                 /* letters in the word */
} Word;
typedef _Ptr<Word> PWord;
typedef _Array_ptr<PWord> PPWord;

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

unsigned long pchDictionarySize;
_Array_ptr<char> pchDictionary : count(pchDictionarySize);               /* the dictionary is read here */

#define Zero(t) memset(t, 0, sizeof(t)) /* quickly zero out an integer array */
#define fprintf(...) _Unchecked { fprintf(__VA_ARGS__); }
#define printf(...) _Unchecked { printf(__VA_ARGS__); }

/* Fatal -- print a message before expiring */
void Fatal(_Nt_array_ptr<const char> pchMsg, unsigned u) {
    fprintf(stderr, pchMsg, u);
    exit(1);
}

#undef isalpha
#undef isdigit
#undef tolower
#undef bzero
void bzero(void *s : byte_count(n), size_t n);

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

int stat(const char * restrict path : itype(restrict _Nt_array_ptr<const char>),
         struct stat * restrict buf : itype(restrict _Ptr<struct stat>));

void ReadDict(_Nt_array_ptr<char> pchFile) {
    _Ptr<FILE> fp = 0;
    unsigned cWords = 0;
    unsigned cLetters;
    int ch;
    struct stat statBuf;

    if (stat(pchFile, &statBuf)) Fatal("Cannot stat dictionary\n", 0);

    unsigned long ulLen;
    pchDictionarySize = ulLen = statBuf.st_size + 2 * (unsigned long)MAXWORDS;
    _Array_ptr<char> buffer : count(ulLen) = 0;
    _Array_ptr<char> pchBase : bounds(buffer, buffer+ulLen) = 0;
    pchBase = buffer = pchDictionary = calloc<char>(pchDictionarySize, sizeof(char));
    _Array_ptr<char> pch : bounds(buffer, buffer+ulLen) = 0;

    if(pchDictionary == NULL)
	Fatal("Unable to allocate memory for dictionary\n", 0);

    if ((fp = fopen(pchFile, "r")) == NULL)
	Fatal("Cannot open dictionary\n", 0);

    while (!feof(fp)) {
        pch = pchBase+2;                /* reserve for length */
        cLetters = 0;
        while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
            if (isalpha(ch)) cLetters++;
            *pch++ = ch;
        }
        *pch++ = '\0';
        *pchBase = pch - pchBase;
        pchBase[1] = cLetters;
        pchBase = pch;
        cWords++;
    }
    fclose(fp);

    *pchBase++ = 0;

    fprintf(stderr, "main dictionary has %u entries\n", cWords);
    if (cWords >= MAXWORDS)
	Fatal("Dictionary too large; increase MAXWORDS\n", 0);
    fprintf(stderr, "%lu bytes wasted\n", ulLen - (pchBase - pchDictionary));
}

void BuildMask(_Array_ptr<char> pchPhrase : bounds(achPhrase, achPhrase+255)) {
    int i;
    int ch;
    unsigned iq;                        /* which Quad? */
    int cbtUsed;                        /* bits used in the current Quad */
    int cbtNeed;                        /* bits needed for current letter */
    Quad qNeed;                         /* used to build the mask */

    bzero(alPhrase, sizeof(Letter)*ALPHABET);
    bzero(aqMainMask, sizeof(Quad)*MAX_QUADS);
    bzero(aqMainSign, sizeof(Quad)*MAX_QUADS);
/*
    Zero(alPhrase);
    Zero(aqMainMask);
    Zero(aqMainSign);
*/

    /* Tabulate letter frequencies in the phrase */
    cchPhraseLength = 0;
    while ((ch = *pchPhrase++) != '\0') {
        if (isalpha(ch)) {
            ch = tolower(ch);
            lPhrase(ch).uFrequency++;
            cchPhraseLength++;
        }
    }

    /* Build  masks */
    iq = 0;                             /* which quad being used */
    cbtUsed = 0;                        /* bits used so far */

    for (i = 0; i < ALPHABET; i++) {
        if (alPhrase[i].uFrequency == 0) {
            auGlobalFrequency[i] = ~0;  /* to make it sort last */
        } else {
            auGlobalFrequency[i] = 0;
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
            aqMainSign[iq] |= qNeed;
            aqMainMask[iq] |= (Quad)alPhrase[i].uFrequency << cbtUsed;
            alPhrase[i].uShift = cbtUsed;
            alPhrase[i].iq = iq;
            cbtUsed += cbtNeed;
        }
    }
}

PWord NewWord(void) {
    PWord pw = calloc<Word>(1, sizeof(Word));
    if (pw == NULL)
        Fatal("Out of memory after %d candidates\n", cpwCand);
    return pw;
}

/* wprint -- print a word, followed by a space
 *
 * We would normally just use printf, but the string being printed is
 * is a huge pointer (on an IBM PC), so special care must be taken.
 */
void wprint(_Nt_array_ptr<char> pch) {
    printf("%s ", pch);
}

PWord NextWord(void);

/* NextWord -- get another candidate entry, creating if necessary */
PWord NextWord(void) {
    PWord pw = 0;
    if (cpwCand >= MAXCAND)
	Fatal("Too many candidates\n", 0);
    pw = apwCand[cpwCand++];
    if (pw != NULL)
	return pw;
    apwCand[cpwCand-1] = NewWord();
    return apwCand[cpwCand-1];
}

/* BuildWord -- build a Word structure from an ASCII word
 * If the word does not fit, then do nothing.
 */
void BuildWord(_Array_ptr<char> pchWord : bounds(wordStart, wordEnd),
        _Array_ptr<char> wordStart, _Array_ptr<char> wordEnd) {
    unsigned char cchFrequency _Checked [ALPHABET];
    int i;
    _Array_ptr<char> pch : bounds(wordStart, wordEnd) = pchWord;
    PWord pw = 0;
    int cchLength = 0;

    bzero(cchFrequency, sizeof(unsigned char)*ALPHABET);
    /* Zero(cchFrequency); */

    /* Build frequency table */
    while ((i = *pch++) != '\0') {
        if (!isalpha(i)) continue;
        i = ch2i(tolower(i));
        if (++cchFrequency[i] > alPhrase[i].uFrequency)
	    return;
        ++cchLength;
    }

    Debug(wprint(pchWord);)

    /* Update global count */
    for (i = 0; i < ALPHABET; i++)
        auGlobalFrequency[i] += cchFrequency[i];

    /* Create a Word structure and fill it in, including building the
     * bitfield of frequencies.
     */
    pw = NextWord();
    bzero(pw->aqMask, sizeof(Quad)*MAX_QUADS);
    /* Zero(pw->aqMask); */
    _Unchecked { pw->cchLength = cchLength, pw->pchWord = pchWord; }
    for (i = 0; i < ALPHABET; i++) {
        pw->aqMask[alPhrase[i].iq] |=
            (Quad)cchFrequency[i] << alPhrase[i].uShift;
    }
}

/* AddWords -- build the list of candidates */
void
AddWords(void) {
    _Array_ptr<char> pchLowerBounds = pchDictionary;
    _Array_ptr<char> pchUpperBounds = pchDictionary + pchDictionarySize;
    UncheckedBoundsInit(_Array_ptr<char>, pch, bounds(pchLowerBounds, pchUpperBounds), pchDictionary)     /* walk through the dictionary */

    cpwCand = 0;

    while (*pch) {
        if ((pch[1] >= cchMinLength && pch[1]+cchMinLength <= cchPhraseLength)
            || pch[1] == cchPhraseLength)
        {
            unsigned char wordLength = pch[0];
            _Array_ptr<char> wordStart = pch;
            _Array_ptr<char> wordEnd = pch+wordLength;
            _Dynamic_check(wordEnd <= pchUpperBounds);
            _Unchecked { BuildWord(pch+2, wordStart, wordEnd); }
        }
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
    for (i = 0; i < cpwLast; i++) _Unchecked { wprint((_Nt_array_ptr<char>)apwSol[i]->pchWord); }
    printf("\n");
}

Stat(unsigned long ulHighCount; unsigned long ulLowCount;)

#define OneStep(i) \
    if ((aqNext[i] = pqMask[i] - pw->aqMask[i]) & aqMainSign[i]) { \
        ppwStart++; \
        continue; \
    }

void FindAnagram(_Array_ptr<Quad> pqMask : count(MAX_QUADS),
        PPWord ppwStart : bounds(ppwStart, apwCand+MAXCAND), int iLetter)
{
    Quad aqNext _Checked [MAX_QUADS];
    register PWord pw = 0;
    Quad qMask;
    unsigned iq;
    UncheckedBoundsInit(PPWord, ppwEnd, bounds(ppwStart, apwCand+cpwCand), &apwCand[0])
    ppwEnd += cpwCand;

    ;

    if (HaltProcessing()) _Unchecked { longjmp(jbAnagram, 1); }

    Debug(printf("Trying :"); DumpWord(pqMask); printf(":\n");)

    for (;;) {
        iq = alPhrase[achByFrequency[iLetter]].iq;
        qMask = alPhrase[achByFrequency[iLetter]].uBits <<
                alPhrase[achByFrequency[iLetter]].uShift;
        if (pqMask[iq] & qMask) break;
        iLetter++;
    }

    Debug(printf("Pivoting on %c\n", i2ch(achByFrequency[iLetter]));)

    // Manually Hoisted Check, (including path condition) from first iteration of the loop.
    _Dynamic_check(ppwStart != NULL);
    _Dynamic_check(ppwStart < ppwEnd && ppwEnd <= (apwCand+MAXCAND));

    while (ppwStart < ppwEnd) {          /* Half of the program execution */
        pw = *ppwStart;                  /* time is spent in these three */

        Stat(if (++ulLowCount == 0) ++ulHighCount;)

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
        if ((pw->aqMask[iq] & qMask) == 0) {
            *ppwStart = *--ppwEnd;
            *ppwEnd = pw;
            continue;
        }

        /* If we get here, this means the word fits. */
        apwSol[cpwLast++] = pw;
        if (cchPhraseLength -= pw->cchLength) { /* recurse */
            Debug(DumpWords();)
            /* The recursive call scrambles the tail, so we have to be
             * pessimistic.
             */
	    _Unchecked { ppwEnd = &apwCand[0]; }
	    ppwEnd += cpwCand;
            _Unchecked { FindAnagram(&aqNext[0],
			ppwStart, iLetter); }
        } else DumpWords();             /* found one */
        cchPhraseLength += pw->cchLength;
        --cpwLast;
        ppwStart++;
        continue;
    }

    ;
}

int Cdecl CompareFrequency(_Ptr<const void> ch1, _Ptr<const void> ch2) {
  _Ptr<const char> pch1 = (_Ptr<const char>)ch1;
  _Ptr<const char> pch2 = (_Ptr<const char>)ch2;
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
    for (i = 0; i < ALPHABET; i++) achByFrequency[i] = i;
    qsort(achByFrequency, ALPHABET, sizeof(char),
          CompareFrequency);

    fprintf(stderr, "Order of search will be ");
    for (i = 0; i < ALPHABET; i++) {
      char val = i2ch(achByFrequency[i]);
      fputc(val, stderr);
    }
    fputc('\n', stderr);
}

int fInteractive;

_Array_ptr<char> GetPhrase(_Array_ptr<char> pch : count(size), int size)
    : count(size) {
    if (fInteractive) printf(">");
    fflush(stdout);
    _Unchecked { if (fgets(pch, size, stdin) == NULL) _Checked {
#ifdef PLUS_STATS
	PrintDerefStats(stderr);
        PrintHeapSize(stderr);
#endif /* PLUS_STATS */
	exit(0);
    } }
    return(pch);
}

int Cdecl main(int cpchArgc, _Array_ptr<_Nt_array_ptr<char>> ppchArgv : count(cpchArgc)) {

    if (cpchArgc != 2 && cpchArgc != 3)
        Fatal("Usage: anagram dictionary [length]\n", 0);

    if (cpchArgc == 3)
	cchMinLength = atoi(ppchArgv[2]);

    fInteractive = isatty(1);

    ReadDict(ppchArgv[1]);

    while (GetPhrase(&achPhrase[0], sizeof(achPhrase)) != NULL) {
        if (isdigit(achPhrase[0])) {
            _Unchecked { cchMinLength = atoi((_Nt_array_ptr<char>)achPhrase); }
            printf("New length: %d\n", cchMinLength);
        } else if (achPhrase[0] == '?') {
            DumpCandidates();
        } else {
            BuildMask(&achPhrase[0]);
            AddWords();

            if (cpwCand == 0 || cchPhraseLength == 0) continue;

            Stat(ulHighCount = ulLowCount = 0;)
            cpwLast = 0;
            SortCandidates();
            _Unchecked{ if (setjmp(jbAnagram) == 0)
                FindAnagram(&aqMainMask[0], &apwCand[0], 0); }
            Stat(printf("%lu:%lu probes\n", ulHighCount, ulLowCount);)
        }
    }
    return 0;
}
