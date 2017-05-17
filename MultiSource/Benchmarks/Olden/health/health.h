/* For copyright information, see olden_v1.0/COPYRIGHT */

/****************************************************************
 * health.h: header file for health.c                           *
 ****************************************************************/


#ifndef _HEALTH
#define _HEALTH

#include <stdchecked.h>
#include <stdio_checked.h>
#include <stdlib_checked.h>
#pragma BOUNDS_CHECKED ON

#define chatting printf

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

extern int  max_level;
extern long max_time;
extern long long seed;

struct Results {
  float                   total_patients;
  float                   total_time;
  float                   total_hosps; 
};

struct Patient {
  int                     hosps_visited;
  int                     time;
  int                     time_left;
  ptr<struct Village>     home_village;
};

struct List {
  ptr<struct List>        forward;
  ptr<struct Patient>     patient;
  ptr<struct List>        back;
};

struct Hosp {
#if 1
  int                    personnel; 
  int                    free_personnel; 
  int                    num_waiting_patients; 
  struct List            waiting; 
  struct List            assess; 
  struct List            inside;
  struct List            up;
#else
  int                    free_personnel; 
  struct List            waiting;
  struct List            assess;
  struct List            inside;
  struct List            up;
  int                    personnel; 
  int                    num_waiting_patients; 
#endif
};
 
struct Village {
#if 1
  ptr<struct Village>    forward checked[4];
  ptr<struct Village>    back;
  struct List            returned;
  struct Hosp            hosp;   
  int                    label;
  long long              seed;
#else
  struct Hosp            hosp;   
  long                   seed;
  ptr<struct Village>    forward checked[4];
  int                    label;
  struct List            returned;
  ptr<struct Village>    back;
#endif
};

ptr<struct Village> alloc_tree(int level, int label, ptr<struct Village> back);
unchecked void dealwithargs(int argc, array_ptr<char*> argv : count(argc));
float my_rand(long long idum);
ptr<struct Patient> generate_patient(ptr<struct Village> village);
void put_in_hosp(ptr<struct Hosp> hosp, ptr<struct Patient>patient);
void addList(ptr<struct List> list, ptr<struct Patient> patient);
void removeList(ptr<struct List> list, ptr<struct Patient> patient);
ptr<struct List> sim(ptr<struct Village> village);
void check_patients_inside(ptr<struct Village> village, ptr<struct List> list);
ptr<struct List> check_patients_assess(ptr<struct Village> village, ptr<struct List> list);
void check_patients_waiting(ptr<struct Village> village, ptr<struct List> list);
float get_num_people(ptr<struct Village> village);
float get_total_time(ptr<struct Village> village);
float get_total_hosps(ptr<struct Village> village);
struct Results get_results(ptr<struct Village> village);

#pragma BOUNDS_CHECKED OFF
#endif

