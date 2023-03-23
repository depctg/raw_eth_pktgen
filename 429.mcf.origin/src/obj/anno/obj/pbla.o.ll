; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

declare ptr @malloc(i64)

declare void @free(ptr)

define ptr @primal_iminus(ptr %0, ptr %1, ptr %2, ptr %3, ptr %4) !dbg !3 {
  br label %6, !dbg !7

6:                                                ; preds = %102, %5
  %7 = phi ptr [ %99, %102 ], [ %3, %5 ]
  %8 = phi ptr [ %100, %102 ], [ %2, %5 ]
  %9 = phi ptr [ %101, %102 ], [ null, %5 ]
  %10 = icmp ne ptr %8, %7, !dbg !9
  br i1 %10, label %11, label %103, !dbg !10

11:                                               ; preds = %6
  %12 = phi ptr [ %8, %6 ]
  %13 = phi ptr [ %9, %6 ]
  %14 = phi ptr [ %7, %6 ]
  %15 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 11, !dbg !11
  %16 = load i64, ptr %15, align 8, !dbg !12
  %17 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 11, !dbg !13
  %18 = load i64, ptr %17, align 8, !dbg !14
  %19 = icmp slt i64 %16, %18, !dbg !15
  br i1 %19, label %20, label %59, !dbg !16

20:                                               ; preds = %11
  %21 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 1, !dbg !17
  %22 = load i32, ptr %21, align 4, !dbg !18
  %23 = icmp ne i32 %22, 0, !dbg !19
  br i1 %23, label %24, label %33, !dbg !20

24:                                               ; preds = %20
  %25 = load i64, ptr %0, align 8, !dbg !21
  %26 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 10, !dbg !22
  %27 = load i64, ptr %26, align 8, !dbg !23
  %28 = icmp sgt i64 %25, %27, !dbg !24
  %29 = select i1 %28, ptr %12, ptr %13, !dbg !25
  br i1 %28, label %30, label %32, !dbg !26

30:                                               ; preds = %24
  %31 = load i64, ptr %26, align 8, !dbg !27
  store i64 %31, ptr %0, align 8, !dbg !28
  store i64 0, ptr %1, align 8, !dbg !29
  br label %32, !dbg !30

32:                                               ; preds = %30, %24
  br label %54, !dbg !31

33:                                               ; preds = %20
  %34 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 3, !dbg !32
  %35 = load ptr, ptr %34, align 8, !dbg !33
  %36 = getelementptr %_Lowered_node, ptr %35, i32 0, i32 3, !dbg !34
  %37 = load ptr, ptr %36, align 8, !dbg !35
  %38 = icmp ne ptr %37, null, !dbg !36
  br i1 %38, label %39, label %50, !dbg !37

39:                                               ; preds = %33
  %40 = load i64, ptr %0, align 8, !dbg !38
  %41 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 10, !dbg !39
  %42 = load i64, ptr %41, align 8, !dbg !40
  %43 = sub i64 1, %42, !dbg !41
  %44 = icmp sgt i64 %40, %43, !dbg !42
  %45 = select i1 %44, ptr %12, ptr %13, !dbg !43
  br i1 %44, label %46, label %49, !dbg !44

46:                                               ; preds = %39
  %47 = load i64, ptr %41, align 8, !dbg !45
  %48 = sub i64 1, %47, !dbg !46
  store i64 %48, ptr %0, align 8, !dbg !47
  store i64 0, ptr %1, align 8, !dbg !48
  br label %49, !dbg !49

49:                                               ; preds = %46, %39
  br label %51, !dbg !50

50:                                               ; preds = %33
  br label %51, !dbg !51

51:                                               ; preds = %49, %50
  %52 = phi ptr [ %13, %50 ], [ %45, %49 ]
  br label %53, !dbg !52

53:                                               ; preds = %51
  br label %54, !dbg !53

54:                                               ; preds = %32, %53
  %55 = phi ptr [ %52, %53 ], [ %29, %32 ]
  br label %56, !dbg !54

56:                                               ; preds = %54
  %57 = getelementptr %_Lowered_node, ptr %12, i32 0, i32 3, !dbg !55
  %58 = load ptr, ptr %57, align 8, !dbg !56
  br label %98, !dbg !57

59:                                               ; preds = %11
  %60 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 1, !dbg !58
  %61 = load i32, ptr %60, align 4, !dbg !59
  %62 = icmp ne i32 %61, 0, !dbg !60
  br i1 %62, label %63, label %84, !dbg !61

63:                                               ; preds = %59
  %64 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 3, !dbg !62
  %65 = load ptr, ptr %64, align 8, !dbg !63
  %66 = getelementptr %_Lowered_node, ptr %65, i32 0, i32 3, !dbg !64
  %67 = load ptr, ptr %66, align 8, !dbg !65
  %68 = icmp ne ptr %67, null, !dbg !66
  br i1 %68, label %69, label %80, !dbg !67

69:                                               ; preds = %63
  %70 = load i64, ptr %0, align 8, !dbg !68
  %71 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 10, !dbg !69
  %72 = load i64, ptr %71, align 8, !dbg !70
  %73 = sub i64 1, %72, !dbg !71
  %74 = icmp sge i64 %70, %73, !dbg !72
  %75 = select i1 %74, ptr %14, ptr %13, !dbg !73
  br i1 %74, label %76, label %79, !dbg !74

76:                                               ; preds = %69
  %77 = load i64, ptr %71, align 8, !dbg !75
  %78 = sub i64 1, %77, !dbg !76
  store i64 %78, ptr %0, align 8, !dbg !77
  store i64 1, ptr %1, align 8, !dbg !78
  br label %79, !dbg !79

79:                                               ; preds = %76, %69
  br label %81, !dbg !80

80:                                               ; preds = %63
  br label %81, !dbg !81

81:                                               ; preds = %79, %80
  %82 = phi ptr [ %13, %80 ], [ %75, %79 ]
  br label %83, !dbg !82

83:                                               ; preds = %81
  br label %93, !dbg !83

84:                                               ; preds = %59
  %85 = load i64, ptr %0, align 8, !dbg !84
  %86 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 10, !dbg !85
  %87 = load i64, ptr %86, align 8, !dbg !86
  %88 = icmp sge i64 %85, %87, !dbg !87
  %89 = select i1 %88, ptr %14, ptr %13, !dbg !88
  br i1 %88, label %90, label %92, !dbg !89

90:                                               ; preds = %84
  %91 = load i64, ptr %86, align 8, !dbg !90
  store i64 %91, ptr %0, align 8, !dbg !91
  store i64 1, ptr %1, align 8, !dbg !92
  br label %92, !dbg !93

92:                                               ; preds = %90, %84
  br label %93, !dbg !94

93:                                               ; preds = %83, %92
  %94 = phi ptr [ %89, %92 ], [ %82, %83 ]
  br label %95, !dbg !95

95:                                               ; preds = %93
  %96 = getelementptr %_Lowered_node, ptr %14, i32 0, i32 3, !dbg !96
  %97 = load ptr, ptr %96, align 8, !dbg !97
  br label %98, !dbg !98

98:                                               ; preds = %56, %95
  %99 = phi ptr [ %97, %95 ], [ %14, %56 ]
  %100 = phi ptr [ %12, %95 ], [ %58, %56 ]
  %101 = phi ptr [ %94, %95 ], [ %55, %56 ]
  br label %102, !dbg !99

102:                                              ; preds = %98
  br label %6, !dbg !100

103:                                              ; preds = %6
  store ptr %8, ptr %4, align 8, !dbg !101
  ret ptr %9, !dbg !102
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "primal_iminus", linkageName: "primal_iminus", scope: null, file: !4, line: 2, type: !5, scopeLine: 2, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/pbla.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf.origin/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 7, column: 5, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 9, column: 10, scope: !8)
!10 = !DILocation(line: 10, column: 5, scope: !8)
!11 = !DILocation(line: 12, column: 11, scope: !8)
!12 = !DILocation(line: 13, column: 11, scope: !8)
!13 = !DILocation(line: 14, column: 11, scope: !8)
!14 = !DILocation(line: 15, column: 11, scope: !8)
!15 = !DILocation(line: 16, column: 11, scope: !8)
!16 = !DILocation(line: 17, column: 5, scope: !8)
!17 = !DILocation(line: 19, column: 11, scope: !8)
!18 = !DILocation(line: 20, column: 11, scope: !8)
!19 = !DILocation(line: 21, column: 11, scope: !8)
!20 = !DILocation(line: 22, column: 5, scope: !8)
!21 = !DILocation(line: 24, column: 11, scope: !8)
!22 = !DILocation(line: 25, column: 11, scope: !8)
!23 = !DILocation(line: 26, column: 11, scope: !8)
!24 = !DILocation(line: 27, column: 11, scope: !8)
!25 = !DILocation(line: 28, column: 11, scope: !8)
!26 = !DILocation(line: 29, column: 5, scope: !8)
!27 = !DILocation(line: 31, column: 11, scope: !8)
!28 = !DILocation(line: 32, column: 5, scope: !8)
!29 = !DILocation(line: 33, column: 5, scope: !8)
!30 = !DILocation(line: 34, column: 5, scope: !8)
!31 = !DILocation(line: 36, column: 5, scope: !8)
!32 = !DILocation(line: 38, column: 11, scope: !8)
!33 = !DILocation(line: 39, column: 11, scope: !8)
!34 = !DILocation(line: 40, column: 11, scope: !8)
!35 = !DILocation(line: 41, column: 11, scope: !8)
!36 = !DILocation(line: 42, column: 11, scope: !8)
!37 = !DILocation(line: 43, column: 5, scope: !8)
!38 = !DILocation(line: 45, column: 11, scope: !8)
!39 = !DILocation(line: 46, column: 11, scope: !8)
!40 = !DILocation(line: 47, column: 11, scope: !8)
!41 = !DILocation(line: 48, column: 11, scope: !8)
!42 = !DILocation(line: 49, column: 11, scope: !8)
!43 = !DILocation(line: 50, column: 11, scope: !8)
!44 = !DILocation(line: 51, column: 5, scope: !8)
!45 = !DILocation(line: 53, column: 11, scope: !8)
!46 = !DILocation(line: 54, column: 11, scope: !8)
!47 = !DILocation(line: 55, column: 5, scope: !8)
!48 = !DILocation(line: 56, column: 5, scope: !8)
!49 = !DILocation(line: 57, column: 5, scope: !8)
!50 = !DILocation(line: 59, column: 5, scope: !8)
!51 = !DILocation(line: 61, column: 5, scope: !8)
!52 = !DILocation(line: 63, column: 5, scope: !8)
!53 = !DILocation(line: 65, column: 5, scope: !8)
!54 = !DILocation(line: 67, column: 5, scope: !8)
!55 = !DILocation(line: 69, column: 11, scope: !8)
!56 = !DILocation(line: 70, column: 11, scope: !8)
!57 = !DILocation(line: 71, column: 5, scope: !8)
!58 = !DILocation(line: 73, column: 11, scope: !8)
!59 = !DILocation(line: 74, column: 11, scope: !8)
!60 = !DILocation(line: 75, column: 11, scope: !8)
!61 = !DILocation(line: 76, column: 5, scope: !8)
!62 = !DILocation(line: 78, column: 11, scope: !8)
!63 = !DILocation(line: 79, column: 11, scope: !8)
!64 = !DILocation(line: 80, column: 11, scope: !8)
!65 = !DILocation(line: 81, column: 11, scope: !8)
!66 = !DILocation(line: 82, column: 11, scope: !8)
!67 = !DILocation(line: 83, column: 5, scope: !8)
!68 = !DILocation(line: 85, column: 11, scope: !8)
!69 = !DILocation(line: 86, column: 11, scope: !8)
!70 = !DILocation(line: 87, column: 11, scope: !8)
!71 = !DILocation(line: 88, column: 11, scope: !8)
!72 = !DILocation(line: 89, column: 11, scope: !8)
!73 = !DILocation(line: 90, column: 11, scope: !8)
!74 = !DILocation(line: 91, column: 5, scope: !8)
!75 = !DILocation(line: 93, column: 11, scope: !8)
!76 = !DILocation(line: 94, column: 11, scope: !8)
!77 = !DILocation(line: 95, column: 5, scope: !8)
!78 = !DILocation(line: 96, column: 5, scope: !8)
!79 = !DILocation(line: 97, column: 5, scope: !8)
!80 = !DILocation(line: 99, column: 5, scope: !8)
!81 = !DILocation(line: 101, column: 5, scope: !8)
!82 = !DILocation(line: 103, column: 5, scope: !8)
!83 = !DILocation(line: 105, column: 5, scope: !8)
!84 = !DILocation(line: 107, column: 11, scope: !8)
!85 = !DILocation(line: 108, column: 11, scope: !8)
!86 = !DILocation(line: 109, column: 11, scope: !8)
!87 = !DILocation(line: 110, column: 11, scope: !8)
!88 = !DILocation(line: 111, column: 11, scope: !8)
!89 = !DILocation(line: 112, column: 5, scope: !8)
!90 = !DILocation(line: 114, column: 11, scope: !8)
!91 = !DILocation(line: 115, column: 5, scope: !8)
!92 = !DILocation(line: 116, column: 5, scope: !8)
!93 = !DILocation(line: 117, column: 5, scope: !8)
!94 = !DILocation(line: 119, column: 5, scope: !8)
!95 = !DILocation(line: 121, column: 5, scope: !8)
!96 = !DILocation(line: 123, column: 11, scope: !8)
!97 = !DILocation(line: 124, column: 11, scope: !8)
!98 = !DILocation(line: 125, column: 5, scope: !8)
!99 = !DILocation(line: 127, column: 5, scope: !8)
!100 = !DILocation(line: 129, column: 5, scope: !8)
!101 = !DILocation(line: 131, column: 5, scope: !8)
!102 = !DILocation(line: 132, column: 5, scope: !8)
