#ifndef TMSTMP_H
#define TMSTMP_H 

#ifdef OSX
#include <CoreMIDI.h> 
typedef MIDITimeStamp vvvv_tmstmp_t;
#define VVVV_TMSTMP_SCANF_STR "%f"
#endif 

#endif /* TMSTMP_H */
