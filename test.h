#ifndef TEST_H
#define TEST_H 

#include <stdio.h> 

#define VVVV_TEST_UNIT_START(str) \
   printf("Testing: %s\n",str)

#define  VVVV_TEST_UNIT_RESULT(x) \
   if (x) { \
      printf("  passed.\n"); \
   } else { \
      printf("  failed.\n"); \
   } 

#endif /* TEST_H */

