/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
#pragma CHECKED_SCOPE ON
int nonlocals=0;
void compute_nodes(register ptr<node_t> nodelist)
{
  register int i;
  register ptr<node_t> localnode = NULL;
  
  for (; nodelist; ) {
    register double cur_value;
    register int from_count ;
    register _Ptr<double>  other_value = NULL;
    register double coeff;
    register double value;
    /*register double *coeffs;*/
    /*register node_t **from_nodes;*/
    
    localnode = nodelist;
    cur_value = *localnode->value;
    from_count = localnode->from_count-1;
    for (i=0; i < from_count; i+=2) {
      other_value = localnode->from_values[i];
      coeff = localnode->coeffs[i];

      if (other_value)
        value = *other_value;
      else
        value = 0;
      
      cur_value -= coeff*value;
      other_value = localnode->from_values[i+1];
      coeff = localnode->coeffs[i];
      
      if (other_value)
        value = *other_value;
      else
        value = 0;
      
      cur_value -= coeff*value;
      
      /*chatting("from %d, coeff %f, value %f\n",count,coeff,value);*/
    }

    if (i==from_count)  {
      other_value = localnode->from_values[i];
      coeff = localnode->coeffs[i];
      
      if (other_value)
        value = *other_value;
      else
        value = 0;
      
      cur_value -= coeff*value;
    }
    *localnode->value = cur_value;
    nodelist = localnode->next;
  } /* for */
}
