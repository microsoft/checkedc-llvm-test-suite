/* For copyright information, see olden_v1.0/COPYRIGHT */

/********************************************************************
 *  List.c:  Handles lists.                                         *
 *           To be used with health.c                               *
 ********************************************************************/

#include <stdchecked.h>
#include <stdio.h>
#include <stdio_checked.h>
#include <stdlib.h>
#include <stdlib_checked.h>
#include "health.h"

void addList(ptr<struct List> list, ptr<struct Patient> patient) {
  ptr<struct List> b = NULL;

  while (list != NULL) {
    b = list;
    list = list->forward; }
  
  list = calloc(1, sizeof(struct List));
  list->patient = patient;
  list->forward = NULL;
  list->back = b;
  b->forward = list;
} 

void removeList(ptr<struct List> list, ptr<struct Patient> patient) {
  ptr<struct List>      l1 = NULL, l2 = NULL;
  ptr<struct Patient>   p = list->patient;

  while(p != patient) {
      list = list->forward; 
      p = list->patient;
  }
    
  l1 = list->back;
  l2 = list->forward;
  l1->forward = l2;
  if (list->forward != NULL) {
    l1 = list->forward;
    l2 = list->back;
    l1->back = l2;
  }
  /*free(list);*/
}
     
