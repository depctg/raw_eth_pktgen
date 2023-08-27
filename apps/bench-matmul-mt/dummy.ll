; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Pack = type { ptr, ptr }

@str0 = internal constant [4 x i8] c"%d\0A\00"

declare ptr @malloc(i64)

declare void @free(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() !dbg !3 {
  %1 = alloca [5 x i32], i64 1, align 4, !dbg !7
  %2 = alloca [5 x i32], i64 1, align 4, !dbg !9
  %3 = getelementptr [5 x i32], ptr %2, i32 0, i32 0, !dbg !10
  store i32 1, ptr %3, align 4, !dbg !11
  %4 = getelementptr [5 x i32], ptr %2, i32 0, i32 1, !dbg !12
  store i32 2, ptr %4, align 4, !dbg !13
  %5 = getelementptr [5 x i32], ptr %2, i32 0, i32 2, !dbg !14
  store i32 3, ptr %5, align 4, !dbg !15
  %6 = getelementptr [5 x i32], ptr %2, i32 0, i32 3, !dbg !16
  store i32 4, ptr %6, align 4, !dbg !17
  %7 = getelementptr [5 x i32], ptr %2, i32 0, i32 4, !dbg !18
  store i32 5, ptr %7, align 4, !dbg !19
  %8 = getelementptr [5 x i32], ptr %1, i32 0, i32 0, !dbg !20
  store i32 2, ptr %8, align 4, !dbg !21
  %9 = getelementptr [5 x i32], ptr %1, i32 0, i32 1, !dbg !22
  store i32 3, ptr %9, align 4, !dbg !23
  %10 = getelementptr [5 x i32], ptr %1, i32 0, i32 2, !dbg !24
  store i32 4, ptr %10, align 4, !dbg !25
  %11 = getelementptr [5 x i32], ptr %1, i32 0, i32 3, !dbg !26
  store i32 5, ptr %11, align 4, !dbg !27
  %12 = getelementptr [5 x i32], ptr %1, i32 0, i32 4, !dbg !28
  store i32 6, ptr %12, align 4, !dbg !29
  call void @driver(ptr %3, ptr %8), !dbg !30
  ret i32 0, !dbg !31
}

define void @driver(ptr %0, ptr %1) !dbg !32 {
  %3 = alloca [5 x i64], i64 1, align 8, !dbg !33
  %4 = alloca [5 x %struct.Pack], i64 1, align 8, !dbg !35
  %5 = getelementptr [5 x %struct.Pack], ptr %4, i32 0, i32 0, !dbg !36
  %6 = getelementptr [5 x i64], ptr %3, i32 0, i32 0, !dbg !37
  br label %7, !dbg !38

7:                                                ; preds = %10, %2
  %8 = phi i64 [ %20, %10 ], [ 0, %2 ]
  %9 = icmp slt i64 %8, 5, !dbg !39
  br i1 %9, label %10, label %21, !dbg !40

10:                                               ; preds = %7
  %11 = trunc i64 %8 to i32, !dbg !41
  %12 = getelementptr %struct.Pack, ptr %5, i64 %8, !dbg !42
  %13 = getelementptr %struct.Pack, ptr %12, i32 0, i32 0, !dbg !43
  %14 = getelementptr i32, ptr %0, i32 %11, !dbg !44
  store ptr %14, ptr %13, align 8, !dbg !45
  %15 = getelementptr %struct.Pack, ptr %12, i32 0, i32 1, !dbg !46
  %16 = getelementptr i32, ptr %1, i32 %11, !dbg !47
  store ptr %16, ptr %15, align 8, !dbg !48
  %17 = getelementptr i64, ptr %6, i32 %11, !dbg !49
  %18 = getelementptr %struct.Pack, ptr %5, i32 %11, !dbg !50
  %19 = call i32 @pthread_create(ptr %17, ptr null, ptr @task, ptr %18), !dbg !51
  %20 = add i64 %8, 1, !dbg !52
  br label %7, !dbg !53

21:                                               ; preds = %7
  br label %22, !dbg !54

22:                                               ; preds = %25, %21
  %23 = phi i64 [ %29, %25 ], [ 0, %21 ]
  %24 = icmp slt i64 %23, 5, !dbg !55
  br i1 %24, label %25, label %30, !dbg !56

25:                                               ; preds = %22
  %26 = getelementptr i64, ptr %6, i64 %23, !dbg !57
  %27 = load i64, ptr %26, align 8, !dbg !58
  %28 = call i32 @pthread_join(i64 %27, ptr null), !dbg !59
  %29 = add i64 %23, 1, !dbg !60
  br label %22, !dbg !61

30:                                               ; preds = %22
  ret void, !dbg !62
}

declare i32 @pthread_create(ptr, ptr, ptr, ptr)

define ptr @task(ptr %0) !dbg !63 {
  %2 = getelementptr %struct.Pack, ptr %0, i32 0, i32 0, !dbg !64
  %3 = load ptr, ptr %2, align 8, !dbg !66
  %4 = getelementptr %struct.Pack, ptr %0, i32 0, i32 1, !dbg !67
  %5 = load ptr, ptr %4, align 8, !dbg !68
  call void @foo(ptr %3, ptr %5), !dbg !69
  ret ptr null, !dbg !70
}

declare i32 @pthread_join(i64, ptr)

define void @foo(ptr %0, ptr %1) !dbg !71 {
  %3 = load i32, ptr %0, align 4, !dbg !72
  %4 = load i32, ptr %1, align 4, !dbg !74
  %5 = add i32 %3, %4, !dbg !75
  %6 = call i32 (ptr, ...) @printf(ptr @str0, i32 %5), !dbg !76
  ret void, !dbg !77
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "main", linkageName: "main", scope: null, file: !4, line: 4, type: !5, scopeLine: 4, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "dummy.llvm.mlir", directory: "/users/Zijian/new_rt/apps/bench-matmul-mt")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 13, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 14, column: 10, scope: !8)
!10 = !DILocation(line: 15, column: 11, scope: !8)
!11 = !DILocation(line: 16, column: 5, scope: !8)
!12 = !DILocation(line: 17, column: 11, scope: !8)
!13 = !DILocation(line: 18, column: 5, scope: !8)
!14 = !DILocation(line: 19, column: 11, scope: !8)
!15 = !DILocation(line: 20, column: 5, scope: !8)
!16 = !DILocation(line: 21, column: 11, scope: !8)
!17 = !DILocation(line: 22, column: 5, scope: !8)
!18 = !DILocation(line: 23, column: 11, scope: !8)
!19 = !DILocation(line: 24, column: 5, scope: !8)
!20 = !DILocation(line: 25, column: 11, scope: !8)
!21 = !DILocation(line: 26, column: 5, scope: !8)
!22 = !DILocation(line: 27, column: 11, scope: !8)
!23 = !DILocation(line: 28, column: 5, scope: !8)
!24 = !DILocation(line: 29, column: 11, scope: !8)
!25 = !DILocation(line: 30, column: 5, scope: !8)
!26 = !DILocation(line: 31, column: 11, scope: !8)
!27 = !DILocation(line: 32, column: 5, scope: !8)
!28 = !DILocation(line: 33, column: 11, scope: !8)
!29 = !DILocation(line: 34, column: 5, scope: !8)
!30 = !DILocation(line: 35, column: 5, scope: !8)
!31 = !DILocation(line: 36, column: 5, scope: !8)
!32 = distinct !DISubprogram(name: "driver", linkageName: "driver", scope: null, file: !4, line: 38, type: !5, scopeLine: 38, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!33 = !DILocation(line: 43, column: 10, scope: !34)
!34 = !DILexicalBlockFile(scope: !32, file: !4, discriminator: 0)
!35 = !DILocation(line: 44, column: 10, scope: !34)
!36 = !DILocation(line: 45, column: 10, scope: !34)
!37 = !DILocation(line: 46, column: 10, scope: !34)
!38 = !DILocation(line: 49, column: 5, scope: !34)
!39 = !DILocation(line: 51, column: 11, scope: !34)
!40 = !DILocation(line: 52, column: 5, scope: !34)
!41 = !DILocation(line: 54, column: 11, scope: !34)
!42 = !DILocation(line: 55, column: 11, scope: !34)
!43 = !DILocation(line: 56, column: 11, scope: !34)
!44 = !DILocation(line: 57, column: 11, scope: !34)
!45 = !DILocation(line: 58, column: 5, scope: !34)
!46 = !DILocation(line: 59, column: 11, scope: !34)
!47 = !DILocation(line: 60, column: 11, scope: !34)
!48 = !DILocation(line: 61, column: 5, scope: !34)
!49 = !DILocation(line: 62, column: 11, scope: !34)
!50 = !DILocation(line: 63, column: 11, scope: !34)
!51 = !DILocation(line: 65, column: 11, scope: !34)
!52 = !DILocation(line: 66, column: 11, scope: !34)
!53 = !DILocation(line: 67, column: 5, scope: !34)
!54 = !DILocation(line: 70, column: 5, scope: !34)
!55 = !DILocation(line: 72, column: 11, scope: !34)
!56 = !DILocation(line: 73, column: 5, scope: !34)
!57 = !DILocation(line: 75, column: 11, scope: !34)
!58 = !DILocation(line: 76, column: 11, scope: !34)
!59 = !DILocation(line: 77, column: 11, scope: !34)
!60 = !DILocation(line: 78, column: 11, scope: !34)
!61 = !DILocation(line: 79, column: 5, scope: !34)
!62 = !DILocation(line: 81, column: 5, scope: !34)
!63 = distinct !DISubprogram(name: "task", linkageName: "task", scope: null, file: !4, line: 84, type: !5, scopeLine: 84, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!64 = !DILocation(line: 86, column: 10, scope: !65)
!65 = !DILexicalBlockFile(scope: !63, file: !4, discriminator: 0)
!66 = !DILocation(line: 87, column: 10, scope: !65)
!67 = !DILocation(line: 88, column: 10, scope: !65)
!68 = !DILocation(line: 89, column: 10, scope: !65)
!69 = !DILocation(line: 90, column: 5, scope: !65)
!70 = !DILocation(line: 92, column: 5, scope: !65)
!71 = distinct !DISubprogram(name: "foo", linkageName: "foo", scope: null, file: !4, line: 95, type: !5, scopeLine: 95, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!72 = !DILocation(line: 98, column: 10, scope: !73)
!73 = !DILexicalBlockFile(scope: !71, file: !4, discriminator: 0)
!74 = !DILocation(line: 99, column: 10, scope: !73)
!75 = !DILocation(line: 100, column: 10, scope: !73)
!76 = !DILocation(line: 101, column: 10, scope: !73)
!77 = !DILocation(line: 102, column: 5, scope: !73)
