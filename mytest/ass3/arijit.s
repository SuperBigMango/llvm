; ModuleID = 'arijit.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [33 x i8] c"Memory allocation is successful\0A\00", align 1
@.str1 = private unnamed_addr constant [14 x i8] c"Unsuccessful\0A\00", align 1

define i32 @main() nounwind uwtable {
entry:
  %retval = alloca i32, align 4
  %mem = alloca i8*, align 8
  store i32 0, i32* %retval
  %call = call i8* @malloc(i64 10)
  store i8* %call, i8** %mem, align 8
  %0 = load i8** %mem, align 8
  %tobool = icmp ne i8* %0, null
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([33 x i8]* @.str, i32 0, i32 0))
  %1 = load i8** %mem, align 8
  %call2 = call i32 (...)* @free(i8* %1)
  br label %if.end

if.else:                                          ; preds = %entry
  %call3 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([14 x i8]* @.str1, i32 0, i32 0))
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0
}

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i32 @free(...)
