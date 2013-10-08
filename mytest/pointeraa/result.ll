; ModuleID = 'result.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() nounwind uwtable {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32*, align 8
  %b = alloca i32*, align 8
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %retval
  store i32* null, i32** %a, align 8
  store i32* inttoptr (i64 20 to i32*), i32** %b, align 8
  %0 = load i32** %a, align 8
  %1 = load i32* %0, align 4
  %2 = load i32* inttoptr (i64 20 to i32*), align 4
  %add = add nsw i32 %1, %2
  %add1 = add nsw i32 %add, 30
  store i32 %add1, i32* %c, align 4
  %3 = load i32** %a, align 8
  %4 = load i32* %3, align 4
  %5 = load i32** %b, align 8
  %6 = load i32* %5, align 4
  %add2 = add nsw i32 %4, %6
  %add3 = add nsw i32 %add2, 40
  store i32 %add3, i32* %d, align 4
  ret i32 0
}
