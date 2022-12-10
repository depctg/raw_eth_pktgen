; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.arc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access(i128)

declare ptr @cache_access_mut(i128)

declare i128 @cache_request(i64)

define void @insert_new_arc(ptr %0, i64 %1, ptr %2, ptr %3, i64 %4, i64 %5) !dbg !3 {
  %7 = getelementptr %struct.arc, ptr %0, i64 %1, !dbg !7
  %8 = ptrtoint ptr %7 to i64, !dbg !9
  %9 = call i128 @cache_request(i64 %8), !dbg !10
  %10 = call ptr @cache_access_mut(i128 %9), !dbg !11
  %11 = getelementptr %struct.arc, ptr %10, i32 0, i32 1, !dbg !12
  store ptr %2, ptr %11, align 8, !dbg !13
  %12 = getelementptr %struct.arc, ptr %10, i32 0, i32 2, !dbg !14
  store ptr %3, ptr %12, align 8, !dbg !15
  %13 = getelementptr %struct.arc, ptr %10, i32 0, i32 7, !dbg !16
  store i64 %4, ptr %13, align 8, !dbg !17
  %14 = getelementptr %struct.arc, ptr %10, i32 0, i32 0, !dbg !18
  store i64 %4, ptr %14, align 8, !dbg !19
  %15 = getelementptr %struct.arc, ptr %10, i32 0, i32 6, !dbg !20
  store i64 %5, ptr %15, align 8, !dbg !21
  %16 = add i64 %1, 1, !dbg !22
  %17 = sdiv i64 %16, 2, !dbg !23
  %18 = getelementptr %struct.arc, ptr %0, i64 %17, !dbg !24
  %19 = getelementptr %struct.arc, ptr %18, i32 -1, !dbg !25
  %20 = ptrtoint ptr %19 to i64, !dbg !26
  %21 = call i128 @cache_request(i64 %20), !dbg !27
  %22 = call ptr @cache_access(i128 %21), !dbg !28
  br label %23, !dbg !29

23:                                               ; preds = %81, %6
  %24 = phi i64 [ %79, %81 ], [ %16, %6 ]
  %25 = phi ptr [ %80, %81 ], [ %22, %6 ]
  %26 = add i64 %24, -1, !dbg !30
  %27 = icmp ne i64 %26, 0, !dbg !31
  br i1 %27, label %28, label %76, !dbg !32

28:                                               ; preds = %23
  %29 = getelementptr %struct.arc, ptr %25, i32 0, i32 6, !dbg !33
  %30 = load i64, ptr %29, align 8, !dbg !34
  %31 = icmp sgt i64 %5, %30, !dbg !35
  br i1 %31, label %32, label %71, !dbg !36

32:                                               ; preds = %28
  %33 = getelementptr %struct.arc, ptr %0, i64 %24, !dbg !37
  %34 = getelementptr %struct.arc, ptr %33, i32 -1, !dbg !38
  %35 = ptrtoint ptr %34 to i64, !dbg !39
  %36 = call i128 @cache_request(i64 %35), !dbg !40
  %37 = call ptr @cache_access_mut(i128 %36), !dbg !41
  %38 = sdiv i64 %24, 2, !dbg !42
  %39 = getelementptr %struct.arc, ptr %0, i64 %38, !dbg !43
  %40 = getelementptr %struct.arc, ptr %39, i32 -1, !dbg !44
  %41 = ptrtoint ptr %40 to i64, !dbg !45
  %42 = call i128 @cache_request(i64 %41), !dbg !46
  %43 = call ptr @cache_access(i128 %42), !dbg !47
  %44 = getelementptr %struct.arc, ptr %37, i32 0, i32 1, !dbg !48
  %45 = getelementptr %struct.arc, ptr %43, i32 0, i32 1, !dbg !49
  %46 = load ptr, ptr %45, align 8, !dbg !50
  store ptr %46, ptr %44, align 8, !dbg !51
  %47 = getelementptr %struct.arc, ptr %37, i32 0, i32 2, !dbg !52
  %48 = getelementptr %struct.arc, ptr %43, i32 0, i32 2, !dbg !53
  %49 = load ptr, ptr %48, align 8, !dbg !54
  store ptr %49, ptr %47, align 8, !dbg !55
  %50 = getelementptr %struct.arc, ptr %37, i32 0, i32 0, !dbg !56
  %51 = getelementptr %struct.arc, ptr %43, i32 0, i32 0, !dbg !57
  %52 = load i64, ptr %51, align 8, !dbg !58
  store i64 %52, ptr %50, align 8, !dbg !59
  %53 = getelementptr %struct.arc, ptr %37, i32 0, i32 7, !dbg !60
  %54 = load i64, ptr %51, align 8, !dbg !61
  store i64 %54, ptr %53, align 8, !dbg !62
  %55 = getelementptr %struct.arc, ptr %37, i32 0, i32 6, !dbg !63
  %56 = getelementptr %struct.arc, ptr %43, i32 0, i32 6, !dbg !64
  %57 = load i64, ptr %56, align 8, !dbg !65
  store i64 %57, ptr %55, align 8, !dbg !66
  %58 = call i128 @cache_request(i64 %41), !dbg !67
  %59 = call ptr @cache_access_mut(i128 %58), !dbg !68
  %60 = getelementptr %struct.arc, ptr %59, i32 0, i32 1, !dbg !69
  store ptr %2, ptr %60, align 8, !dbg !70
  %61 = getelementptr %struct.arc, ptr %59, i32 0, i32 2, !dbg !71
  store ptr %3, ptr %61, align 8, !dbg !72
  %62 = getelementptr %struct.arc, ptr %59, i32 0, i32 0, !dbg !73
  store i64 %4, ptr %62, align 8, !dbg !74
  %63 = getelementptr %struct.arc, ptr %59, i32 0, i32 7, !dbg !75
  store i64 %4, ptr %63, align 8, !dbg !76
  %64 = getelementptr %struct.arc, ptr %59, i32 0, i32 6, !dbg !77
  store i64 %5, ptr %64, align 8, !dbg !78
  %65 = sdiv i64 %38, 2, !dbg !79
  %66 = getelementptr %struct.arc, ptr %0, i64 %65, !dbg !80
  %67 = getelementptr %struct.arc, ptr %66, i32 -1, !dbg !81
  %68 = ptrtoint ptr %67 to i64, !dbg !82
  %69 = call i128 @cache_request(i64 %68), !dbg !83
  %70 = call ptr @cache_access(i128 %69), !dbg !84
  br label %72, !dbg !85

71:                                               ; preds = %28
  br label %72, !dbg !86

72:                                               ; preds = %32, %71
  %73 = phi i64 [ %24, %71 ], [ %38, %32 ]
  %74 = phi ptr [ %25, %71 ], [ %70, %32 ]
  br label %75, !dbg !87

75:                                               ; preds = %72
  br label %77, !dbg !88

76:                                               ; preds = %23
  br label %77, !dbg !89

77:                                               ; preds = %75, %76
  %78 = phi i1 [ false, %76 ], [ %31, %75 ]
  %79 = phi i64 [ %24, %76 ], [ %73, %75 ]
  %80 = phi ptr [ %25, %76 ], [ %74, %75 ]
  br label %81, !dbg !90

81:                                               ; preds = %77
  br i1 %78, label %23, label %82, !dbg !91

82:                                               ; preds = %81
  ret void, !dbg !92
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "insert_new_arc", linkageName: "insert_new_arc", scope: null, file: !4, line: 5, type: !5, scopeLine: 5, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/insert_new_arc.o_llvm.mlir", directory: "/home/wuklab/spec-eval/429.mcf/src")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 11, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 12, column: 10, scope: !8)
!10 = !DILocation(line: 13, column: 10, scope: !8)
!11 = !DILocation(line: 14, column: 10, scope: !8)
!12 = !DILocation(line: 16, column: 11, scope: !8)
!13 = !DILocation(line: 17, column: 5, scope: !8)
!14 = !DILocation(line: 18, column: 11, scope: !8)
!15 = !DILocation(line: 19, column: 5, scope: !8)
!16 = !DILocation(line: 20, column: 11, scope: !8)
!17 = !DILocation(line: 21, column: 5, scope: !8)
!18 = !DILocation(line: 22, column: 11, scope: !8)
!19 = !DILocation(line: 23, column: 5, scope: !8)
!20 = !DILocation(line: 24, column: 11, scope: !8)
!21 = !DILocation(line: 25, column: 5, scope: !8)
!22 = !DILocation(line: 26, column: 11, scope: !8)
!23 = !DILocation(line: 27, column: 11, scope: !8)
!24 = !DILocation(line: 28, column: 11, scope: !8)
!25 = !DILocation(line: 29, column: 11, scope: !8)
!26 = !DILocation(line: 30, column: 11, scope: !8)
!27 = !DILocation(line: 31, column: 11, scope: !8)
!28 = !DILocation(line: 32, column: 11, scope: !8)
!29 = !DILocation(line: 34, column: 5, scope: !8)
!30 = !DILocation(line: 36, column: 11, scope: !8)
!31 = !DILocation(line: 37, column: 11, scope: !8)
!32 = !DILocation(line: 38, column: 5, scope: !8)
!33 = !DILocation(line: 40, column: 11, scope: !8)
!34 = !DILocation(line: 41, column: 11, scope: !8)
!35 = !DILocation(line: 42, column: 11, scope: !8)
!36 = !DILocation(line: 43, column: 5, scope: !8)
!37 = !DILocation(line: 45, column: 11, scope: !8)
!38 = !DILocation(line: 46, column: 11, scope: !8)
!39 = !DILocation(line: 47, column: 11, scope: !8)
!40 = !DILocation(line: 48, column: 11, scope: !8)
!41 = !DILocation(line: 49, column: 11, scope: !8)
!42 = !DILocation(line: 51, column: 11, scope: !8)
!43 = !DILocation(line: 52, column: 11, scope: !8)
!44 = !DILocation(line: 53, column: 11, scope: !8)
!45 = !DILocation(line: 54, column: 11, scope: !8)
!46 = !DILocation(line: 55, column: 11, scope: !8)
!47 = !DILocation(line: 56, column: 11, scope: !8)
!48 = !DILocation(line: 58, column: 11, scope: !8)
!49 = !DILocation(line: 59, column: 11, scope: !8)
!50 = !DILocation(line: 60, column: 11, scope: !8)
!51 = !DILocation(line: 61, column: 5, scope: !8)
!52 = !DILocation(line: 62, column: 11, scope: !8)
!53 = !DILocation(line: 63, column: 11, scope: !8)
!54 = !DILocation(line: 64, column: 11, scope: !8)
!55 = !DILocation(line: 65, column: 5, scope: !8)
!56 = !DILocation(line: 66, column: 11, scope: !8)
!57 = !DILocation(line: 67, column: 11, scope: !8)
!58 = !DILocation(line: 68, column: 11, scope: !8)
!59 = !DILocation(line: 69, column: 5, scope: !8)
!60 = !DILocation(line: 70, column: 11, scope: !8)
!61 = !DILocation(line: 71, column: 11, scope: !8)
!62 = !DILocation(line: 72, column: 5, scope: !8)
!63 = !DILocation(line: 73, column: 11, scope: !8)
!64 = !DILocation(line: 74, column: 11, scope: !8)
!65 = !DILocation(line: 75, column: 11, scope: !8)
!66 = !DILocation(line: 76, column: 5, scope: !8)
!67 = !DILocation(line: 77, column: 11, scope: !8)
!68 = !DILocation(line: 78, column: 11, scope: !8)
!69 = !DILocation(line: 80, column: 11, scope: !8)
!70 = !DILocation(line: 81, column: 5, scope: !8)
!71 = !DILocation(line: 82, column: 11, scope: !8)
!72 = !DILocation(line: 83, column: 5, scope: !8)
!73 = !DILocation(line: 84, column: 11, scope: !8)
!74 = !DILocation(line: 85, column: 5, scope: !8)
!75 = !DILocation(line: 86, column: 11, scope: !8)
!76 = !DILocation(line: 87, column: 5, scope: !8)
!77 = !DILocation(line: 88, column: 11, scope: !8)
!78 = !DILocation(line: 89, column: 5, scope: !8)
!79 = !DILocation(line: 90, column: 11, scope: !8)
!80 = !DILocation(line: 91, column: 11, scope: !8)
!81 = !DILocation(line: 92, column: 11, scope: !8)
!82 = !DILocation(line: 93, column: 11, scope: !8)
!83 = !DILocation(line: 94, column: 11, scope: !8)
!84 = !DILocation(line: 95, column: 11, scope: !8)
!85 = !DILocation(line: 97, column: 5, scope: !8)
!86 = !DILocation(line: 99, column: 5, scope: !8)
!87 = !DILocation(line: 101, column: 5, scope: !8)
!88 = !DILocation(line: 103, column: 5, scope: !8)
!89 = !DILocation(line: 105, column: 5, scope: !8)
!90 = !DILocation(line: 107, column: 5, scope: !8)
!91 = !DILocation(line: 109, column: 5, scope: !8)
!92 = !DILocation(line: 111, column: 5, scope: !8)
