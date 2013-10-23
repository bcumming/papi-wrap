rm -r *.o a.out
echo "building cplusplus.o"
CC cplusplus.cpp -c
echo "building a.out"
cc test.c cplusplus.o -lcray-c++-rts

