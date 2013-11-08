rm *.bc
clang -emit-llvm -c arijit.c -o arijit.bc
clang -emit-llvm -c hook.c -o hook.bc
llvm-link *bc -o one.bc
#opt -load ../../../../../Debug+Asserts/lib/Ass3.so -mem <one.bc> output.bc

