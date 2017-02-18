#/bin/bash

bin=nt_evnt_cmd_prsr_test.bin;

rm $bin;

gcc \
-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/CoreMIDI.framework/Headers \
-DOSX \
cmd_q.c \
emalloc.c \
nt_evnt.c \
nt_evnt_cmd.c \
cmd_prsr.c \
nt_evnt_cmd_prsr.c \
nt_evnt_cmd_prsr_test.c \
nt_evnt_lst.c \
nt_evnt_sq.c \
rm_nt_evnt_cmd.c \
-o $bin \
-I. \
-g \
;

./nt_evnt_cmd_prsr_test.bin;
