; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access_mut(i128)

declare ptr @cache_access(i128)

declare i128 @cache_request(i64)

define i64 @primal_net_simplex(ptr %0) !dbg !3 {
  %2 = alloca i64, i64 1, align 8, !dbg !7
  store i64 undef, ptr %2, align 8, !dbg !9
  %3 = alloca ptr, i64 1, align 8, !dbg !10
  %4 = alloca i64, i64 1, align 8, !dbg !11
  store i64 undef, ptr %4, align 8, !dbg !12
  %5 = alloca i64, i64 1, align 8, !dbg !13
  store i64 undef, ptr %5, align 8, !dbg !14
  %6 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !15
  %7 = load ptr, ptr %6, align 8, !dbg !16
  %8 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !17
  %9 = load ptr, ptr %8, align 8, !dbg !18
  %10 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !19
  %11 = load i64, ptr %10, align 8, !dbg !20
  %12 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 27, !dbg !21
  %13 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 28, !dbg !22
  %14 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 29, !dbg !23
  br label %15, !dbg !24

15:                                               ; preds = %148, %1
  %16 = phi i64 [ %145, %148 ], [ undef, %1 ]
  %17 = phi i64 [ %146, %148 ], [ undef, %1 ]
  %18 = phi i64 [ %28, %148 ], [ 0, %1 ]
  %19 = phi i64 [ %147, %148 ], [ undef, %1 ]
  %20 = icmp eq i64 %18, 0, !dbg !25
  br i1 %20, label %21, label %149, !dbg !26

21:                                               ; preds = %15
  %22 = phi i64 [ %16, %15 ]
  %23 = phi i64 [ %17, %15 ]
  %24 = phi i64 [ %18, %15 ]
  %25 = phi i64 [ %19, %15 ]
  %26 = call ptr @primal_bea_mpp(i64 %11, ptr %7, ptr %9, ptr %2), !dbg !27
  %27 = icmp ne ptr %26, null, !dbg !28
  %28 = select i1 %27, i64 %24, i64 1, !dbg !29
  br i1 %27, label %29, label %143, !dbg !30

29:                                               ; preds = %21
  %30 = load i64, ptr %12, align 8, !dbg !31
  %31 = add i64 %30, 1, !dbg !32
  store i64 %31, ptr %12, align 8, !dbg !33
  %32 = load i64, ptr %2, align 8, !dbg !34
  %33 = icmp sgt i64 %32, 0, !dbg !35
  br i1 %33, label %34, label %45, !dbg !36

34:                                               ; preds = %29
  %35 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !37
  %36 = ptrtoint ptr %35 to i64, !dbg !38
  %37 = call i128 @cache_request(i64 %36), !dbg !39
  %38 = call ptr @cache_access(i128 %37), !dbg !40
  %39 = load ptr, ptr %38, align 8, !dbg !41
  %40 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !42
  %41 = ptrtoint ptr %40 to i64, !dbg !43
  %42 = call i128 @cache_request(i64 %41), !dbg !44
  %43 = call ptr @cache_access(i128 %42), !dbg !45
  %44 = load ptr, ptr %43, align 8, !dbg !46
  br label %56, !dbg !47

45:                                               ; preds = %29
  %46 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !48
  %47 = ptrtoint ptr %46 to i64, !dbg !49
  %48 = call i128 @cache_request(i64 %47), !dbg !50
  %49 = call ptr @cache_access(i128 %48), !dbg !51
  %50 = load ptr, ptr %49, align 8, !dbg !52
  %51 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !53
  %52 = ptrtoint ptr %51 to i64, !dbg !54
  %53 = call i128 @cache_request(i64 %52), !dbg !55
  %54 = call ptr @cache_access(i128 %53), !dbg !56
  %55 = load ptr, ptr %54, align 8, !dbg !57
  br label %56, !dbg !58

56:                                               ; preds = %34, %45
  %57 = phi ptr [ %55, %45 ], [ %44, %34 ]
  %58 = phi ptr [ %50, %45 ], [ %39, %34 ]
  br label %59, !dbg !59

59:                                               ; preds = %56
  store i64 1, ptr %5, align 8, !dbg !60
  %60 = call ptr @primal_iminus(ptr %5, ptr %4, ptr %58, ptr %57, ptr %3), !dbg !61
  %61 = icmp eq ptr %60, null, !dbg !62
  br i1 %61, label %62, label %83, !dbg !63

62:                                               ; preds = %59
  %63 = load i64, ptr %13, align 8, !dbg !64
  %64 = add i64 %63, 1, !dbg !65
  store i64 %64, ptr %13, align 8, !dbg !66
  %65 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 3, !dbg !67
  %66 = ptrtoint ptr %65 to i64, !dbg !68
  %67 = call i128 @cache_request(i64 %66), !dbg !69
  %68 = call ptr @cache_access(i128 %67), !dbg !70
  %69 = load i32, ptr %68, align 4, !dbg !71
  %70 = icmp eq i32 %69, 2, !dbg !72
  br i1 %70, label %71, label %74, !dbg !73

71:                                               ; preds = %62
  %72 = call i128 @cache_request(i64 %66), !dbg !74
  %73 = call ptr @cache_access_mut(i128 %72), !dbg !75
  store i32 1, ptr %73, align 4, !dbg !76
  br label %77, !dbg !77

74:                                               ; preds = %62
  %75 = call i128 @cache_request(i64 %66), !dbg !78
  %76 = call ptr @cache_access_mut(i128 %75), !dbg !79
  store i32 2, ptr %76, align 4, !dbg !80
  br label %77, !dbg !81

77:                                               ; preds = %71, %74
  %78 = load i64, ptr %5, align 8, !dbg !82
  %79 = icmp ne i64 %78, 0, !dbg !83
  br i1 %79, label %80, label %82, !dbg !84

80:                                               ; preds = %77
  %81 = load ptr, ptr %3, align 8, !dbg !85
  call void @primal_update_flow(ptr %58, ptr %57, ptr %81), !dbg !86
  br label %82, !dbg !87

82:                                               ; preds = %80, %77
  br label %138, !dbg !88

83:                                               ; preds = %59
  %84 = load i64, ptr %4, align 8, !dbg !89
  %85 = icmp ne i64 %84, 0, !dbg !90
  %86 = select i1 %85, ptr %58, ptr %57, !dbg !91
  %87 = select i1 %85, ptr %57, ptr %58, !dbg !92
  %88 = getelementptr %_Lowered_node, ptr %60, i32 0, i32 3, !dbg !93
  %89 = load ptr, ptr %88, align 8, !dbg !94
  %90 = getelementptr %_Lowered_node, ptr %60, i32 0, i32 6, !dbg !95
  %91 = load ptr, ptr %90, align 8, !dbg !96
  %92 = getelementptr %_Lowered_node, ptr %60, i32 0, i32 1, !dbg !97
  %93 = load i32, ptr %92, align 4, !dbg !98
  %94 = sext i32 %93 to i64, !dbg !99
  %95 = icmp ne i64 %84, %94, !dbg !100
  %96 = select i1 %95, i64 1, i64 2, !dbg !101
  %97 = load i64, ptr %2, align 8, !dbg !102
  %98 = icmp sgt i64 %97, 0, !dbg !103
  br i1 %98, label %99, label %102, !dbg !104

99:                                               ; preds = %83
  %100 = load i64, ptr %5, align 8, !dbg !105
  %101 = sub i64 1, %100, !dbg !106
  br label %104, !dbg !107

102:                                              ; preds = %83
  %103 = load i64, ptr %5, align 8, !dbg !108
  br label %104, !dbg !109

104:                                              ; preds = %99, %102
  %105 = phi i64 [ %103, %102 ], [ %101, %99 ]
  br label %106, !dbg !110

106:                                              ; preds = %104
  %107 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !111
  %108 = ptrtoint ptr %107 to i64, !dbg !112
  %109 = call i128 @cache_request(i64 %108), !dbg !113
  %110 = call ptr @cache_access(i128 %109), !dbg !114
  %111 = load ptr, ptr %110, align 8, !dbg !115
  %112 = icmp eq ptr %111, %87, !dbg !116
  %113 = zext i1 %112 to i64, !dbg !117
  %114 = icmp eq i64 %84, 0, !dbg !118
  %115 = zext i1 %114 to i64, !dbg !119
  %116 = load i64, ptr %5, align 8, !dbg !120
  %117 = load ptr, ptr %3, align 8, !dbg !121
  %118 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 16, !dbg !122
  %119 = load i64, ptr %118, align 8, !dbg !123
  call void @update_tree(i64 %115, i64 %113, i64 %116, i64 %105, ptr %87, ptr %86, ptr %60, ptr %89, ptr %117, ptr %26, i64 %97, i64 %119), !dbg !124
  %120 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 3, !dbg !125
  %121 = ptrtoint ptr %120 to i64, !dbg !126
  %122 = call i128 @cache_request(i64 %121), !dbg !127
  %123 = call ptr @cache_access_mut(i128 %122), !dbg !128
  store i32 0, ptr %123, align 4, !dbg !129
  %124 = getelementptr %_Lowered_rarc, ptr %91, i32 0, i32 3, !dbg !130
  %125 = trunc i64 %96 to i32, !dbg !131
  %126 = ptrtoint ptr %124 to i64, !dbg !132
  %127 = call i128 @cache_request(i64 %126), !dbg !133
  %128 = call ptr @cache_access_mut(i128 %127), !dbg !134
  store i32 %125, ptr %128, align 4, !dbg !135
  %129 = load i64, ptr %12, align 8, !dbg !136
  %130 = add i64 %129, -1, !dbg !137
  %131 = srem i64 %130, 200, !dbg !138
  %132 = icmp eq i64 %131, 0, !dbg !139
  br i1 %132, label %133, label %137, !dbg !140

133:                                              ; preds = %106
  %134 = call i64 @refresh_potential(ptr %0), !dbg !141
  %135 = load i64, ptr %14, align 8, !dbg !142
  %136 = add i64 %135, %134, !dbg !143
  store i64 %136, ptr %14, align 8, !dbg !144
  br label %137, !dbg !145

137:                                              ; preds = %133, %106
  br label %138, !dbg !146

138:                                              ; preds = %82, %137
  %139 = phi i64 [ %96, %137 ], [ %22, %82 ]
  %140 = phi i64 [ %113, %137 ], [ %23, %82 ]
  %141 = phi i64 [ %105, %137 ], [ %25, %82 ]
  br label %142, !dbg !147

142:                                              ; preds = %138
  br label %144, !dbg !148

143:                                              ; preds = %21
  br label %144, !dbg !149

144:                                              ; preds = %142, %143
  %145 = phi i64 [ %22, %143 ], [ %139, %142 ]
  %146 = phi i64 [ %23, %143 ], [ %140, %142 ]
  %147 = phi i64 [ %25, %143 ], [ %141, %142 ]
  br label %148, !dbg !150

148:                                              ; preds = %144
  br label %15, !dbg !151

149:                                              ; preds = %15
  %150 = call i64 @refresh_potential(ptr %0), !dbg !152
  %151 = load i64, ptr %14, align 8, !dbg !153
  %152 = add i64 %151, %150, !dbg !154
  store i64 %152, ptr %14, align 8, !dbg !155
  %153 = call i64 @primal_feasible(ptr %0), !dbg !156
  %154 = call i64 @dual_feasible(ptr %0), !dbg !157
  ret i64 0, !dbg !158
}

declare ptr @primal_bea_mpp(i64, ptr, ptr, ptr)

declare ptr @primal_iminus(ptr, ptr, ptr, ptr, ptr)

declare void @primal_update_flow(ptr, ptr, ptr)

declare void @update_tree(i64, i64, i64, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64)

declare i64 @refresh_potential(ptr)

declare i64 @primal_feasible(ptr)

declare i64 @dual_feasible(ptr)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "primal_net_simplex", linkageName: "primal_net_simplex", scope: null, file: !4, line: 5, type: !5, scopeLine: 5, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/psimplex.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf.origin/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 14, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 16, column: 5, scope: !8)
!10 = !DILocation(line: 17, column: 11, scope: !8)
!11 = !DILocation(line: 18, column: 11, scope: !8)
!12 = !DILocation(line: 19, column: 5, scope: !8)
!13 = !DILocation(line: 20, column: 11, scope: !8)
!14 = !DILocation(line: 21, column: 5, scope: !8)
!15 = !DILocation(line: 22, column: 11, scope: !8)
!16 = !DILocation(line: 23, column: 11, scope: !8)
!17 = !DILocation(line: 24, column: 11, scope: !8)
!18 = !DILocation(line: 25, column: 11, scope: !8)
!19 = !DILocation(line: 26, column: 11, scope: !8)
!20 = !DILocation(line: 27, column: 11, scope: !8)
!21 = !DILocation(line: 28, column: 11, scope: !8)
!22 = !DILocation(line: 29, column: 11, scope: !8)
!23 = !DILocation(line: 30, column: 11, scope: !8)
!24 = !DILocation(line: 32, column: 5, scope: !8)
!25 = !DILocation(line: 34, column: 11, scope: !8)
!26 = !DILocation(line: 35, column: 5, scope: !8)
!27 = !DILocation(line: 37, column: 11, scope: !8)
!28 = !DILocation(line: 38, column: 11, scope: !8)
!29 = !DILocation(line: 39, column: 11, scope: !8)
!30 = !DILocation(line: 40, column: 5, scope: !8)
!31 = !DILocation(line: 42, column: 11, scope: !8)
!32 = !DILocation(line: 43, column: 11, scope: !8)
!33 = !DILocation(line: 44, column: 5, scope: !8)
!34 = !DILocation(line: 45, column: 11, scope: !8)
!35 = !DILocation(line: 46, column: 11, scope: !8)
!36 = !DILocation(line: 47, column: 5, scope: !8)
!37 = !DILocation(line: 49, column: 11, scope: !8)
!38 = !DILocation(line: 50, column: 11, scope: !8)
!39 = !DILocation(line: 51, column: 11, scope: !8)
!40 = !DILocation(line: 52, column: 11, scope: !8)
!41 = !DILocation(line: 54, column: 11, scope: !8)
!42 = !DILocation(line: 55, column: 11, scope: !8)
!43 = !DILocation(line: 56, column: 11, scope: !8)
!44 = !DILocation(line: 57, column: 11, scope: !8)
!45 = !DILocation(line: 58, column: 11, scope: !8)
!46 = !DILocation(line: 60, column: 11, scope: !8)
!47 = !DILocation(line: 61, column: 5, scope: !8)
!48 = !DILocation(line: 63, column: 11, scope: !8)
!49 = !DILocation(line: 64, column: 11, scope: !8)
!50 = !DILocation(line: 65, column: 11, scope: !8)
!51 = !DILocation(line: 66, column: 11, scope: !8)
!52 = !DILocation(line: 68, column: 11, scope: !8)
!53 = !DILocation(line: 69, column: 11, scope: !8)
!54 = !DILocation(line: 70, column: 11, scope: !8)
!55 = !DILocation(line: 71, column: 11, scope: !8)
!56 = !DILocation(line: 72, column: 11, scope: !8)
!57 = !DILocation(line: 74, column: 11, scope: !8)
!58 = !DILocation(line: 75, column: 5, scope: !8)
!59 = !DILocation(line: 77, column: 5, scope: !8)
!60 = !DILocation(line: 79, column: 5, scope: !8)
!61 = !DILocation(line: 80, column: 11, scope: !8)
!62 = !DILocation(line: 82, column: 11, scope: !8)
!63 = !DILocation(line: 83, column: 5, scope: !8)
!64 = !DILocation(line: 85, column: 11, scope: !8)
!65 = !DILocation(line: 86, column: 11, scope: !8)
!66 = !DILocation(line: 87, column: 5, scope: !8)
!67 = !DILocation(line: 88, column: 11, scope: !8)
!68 = !DILocation(line: 89, column: 11, scope: !8)
!69 = !DILocation(line: 90, column: 11, scope: !8)
!70 = !DILocation(line: 91, column: 11, scope: !8)
!71 = !DILocation(line: 93, column: 11, scope: !8)
!72 = !DILocation(line: 94, column: 11, scope: !8)
!73 = !DILocation(line: 95, column: 5, scope: !8)
!74 = !DILocation(line: 97, column: 11, scope: !8)
!75 = !DILocation(line: 98, column: 11, scope: !8)
!76 = !DILocation(line: 100, column: 5, scope: !8)
!77 = !DILocation(line: 101, column: 5, scope: !8)
!78 = !DILocation(line: 103, column: 11, scope: !8)
!79 = !DILocation(line: 104, column: 11, scope: !8)
!80 = !DILocation(line: 106, column: 5, scope: !8)
!81 = !DILocation(line: 107, column: 5, scope: !8)
!82 = !DILocation(line: 109, column: 11, scope: !8)
!83 = !DILocation(line: 110, column: 11, scope: !8)
!84 = !DILocation(line: 111, column: 5, scope: !8)
!85 = !DILocation(line: 113, column: 11, scope: !8)
!86 = !DILocation(line: 114, column: 5, scope: !8)
!87 = !DILocation(line: 115, column: 5, scope: !8)
!88 = !DILocation(line: 117, column: 5, scope: !8)
!89 = !DILocation(line: 119, column: 11, scope: !8)
!90 = !DILocation(line: 120, column: 11, scope: !8)
!91 = !DILocation(line: 121, column: 11, scope: !8)
!92 = !DILocation(line: 122, column: 11, scope: !8)
!93 = !DILocation(line: 123, column: 11, scope: !8)
!94 = !DILocation(line: 124, column: 11, scope: !8)
!95 = !DILocation(line: 125, column: 11, scope: !8)
!96 = !DILocation(line: 126, column: 11, scope: !8)
!97 = !DILocation(line: 127, column: 11, scope: !8)
!98 = !DILocation(line: 128, column: 11, scope: !8)
!99 = !DILocation(line: 129, column: 11, scope: !8)
!100 = !DILocation(line: 130, column: 11, scope: !8)
!101 = !DILocation(line: 131, column: 11, scope: !8)
!102 = !DILocation(line: 132, column: 11, scope: !8)
!103 = !DILocation(line: 133, column: 12, scope: !8)
!104 = !DILocation(line: 134, column: 5, scope: !8)
!105 = !DILocation(line: 136, column: 12, scope: !8)
!106 = !DILocation(line: 137, column: 12, scope: !8)
!107 = !DILocation(line: 138, column: 5, scope: !8)
!108 = !DILocation(line: 140, column: 12, scope: !8)
!109 = !DILocation(line: 141, column: 5, scope: !8)
!110 = !DILocation(line: 143, column: 5, scope: !8)
!111 = !DILocation(line: 145, column: 12, scope: !8)
!112 = !DILocation(line: 146, column: 12, scope: !8)
!113 = !DILocation(line: 147, column: 12, scope: !8)
!114 = !DILocation(line: 148, column: 12, scope: !8)
!115 = !DILocation(line: 150, column: 12, scope: !8)
!116 = !DILocation(line: 151, column: 12, scope: !8)
!117 = !DILocation(line: 152, column: 12, scope: !8)
!118 = !DILocation(line: 153, column: 12, scope: !8)
!119 = !DILocation(line: 154, column: 12, scope: !8)
!120 = !DILocation(line: 155, column: 12, scope: !8)
!121 = !DILocation(line: 156, column: 12, scope: !8)
!122 = !DILocation(line: 157, column: 12, scope: !8)
!123 = !DILocation(line: 158, column: 12, scope: !8)
!124 = !DILocation(line: 159, column: 5, scope: !8)
!125 = !DILocation(line: 160, column: 12, scope: !8)
!126 = !DILocation(line: 161, column: 12, scope: !8)
!127 = !DILocation(line: 162, column: 12, scope: !8)
!128 = !DILocation(line: 163, column: 12, scope: !8)
!129 = !DILocation(line: 165, column: 5, scope: !8)
!130 = !DILocation(line: 166, column: 12, scope: !8)
!131 = !DILocation(line: 167, column: 12, scope: !8)
!132 = !DILocation(line: 168, column: 12, scope: !8)
!133 = !DILocation(line: 169, column: 12, scope: !8)
!134 = !DILocation(line: 170, column: 12, scope: !8)
!135 = !DILocation(line: 172, column: 5, scope: !8)
!136 = !DILocation(line: 173, column: 12, scope: !8)
!137 = !DILocation(line: 174, column: 12, scope: !8)
!138 = !DILocation(line: 175, column: 12, scope: !8)
!139 = !DILocation(line: 176, column: 12, scope: !8)
!140 = !DILocation(line: 177, column: 5, scope: !8)
!141 = !DILocation(line: 179, column: 12, scope: !8)
!142 = !DILocation(line: 180, column: 12, scope: !8)
!143 = !DILocation(line: 181, column: 12, scope: !8)
!144 = !DILocation(line: 182, column: 5, scope: !8)
!145 = !DILocation(line: 183, column: 5, scope: !8)
!146 = !DILocation(line: 185, column: 5, scope: !8)
!147 = !DILocation(line: 187, column: 5, scope: !8)
!148 = !DILocation(line: 189, column: 5, scope: !8)
!149 = !DILocation(line: 191, column: 5, scope: !8)
!150 = !DILocation(line: 193, column: 5, scope: !8)
!151 = !DILocation(line: 195, column: 5, scope: !8)
!152 = !DILocation(line: 197, column: 12, scope: !8)
!153 = !DILocation(line: 198, column: 12, scope: !8)
!154 = !DILocation(line: 199, column: 12, scope: !8)
!155 = !DILocation(line: 200, column: 5, scope: !8)
!156 = !DILocation(line: 201, column: 12, scope: !8)
!157 = !DILocation(line: 202, column: 12, scope: !8)
!158 = !DILocation(line: 203, column: 5, scope: !8)
