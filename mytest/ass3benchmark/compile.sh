#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Please eneter the option"
	exit -1
fi
llvm_lib=/home/arijit/llvm/llvm/Debug+Asserts/lib

if [ $1 = "jpeg" ]
then
	rm hook.bc
	rm one.bc
	rm output.bc
	clang  -emit-llvm -c hook.c -o hook.bc
	llvm-link jpeg-6a.bc hook.bc -o one.bc
     opt -load $llvm_lib/Ass3.so -mem <one.bc> output.bc
     lli output.bc -dct int -progressive -opt ./testimg.ppm > jpeg.out

elif [ $1 = "mpeg" ]
then
	rm hook.bc
	rm one.bc
	rm output.bc
	clang -S -emit-llvm -c hook.c -o hook.bc
	llvm-link hook.bc mpeg2dec.bc -o one.bc
	opt -load $llvm_lib/Ass3.so -mem <one.bc> output.bc
	lli output.bc -b  ./input_base_4CIF_96bps.mpg -r -f -o0 ./tmp%d

elif [ $1 = "clean" ]
then 
	rm hook.bc
	rm one.bc
	rm output.bc
	rm -rf *.out
	rm -rf *.ll
fi
