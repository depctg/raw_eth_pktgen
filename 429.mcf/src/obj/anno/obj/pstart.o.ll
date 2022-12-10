; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access_mut(i128)

declare ptr @cache_access(i128)

declare i128 @cache_request(i64)

define i64 @primal_start_artificial(ptr %0) !dbg !3 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !7
  %3 = load ptr, ptr %2, align 8, !dbg !9
  %4 = getelementptr %_Lowered_node, ptr %3, i32 1, !dbg !10
  %5 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 6, !dbg !11
  store ptr null, ptr %5, align 8, !dbg !12
  %6 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 3, !dbg !13
  store ptr null, ptr %6, align 8, !dbg !14
  %7 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 2, !dbg !15
  store ptr %4, ptr %7, align 8, !dbg !16
  %8 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 4, !dbg !17
  store ptr null, ptr %8, align 8, !dbg !18
  %9 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 5, !dbg !19
  store ptr null, ptr %9, align 8, !dbg !20
  %10 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 11, !dbg !21
  %11 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 2, !dbg !22
  %12 = load i64, ptr %11, align 8, !dbg !23
  %13 = add i64 %12, 1, !dbg !24
  store i64 %13, ptr %10, align 8, !dbg !25
  %14 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 1, !dbg !26
  store i32 0, ptr %14, align 4, !dbg !27
  %15 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 0, !dbg !28
  store i64 -100000000, ptr %15, align 8, !dbg !29
  %16 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 10, !dbg !30
  store i64 0, ptr %16, align 8, !dbg !31
  %17 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !32
  %18 = load ptr, ptr %17, align 8, !dbg !33
  %19 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !34
  %20 = load ptr, ptr %19, align 8, !dbg !35
  br label %21, !dbg !36

21:                                               ; preds = %35, %1
  %22 = phi ptr [ %36, %35 ], [ %20, %1 ]
  %23 = icmp ne ptr %22, %18, !dbg !37
  br i1 %23, label %24, label %37, !dbg !38

24:                                               ; preds = %21
  %25 = phi ptr [ %22, %21 ]
  %26 = getelementptr %_Lowered_rarc, ptr %25, i32 0, i32 3, !dbg !39
  %27 = ptrtoint ptr %26 to i64, !dbg !40
  %28 = call i128 @cache_request(i64 %27), !dbg !41
  %29 = call ptr @cache_access(i128 %28), !dbg !42
  %30 = load i32, ptr %29, align 4, !dbg !43
  %31 = icmp ne i32 %30, -1, !dbg !44
  br i1 %31, label %32, label %35, !dbg !45

32:                                               ; preds = %24
  %33 = call i128 @cache_request(i64 %27), !dbg !46
  %34 = call ptr @cache_access_mut(i128 %33), !dbg !47
  store i32 1, ptr %34, align 4, !dbg !48
  br label %35, !dbg !49

35:                                               ; preds = %32, %24
  %36 = getelementptr %_Lowered_rarc, ptr %25, i32 1, !dbg !50
  br label %21, !dbg !51

37:                                               ; preds = %21
  %38 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 25, !dbg !52
  %39 = load ptr, ptr %38, align 8, !dbg !53
  %40 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !54
  %41 = load ptr, ptr %40, align 8, !dbg !55
  br label %42, !dbg !56

42:                                               ; preds = %46, %37
  %43 = phi ptr [ %76, %46 ], [ %39, %37 ]
  %44 = phi ptr [ %53, %46 ], [ %4, %37 ]
  %45 = icmp ne ptr %44, %41, !dbg !57
  br i1 %45, label %46, label %77, !dbg !58

46:                                               ; preds = %42
  %47 = phi ptr [ %44, %42 ]
  %48 = phi ptr [ %43, %42 ]
  %49 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 6, !dbg !59
  store ptr %48, ptr %49, align 8, !dbg !60
  %50 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 3, !dbg !61
  store ptr %3, ptr %50, align 8, !dbg !62
  %51 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 2, !dbg !63
  store ptr null, ptr %51, align 8, !dbg !64
  %52 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 4, !dbg !65
  %53 = getelementptr %_Lowered_node, ptr %47, i32 1, !dbg !66
  store ptr %53, ptr %52, align 8, !dbg !67
  %54 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 5, !dbg !68
  %55 = getelementptr %_Lowered_node, ptr %47, i32 -1, !dbg !69
  store ptr %55, ptr %54, align 8, !dbg !70
  %56 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 11, !dbg !71
  store i64 1, ptr %56, align 8, !dbg !72
  %57 = getelementptr %_Lowered_rarc, ptr %48, i32 0, i32 0, !dbg !73
  %58 = ptrtoint ptr %57 to i64, !dbg !74
  %59 = call i128 @cache_request(i64 %58), !dbg !75
  %60 = call ptr @cache_access_mut(i128 %59), !dbg !76
  store i64 100000000, ptr %60, align 8, !dbg !77
  %61 = getelementptr %_Lowered_rarc, ptr %48, i32 0, i32 3, !dbg !78
  %62 = ptrtoint ptr %61 to i64, !dbg !79
  %63 = call i128 @cache_request(i64 %62), !dbg !80
  %64 = call ptr @cache_access_mut(i128 %63), !dbg !81
  store i32 0, ptr %64, align 4, !dbg !82
  %65 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 1, !dbg !83
  store i32 1, ptr %65, align 4, !dbg !84
  %66 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 0, !dbg !85
  store i64 0, ptr %66, align 8, !dbg !86
  %67 = getelementptr %_Lowered_rarc, ptr %48, i32 0, i32 1, !dbg !87
  %68 = ptrtoint ptr %67 to i64, !dbg !88
  %69 = call i128 @cache_request(i64 %68), !dbg !89
  %70 = call ptr @cache_access_mut(i128 %69), !dbg !90
  store ptr %47, ptr %70, align 8, !dbg !91
  %71 = getelementptr %_Lowered_rarc, ptr %48, i32 0, i32 2, !dbg !92
  %72 = ptrtoint ptr %71 to i64, !dbg !93
  %73 = call i128 @cache_request(i64 %72), !dbg !94
  %74 = call ptr @cache_access_mut(i128 %73), !dbg !95
  store ptr %3, ptr %74, align 8, !dbg !96
  %75 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 10, !dbg !97
  store i64 0, ptr %75, align 8, !dbg !98
  %76 = getelementptr %_Lowered_rarc, ptr %48, i32 1, !dbg !99
  br label %42, !dbg !100

77:                                               ; preds = %42
  %78 = getelementptr %_Lowered_node, ptr %44, i32 -1, !dbg !101
  %79 = getelementptr %_Lowered_node, ptr %78, i32 0, i32 4, !dbg !102
  store ptr null, ptr %79, align 8, !dbg !103
  %80 = getelementptr %_Lowered_node, ptr %4, i32 0, i32 5, !dbg !104
  store ptr null, ptr %80, align 8, !dbg !105
  ret i64 0, !dbg !106
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "primal_start_artificial", linkageName: "primal_start_artificial", scope: null, file: !4, line: 5, type: !5, scopeLine: 5, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/pstart.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 13, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 14, column: 10, scope: !8)
!10 = !DILocation(line: 15, column: 10, scope: !8)
!11 = !DILocation(line: 16, column: 11, scope: !8)
!12 = !DILocation(line: 18, column: 5, scope: !8)
!13 = !DILocation(line: 19, column: 11, scope: !8)
!14 = !DILocation(line: 21, column: 5, scope: !8)
!15 = !DILocation(line: 22, column: 11, scope: !8)
!16 = !DILocation(line: 23, column: 5, scope: !8)
!17 = !DILocation(line: 24, column: 11, scope: !8)
!18 = !DILocation(line: 25, column: 5, scope: !8)
!19 = !DILocation(line: 26, column: 11, scope: !8)
!20 = !DILocation(line: 27, column: 5, scope: !8)
!21 = !DILocation(line: 28, column: 11, scope: !8)
!22 = !DILocation(line: 29, column: 11, scope: !8)
!23 = !DILocation(line: 30, column: 11, scope: !8)
!24 = !DILocation(line: 31, column: 11, scope: !8)
!25 = !DILocation(line: 32, column: 5, scope: !8)
!26 = !DILocation(line: 33, column: 11, scope: !8)
!27 = !DILocation(line: 34, column: 5, scope: !8)
!28 = !DILocation(line: 35, column: 11, scope: !8)
!29 = !DILocation(line: 36, column: 5, scope: !8)
!30 = !DILocation(line: 37, column: 11, scope: !8)
!31 = !DILocation(line: 38, column: 5, scope: !8)
!32 = !DILocation(line: 39, column: 11, scope: !8)
!33 = !DILocation(line: 40, column: 11, scope: !8)
!34 = !DILocation(line: 41, column: 11, scope: !8)
!35 = !DILocation(line: 42, column: 11, scope: !8)
!36 = !DILocation(line: 43, column: 5, scope: !8)
!37 = !DILocation(line: 45, column: 11, scope: !8)
!38 = !DILocation(line: 46, column: 5, scope: !8)
!39 = !DILocation(line: 48, column: 11, scope: !8)
!40 = !DILocation(line: 49, column: 11, scope: !8)
!41 = !DILocation(line: 50, column: 11, scope: !8)
!42 = !DILocation(line: 51, column: 11, scope: !8)
!43 = !DILocation(line: 53, column: 11, scope: !8)
!44 = !DILocation(line: 54, column: 11, scope: !8)
!45 = !DILocation(line: 55, column: 5, scope: !8)
!46 = !DILocation(line: 57, column: 11, scope: !8)
!47 = !DILocation(line: 58, column: 11, scope: !8)
!48 = !DILocation(line: 60, column: 5, scope: !8)
!49 = !DILocation(line: 61, column: 5, scope: !8)
!50 = !DILocation(line: 63, column: 11, scope: !8)
!51 = !DILocation(line: 64, column: 5, scope: !8)
!52 = !DILocation(line: 66, column: 11, scope: !8)
!53 = !DILocation(line: 67, column: 11, scope: !8)
!54 = !DILocation(line: 68, column: 11, scope: !8)
!55 = !DILocation(line: 69, column: 11, scope: !8)
!56 = !DILocation(line: 70, column: 5, scope: !8)
!57 = !DILocation(line: 72, column: 11, scope: !8)
!58 = !DILocation(line: 73, column: 5, scope: !8)
!59 = !DILocation(line: 75, column: 11, scope: !8)
!60 = !DILocation(line: 76, column: 5, scope: !8)
!61 = !DILocation(line: 77, column: 11, scope: !8)
!62 = !DILocation(line: 78, column: 5, scope: !8)
!63 = !DILocation(line: 79, column: 11, scope: !8)
!64 = !DILocation(line: 80, column: 5, scope: !8)
!65 = !DILocation(line: 81, column: 11, scope: !8)
!66 = !DILocation(line: 82, column: 11, scope: !8)
!67 = !DILocation(line: 83, column: 5, scope: !8)
!68 = !DILocation(line: 84, column: 11, scope: !8)
!69 = !DILocation(line: 85, column: 11, scope: !8)
!70 = !DILocation(line: 86, column: 5, scope: !8)
!71 = !DILocation(line: 87, column: 11, scope: !8)
!72 = !DILocation(line: 88, column: 5, scope: !8)
!73 = !DILocation(line: 89, column: 11, scope: !8)
!74 = !DILocation(line: 90, column: 11, scope: !8)
!75 = !DILocation(line: 91, column: 11, scope: !8)
!76 = !DILocation(line: 92, column: 11, scope: !8)
!77 = !DILocation(line: 94, column: 5, scope: !8)
!78 = !DILocation(line: 95, column: 11, scope: !8)
!79 = !DILocation(line: 96, column: 11, scope: !8)
!80 = !DILocation(line: 97, column: 11, scope: !8)
!81 = !DILocation(line: 98, column: 11, scope: !8)
!82 = !DILocation(line: 100, column: 5, scope: !8)
!83 = !DILocation(line: 101, column: 11, scope: !8)
!84 = !DILocation(line: 102, column: 5, scope: !8)
!85 = !DILocation(line: 103, column: 11, scope: !8)
!86 = !DILocation(line: 104, column: 5, scope: !8)
!87 = !DILocation(line: 105, column: 11, scope: !8)
!88 = !DILocation(line: 106, column: 11, scope: !8)
!89 = !DILocation(line: 107, column: 11, scope: !8)
!90 = !DILocation(line: 108, column: 11, scope: !8)
!91 = !DILocation(line: 110, column: 5, scope: !8)
!92 = !DILocation(line: 111, column: 11, scope: !8)
!93 = !DILocation(line: 112, column: 11, scope: !8)
!94 = !DILocation(line: 113, column: 11, scope: !8)
!95 = !DILocation(line: 114, column: 11, scope: !8)
!96 = !DILocation(line: 116, column: 5, scope: !8)
!97 = !DILocation(line: 117, column: 11, scope: !8)
!98 = !DILocation(line: 118, column: 5, scope: !8)
!99 = !DILocation(line: 119, column: 11, scope: !8)
!100 = !DILocation(line: 120, column: 5, scope: !8)
!101 = !DILocation(line: 122, column: 11, scope: !8)
!102 = !DILocation(line: 123, column: 11, scope: !8)
!103 = !DILocation(line: 124, column: 5, scope: !8)
!104 = !DILocation(line: 125, column: 11, scope: !8)
!105 = !DILocation(line: 126, column: 5, scope: !8)
!106 = !DILocation(line: 127, column: 5, scope: !8)
