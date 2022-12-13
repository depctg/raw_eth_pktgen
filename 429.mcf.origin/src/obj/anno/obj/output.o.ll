; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

@str3 = internal constant [4 x i8] c"%d\0A\00"
@str2 = internal constant [5 x i8] c"***\0A\00"
@str1 = internal constant [4 x i8] c"()\0A\00"
@str0 = internal constant [2 x i8] c"w\00"

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access(i128)

declare i128 @cache_request(i64)

declare i32 @fclose(ptr)

declare i32 @fprintf(ptr, ptr, ...)

declare ptr @fopen(ptr, ptr)

define i64 @write_circulations(ptr %0, ptr %1) !dbg !3 {
  %3 = getelementptr %_Lowered_network, ptr %1, i32 0, i32 24, !dbg !7
  %4 = load ptr, ptr %3, align 8, !dbg !9
  %5 = getelementptr %_Lowered_network, ptr %1, i32 0, i32 7, !dbg !10
  %6 = load i64, ptr %5, align 8, !dbg !11
  %7 = sub i64 0, %6, !dbg !12
  %8 = getelementptr %_Lowered_rarc, ptr %4, i64 %7, !dbg !13
  %9 = call ptr @fopen(ptr %0, ptr @str0), !dbg !14
  %10 = icmp eq ptr %9, null, !dbg !15
  br i1 %10, label %11, label %12, !dbg !16

11:                                               ; preds = %2
  br label %135, !dbg !17

12:                                               ; preds = %2
  call void @refresh_neighbour_lists(ptr %1), !dbg !18
  %13 = getelementptr %_Lowered_network, ptr %1, i32 0, i32 21, !dbg !19
  %14 = load ptr, ptr %13, align 8, !dbg !20
  %15 = getelementptr %_Lowered_network, ptr %1, i32 0, i32 2, !dbg !21
  %16 = load i64, ptr %15, align 8, !dbg !22
  %17 = getelementptr %_Lowered_node, ptr %14, i64 %16, !dbg !23
  %18 = getelementptr %_Lowered_node, ptr %17, i32 0, i32 7, !dbg !24
  %19 = load ptr, ptr %18, align 8, !dbg !25
  br label %20, !dbg !26

20:                                               ; preds = %133, %12
  %21 = phi i1 [ %120, %133 ], [ true, %12 ]
  %22 = phi i64 [ %121, %133 ], [ undef, %12 ]
  %23 = phi i1 [ %122, %133 ], [ true, %12 ]
  %24 = phi ptr [ %132, %133 ], [ %19, %12 ]
  %25 = icmp ne ptr %24, null, !dbg !27
  %26 = and i1 %25, %23, !dbg !28
  br i1 %26, label %27, label %134, !dbg !29

27:                                               ; preds = %20
  %28 = phi i1 [ %21, %20 ]
  %29 = phi i64 [ %22, %20 ]
  %30 = phi ptr [ %24, %20 ]
  %31 = getelementptr %_Lowered_rarc, ptr %30, i32 0, i32 6, !dbg !30
  %32 = ptrtoint ptr %31 to i64, !dbg !31
  %33 = call i128 @cache_request(i64 %32), !dbg !32
  %34 = call ptr @cache_access(i128 %33), !dbg !33
  %35 = load i64, ptr %34, align 8, !dbg !34
  %36 = icmp ne i64 %35, 0, !dbg !35
  br i1 %36, label %37, label %118, !dbg !36

37:                                               ; preds = %27
  %38 = call i32 (ptr, ptr, ...) @fprintf(ptr %9, ptr @str1), !dbg !37
  %39 = getelementptr %_Lowered_network, ptr %1, i32 0, i32 3, !dbg !38
  br label %40, !dbg !39

40:                                               ; preds = %116, %37
  %41 = phi i1 [ %99, %116 ], [ %28, %37 ]
  %42 = phi i64 [ %100, %116 ], [ %29, %37 ]
  %43 = phi i1 [ %101, %116 ], [ true, %37 ]
  %44 = phi i1 [ %98, %116 ], [ true, %37 ]
  %45 = phi ptr [ %115, %116 ], [ %30, %37 ]
  %46 = icmp ne ptr %45, null, !dbg !40
  %47 = and i1 %46, %44, !dbg !41
  br i1 %47, label %48, label %117, !dbg !42

48:                                               ; preds = %40
  %49 = phi i1 [ %41, %40 ]
  %50 = phi i64 [ %42, %40 ]
  %51 = phi i1 [ %43, %40 ]
  %52 = phi ptr [ %45, %40 ]
  %53 = icmp uge ptr %52, %8, !dbg !43
  br i1 %53, label %54, label %56, !dbg !44

54:                                               ; preds = %48
  %55 = call i32 (ptr, ptr, ...) @fprintf(ptr %9, ptr @str2), !dbg !45
  br label %56, !dbg !46

56:                                               ; preds = %54, %48
  %57 = getelementptr %_Lowered_rarc, ptr %52, i32 0, i32 2, !dbg !47
  %58 = ptrtoint ptr %57 to i64, !dbg !48
  %59 = call i128 @cache_request(i64 %58), !dbg !49
  %60 = call ptr @cache_access(i128 %59), !dbg !50
  %61 = load ptr, ptr %60, align 8, !dbg !51
  %62 = getelementptr %_Lowered_node, ptr %61, i32 0, i32 12, !dbg !52
  %63 = load i32, ptr %62, align 4, !dbg !53
  %64 = sub i32 0, %63, !dbg !54
  %65 = call i32 (ptr, ptr, ...) @fprintf(ptr %9, ptr @str3, i32 %64), !dbg !55
  %66 = call i128 @cache_request(i64 %58), !dbg !56
  %67 = call ptr @cache_access(i128 %66), !dbg !57
  %68 = load ptr, ptr %67, align 8, !dbg !58
  %69 = load i64, ptr %39, align 8, !dbg !59
  %70 = getelementptr %_Lowered_node, ptr %68, i64 %69, !dbg !60
  %71 = getelementptr %_Lowered_node, ptr %70, i32 0, i32 7, !dbg !61
  %72 = load ptr, ptr %71, align 8, !dbg !62
  br label %73, !dbg !63

73:                                               ; preds = %95, %56
  %74 = phi i1 [ %85, %95 ], [ true, %56 ]
  %75 = phi ptr [ %94, %95 ], [ %72, %56 ]
  %76 = icmp ne ptr %75, null, !dbg !64
  %77 = and i1 %76, %74, !dbg !65
  br i1 %77, label %78, label %96, !dbg !66

78:                                               ; preds = %73
  %79 = phi ptr [ %75, %73 ]
  %80 = getelementptr %_Lowered_rarc, ptr %79, i32 0, i32 6, !dbg !67
  %81 = ptrtoint ptr %80 to i64, !dbg !68
  %82 = call i128 @cache_request(i64 %81), !dbg !69
  %83 = call ptr @cache_access(i128 %82), !dbg !70
  %84 = load i64, ptr %83, align 8, !dbg !71
  %85 = icmp eq i64 %84, 0, !dbg !72
  br i1 %85, label %86, label %92, !dbg !73

86:                                               ; preds = %78
  %87 = getelementptr %_Lowered_rarc, ptr %79, i32 0, i32 4, !dbg !74
  %88 = ptrtoint ptr %87 to i64, !dbg !75
  %89 = call i128 @cache_request(i64 %88), !dbg !76
  %90 = call ptr @cache_access(i128 %89), !dbg !77
  %91 = load ptr, ptr %90, align 8, !dbg !78
  br label %93, !dbg !79

92:                                               ; preds = %78
  br label %93, !dbg !80

93:                                               ; preds = %86, %92
  %94 = phi ptr [ %79, %92 ], [ %91, %86 ]
  br label %95, !dbg !81

95:                                               ; preds = %93
  br label %73, !dbg !82

96:                                               ; preds = %73
  %97 = icmp eq ptr %75, null, !dbg !83
  %98 = xor i1 %97, true, !dbg !84
  %99 = and i1 %98, %49, !dbg !85
  %100 = select i1 %97, i64 -1, i64 %50, !dbg !86
  %101 = and i1 %98, %51, !dbg !87
  br i1 %97, label %102, label %104, !dbg !88

102:                                              ; preds = %96
  %103 = call i32 @fclose(ptr %9), !dbg !89
  br label %114, !dbg !90

104:                                              ; preds = %96
  %105 = getelementptr %_Lowered_rarc, ptr %75, i32 0, i32 2, !dbg !91
  %106 = ptrtoint ptr %105 to i64, !dbg !92
  %107 = call i128 @cache_request(i64 %106), !dbg !93
  %108 = call ptr @cache_access(i128 %107), !dbg !94
  %109 = load ptr, ptr %108, align 8, !dbg !95
  %110 = getelementptr %_Lowered_node, ptr %109, i32 0, i32 12, !dbg !96
  %111 = load i32, ptr %110, align 4, !dbg !97
  %112 = icmp ne i32 %111, 0, !dbg !98
  %113 = select i1 %112, ptr %75, ptr null, !dbg !99
  br label %114, !dbg !100

114:                                              ; preds = %102, %104
  %115 = phi ptr [ %113, %104 ], [ %52, %102 ]
  br label %116, !dbg !101

116:                                              ; preds = %114
  br label %40, !dbg !102

117:                                              ; preds = %40
  br label %119, !dbg !103

118:                                              ; preds = %27
  br label %119, !dbg !104

119:                                              ; preds = %117, %118
  %120 = phi i1 [ %28, %118 ], [ %41, %117 ]
  %121 = phi i64 [ %29, %118 ], [ %42, %117 ]
  %122 = phi i1 [ true, %118 ], [ %43, %117 ]
  br label %123, !dbg !105

123:                                              ; preds = %119
  br i1 %122, label %124, label %130, !dbg !106

124:                                              ; preds = %123
  %125 = getelementptr %_Lowered_rarc, ptr %30, i32 0, i32 4, !dbg !107
  %126 = ptrtoint ptr %125 to i64, !dbg !108
  %127 = call i128 @cache_request(i64 %126), !dbg !109
  %128 = call ptr @cache_access(i128 %127), !dbg !110
  %129 = load ptr, ptr %128, align 8, !dbg !111
  br label %131, !dbg !112

130:                                              ; preds = %123
  br label %131, !dbg !113

131:                                              ; preds = %124, %130
  %132 = phi ptr [ %30, %130 ], [ %129, %124 ]
  br label %133, !dbg !114

133:                                              ; preds = %131
  br label %20, !dbg !115

134:                                              ; preds = %20
  br label %135, !dbg !116

135:                                              ; preds = %11, %134
  %136 = phi i1 [ %21, %134 ], [ false, %11 ]
  %137 = phi i64 [ %22, %134 ], [ -1, %11 ]
  br label %138, !dbg !117

138:                                              ; preds = %135
  %139 = select i1 %136, i64 0, i64 %137, !dbg !118
  br i1 %136, label %140, label %142, !dbg !119

140:                                              ; preds = %138
  %141 = call i32 @fclose(ptr %9), !dbg !120
  br label %142, !dbg !121

142:                                              ; preds = %140, %138
  ret i64 %139, !dbg !122
}

declare void @refresh_neighbour_lists(ptr)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "write_circulations", linkageName: "write_circulations", scope: null, file: !4, line: 11, type: !5, scopeLine: 11, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/output.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf.origin/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 18, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 19, column: 10, scope: !8)
!10 = !DILocation(line: 20, column: 10, scope: !8)
!11 = !DILocation(line: 21, column: 10, scope: !8)
!12 = !DILocation(line: 22, column: 11, scope: !8)
!13 = !DILocation(line: 23, column: 11, scope: !8)
!14 = !DILocation(line: 26, column: 11, scope: !8)
!15 = !DILocation(line: 29, column: 11, scope: !8)
!16 = !DILocation(line: 30, column: 5, scope: !8)
!17 = !DILocation(line: 32, column: 5, scope: !8)
!18 = !DILocation(line: 34, column: 5, scope: !8)
!19 = !DILocation(line: 35, column: 11, scope: !8)
!20 = !DILocation(line: 36, column: 11, scope: !8)
!21 = !DILocation(line: 37, column: 11, scope: !8)
!22 = !DILocation(line: 38, column: 11, scope: !8)
!23 = !DILocation(line: 39, column: 11, scope: !8)
!24 = !DILocation(line: 40, column: 11, scope: !8)
!25 = !DILocation(line: 41, column: 11, scope: !8)
!26 = !DILocation(line: 43, column: 5, scope: !8)
!27 = !DILocation(line: 45, column: 11, scope: !8)
!28 = !DILocation(line: 46, column: 11, scope: !8)
!29 = !DILocation(line: 47, column: 5, scope: !8)
!30 = !DILocation(line: 49, column: 11, scope: !8)
!31 = !DILocation(line: 50, column: 11, scope: !8)
!32 = !DILocation(line: 51, column: 11, scope: !8)
!33 = !DILocation(line: 52, column: 11, scope: !8)
!34 = !DILocation(line: 54, column: 11, scope: !8)
!35 = !DILocation(line: 55, column: 11, scope: !8)
!36 = !DILocation(line: 56, column: 5, scope: !8)
!37 = !DILocation(line: 60, column: 11, scope: !8)
!38 = !DILocation(line: 63, column: 11, scope: !8)
!39 = !DILocation(line: 64, column: 5, scope: !8)
!40 = !DILocation(line: 66, column: 11, scope: !8)
!41 = !DILocation(line: 67, column: 11, scope: !8)
!42 = !DILocation(line: 68, column: 5, scope: !8)
!43 = !DILocation(line: 70, column: 11, scope: !8)
!44 = !DILocation(line: 71, column: 5, scope: !8)
!45 = !DILocation(line: 75, column: 11, scope: !8)
!46 = !DILocation(line: 76, column: 5, scope: !8)
!47 = !DILocation(line: 78, column: 11, scope: !8)
!48 = !DILocation(line: 79, column: 11, scope: !8)
!49 = !DILocation(line: 80, column: 11, scope: !8)
!50 = !DILocation(line: 81, column: 11, scope: !8)
!51 = !DILocation(line: 83, column: 11, scope: !8)
!52 = !DILocation(line: 84, column: 11, scope: !8)
!53 = !DILocation(line: 85, column: 11, scope: !8)
!54 = !DILocation(line: 86, column: 11, scope: !8)
!55 = !DILocation(line: 87, column: 11, scope: !8)
!56 = !DILocation(line: 88, column: 11, scope: !8)
!57 = !DILocation(line: 89, column: 11, scope: !8)
!58 = !DILocation(line: 91, column: 11, scope: !8)
!59 = !DILocation(line: 92, column: 11, scope: !8)
!60 = !DILocation(line: 93, column: 11, scope: !8)
!61 = !DILocation(line: 94, column: 11, scope: !8)
!62 = !DILocation(line: 95, column: 11, scope: !8)
!63 = !DILocation(line: 96, column: 5, scope: !8)
!64 = !DILocation(line: 98, column: 11, scope: !8)
!65 = !DILocation(line: 99, column: 11, scope: !8)
!66 = !DILocation(line: 100, column: 5, scope: !8)
!67 = !DILocation(line: 102, column: 11, scope: !8)
!68 = !DILocation(line: 103, column: 11, scope: !8)
!69 = !DILocation(line: 104, column: 11, scope: !8)
!70 = !DILocation(line: 105, column: 11, scope: !8)
!71 = !DILocation(line: 107, column: 11, scope: !8)
!72 = !DILocation(line: 108, column: 11, scope: !8)
!73 = !DILocation(line: 109, column: 5, scope: !8)
!74 = !DILocation(line: 111, column: 11, scope: !8)
!75 = !DILocation(line: 112, column: 11, scope: !8)
!76 = !DILocation(line: 113, column: 11, scope: !8)
!77 = !DILocation(line: 114, column: 11, scope: !8)
!78 = !DILocation(line: 116, column: 11, scope: !8)
!79 = !DILocation(line: 117, column: 5, scope: !8)
!80 = !DILocation(line: 119, column: 5, scope: !8)
!81 = !DILocation(line: 121, column: 5, scope: !8)
!82 = !DILocation(line: 123, column: 5, scope: !8)
!83 = !DILocation(line: 125, column: 12, scope: !8)
!84 = !DILocation(line: 126, column: 12, scope: !8)
!85 = !DILocation(line: 127, column: 12, scope: !8)
!86 = !DILocation(line: 128, column: 12, scope: !8)
!87 = !DILocation(line: 129, column: 12, scope: !8)
!88 = !DILocation(line: 130, column: 5, scope: !8)
!89 = !DILocation(line: 132, column: 12, scope: !8)
!90 = !DILocation(line: 133, column: 5, scope: !8)
!91 = !DILocation(line: 135, column: 12, scope: !8)
!92 = !DILocation(line: 136, column: 12, scope: !8)
!93 = !DILocation(line: 137, column: 12, scope: !8)
!94 = !DILocation(line: 138, column: 12, scope: !8)
!95 = !DILocation(line: 140, column: 12, scope: !8)
!96 = !DILocation(line: 141, column: 12, scope: !8)
!97 = !DILocation(line: 142, column: 12, scope: !8)
!98 = !DILocation(line: 143, column: 12, scope: !8)
!99 = !DILocation(line: 144, column: 12, scope: !8)
!100 = !DILocation(line: 145, column: 5, scope: !8)
!101 = !DILocation(line: 147, column: 5, scope: !8)
!102 = !DILocation(line: 149, column: 5, scope: !8)
!103 = !DILocation(line: 151, column: 5, scope: !8)
!104 = !DILocation(line: 153, column: 5, scope: !8)
!105 = !DILocation(line: 155, column: 5, scope: !8)
!106 = !DILocation(line: 157, column: 5, scope: !8)
!107 = !DILocation(line: 159, column: 12, scope: !8)
!108 = !DILocation(line: 160, column: 12, scope: !8)
!109 = !DILocation(line: 161, column: 12, scope: !8)
!110 = !DILocation(line: 162, column: 12, scope: !8)
!111 = !DILocation(line: 164, column: 12, scope: !8)
!112 = !DILocation(line: 165, column: 5, scope: !8)
!113 = !DILocation(line: 167, column: 5, scope: !8)
!114 = !DILocation(line: 169, column: 5, scope: !8)
!115 = !DILocation(line: 171, column: 5, scope: !8)
!116 = !DILocation(line: 173, column: 5, scope: !8)
!117 = !DILocation(line: 175, column: 5, scope: !8)
!118 = !DILocation(line: 177, column: 12, scope: !8)
!119 = !DILocation(line: 178, column: 5, scope: !8)
!120 = !DILocation(line: 180, column: 12, scope: !8)
!121 = !DILocation(line: 181, column: 5, scope: !8)
!122 = !DILocation(line: 183, column: 5, scope: !8)
