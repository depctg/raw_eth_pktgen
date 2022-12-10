; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

declare ptr @malloc(i64)

declare void @free(ptr)

define void @primal_update_flow(ptr %0, ptr %1, ptr %2) !dbg !3 {
  br label %4, !dbg !7

4:                                                ; preds = %16, %3
  %5 = phi ptr [ %18, %16 ], [ %0, %3 ]
  %6 = icmp ne ptr %5, %2, !dbg !9
  br i1 %6, label %7, label %19, !dbg !10

7:                                                ; preds = %4
  %8 = phi ptr [ %5, %4 ]
  %9 = getelementptr %_Lowered_node, ptr %8, i32 0, i32 1, !dbg !11
  %10 = load i32, ptr %9, align 4, !dbg !12
  %11 = icmp ne i32 %10, 0, !dbg !13
  br i1 %11, label %12, label %14, !dbg !14

12:                                               ; preds = %7
  %13 = getelementptr %_Lowered_node, ptr %8, i32 0, i32 10, !dbg !15
  store i64 0, ptr %13, align 8, !dbg !16
  br label %16, !dbg !17

14:                                               ; preds = %7
  %15 = getelementptr %_Lowered_node, ptr %8, i32 0, i32 10, !dbg !18
  store i64 1, ptr %15, align 8, !dbg !19
  br label %16, !dbg !20

16:                                               ; preds = %12, %14
  %17 = getelementptr %_Lowered_node, ptr %8, i32 0, i32 3, !dbg !21
  %18 = load ptr, ptr %17, align 8, !dbg !22
  br label %4, !dbg !23

19:                                               ; preds = %4
  br label %20, !dbg !24

20:                                               ; preds = %32, %19
  %21 = phi ptr [ %34, %32 ], [ %1, %19 ]
  %22 = icmp ne ptr %21, %2, !dbg !25
  br i1 %22, label %23, label %35, !dbg !26

23:                                               ; preds = %20
  %24 = phi ptr [ %21, %20 ]
  %25 = getelementptr %_Lowered_node, ptr %24, i32 0, i32 1, !dbg !27
  %26 = load i32, ptr %25, align 4, !dbg !28
  %27 = icmp ne i32 %26, 0, !dbg !29
  br i1 %27, label %28, label %30, !dbg !30

28:                                               ; preds = %23
  %29 = getelementptr %_Lowered_node, ptr %24, i32 0, i32 10, !dbg !31
  store i64 1, ptr %29, align 8, !dbg !32
  br label %32, !dbg !33

30:                                               ; preds = %23
  %31 = getelementptr %_Lowered_node, ptr %24, i32 0, i32 10, !dbg !34
  store i64 0, ptr %31, align 8, !dbg !35
  br label %32, !dbg !36

32:                                               ; preds = %28, %30
  %33 = getelementptr %_Lowered_node, ptr %24, i32 0, i32 3, !dbg !37
  %34 = load ptr, ptr %33, align 8, !dbg !38
  br label %20, !dbg !39

35:                                               ; preds = %20
  ret void, !dbg !40
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "primal_update_flow", linkageName: "primal_update_flow", scope: null, file: !4, line: 2, type: !5, scopeLine: 2, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/pflowup.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 6, column: 5, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 8, column: 10, scope: !8)
!10 = !DILocation(line: 9, column: 5, scope: !8)
!11 = !DILocation(line: 11, column: 10, scope: !8)
!12 = !DILocation(line: 12, column: 10, scope: !8)
!13 = !DILocation(line: 13, column: 10, scope: !8)
!14 = !DILocation(line: 14, column: 5, scope: !8)
!15 = !DILocation(line: 16, column: 10, scope: !8)
!16 = !DILocation(line: 17, column: 5, scope: !8)
!17 = !DILocation(line: 18, column: 5, scope: !8)
!18 = !DILocation(line: 20, column: 11, scope: !8)
!19 = !DILocation(line: 21, column: 5, scope: !8)
!20 = !DILocation(line: 22, column: 5, scope: !8)
!21 = !DILocation(line: 24, column: 11, scope: !8)
!22 = !DILocation(line: 25, column: 11, scope: !8)
!23 = !DILocation(line: 26, column: 5, scope: !8)
!24 = !DILocation(line: 28, column: 5, scope: !8)
!25 = !DILocation(line: 30, column: 11, scope: !8)
!26 = !DILocation(line: 31, column: 5, scope: !8)
!27 = !DILocation(line: 33, column: 11, scope: !8)
!28 = !DILocation(line: 34, column: 11, scope: !8)
!29 = !DILocation(line: 35, column: 11, scope: !8)
!30 = !DILocation(line: 36, column: 5, scope: !8)
!31 = !DILocation(line: 38, column: 11, scope: !8)
!32 = !DILocation(line: 39, column: 5, scope: !8)
!33 = !DILocation(line: 40, column: 5, scope: !8)
!34 = !DILocation(line: 42, column: 11, scope: !8)
!35 = !DILocation(line: 43, column: 5, scope: !8)
!36 = !DILocation(line: 44, column: 5, scope: !8)
!37 = !DILocation(line: 46, column: 11, scope: !8)
!38 = !DILocation(line: 47, column: 11, scope: !8)
!39 = !DILocation(line: 48, column: 5, scope: !8)
!40 = !DILocation(line: 50, column: 5, scope: !8)
