; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }

@str4 = internal constant [23 x i8] c"DUAL NETWORK SIMPLEX: \00"
@str3 = internal constant [23 x i8] c"basis dual infeasible\0A\00"
@stderr = external global ptr
@str2 = internal constant [31 x i8] c"basis primal infeasible (%ld)\0A\00"
@str1 = internal constant [49 x i8] c"artificial arc with nonzero flow, node %d (%ld)\0A\00"
@str0 = internal constant [25 x i8] c"PRIMAL NETWORK SIMPLEX: \00"

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access_mut(i128)

declare ptr @cache_access(i128)

declare i128 @cache_request(i64)

declare i32 @fprintf(ptr, ptr, ...)

declare i32 @printf(ptr, ...)

define void @refresh_neighbour_lists(ptr %0) !dbg !3 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !7
  %3 = load ptr, ptr %2, align 8, !dbg !9
  %4 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !10
  %5 = load ptr, ptr %4, align 8, !dbg !11
  br label %6, !dbg !12

6:                                                ; preds = %9, %1
  %7 = phi ptr [ %13, %9 ], [ %3, %1 ]
  %8 = icmp ult ptr %7, %5, !dbg !13
  br i1 %8, label %9, label %14, !dbg !14

9:                                                ; preds = %6
  %10 = phi ptr [ %7, %6 ]
  %11 = getelementptr %_Lowered_node, ptr %10, i32 0, i32 8, !dbg !15
  store ptr null, ptr %11, align 8, !dbg !16
  %12 = getelementptr %_Lowered_node, ptr %10, i32 0, i32 7, !dbg !17
  store ptr null, ptr %12, align 8, !dbg !18
  %13 = getelementptr %_Lowered_node, ptr %10, i32 1, !dbg !19
  br label %6, !dbg !20

14:                                               ; preds = %6
  %15 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !21
  %16 = load ptr, ptr %15, align 8, !dbg !22
  %17 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !23
  %18 = load ptr, ptr %17, align 8, !dbg !24
  br label %19, !dbg !25

19:                                               ; preds = %22, %14
  %20 = phi ptr [ %54, %22 ], [ %16, %14 ]
  %21 = icmp ult ptr %20, %18, !dbg !26
  br i1 %21, label %22, label %55, !dbg !27

22:                                               ; preds = %19
  %23 = phi ptr [ %20, %19 ]
  %24 = getelementptr %_Lowered_rarc, ptr %23, i32 0, i32 4, !dbg !28
  %25 = getelementptr %_Lowered_rarc, ptr %23, i32 0, i32 1, !dbg !29
  %26 = ptrtoint ptr %25 to i64, !dbg !30
  %27 = call i128 @cache_request(i64 %26), !dbg !31
  %28 = call ptr @cache_access(i128 %27), !dbg !32
  %29 = load ptr, ptr %28, align 8, !dbg !33
  %30 = getelementptr %_Lowered_node, ptr %29, i32 0, i32 7, !dbg !34
  %31 = load ptr, ptr %30, align 8, !dbg !35
  %32 = ptrtoint ptr %24 to i64, !dbg !36
  %33 = call i128 @cache_request(i64 %32), !dbg !37
  %34 = call ptr @cache_access_mut(i128 %33), !dbg !38
  store ptr %31, ptr %34, align 8, !dbg !39
  %35 = call i128 @cache_request(i64 %26), !dbg !40
  %36 = call ptr @cache_access(i128 %35), !dbg !41
  %37 = load ptr, ptr %36, align 8, !dbg !42
  %38 = getelementptr %_Lowered_node, ptr %37, i32 0, i32 7, !dbg !43
  store ptr %23, ptr %38, align 8, !dbg !44
  %39 = getelementptr %_Lowered_rarc, ptr %23, i32 0, i32 5, !dbg !45
  %40 = getelementptr %_Lowered_rarc, ptr %23, i32 0, i32 2, !dbg !46
  %41 = ptrtoint ptr %40 to i64, !dbg !47
  %42 = call i128 @cache_request(i64 %41), !dbg !48
  %43 = call ptr @cache_access(i128 %42), !dbg !49
  %44 = load ptr, ptr %43, align 8, !dbg !50
  %45 = getelementptr %_Lowered_node, ptr %44, i32 0, i32 8, !dbg !51
  %46 = load ptr, ptr %45, align 8, !dbg !52
  %47 = ptrtoint ptr %39 to i64, !dbg !53
  %48 = call i128 @cache_request(i64 %47), !dbg !54
  %49 = call ptr @cache_access_mut(i128 %48), !dbg !55
  store ptr %46, ptr %49, align 8, !dbg !56
  %50 = call i128 @cache_request(i64 %41), !dbg !57
  %51 = call ptr @cache_access(i128 %50), !dbg !58
  %52 = load ptr, ptr %51, align 8, !dbg !59
  %53 = getelementptr %_Lowered_node, ptr %52, i32 0, i32 8, !dbg !60
  store ptr %23, ptr %53, align 8, !dbg !61
  %54 = getelementptr %_Lowered_rarc, ptr %23, i32 1, !dbg !62
  br label %19, !dbg !63

55:                                               ; preds = %19
  ret void, !dbg !64
}

define i64 @refresh_potential(ptr %0) !dbg !65 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !66
  %3 = load ptr, ptr %2, align 8, !dbg !68
  %4 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 0, !dbg !69
  store i64 -100000000, ptr %4, align 8, !dbg !70
  %5 = getelementptr %_Lowered_node, ptr %3, i32 0, i32 2, !dbg !71
  %6 = load ptr, ptr %5, align 8, !dbg !72
  br label %7, !dbg !73

7:                                                ; preds = %119, %1
  %8 = phi i64 [ %61, %119 ], [ 0, %1 ]
  %9 = phi ptr [ %109, %119 ], [ %6, %1 ]
  %10 = phi ptr [ %110, %119 ], [ %6, %1 ]
  %11 = icmp ne ptr %10, %3, !dbg !74
  br i1 %11, label %12, label %120, !dbg !75

12:                                               ; preds = %7
  %13 = phi i64 [ %8, %7 ]
  %14 = phi ptr [ %9, %7 ]
  %15 = phi ptr [ %10, %7 ]
  br label %16, !dbg !76

16:                                               ; preds = %67, %12
  %17 = phi i64 [ %68, %67 ], [ %13, %12 ]
  %18 = phi ptr [ %69, %67 ], [ %15, %12 ]
  %19 = phi ptr [ %70, %67 ], [ %14, %12 ]
  %20 = icmp ne ptr %18, null, !dbg !77
  br i1 %20, label %21, label %59, !dbg !78

21:                                               ; preds = %16
  %22 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 1, !dbg !79
  %23 = load i32, ptr %22, align 4, !dbg !80
  %24 = icmp eq i32 %23, 1, !dbg !81
  br i1 %24, label %25, label %39, !dbg !82

25:                                               ; preds = %21
  %26 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 0, !dbg !83
  %27 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 6, !dbg !84
  %28 = load ptr, ptr %27, align 8, !dbg !85
  %29 = getelementptr %_Lowered_rarc, ptr %28, i32 0, i32 0, !dbg !86
  %30 = ptrtoint ptr %29 to i64, !dbg !87
  %31 = call i128 @cache_request(i64 %30), !dbg !88
  %32 = call ptr @cache_access(i128 %31), !dbg !89
  %33 = load i64, ptr %32, align 8, !dbg !90
  %34 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 3, !dbg !91
  %35 = load ptr, ptr %34, align 8, !dbg !92
  %36 = getelementptr %_Lowered_node, ptr %35, i32 0, i32 0, !dbg !93
  %37 = load i64, ptr %36, align 8, !dbg !94
  %38 = add i64 %33, %37, !dbg !95
  store i64 %38, ptr %26, align 8, !dbg !96
  br label %54, !dbg !97

39:                                               ; preds = %21
  %40 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 0, !dbg !98
  %41 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 3, !dbg !99
  %42 = load ptr, ptr %41, align 8, !dbg !100
  %43 = getelementptr %_Lowered_node, ptr %42, i32 0, i32 0, !dbg !101
  %44 = load i64, ptr %43, align 8, !dbg !102
  %45 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 6, !dbg !103
  %46 = load ptr, ptr %45, align 8, !dbg !104
  %47 = getelementptr %_Lowered_rarc, ptr %46, i32 0, i32 0, !dbg !105
  %48 = ptrtoint ptr %47 to i64, !dbg !106
  %49 = call i128 @cache_request(i64 %48), !dbg !107
  %50 = call ptr @cache_access(i128 %49), !dbg !108
  %51 = load i64, ptr %50, align 8, !dbg !109
  %52 = sub i64 %44, %51, !dbg !110
  store i64 %52, ptr %40, align 8, !dbg !111
  %53 = add i64 %17, 1, !dbg !112
  br label %54, !dbg !113

54:                                               ; preds = %25, %39
  %55 = phi i64 [ %53, %39 ], [ %17, %25 ]
  br label %56, !dbg !114

56:                                               ; preds = %54
  %57 = getelementptr %_Lowered_node, ptr %18, i32 0, i32 2, !dbg !115
  %58 = load ptr, ptr %57, align 8, !dbg !116
  br label %60, !dbg !117

59:                                               ; preds = %16
  br label %60, !dbg !118

60:                                               ; preds = %56, %59
  %61 = phi i64 [ %17, %59 ], [ %55, %56 ]
  %62 = phi ptr [ %18, %59 ], [ %58, %56 ]
  %63 = phi ptr [ %19, %59 ], [ undef, %56 ]
  %64 = phi ptr [ %19, %59 ], [ undef, %56 ]
  %65 = phi ptr [ %19, %59 ], [ undef, %56 ]
  br label %66, !dbg !119

66:                                               ; preds = %60
  br i1 %20, label %67, label %74, !dbg !120

67:                                               ; preds = %66
  %68 = phi i64 [ %61, %66 ]
  %69 = phi ptr [ %62, %66 ]
  %70 = phi ptr [ %18, %66 ]
  %71 = phi ptr [ %63, %66 ]
  %72 = phi ptr [ %64, %66 ]
  %73 = phi ptr [ %65, %66 ]
  br label %16, !dbg !121

74:                                               ; preds = %66
  br label %75, !dbg !122

75:                                               ; preds = %112, %74
  %76 = phi i1 [ %113, %112 ], [ true, %74 ]
  %77 = phi ptr [ %114, %112 ], [ %63, %74 ]
  %78 = phi ptr [ %115, %112 ], [ %64, %74 ]
  %79 = phi ptr [ %116, %112 ], [ %65, %74 ]
  %80 = getelementptr %_Lowered_node, ptr %77, i32 0, i32 3, !dbg !123
  %81 = load ptr, ptr %80, align 8, !dbg !124
  %82 = icmp ne ptr %81, null, !dbg !125
  %83 = and i1 %82, %76, !dbg !126
  br i1 %83, label %84, label %103, !dbg !127

84:                                               ; preds = %75
  br i1 %76, label %85, label %96, !dbg !128

85:                                               ; preds = %84
  %86 = getelementptr %_Lowered_node, ptr %77, i32 0, i32 4, !dbg !129
  %87 = load ptr, ptr %86, align 8, !dbg !130
  %88 = icmp ne ptr %87, null, !dbg !131
  %89 = xor i1 %88, true, !dbg !132
  br i1 %88, label %90, label %91, !dbg !133

90:                                               ; preds = %85
  br label %93, !dbg !134

91:                                               ; preds = %85
  %92 = load ptr, ptr %80, align 8, !dbg !135
  br label %93, !dbg !136

93:                                               ; preds = %90, %91
  %94 = phi ptr [ %92, %91 ], [ %87, %90 ]
  br label %95, !dbg !137

95:                                               ; preds = %93
  br label %97, !dbg !138

96:                                               ; preds = %84
  br label %97, !dbg !139

97:                                               ; preds = %95, %96
  %98 = phi i1 [ false, %96 ], [ %89, %95 ]
  %99 = phi ptr [ %77, %96 ], [ %94, %95 ]
  %100 = phi ptr [ %78, %96 ], [ %87, %95 ]
  %101 = phi ptr [ %79, %96 ], [ %94, %95 ]
  br label %102, !dbg !140

102:                                              ; preds = %97
  br label %104, !dbg !141

103:                                              ; preds = %75
  br label %104, !dbg !142

104:                                              ; preds = %102, %103
  %105 = phi i1 [ %76, %103 ], [ %98, %102 ]
  %106 = phi ptr [ %77, %103 ], [ %99, %102 ]
  %107 = phi ptr [ %78, %103 ], [ %100, %102 ]
  %108 = phi ptr [ %79, %103 ], [ %101, %102 ]
  %109 = phi ptr [ %78, %103 ], [ undef, %102 ]
  %110 = phi ptr [ %79, %103 ], [ undef, %102 ]
  br label %111, !dbg !143

111:                                              ; preds = %104
  br i1 %83, label %112, label %119, !dbg !144

112:                                              ; preds = %111
  %113 = phi i1 [ %105, %111 ]
  %114 = phi ptr [ %106, %111 ]
  %115 = phi ptr [ %107, %111 ]
  %116 = phi ptr [ %108, %111 ]
  %117 = phi ptr [ %109, %111 ]
  %118 = phi ptr [ %110, %111 ]
  br label %75, !dbg !145

119:                                              ; preds = %111
  br label %7, !dbg !146

120:                                              ; preds = %7
  ret i64 %8, !dbg !147
}

define double @flow_cost(ptr %0) !dbg !148 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !149
  %3 = load ptr, ptr %2, align 8, !dbg !151
  %4 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !152
  %5 = load ptr, ptr %4, align 8, !dbg !153
  br label %6, !dbg !154

6:                                                ; preds = %27, %1
  %7 = phi ptr [ %28, %27 ], [ %5, %1 ]
  %8 = icmp ne ptr %7, %3, !dbg !155
  br i1 %8, label %9, label %29, !dbg !156

9:                                                ; preds = %6
  %10 = phi ptr [ %7, %6 ]
  %11 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 3, !dbg !157
  %12 = ptrtoint ptr %11 to i64, !dbg !158
  %13 = call i128 @cache_request(i64 %12), !dbg !159
  %14 = call ptr @cache_access(i128 %13), !dbg !160
  %15 = load i32, ptr %14, align 4, !dbg !161
  %16 = icmp eq i32 %15, 2, !dbg !162
  br i1 %16, label %17, label %22, !dbg !163

17:                                               ; preds = %9
  %18 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 6, !dbg !164
  %19 = ptrtoint ptr %18 to i64, !dbg !165
  %20 = call i128 @cache_request(i64 %19), !dbg !166
  %21 = call ptr @cache_access_mut(i128 %20), !dbg !167
  store i64 1, ptr %21, align 8, !dbg !168
  br label %27, !dbg !169

22:                                               ; preds = %9
  %23 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 6, !dbg !170
  %24 = ptrtoint ptr %23 to i64, !dbg !171
  %25 = call i128 @cache_request(i64 %24), !dbg !172
  %26 = call ptr @cache_access_mut(i128 %25), !dbg !173
  store i64 0, ptr %26, align 8, !dbg !174
  br label %27, !dbg !175

27:                                               ; preds = %17, %22
  %28 = getelementptr %_Lowered_rarc, ptr %10, i32 1, !dbg !176
  br label %6, !dbg !177

29:                                               ; preds = %6
  %30 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !178
  %31 = load ptr, ptr %30, align 8, !dbg !179
  %32 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !180
  %33 = load ptr, ptr %32, align 8, !dbg !181
  %34 = getelementptr %_Lowered_node, ptr %33, i32 1, !dbg !182
  br label %35, !dbg !183

35:                                               ; preds = %38, %29
  %36 = phi ptr [ %48, %38 ], [ %34, %29 ]
  %37 = icmp ne ptr %36, %31, !dbg !184
  br i1 %37, label %38, label %49, !dbg !185

38:                                               ; preds = %35
  %39 = phi ptr [ %36, %35 ]
  %40 = getelementptr %_Lowered_node, ptr %39, i32 0, i32 6, !dbg !186
  %41 = load ptr, ptr %40, align 8, !dbg !187
  %42 = getelementptr %_Lowered_rarc, ptr %41, i32 0, i32 6, !dbg !188
  %43 = getelementptr %_Lowered_node, ptr %39, i32 0, i32 10, !dbg !189
  %44 = load i64, ptr %43, align 8, !dbg !190
  %45 = ptrtoint ptr %42 to i64, !dbg !191
  %46 = call i128 @cache_request(i64 %45), !dbg !192
  %47 = call ptr @cache_access_mut(i128 %46), !dbg !193
  store i64 %44, ptr %47, align 8, !dbg !194
  %48 = getelementptr %_Lowered_node, ptr %39, i32 1, !dbg !195
  br label %35, !dbg !196

49:                                               ; preds = %35
  %50 = load ptr, ptr %2, align 8, !dbg !197
  %51 = load ptr, ptr %4, align 8, !dbg !198
  br label %52, !dbg !199

52:                                               ; preds = %129, %49
  %53 = phi i64 [ %127, %129 ], [ 0, %49 ]
  %54 = phi i64 [ %128, %129 ], [ 0, %49 ]
  %55 = phi ptr [ %130, %129 ], [ %51, %49 ]
  %56 = icmp ne ptr %55, %50, !dbg !200
  br i1 %56, label %57, label %131, !dbg !201

57:                                               ; preds = %52
  %58 = phi i64 [ %53, %52 ]
  %59 = phi i64 [ %54, %52 ]
  %60 = phi ptr [ %55, %52 ]
  %61 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 6, !dbg !202
  %62 = ptrtoint ptr %61 to i64, !dbg !203
  %63 = call i128 @cache_request(i64 %62), !dbg !204
  %64 = call ptr @cache_access(i128 %63), !dbg !205
  %65 = load i64, ptr %64, align 8, !dbg !206
  %66 = icmp ne i64 %65, 0, !dbg !207
  br i1 %66, label %67, label %125, !dbg !208

67:                                               ; preds = %57
  %68 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 1, !dbg !209
  %69 = ptrtoint ptr %68 to i64, !dbg !210
  %70 = call i128 @cache_request(i64 %69), !dbg !211
  %71 = call ptr @cache_access(i128 %70), !dbg !212
  %72 = load ptr, ptr %71, align 8, !dbg !213
  %73 = getelementptr %_Lowered_node, ptr %72, i32 0, i32 12, !dbg !214
  %74 = load i32, ptr %73, align 4, !dbg !215
  %75 = icmp slt i32 %74, 0, !dbg !216
  br i1 %75, label %76, label %85, !dbg !217

76:                                               ; preds = %67
  %77 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 2, !dbg !218
  %78 = ptrtoint ptr %77 to i64, !dbg !219
  %79 = call i128 @cache_request(i64 %78), !dbg !220
  %80 = call ptr @cache_access(i128 %79), !dbg !221
  %81 = load ptr, ptr %80, align 8, !dbg !222
  %82 = getelementptr %_Lowered_node, ptr %81, i32 0, i32 12, !dbg !223
  %83 = load i32, ptr %82, align 4, !dbg !224
  %84 = icmp sgt i32 %83, 0, !dbg !225
  br label %86, !dbg !226

85:                                               ; preds = %67
  br label %86, !dbg !227

86:                                               ; preds = %76, %85
  %87 = phi i1 [ false, %85 ], [ %84, %76 ]
  br label %88, !dbg !228

88:                                               ; preds = %86
  %89 = sext i1 %87 to i32, !dbg !229
  %90 = icmp eq i32 %89, 0, !dbg !230
  br i1 %90, label %91, label %120, !dbg !231

91:                                               ; preds = %88
  %92 = call i128 @cache_request(i64 %69), !dbg !232
  %93 = call ptr @cache_access(i128 %92), !dbg !233
  %94 = load ptr, ptr %93, align 8, !dbg !234
  %95 = getelementptr %_Lowered_node, ptr %94, i32 0, i32 12, !dbg !235
  %96 = load i32, ptr %95, align 4, !dbg !236
  %97 = icmp ne i32 %96, 0, !dbg !237
  br i1 %97, label %98, label %105, !dbg !238

98:                                               ; preds = %91
  %99 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 0, !dbg !239
  %100 = ptrtoint ptr %99 to i64, !dbg !240
  %101 = call i128 @cache_request(i64 %100), !dbg !241
  %102 = call ptr @cache_access(i128 %101), !dbg !242
  %103 = load i64, ptr %102, align 8, !dbg !243
  %104 = add i64 %58, %103, !dbg !244
  br label %116, !dbg !245

105:                                              ; preds = %91
  %106 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 0, !dbg !246
  %107 = ptrtoint ptr %106 to i64, !dbg !247
  %108 = call i128 @cache_request(i64 %107), !dbg !248
  %109 = call ptr @cache_access(i128 %108), !dbg !249
  %110 = load i64, ptr %109, align 8, !dbg !250
  %111 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !251
  %112 = load i64, ptr %111, align 8, !dbg !252
  %113 = sub i64 %110, %112, !dbg !253
  %114 = add i64 %58, %113, !dbg !254
  %115 = add i64 %59, 1, !dbg !255
  br label %116, !dbg !256

116:                                              ; preds = %98, %105
  %117 = phi i64 [ %114, %105 ], [ %104, %98 ]
  %118 = phi i64 [ %115, %105 ], [ %59, %98 ]
  br label %119, !dbg !257

119:                                              ; preds = %116
  br label %121, !dbg !258

120:                                              ; preds = %88
  br label %121, !dbg !259

121:                                              ; preds = %119, %120
  %122 = phi i64 [ %58, %120 ], [ %117, %119 ]
  %123 = phi i64 [ %59, %120 ], [ %118, %119 ]
  br label %124, !dbg !260

124:                                              ; preds = %121
  br label %126, !dbg !261

125:                                              ; preds = %57
  br label %126, !dbg !262

126:                                              ; preds = %124, %125
  %127 = phi i64 [ %58, %125 ], [ %122, %124 ]
  %128 = phi i64 [ %59, %125 ], [ %123, %124 ]
  br label %129, !dbg !263

129:                                              ; preds = %126
  %130 = getelementptr %_Lowered_rarc, ptr %60, i32 1, !dbg !264
  br label %52, !dbg !265

131:                                              ; preds = %52
  %132 = sitofp i64 %54 to double, !dbg !266
  %133 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !267
  %134 = load i64, ptr %133, align 8, !dbg !268
  %135 = sitofp i64 %134 to double, !dbg !269
  %136 = fmul double %132, %135, !dbg !270
  %137 = sitofp i64 %53 to double, !dbg !271
  %138 = fadd double %136, %137, !dbg !272
  ret double %138, !dbg !273
}

define double @flow_org_cost(ptr %0) !dbg !274 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !275
  %3 = load ptr, ptr %2, align 8, !dbg !277
  %4 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !278
  %5 = load ptr, ptr %4, align 8, !dbg !279
  br label %6, !dbg !280

6:                                                ; preds = %27, %1
  %7 = phi ptr [ %28, %27 ], [ %5, %1 ]
  %8 = icmp ne ptr %7, %3, !dbg !281
  br i1 %8, label %9, label %29, !dbg !282

9:                                                ; preds = %6
  %10 = phi ptr [ %7, %6 ]
  %11 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 3, !dbg !283
  %12 = ptrtoint ptr %11 to i64, !dbg !284
  %13 = call i128 @cache_request(i64 %12), !dbg !285
  %14 = call ptr @cache_access(i128 %13), !dbg !286
  %15 = load i32, ptr %14, align 4, !dbg !287
  %16 = icmp eq i32 %15, 2, !dbg !288
  br i1 %16, label %17, label %22, !dbg !289

17:                                               ; preds = %9
  %18 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 6, !dbg !290
  %19 = ptrtoint ptr %18 to i64, !dbg !291
  %20 = call i128 @cache_request(i64 %19), !dbg !292
  %21 = call ptr @cache_access_mut(i128 %20), !dbg !293
  store i64 1, ptr %21, align 8, !dbg !294
  br label %27, !dbg !295

22:                                               ; preds = %9
  %23 = getelementptr %_Lowered_rarc, ptr %10, i32 0, i32 6, !dbg !296
  %24 = ptrtoint ptr %23 to i64, !dbg !297
  %25 = call i128 @cache_request(i64 %24), !dbg !298
  %26 = call ptr @cache_access_mut(i128 %25), !dbg !299
  store i64 0, ptr %26, align 8, !dbg !300
  br label %27, !dbg !301

27:                                               ; preds = %17, %22
  %28 = getelementptr %_Lowered_rarc, ptr %10, i32 1, !dbg !302
  br label %6, !dbg !303

29:                                               ; preds = %6
  %30 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !304
  %31 = load ptr, ptr %30, align 8, !dbg !305
  %32 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !306
  %33 = load ptr, ptr %32, align 8, !dbg !307
  %34 = getelementptr %_Lowered_node, ptr %33, i32 1, !dbg !308
  br label %35, !dbg !309

35:                                               ; preds = %38, %29
  %36 = phi ptr [ %48, %38 ], [ %34, %29 ]
  %37 = icmp ne ptr %36, %31, !dbg !310
  br i1 %37, label %38, label %49, !dbg !311

38:                                               ; preds = %35
  %39 = phi ptr [ %36, %35 ]
  %40 = getelementptr %_Lowered_node, ptr %39, i32 0, i32 6, !dbg !312
  %41 = load ptr, ptr %40, align 8, !dbg !313
  %42 = getelementptr %_Lowered_rarc, ptr %41, i32 0, i32 6, !dbg !314
  %43 = getelementptr %_Lowered_node, ptr %39, i32 0, i32 10, !dbg !315
  %44 = load i64, ptr %43, align 8, !dbg !316
  %45 = ptrtoint ptr %42 to i64, !dbg !317
  %46 = call i128 @cache_request(i64 %45), !dbg !318
  %47 = call ptr @cache_access_mut(i128 %46), !dbg !319
  store i64 %44, ptr %47, align 8, !dbg !320
  %48 = getelementptr %_Lowered_node, ptr %39, i32 1, !dbg !321
  br label %35, !dbg !322

49:                                               ; preds = %35
  %50 = load ptr, ptr %2, align 8, !dbg !323
  %51 = load ptr, ptr %4, align 8, !dbg !324
  br label %52, !dbg !325

52:                                               ; preds = %129, %49
  %53 = phi i64 [ %127, %129 ], [ 0, %49 ]
  %54 = phi i64 [ %128, %129 ], [ 0, %49 ]
  %55 = phi ptr [ %130, %129 ], [ %51, %49 ]
  %56 = icmp ne ptr %55, %50, !dbg !326
  br i1 %56, label %57, label %131, !dbg !327

57:                                               ; preds = %52
  %58 = phi i64 [ %53, %52 ]
  %59 = phi i64 [ %54, %52 ]
  %60 = phi ptr [ %55, %52 ]
  %61 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 6, !dbg !328
  %62 = ptrtoint ptr %61 to i64, !dbg !329
  %63 = call i128 @cache_request(i64 %62), !dbg !330
  %64 = call ptr @cache_access(i128 %63), !dbg !331
  %65 = load i64, ptr %64, align 8, !dbg !332
  %66 = icmp ne i64 %65, 0, !dbg !333
  br i1 %66, label %67, label %125, !dbg !334

67:                                               ; preds = %57
  %68 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 1, !dbg !335
  %69 = ptrtoint ptr %68 to i64, !dbg !336
  %70 = call i128 @cache_request(i64 %69), !dbg !337
  %71 = call ptr @cache_access(i128 %70), !dbg !338
  %72 = load ptr, ptr %71, align 8, !dbg !339
  %73 = getelementptr %_Lowered_node, ptr %72, i32 0, i32 12, !dbg !340
  %74 = load i32, ptr %73, align 4, !dbg !341
  %75 = icmp slt i32 %74, 0, !dbg !342
  br i1 %75, label %76, label %85, !dbg !343

76:                                               ; preds = %67
  %77 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 2, !dbg !344
  %78 = ptrtoint ptr %77 to i64, !dbg !345
  %79 = call i128 @cache_request(i64 %78), !dbg !346
  %80 = call ptr @cache_access(i128 %79), !dbg !347
  %81 = load ptr, ptr %80, align 8, !dbg !348
  %82 = getelementptr %_Lowered_node, ptr %81, i32 0, i32 12, !dbg !349
  %83 = load i32, ptr %82, align 4, !dbg !350
  %84 = icmp sgt i32 %83, 0, !dbg !351
  br label %86, !dbg !352

85:                                               ; preds = %67
  br label %86, !dbg !353

86:                                               ; preds = %76, %85
  %87 = phi i1 [ false, %85 ], [ %84, %76 ]
  br label %88, !dbg !354

88:                                               ; preds = %86
  %89 = sext i1 %87 to i32, !dbg !355
  %90 = icmp eq i32 %89, 0, !dbg !356
  br i1 %90, label %91, label %120, !dbg !357

91:                                               ; preds = %88
  %92 = call i128 @cache_request(i64 %69), !dbg !358
  %93 = call ptr @cache_access(i128 %92), !dbg !359
  %94 = load ptr, ptr %93, align 8, !dbg !360
  %95 = getelementptr %_Lowered_node, ptr %94, i32 0, i32 12, !dbg !361
  %96 = load i32, ptr %95, align 4, !dbg !362
  %97 = icmp ne i32 %96, 0, !dbg !363
  br i1 %97, label %98, label %105, !dbg !364

98:                                               ; preds = %91
  %99 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 7, !dbg !365
  %100 = ptrtoint ptr %99 to i64, !dbg !366
  %101 = call i128 @cache_request(i64 %100), !dbg !367
  %102 = call ptr @cache_access(i128 %101), !dbg !368
  %103 = load i64, ptr %102, align 8, !dbg !369
  %104 = add i64 %58, %103, !dbg !370
  br label %116, !dbg !371

105:                                              ; preds = %91
  %106 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 7, !dbg !372
  %107 = ptrtoint ptr %106 to i64, !dbg !373
  %108 = call i128 @cache_request(i64 %107), !dbg !374
  %109 = call ptr @cache_access(i128 %108), !dbg !375
  %110 = load i64, ptr %109, align 8, !dbg !376
  %111 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !377
  %112 = load i64, ptr %111, align 8, !dbg !378
  %113 = sub i64 %110, %112, !dbg !379
  %114 = add i64 %58, %113, !dbg !380
  %115 = add i64 %59, 1, !dbg !381
  br label %116, !dbg !382

116:                                              ; preds = %98, %105
  %117 = phi i64 [ %114, %105 ], [ %104, %98 ]
  %118 = phi i64 [ %115, %105 ], [ %59, %98 ]
  br label %119, !dbg !383

119:                                              ; preds = %116
  br label %121, !dbg !384

120:                                              ; preds = %88
  br label %121, !dbg !385

121:                                              ; preds = %119, %120
  %122 = phi i64 [ %58, %120 ], [ %117, %119 ]
  %123 = phi i64 [ %59, %120 ], [ %118, %119 ]
  br label %124, !dbg !386

124:                                              ; preds = %121
  br label %126, !dbg !387

125:                                              ; preds = %57
  br label %126, !dbg !388

126:                                              ; preds = %124, %125
  %127 = phi i64 [ %58, %125 ], [ %122, %124 ]
  %128 = phi i64 [ %59, %125 ], [ %123, %124 ]
  br label %129, !dbg !389

129:                                              ; preds = %126
  %130 = getelementptr %_Lowered_rarc, ptr %60, i32 1, !dbg !390
  br label %52, !dbg !391

131:                                              ; preds = %52
  %132 = sitofp i64 %54 to double, !dbg !392
  %133 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !393
  %134 = load i64, ptr %133, align 8, !dbg !394
  %135 = sitofp i64 %134 to double, !dbg !395
  %136 = fmul double %132, %135, !dbg !396
  %137 = sitofp i64 %53 to double, !dbg !397
  %138 = fadd double %136, %137, !dbg !398
  ret double %138, !dbg !399
}

define i64 @primal_feasible(ptr %0) !dbg !400 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 25, !dbg !401
  %3 = load ptr, ptr %2, align 8, !dbg !403
  %4 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 26, !dbg !404
  %5 = load ptr, ptr %4, align 8, !dbg !405
  %6 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !406
  %7 = load ptr, ptr %6, align 8, !dbg !407
  %8 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !408
  %9 = load ptr, ptr %8, align 8, !dbg !409
  %10 = getelementptr %_Lowered_node, ptr %7, i32 1, !dbg !410
  br label %11, !dbg !411

11:                                               ; preds = %82, %1
  %12 = phi i1 [ %73, %82 ], [ true, %1 ]
  %13 = phi i64 [ %74, %82 ], [ undef, %1 ]
  %14 = phi i1 [ %75, %82 ], [ true, %1 ]
  %15 = phi i64 [ %26, %82 ], [ undef, %1 ]
  %16 = phi ptr [ %81, %82 ], [ %10, %1 ]
  %17 = icmp ult ptr %16, %9, !dbg !412
  %18 = and i1 %17, %14, !dbg !413
  br i1 %18, label %19, label %83, !dbg !414

19:                                               ; preds = %11
  %20 = phi i1 [ %12, %11 ]
  %21 = phi i64 [ %13, %11 ]
  %22 = phi ptr [ %16, %11 ]
  %23 = getelementptr %_Lowered_node, ptr %22, i32 0, i32 6, !dbg !415
  %24 = load ptr, ptr %23, align 8, !dbg !416
  %25 = getelementptr %_Lowered_node, ptr %22, i32 0, i32 10, !dbg !417
  %26 = load i64, ptr %25, align 8, !dbg !418
  %27 = icmp uge ptr %24, %3, !dbg !419
  br i1 %27, label %28, label %30, !dbg !420

28:                                               ; preds = %19
  %29 = icmp ult ptr %24, %5, !dbg !421
  br label %31, !dbg !422

30:                                               ; preds = %19
  br label %31, !dbg !423

31:                                               ; preds = %28, %30
  %32 = phi i1 [ false, %30 ], [ %29, %28 ]
  br label %33, !dbg !424

33:                                               ; preds = %31
  br i1 %32, label %34, label %51, !dbg !425

34:                                               ; preds = %33
  %35 = icmp sge i64 %26, 0, !dbg !426
  br i1 %35, label %36, label %37, !dbg !427

36:                                               ; preds = %34
  br label %39, !dbg !428

37:                                               ; preds = %34
  %38 = sub i64 0, %26, !dbg !429
  br label %39, !dbg !430

39:                                               ; preds = %36, %37
  %40 = phi i64 [ %38, %37 ], [ %26, %36 ]
  br label %41, !dbg !431

41:                                               ; preds = %39
  %42 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 16, !dbg !432
  %43 = load i64, ptr %42, align 8, !dbg !433
  %44 = icmp sgt i64 %40, %43, !dbg !434
  br i1 %44, label %45, label %50, !dbg !435

45:                                               ; preds = %41
  %46 = call i32 (ptr, ...) @printf(ptr @str0), !dbg !436
  %47 = getelementptr %_Lowered_node, ptr %22, i32 0, i32 12, !dbg !437
  %48 = load i32, ptr %47, align 4, !dbg !438
  %49 = call i32 (ptr, ...) @printf(ptr @str1, i32 %48, i64 %26), !dbg !439
  br label %50, !dbg !440

50:                                               ; preds = %45, %41
  br label %72, !dbg !441

51:                                               ; preds = %33
  %52 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 16, !dbg !442
  %53 = load i64, ptr %52, align 8, !dbg !443
  %54 = sub i64 0, %53, !dbg !444
  %55 = icmp slt i64 %26, %54, !dbg !445
  br i1 %55, label %56, label %57, !dbg !446

56:                                               ; preds = %51
  br label %61, !dbg !447

57:                                               ; preds = %51
  %58 = add i64 %26, -1, !dbg !448
  %59 = load i64, ptr %52, align 8, !dbg !449
  %60 = icmp sgt i64 %58, %59, !dbg !450
  br label %61, !dbg !451

61:                                               ; preds = %56, %57
  %62 = phi i1 [ %60, %57 ], [ true, %56 ]
  br label %63, !dbg !452

63:                                               ; preds = %61
  %64 = xor i1 %62, true, !dbg !453
  %65 = and i1 %64, %20, !dbg !454
  %66 = select i1 %62, i64 1, i64 %21, !dbg !455
  br i1 %62, label %67, label %71, !dbg !456

67:                                               ; preds = %63
  %68 = call i32 (ptr, ...) @printf(ptr @str0), !dbg !457
  %69 = call i32 (ptr, ...) @printf(ptr @str2, i64 %26), !dbg !458
  %70 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 13, !dbg !459
  store i64 0, ptr %70, align 8, !dbg !460
  br label %71, !dbg !461

71:                                               ; preds = %67, %63
  br label %72, !dbg !462

72:                                               ; preds = %50, %71
  %73 = phi i1 [ %65, %71 ], [ %20, %50 ]
  %74 = phi i64 [ %66, %71 ], [ %21, %50 ]
  %75 = phi i1 [ %64, %71 ], [ true, %50 ]
  br label %76, !dbg !463

76:                                               ; preds = %72
  br i1 %75, label %77, label %79, !dbg !464

77:                                               ; preds = %76
  %78 = getelementptr %_Lowered_node, ptr %22, i32 1, !dbg !465
  br label %80, !dbg !466

79:                                               ; preds = %76
  br label %80, !dbg !467

80:                                               ; preds = %77, %79
  %81 = phi ptr [ %22, %79 ], [ %78, %77 ]
  br label %82, !dbg !468

82:                                               ; preds = %80
  br label %11, !dbg !469

83:                                               ; preds = %11
  %84 = select i1 %12, i64 0, i64 %13, !dbg !470
  br i1 %12, label %85, label %87, !dbg !471

85:                                               ; preds = %83
  %86 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 13, !dbg !472
  store i64 1, ptr %86, align 8, !dbg !473
  br label %87, !dbg !474

87:                                               ; preds = %85, %83
  ret i64 %84, !dbg !475
}

define i64 @dual_feasible(ptr %0) !dbg !476 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !477
  %3 = load ptr, ptr %2, align 8, !dbg !479
  %4 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !480
  %5 = load ptr, ptr %4, align 8, !dbg !481
  br label %6, !dbg !482

6:                                                ; preds = %71, %1
  %7 = phi i1 [ %63, %71 ], [ true, %1 ]
  %8 = phi i64 [ %64, %71 ], [ undef, %1 ]
  %9 = phi i1 [ %65, %71 ], [ true, %1 ]
  %10 = phi i64 [ %38, %71 ], [ undef, %1 ]
  %11 = phi ptr [ %70, %71 ], [ %5, %1 ]
  %12 = icmp ult ptr %11, %3, !dbg !483
  %13 = and i1 %12, %9, !dbg !484
  br i1 %13, label %14, label %72, !dbg !485

14:                                               ; preds = %6
  %15 = phi i1 [ %7, %6 ]
  %16 = phi i64 [ %8, %6 ]
  %17 = phi ptr [ %11, %6 ]
  %18 = getelementptr %_Lowered_rarc, ptr %17, i32 0, i32 0, !dbg !486
  %19 = ptrtoint ptr %18 to i64, !dbg !487
  %20 = call i128 @cache_request(i64 %19), !dbg !488
  %21 = call ptr @cache_access(i128 %20), !dbg !489
  %22 = load i64, ptr %21, align 8, !dbg !490
  %23 = getelementptr %_Lowered_rarc, ptr %17, i32 0, i32 1, !dbg !491
  %24 = ptrtoint ptr %23 to i64, !dbg !492
  %25 = call i128 @cache_request(i64 %24), !dbg !493
  %26 = call ptr @cache_access(i128 %25), !dbg !494
  %27 = load ptr, ptr %26, align 8, !dbg !495
  %28 = getelementptr %_Lowered_node, ptr %27, i32 0, i32 0, !dbg !496
  %29 = load i64, ptr %28, align 8, !dbg !497
  %30 = sub i64 %22, %29, !dbg !498
  %31 = getelementptr %_Lowered_rarc, ptr %17, i32 0, i32 2, !dbg !499
  %32 = ptrtoint ptr %31 to i64, !dbg !500
  %33 = call i128 @cache_request(i64 %32), !dbg !501
  %34 = call ptr @cache_access(i128 %33), !dbg !502
  %35 = load ptr, ptr %34, align 8, !dbg !503
  %36 = getelementptr %_Lowered_node, ptr %35, i32 0, i32 0, !dbg !504
  %37 = load i64, ptr %36, align 8, !dbg !505
  %38 = add i64 %30, %37, !dbg !506
  %39 = getelementptr %_Lowered_rarc, ptr %17, i32 0, i32 3, !dbg !507
  %40 = ptrtoint ptr %39 to i64, !dbg !508
  %41 = call i128 @cache_request(i64 %40), !dbg !509
  %42 = call ptr @cache_access(i128 %41), !dbg !510
  %43 = load i32, ptr %42, align 4, !dbg !511
  br label %44, !dbg !512

44:                                               ; preds = %14
  switch i32 %43, label %58 [
    i32 0, label %45
    i32 2, label %48
  ], !dbg !513

45:                                               ; preds = %44
  %46 = load ptr, ptr @stderr, align 8, !dbg !514
  %47 = call i32 (ptr, ptr, ...) @fprintf(ptr %46, ptr @str3), !dbg !515
  br label %58, !dbg !516

48:                                               ; preds = %44
  %49 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 16, !dbg !517
  %50 = load i64, ptr %49, align 8, !dbg !518
  %51 = icmp sgt i64 %38, %50, !dbg !519
  br i1 %51, label %52, label %55, !dbg !520

52:                                               ; preds = %48
  %53 = load ptr, ptr @stderr, align 8, !dbg !521
  %54 = call i32 (ptr, ptr, ...) @fprintf(ptr %53, ptr @str4), !dbg !522
  br label %55, !dbg !523

55:                                               ; preds = %52, %48
  %56 = load ptr, ptr @stderr, align 8, !dbg !524
  %57 = call i32 (ptr, ptr, ...) @fprintf(ptr %56, ptr @str3), !dbg !525
  br label %58, !dbg !526

58:                                               ; preds = %45, %55, %44
  %59 = phi i1 [ false, %55 ], [ false, %45 ], [ %15, %44 ]
  %60 = phi i64 [ 1, %55 ], [ 1, %45 ], [ %16, %44 ]
  %61 = phi i1 [ false, %55 ], [ false, %45 ], [ true, %44 ]
  br label %62, !dbg !527

62:                                               ; preds = %58
  %63 = phi i1 [ %59, %58 ]
  %64 = phi i64 [ %60, %58 ]
  %65 = phi i1 [ %61, %58 ]
  br i1 %65, label %66, label %68, !dbg !528

66:                                               ; preds = %62
  %67 = getelementptr %_Lowered_rarc, ptr %17, i32 1, !dbg !529
  br label %69, !dbg !530

68:                                               ; preds = %62
  br label %69, !dbg !531

69:                                               ; preds = %66, %68
  %70 = phi ptr [ %17, %68 ], [ %67, %66 ]
  br label %71, !dbg !532

71:                                               ; preds = %69
  br label %6, !dbg !533

72:                                               ; preds = %6
  ret i64 0, !dbg !534
}

define i64 @getfree(ptr %0) !dbg !535 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !536
  %3 = load ptr, ptr %2, align 8, !dbg !538
  %4 = icmp ne ptr %3, null, !dbg !539
  br i1 %4, label %5, label %7, !dbg !540

5:                                                ; preds = %1
  %6 = load ptr, ptr %2, align 8, !dbg !541
  call void @free(ptr %6), !dbg !542
  br label %7, !dbg !543

7:                                                ; preds = %5, %1
  %8 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !544
  %9 = load ptr, ptr %8, align 8, !dbg !545
  %10 = icmp ne ptr %9, null, !dbg !546
  br i1 %10, label %11, label %13, !dbg !547

11:                                               ; preds = %7
  %12 = load ptr, ptr %8, align 8, !dbg !548
  call void @free(ptr %12), !dbg !549
  br label %13, !dbg !550

13:                                               ; preds = %11, %7
  %14 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 25, !dbg !551
  %15 = load ptr, ptr %14, align 8, !dbg !552
  %16 = icmp ne ptr %15, null, !dbg !553
  br i1 %16, label %17, label %19, !dbg !554

17:                                               ; preds = %13
  %18 = load ptr, ptr %14, align 8, !dbg !555
  call void @free(ptr %18), !dbg !556
  br label %19, !dbg !557

19:                                               ; preds = %17, %13
  %20 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !558
  store ptr null, ptr %20, align 8, !dbg !559
  %21 = load ptr, ptr %20, align 8, !dbg !560
  store ptr %21, ptr %2, align 8, !dbg !561
  %22 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !562
  store ptr null, ptr %22, align 8, !dbg !563
  %23 = load ptr, ptr %22, align 8, !dbg !564
  store ptr %23, ptr %8, align 8, !dbg !565
  %24 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 26, !dbg !566
  store ptr null, ptr %24, align 8, !dbg !567
  %25 = load ptr, ptr %24, align 8, !dbg !568
  store ptr %25, ptr %14, align 8, !dbg !569
  ret i64 0, !dbg !570
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "refresh_neighbour_lists", linkageName: "refresh_neighbour_lists", scope: null, file: !4, line: 14, type: !5, scopeLine: 14, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/mcfutil.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 15, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 16, column: 10, scope: !8)
!10 = !DILocation(line: 17, column: 10, scope: !8)
!11 = !DILocation(line: 18, column: 10, scope: !8)
!12 = !DILocation(line: 21, column: 5, scope: !8)
!13 = !DILocation(line: 23, column: 10, scope: !8)
!14 = !DILocation(line: 24, column: 5, scope: !8)
!15 = !DILocation(line: 26, column: 10, scope: !8)
!16 = !DILocation(line: 27, column: 5, scope: !8)
!17 = !DILocation(line: 28, column: 11, scope: !8)
!18 = !DILocation(line: 29, column: 5, scope: !8)
!19 = !DILocation(line: 30, column: 11, scope: !8)
!20 = !DILocation(line: 31, column: 5, scope: !8)
!21 = !DILocation(line: 33, column: 11, scope: !8)
!22 = !DILocation(line: 34, column: 11, scope: !8)
!23 = !DILocation(line: 35, column: 11, scope: !8)
!24 = !DILocation(line: 36, column: 11, scope: !8)
!25 = !DILocation(line: 37, column: 5, scope: !8)
!26 = !DILocation(line: 39, column: 11, scope: !8)
!27 = !DILocation(line: 40, column: 5, scope: !8)
!28 = !DILocation(line: 42, column: 11, scope: !8)
!29 = !DILocation(line: 43, column: 11, scope: !8)
!30 = !DILocation(line: 44, column: 11, scope: !8)
!31 = !DILocation(line: 45, column: 11, scope: !8)
!32 = !DILocation(line: 46, column: 11, scope: !8)
!33 = !DILocation(line: 48, column: 11, scope: !8)
!34 = !DILocation(line: 49, column: 11, scope: !8)
!35 = !DILocation(line: 50, column: 11, scope: !8)
!36 = !DILocation(line: 51, column: 11, scope: !8)
!37 = !DILocation(line: 52, column: 11, scope: !8)
!38 = !DILocation(line: 53, column: 11, scope: !8)
!39 = !DILocation(line: 55, column: 5, scope: !8)
!40 = !DILocation(line: 56, column: 11, scope: !8)
!41 = !DILocation(line: 57, column: 11, scope: !8)
!42 = !DILocation(line: 59, column: 11, scope: !8)
!43 = !DILocation(line: 60, column: 11, scope: !8)
!44 = !DILocation(line: 61, column: 5, scope: !8)
!45 = !DILocation(line: 62, column: 11, scope: !8)
!46 = !DILocation(line: 63, column: 11, scope: !8)
!47 = !DILocation(line: 64, column: 11, scope: !8)
!48 = !DILocation(line: 65, column: 11, scope: !8)
!49 = !DILocation(line: 66, column: 11, scope: !8)
!50 = !DILocation(line: 68, column: 11, scope: !8)
!51 = !DILocation(line: 69, column: 11, scope: !8)
!52 = !DILocation(line: 70, column: 11, scope: !8)
!53 = !DILocation(line: 71, column: 11, scope: !8)
!54 = !DILocation(line: 72, column: 11, scope: !8)
!55 = !DILocation(line: 73, column: 11, scope: !8)
!56 = !DILocation(line: 75, column: 5, scope: !8)
!57 = !DILocation(line: 76, column: 11, scope: !8)
!58 = !DILocation(line: 77, column: 11, scope: !8)
!59 = !DILocation(line: 79, column: 11, scope: !8)
!60 = !DILocation(line: 80, column: 11, scope: !8)
!61 = !DILocation(line: 81, column: 5, scope: !8)
!62 = !DILocation(line: 82, column: 11, scope: !8)
!63 = !DILocation(line: 83, column: 5, scope: !8)
!64 = !DILocation(line: 85, column: 5, scope: !8)
!65 = distinct !DISubprogram(name: "refresh_potential", linkageName: "refresh_potential", scope: null, file: !4, line: 87, type: !5, scopeLine: 87, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!66 = !DILocation(line: 94, column: 10, scope: !67)
!67 = !DILexicalBlockFile(scope: !65, file: !4, discriminator: 0)
!68 = !DILocation(line: 95, column: 10, scope: !67)
!69 = !DILocation(line: 96, column: 10, scope: !67)
!70 = !DILocation(line: 97, column: 5, scope: !67)
!71 = !DILocation(line: 98, column: 10, scope: !67)
!72 = !DILocation(line: 99, column: 11, scope: !67)
!73 = !DILocation(line: 101, column: 5, scope: !67)
!74 = !DILocation(line: 103, column: 11, scope: !67)
!75 = !DILocation(line: 104, column: 5, scope: !67)
!76 = !DILocation(line: 106, column: 5, scope: !67)
!77 = !DILocation(line: 108, column: 11, scope: !67)
!78 = !DILocation(line: 109, column: 5, scope: !67)
!79 = !DILocation(line: 111, column: 11, scope: !67)
!80 = !DILocation(line: 112, column: 11, scope: !67)
!81 = !DILocation(line: 113, column: 11, scope: !67)
!82 = !DILocation(line: 114, column: 5, scope: !67)
!83 = !DILocation(line: 116, column: 11, scope: !67)
!84 = !DILocation(line: 117, column: 11, scope: !67)
!85 = !DILocation(line: 118, column: 11, scope: !67)
!86 = !DILocation(line: 119, column: 11, scope: !67)
!87 = !DILocation(line: 120, column: 11, scope: !67)
!88 = !DILocation(line: 121, column: 11, scope: !67)
!89 = !DILocation(line: 122, column: 11, scope: !67)
!90 = !DILocation(line: 124, column: 11, scope: !67)
!91 = !DILocation(line: 125, column: 11, scope: !67)
!92 = !DILocation(line: 126, column: 11, scope: !67)
!93 = !DILocation(line: 127, column: 11, scope: !67)
!94 = !DILocation(line: 128, column: 11, scope: !67)
!95 = !DILocation(line: 129, column: 11, scope: !67)
!96 = !DILocation(line: 130, column: 5, scope: !67)
!97 = !DILocation(line: 131, column: 5, scope: !67)
!98 = !DILocation(line: 133, column: 11, scope: !67)
!99 = !DILocation(line: 134, column: 11, scope: !67)
!100 = !DILocation(line: 135, column: 11, scope: !67)
!101 = !DILocation(line: 136, column: 11, scope: !67)
!102 = !DILocation(line: 137, column: 11, scope: !67)
!103 = !DILocation(line: 138, column: 11, scope: !67)
!104 = !DILocation(line: 139, column: 11, scope: !67)
!105 = !DILocation(line: 140, column: 11, scope: !67)
!106 = !DILocation(line: 141, column: 11, scope: !67)
!107 = !DILocation(line: 142, column: 11, scope: !67)
!108 = !DILocation(line: 143, column: 11, scope: !67)
!109 = !DILocation(line: 145, column: 11, scope: !67)
!110 = !DILocation(line: 146, column: 11, scope: !67)
!111 = !DILocation(line: 147, column: 5, scope: !67)
!112 = !DILocation(line: 148, column: 11, scope: !67)
!113 = !DILocation(line: 149, column: 5, scope: !67)
!114 = !DILocation(line: 151, column: 5, scope: !67)
!115 = !DILocation(line: 153, column: 11, scope: !67)
!116 = !DILocation(line: 154, column: 11, scope: !67)
!117 = !DILocation(line: 156, column: 5, scope: !67)
!118 = !DILocation(line: 158, column: 5, scope: !67)
!119 = !DILocation(line: 160, column: 5, scope: !67)
!120 = !DILocation(line: 162, column: 5, scope: !67)
!121 = !DILocation(line: 164, column: 5, scope: !67)
!122 = !DILocation(line: 166, column: 5, scope: !67)
!123 = !DILocation(line: 168, column: 11, scope: !67)
!124 = !DILocation(line: 169, column: 11, scope: !67)
!125 = !DILocation(line: 170, column: 11, scope: !67)
!126 = !DILocation(line: 171, column: 11, scope: !67)
!127 = !DILocation(line: 172, column: 5, scope: !67)
!128 = !DILocation(line: 174, column: 5, scope: !67)
!129 = !DILocation(line: 176, column: 11, scope: !67)
!130 = !DILocation(line: 177, column: 11, scope: !67)
!131 = !DILocation(line: 178, column: 11, scope: !67)
!132 = !DILocation(line: 179, column: 11, scope: !67)
!133 = !DILocation(line: 180, column: 5, scope: !67)
!134 = !DILocation(line: 182, column: 5, scope: !67)
!135 = !DILocation(line: 184, column: 11, scope: !67)
!136 = !DILocation(line: 185, column: 5, scope: !67)
!137 = !DILocation(line: 187, column: 5, scope: !67)
!138 = !DILocation(line: 189, column: 5, scope: !67)
!139 = !DILocation(line: 191, column: 5, scope: !67)
!140 = !DILocation(line: 193, column: 5, scope: !67)
!141 = !DILocation(line: 196, column: 5, scope: !67)
!142 = !DILocation(line: 198, column: 5, scope: !67)
!143 = !DILocation(line: 200, column: 5, scope: !67)
!144 = !DILocation(line: 202, column: 5, scope: !67)
!145 = !DILocation(line: 204, column: 5, scope: !67)
!146 = !DILocation(line: 206, column: 5, scope: !67)
!147 = !DILocation(line: 208, column: 5, scope: !67)
!148 = distinct !DISubprogram(name: "flow_cost", linkageName: "flow_cost", scope: null, file: !4, line: 210, type: !5, scopeLine: 210, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!149 = !DILocation(line: 216, column: 10, scope: !150)
!150 = !DILexicalBlockFile(scope: !148, file: !4, discriminator: 0)
!151 = !DILocation(line: 217, column: 10, scope: !150)
!152 = !DILocation(line: 218, column: 10, scope: !150)
!153 = !DILocation(line: 219, column: 10, scope: !150)
!154 = !DILocation(line: 220, column: 5, scope: !150)
!155 = !DILocation(line: 222, column: 11, scope: !150)
!156 = !DILocation(line: 223, column: 5, scope: !150)
!157 = !DILocation(line: 225, column: 11, scope: !150)
!158 = !DILocation(line: 226, column: 11, scope: !150)
!159 = !DILocation(line: 227, column: 11, scope: !150)
!160 = !DILocation(line: 228, column: 11, scope: !150)
!161 = !DILocation(line: 230, column: 11, scope: !150)
!162 = !DILocation(line: 231, column: 11, scope: !150)
!163 = !DILocation(line: 232, column: 5, scope: !150)
!164 = !DILocation(line: 234, column: 11, scope: !150)
!165 = !DILocation(line: 235, column: 11, scope: !150)
!166 = !DILocation(line: 236, column: 11, scope: !150)
!167 = !DILocation(line: 237, column: 11, scope: !150)
!168 = !DILocation(line: 239, column: 5, scope: !150)
!169 = !DILocation(line: 240, column: 5, scope: !150)
!170 = !DILocation(line: 242, column: 11, scope: !150)
!171 = !DILocation(line: 243, column: 11, scope: !150)
!172 = !DILocation(line: 244, column: 11, scope: !150)
!173 = !DILocation(line: 245, column: 11, scope: !150)
!174 = !DILocation(line: 247, column: 5, scope: !150)
!175 = !DILocation(line: 248, column: 5, scope: !150)
!176 = !DILocation(line: 250, column: 11, scope: !150)
!177 = !DILocation(line: 251, column: 5, scope: !150)
!178 = !DILocation(line: 253, column: 11, scope: !150)
!179 = !DILocation(line: 254, column: 11, scope: !150)
!180 = !DILocation(line: 255, column: 11, scope: !150)
!181 = !DILocation(line: 256, column: 11, scope: !150)
!182 = !DILocation(line: 257, column: 11, scope: !150)
!183 = !DILocation(line: 258, column: 5, scope: !150)
!184 = !DILocation(line: 260, column: 11, scope: !150)
!185 = !DILocation(line: 261, column: 5, scope: !150)
!186 = !DILocation(line: 263, column: 11, scope: !150)
!187 = !DILocation(line: 264, column: 11, scope: !150)
!188 = !DILocation(line: 265, column: 11, scope: !150)
!189 = !DILocation(line: 266, column: 11, scope: !150)
!190 = !DILocation(line: 267, column: 11, scope: !150)
!191 = !DILocation(line: 268, column: 11, scope: !150)
!192 = !DILocation(line: 269, column: 11, scope: !150)
!193 = !DILocation(line: 270, column: 11, scope: !150)
!194 = !DILocation(line: 272, column: 5, scope: !150)
!195 = !DILocation(line: 273, column: 11, scope: !150)
!196 = !DILocation(line: 274, column: 5, scope: !150)
!197 = !DILocation(line: 276, column: 11, scope: !150)
!198 = !DILocation(line: 277, column: 11, scope: !150)
!199 = !DILocation(line: 278, column: 5, scope: !150)
!200 = !DILocation(line: 280, column: 11, scope: !150)
!201 = !DILocation(line: 281, column: 5, scope: !150)
!202 = !DILocation(line: 283, column: 11, scope: !150)
!203 = !DILocation(line: 284, column: 11, scope: !150)
!204 = !DILocation(line: 285, column: 11, scope: !150)
!205 = !DILocation(line: 286, column: 11, scope: !150)
!206 = !DILocation(line: 288, column: 11, scope: !150)
!207 = !DILocation(line: 289, column: 11, scope: !150)
!208 = !DILocation(line: 290, column: 5, scope: !150)
!209 = !DILocation(line: 292, column: 11, scope: !150)
!210 = !DILocation(line: 293, column: 11, scope: !150)
!211 = !DILocation(line: 294, column: 11, scope: !150)
!212 = !DILocation(line: 295, column: 11, scope: !150)
!213 = !DILocation(line: 297, column: 11, scope: !150)
!214 = !DILocation(line: 298, column: 11, scope: !150)
!215 = !DILocation(line: 299, column: 11, scope: !150)
!216 = !DILocation(line: 300, column: 11, scope: !150)
!217 = !DILocation(line: 301, column: 5, scope: !150)
!218 = !DILocation(line: 303, column: 11, scope: !150)
!219 = !DILocation(line: 304, column: 11, scope: !150)
!220 = !DILocation(line: 305, column: 11, scope: !150)
!221 = !DILocation(line: 306, column: 11, scope: !150)
!222 = !DILocation(line: 308, column: 11, scope: !150)
!223 = !DILocation(line: 309, column: 11, scope: !150)
!224 = !DILocation(line: 310, column: 11, scope: !150)
!225 = !DILocation(line: 311, column: 11, scope: !150)
!226 = !DILocation(line: 312, column: 5, scope: !150)
!227 = !DILocation(line: 314, column: 5, scope: !150)
!228 = !DILocation(line: 316, column: 5, scope: !150)
!229 = !DILocation(line: 318, column: 11, scope: !150)
!230 = !DILocation(line: 319, column: 11, scope: !150)
!231 = !DILocation(line: 320, column: 5, scope: !150)
!232 = !DILocation(line: 322, column: 11, scope: !150)
!233 = !DILocation(line: 323, column: 11, scope: !150)
!234 = !DILocation(line: 325, column: 11, scope: !150)
!235 = !DILocation(line: 326, column: 11, scope: !150)
!236 = !DILocation(line: 327, column: 11, scope: !150)
!237 = !DILocation(line: 328, column: 11, scope: !150)
!238 = !DILocation(line: 329, column: 5, scope: !150)
!239 = !DILocation(line: 331, column: 11, scope: !150)
!240 = !DILocation(line: 332, column: 11, scope: !150)
!241 = !DILocation(line: 333, column: 11, scope: !150)
!242 = !DILocation(line: 334, column: 11, scope: !150)
!243 = !DILocation(line: 336, column: 11, scope: !150)
!244 = !DILocation(line: 337, column: 11, scope: !150)
!245 = !DILocation(line: 338, column: 5, scope: !150)
!246 = !DILocation(line: 340, column: 11, scope: !150)
!247 = !DILocation(line: 341, column: 12, scope: !150)
!248 = !DILocation(line: 342, column: 12, scope: !150)
!249 = !DILocation(line: 343, column: 12, scope: !150)
!250 = !DILocation(line: 345, column: 12, scope: !150)
!251 = !DILocation(line: 346, column: 12, scope: !150)
!252 = !DILocation(line: 347, column: 12, scope: !150)
!253 = !DILocation(line: 348, column: 12, scope: !150)
!254 = !DILocation(line: 349, column: 12, scope: !150)
!255 = !DILocation(line: 350, column: 12, scope: !150)
!256 = !DILocation(line: 351, column: 5, scope: !150)
!257 = !DILocation(line: 353, column: 5, scope: !150)
!258 = !DILocation(line: 355, column: 5, scope: !150)
!259 = !DILocation(line: 357, column: 5, scope: !150)
!260 = !DILocation(line: 359, column: 5, scope: !150)
!261 = !DILocation(line: 361, column: 5, scope: !150)
!262 = !DILocation(line: 363, column: 5, scope: !150)
!263 = !DILocation(line: 365, column: 5, scope: !150)
!264 = !DILocation(line: 367, column: 12, scope: !150)
!265 = !DILocation(line: 368, column: 5, scope: !150)
!266 = !DILocation(line: 370, column: 12, scope: !150)
!267 = !DILocation(line: 371, column: 12, scope: !150)
!268 = !DILocation(line: 372, column: 12, scope: !150)
!269 = !DILocation(line: 373, column: 12, scope: !150)
!270 = !DILocation(line: 374, column: 12, scope: !150)
!271 = !DILocation(line: 375, column: 12, scope: !150)
!272 = !DILocation(line: 376, column: 12, scope: !150)
!273 = !DILocation(line: 377, column: 5, scope: !150)
!274 = distinct !DISubprogram(name: "flow_org_cost", linkageName: "flow_org_cost", scope: null, file: !4, line: 379, type: !5, scopeLine: 379, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!275 = !DILocation(line: 385, column: 10, scope: !276)
!276 = !DILexicalBlockFile(scope: !274, file: !4, discriminator: 0)
!277 = !DILocation(line: 386, column: 10, scope: !276)
!278 = !DILocation(line: 387, column: 10, scope: !276)
!279 = !DILocation(line: 388, column: 10, scope: !276)
!280 = !DILocation(line: 389, column: 5, scope: !276)
!281 = !DILocation(line: 391, column: 11, scope: !276)
!282 = !DILocation(line: 392, column: 5, scope: !276)
!283 = !DILocation(line: 394, column: 11, scope: !276)
!284 = !DILocation(line: 395, column: 11, scope: !276)
!285 = !DILocation(line: 396, column: 11, scope: !276)
!286 = !DILocation(line: 397, column: 11, scope: !276)
!287 = !DILocation(line: 399, column: 11, scope: !276)
!288 = !DILocation(line: 400, column: 11, scope: !276)
!289 = !DILocation(line: 401, column: 5, scope: !276)
!290 = !DILocation(line: 403, column: 11, scope: !276)
!291 = !DILocation(line: 404, column: 11, scope: !276)
!292 = !DILocation(line: 405, column: 11, scope: !276)
!293 = !DILocation(line: 406, column: 11, scope: !276)
!294 = !DILocation(line: 408, column: 5, scope: !276)
!295 = !DILocation(line: 409, column: 5, scope: !276)
!296 = !DILocation(line: 411, column: 11, scope: !276)
!297 = !DILocation(line: 412, column: 11, scope: !276)
!298 = !DILocation(line: 413, column: 11, scope: !276)
!299 = !DILocation(line: 414, column: 11, scope: !276)
!300 = !DILocation(line: 416, column: 5, scope: !276)
!301 = !DILocation(line: 417, column: 5, scope: !276)
!302 = !DILocation(line: 419, column: 11, scope: !276)
!303 = !DILocation(line: 420, column: 5, scope: !276)
!304 = !DILocation(line: 422, column: 11, scope: !276)
!305 = !DILocation(line: 423, column: 11, scope: !276)
!306 = !DILocation(line: 424, column: 11, scope: !276)
!307 = !DILocation(line: 425, column: 11, scope: !276)
!308 = !DILocation(line: 426, column: 11, scope: !276)
!309 = !DILocation(line: 427, column: 5, scope: !276)
!310 = !DILocation(line: 429, column: 11, scope: !276)
!311 = !DILocation(line: 430, column: 5, scope: !276)
!312 = !DILocation(line: 432, column: 11, scope: !276)
!313 = !DILocation(line: 433, column: 11, scope: !276)
!314 = !DILocation(line: 434, column: 11, scope: !276)
!315 = !DILocation(line: 435, column: 11, scope: !276)
!316 = !DILocation(line: 436, column: 11, scope: !276)
!317 = !DILocation(line: 437, column: 11, scope: !276)
!318 = !DILocation(line: 438, column: 11, scope: !276)
!319 = !DILocation(line: 439, column: 11, scope: !276)
!320 = !DILocation(line: 441, column: 5, scope: !276)
!321 = !DILocation(line: 442, column: 11, scope: !276)
!322 = !DILocation(line: 443, column: 5, scope: !276)
!323 = !DILocation(line: 445, column: 11, scope: !276)
!324 = !DILocation(line: 446, column: 11, scope: !276)
!325 = !DILocation(line: 447, column: 5, scope: !276)
!326 = !DILocation(line: 449, column: 11, scope: !276)
!327 = !DILocation(line: 450, column: 5, scope: !276)
!328 = !DILocation(line: 452, column: 11, scope: !276)
!329 = !DILocation(line: 453, column: 11, scope: !276)
!330 = !DILocation(line: 454, column: 11, scope: !276)
!331 = !DILocation(line: 455, column: 11, scope: !276)
!332 = !DILocation(line: 457, column: 11, scope: !276)
!333 = !DILocation(line: 458, column: 11, scope: !276)
!334 = !DILocation(line: 459, column: 5, scope: !276)
!335 = !DILocation(line: 461, column: 11, scope: !276)
!336 = !DILocation(line: 462, column: 11, scope: !276)
!337 = !DILocation(line: 463, column: 11, scope: !276)
!338 = !DILocation(line: 464, column: 11, scope: !276)
!339 = !DILocation(line: 466, column: 11, scope: !276)
!340 = !DILocation(line: 467, column: 11, scope: !276)
!341 = !DILocation(line: 468, column: 11, scope: !276)
!342 = !DILocation(line: 469, column: 11, scope: !276)
!343 = !DILocation(line: 470, column: 5, scope: !276)
!344 = !DILocation(line: 472, column: 11, scope: !276)
!345 = !DILocation(line: 473, column: 11, scope: !276)
!346 = !DILocation(line: 474, column: 11, scope: !276)
!347 = !DILocation(line: 475, column: 11, scope: !276)
!348 = !DILocation(line: 477, column: 11, scope: !276)
!349 = !DILocation(line: 478, column: 11, scope: !276)
!350 = !DILocation(line: 479, column: 11, scope: !276)
!351 = !DILocation(line: 480, column: 11, scope: !276)
!352 = !DILocation(line: 481, column: 5, scope: !276)
!353 = !DILocation(line: 483, column: 5, scope: !276)
!354 = !DILocation(line: 485, column: 5, scope: !276)
!355 = !DILocation(line: 487, column: 11, scope: !276)
!356 = !DILocation(line: 488, column: 11, scope: !276)
!357 = !DILocation(line: 489, column: 5, scope: !276)
!358 = !DILocation(line: 491, column: 11, scope: !276)
!359 = !DILocation(line: 492, column: 11, scope: !276)
!360 = !DILocation(line: 494, column: 11, scope: !276)
!361 = !DILocation(line: 495, column: 11, scope: !276)
!362 = !DILocation(line: 496, column: 11, scope: !276)
!363 = !DILocation(line: 497, column: 11, scope: !276)
!364 = !DILocation(line: 498, column: 5, scope: !276)
!365 = !DILocation(line: 500, column: 11, scope: !276)
!366 = !DILocation(line: 501, column: 11, scope: !276)
!367 = !DILocation(line: 502, column: 11, scope: !276)
!368 = !DILocation(line: 503, column: 11, scope: !276)
!369 = !DILocation(line: 505, column: 11, scope: !276)
!370 = !DILocation(line: 506, column: 11, scope: !276)
!371 = !DILocation(line: 507, column: 5, scope: !276)
!372 = !DILocation(line: 509, column: 11, scope: !276)
!373 = !DILocation(line: 510, column: 12, scope: !276)
!374 = !DILocation(line: 511, column: 12, scope: !276)
!375 = !DILocation(line: 512, column: 12, scope: !276)
!376 = !DILocation(line: 514, column: 12, scope: !276)
!377 = !DILocation(line: 515, column: 12, scope: !276)
!378 = !DILocation(line: 516, column: 12, scope: !276)
!379 = !DILocation(line: 517, column: 12, scope: !276)
!380 = !DILocation(line: 518, column: 12, scope: !276)
!381 = !DILocation(line: 519, column: 12, scope: !276)
!382 = !DILocation(line: 520, column: 5, scope: !276)
!383 = !DILocation(line: 522, column: 5, scope: !276)
!384 = !DILocation(line: 524, column: 5, scope: !276)
!385 = !DILocation(line: 526, column: 5, scope: !276)
!386 = !DILocation(line: 528, column: 5, scope: !276)
!387 = !DILocation(line: 530, column: 5, scope: !276)
!388 = !DILocation(line: 532, column: 5, scope: !276)
!389 = !DILocation(line: 534, column: 5, scope: !276)
!390 = !DILocation(line: 536, column: 12, scope: !276)
!391 = !DILocation(line: 537, column: 5, scope: !276)
!392 = !DILocation(line: 539, column: 12, scope: !276)
!393 = !DILocation(line: 540, column: 12, scope: !276)
!394 = !DILocation(line: 541, column: 12, scope: !276)
!395 = !DILocation(line: 542, column: 12, scope: !276)
!396 = !DILocation(line: 543, column: 12, scope: !276)
!397 = !DILocation(line: 544, column: 12, scope: !276)
!398 = !DILocation(line: 545, column: 12, scope: !276)
!399 = !DILocation(line: 546, column: 5, scope: !276)
!400 = distinct !DISubprogram(name: "primal_feasible", linkageName: "primal_feasible", scope: null, file: !4, line: 548, type: !5, scopeLine: 548, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!401 = !DILocation(line: 555, column: 10, scope: !402)
!402 = !DILexicalBlockFile(scope: !400, file: !4, discriminator: 0)
!403 = !DILocation(line: 556, column: 10, scope: !402)
!404 = !DILocation(line: 557, column: 10, scope: !402)
!405 = !DILocation(line: 558, column: 10, scope: !402)
!406 = !DILocation(line: 559, column: 11, scope: !402)
!407 = !DILocation(line: 560, column: 11, scope: !402)
!408 = !DILocation(line: 561, column: 11, scope: !402)
!409 = !DILocation(line: 562, column: 11, scope: !402)
!410 = !DILocation(line: 563, column: 11, scope: !402)
!411 = !DILocation(line: 564, column: 5, scope: !402)
!412 = !DILocation(line: 566, column: 11, scope: !402)
!413 = !DILocation(line: 567, column: 11, scope: !402)
!414 = !DILocation(line: 568, column: 5, scope: !402)
!415 = !DILocation(line: 570, column: 11, scope: !402)
!416 = !DILocation(line: 571, column: 11, scope: !402)
!417 = !DILocation(line: 572, column: 11, scope: !402)
!418 = !DILocation(line: 573, column: 11, scope: !402)
!419 = !DILocation(line: 574, column: 11, scope: !402)
!420 = !DILocation(line: 575, column: 5, scope: !402)
!421 = !DILocation(line: 577, column: 11, scope: !402)
!422 = !DILocation(line: 578, column: 5, scope: !402)
!423 = !DILocation(line: 580, column: 5, scope: !402)
!424 = !DILocation(line: 582, column: 5, scope: !402)
!425 = !DILocation(line: 584, column: 5, scope: !402)
!426 = !DILocation(line: 586, column: 11, scope: !402)
!427 = !DILocation(line: 587, column: 5, scope: !402)
!428 = !DILocation(line: 589, column: 5, scope: !402)
!429 = !DILocation(line: 591, column: 11, scope: !402)
!430 = !DILocation(line: 592, column: 5, scope: !402)
!431 = !DILocation(line: 594, column: 5, scope: !402)
!432 = !DILocation(line: 596, column: 11, scope: !402)
!433 = !DILocation(line: 597, column: 11, scope: !402)
!434 = !DILocation(line: 598, column: 11, scope: !402)
!435 = !DILocation(line: 599, column: 5, scope: !402)
!436 = !DILocation(line: 603, column: 11, scope: !402)
!437 = !DILocation(line: 606, column: 11, scope: !402)
!438 = !DILocation(line: 607, column: 11, scope: !402)
!439 = !DILocation(line: 608, column: 11, scope: !402)
!440 = !DILocation(line: 609, column: 5, scope: !402)
!441 = !DILocation(line: 611, column: 5, scope: !402)
!442 = !DILocation(line: 613, column: 11, scope: !402)
!443 = !DILocation(line: 614, column: 11, scope: !402)
!444 = !DILocation(line: 615, column: 11, scope: !402)
!445 = !DILocation(line: 616, column: 11, scope: !402)
!446 = !DILocation(line: 617, column: 5, scope: !402)
!447 = !DILocation(line: 619, column: 5, scope: !402)
!448 = !DILocation(line: 621, column: 11, scope: !402)
!449 = !DILocation(line: 622, column: 11, scope: !402)
!450 = !DILocation(line: 623, column: 11, scope: !402)
!451 = !DILocation(line: 624, column: 5, scope: !402)
!452 = !DILocation(line: 626, column: 5, scope: !402)
!453 = !DILocation(line: 628, column: 11, scope: !402)
!454 = !DILocation(line: 629, column: 11, scope: !402)
!455 = !DILocation(line: 630, column: 11, scope: !402)
!456 = !DILocation(line: 631, column: 5, scope: !402)
!457 = !DILocation(line: 635, column: 11, scope: !402)
!458 = !DILocation(line: 638, column: 11, scope: !402)
!459 = !DILocation(line: 639, column: 11, scope: !402)
!460 = !DILocation(line: 640, column: 5, scope: !402)
!461 = !DILocation(line: 641, column: 5, scope: !402)
!462 = !DILocation(line: 643, column: 5, scope: !402)
!463 = !DILocation(line: 645, column: 5, scope: !402)
!464 = !DILocation(line: 647, column: 5, scope: !402)
!465 = !DILocation(line: 649, column: 11, scope: !402)
!466 = !DILocation(line: 650, column: 5, scope: !402)
!467 = !DILocation(line: 652, column: 5, scope: !402)
!468 = !DILocation(line: 654, column: 5, scope: !402)
!469 = !DILocation(line: 656, column: 5, scope: !402)
!470 = !DILocation(line: 658, column: 11, scope: !402)
!471 = !DILocation(line: 659, column: 5, scope: !402)
!472 = !DILocation(line: 661, column: 11, scope: !402)
!473 = !DILocation(line: 662, column: 5, scope: !402)
!474 = !DILocation(line: 663, column: 5, scope: !402)
!475 = !DILocation(line: 665, column: 5, scope: !402)
!476 = distinct !DISubprogram(name: "dual_feasible", linkageName: "dual_feasible", scope: null, file: !4, line: 667, type: !5, scopeLine: 667, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!477 = !DILocation(line: 673, column: 10, scope: !478)
!478 = !DILexicalBlockFile(scope: !476, file: !4, discriminator: 0)
!479 = !DILocation(line: 674, column: 10, scope: !478)
!480 = !DILocation(line: 675, column: 10, scope: !478)
!481 = !DILocation(line: 676, column: 10, scope: !478)
!482 = !DILocation(line: 677, column: 5, scope: !478)
!483 = !DILocation(line: 679, column: 11, scope: !478)
!484 = !DILocation(line: 680, column: 11, scope: !478)
!485 = !DILocation(line: 681, column: 5, scope: !478)
!486 = !DILocation(line: 683, column: 11, scope: !478)
!487 = !DILocation(line: 684, column: 11, scope: !478)
!488 = !DILocation(line: 685, column: 11, scope: !478)
!489 = !DILocation(line: 686, column: 11, scope: !478)
!490 = !DILocation(line: 688, column: 11, scope: !478)
!491 = !DILocation(line: 689, column: 11, scope: !478)
!492 = !DILocation(line: 690, column: 11, scope: !478)
!493 = !DILocation(line: 691, column: 11, scope: !478)
!494 = !DILocation(line: 692, column: 11, scope: !478)
!495 = !DILocation(line: 694, column: 11, scope: !478)
!496 = !DILocation(line: 695, column: 11, scope: !478)
!497 = !DILocation(line: 696, column: 11, scope: !478)
!498 = !DILocation(line: 697, column: 11, scope: !478)
!499 = !DILocation(line: 698, column: 11, scope: !478)
!500 = !DILocation(line: 699, column: 11, scope: !478)
!501 = !DILocation(line: 700, column: 11, scope: !478)
!502 = !DILocation(line: 701, column: 11, scope: !478)
!503 = !DILocation(line: 703, column: 11, scope: !478)
!504 = !DILocation(line: 704, column: 11, scope: !478)
!505 = !DILocation(line: 705, column: 11, scope: !478)
!506 = !DILocation(line: 706, column: 11, scope: !478)
!507 = !DILocation(line: 707, column: 11, scope: !478)
!508 = !DILocation(line: 708, column: 11, scope: !478)
!509 = !DILocation(line: 709, column: 11, scope: !478)
!510 = !DILocation(line: 710, column: 11, scope: !478)
!511 = !DILocation(line: 712, column: 11, scope: !478)
!512 = !DILocation(line: 713, column: 5, scope: !478)
!513 = !DILocation(line: 715, column: 5, scope: !478)
!514 = !DILocation(line: 721, column: 11, scope: !478)
!515 = !DILocation(line: 724, column: 11, scope: !478)
!516 = !DILocation(line: 725, column: 5, scope: !478)
!517 = !DILocation(line: 727, column: 11, scope: !478)
!518 = !DILocation(line: 728, column: 11, scope: !478)
!519 = !DILocation(line: 729, column: 11, scope: !478)
!520 = !DILocation(line: 730, column: 5, scope: !478)
!521 = !DILocation(line: 733, column: 11, scope: !478)
!522 = !DILocation(line: 736, column: 11, scope: !478)
!523 = !DILocation(line: 737, column: 5, scope: !478)
!524 = !DILocation(line: 740, column: 11, scope: !478)
!525 = !DILocation(line: 743, column: 11, scope: !478)
!526 = !DILocation(line: 744, column: 5, scope: !478)
!527 = !DILocation(line: 746, column: 5, scope: !478)
!528 = !DILocation(line: 748, column: 5, scope: !478)
!529 = !DILocation(line: 750, column: 11, scope: !478)
!530 = !DILocation(line: 751, column: 5, scope: !478)
!531 = !DILocation(line: 753, column: 5, scope: !478)
!532 = !DILocation(line: 755, column: 5, scope: !478)
!533 = !DILocation(line: 757, column: 5, scope: !478)
!534 = !DILocation(line: 759, column: 5, scope: !478)
!535 = distinct !DISubprogram(name: "getfree", linkageName: "getfree", scope: null, file: !4, line: 761, type: !5, scopeLine: 761, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!536 = !DILocation(line: 763, column: 10, scope: !537)
!537 = !DILexicalBlockFile(scope: !535, file: !4, discriminator: 0)
!538 = !DILocation(line: 764, column: 10, scope: !537)
!539 = !DILocation(line: 766, column: 10, scope: !537)
!540 = !DILocation(line: 767, column: 5, scope: !537)
!541 = !DILocation(line: 769, column: 10, scope: !537)
!542 = !DILocation(line: 771, column: 5, scope: !537)
!543 = !DILocation(line: 772, column: 5, scope: !537)
!544 = !DILocation(line: 774, column: 10, scope: !537)
!545 = !DILocation(line: 775, column: 10, scope: !537)
!546 = !DILocation(line: 777, column: 11, scope: !537)
!547 = !DILocation(line: 778, column: 5, scope: !537)
!548 = !DILocation(line: 780, column: 11, scope: !537)
!549 = !DILocation(line: 782, column: 5, scope: !537)
!550 = !DILocation(line: 783, column: 5, scope: !537)
!551 = !DILocation(line: 785, column: 11, scope: !537)
!552 = !DILocation(line: 786, column: 11, scope: !537)
!553 = !DILocation(line: 787, column: 11, scope: !537)
!554 = !DILocation(line: 788, column: 5, scope: !537)
!555 = !DILocation(line: 790, column: 11, scope: !537)
!556 = !DILocation(line: 792, column: 5, scope: !537)
!557 = !DILocation(line: 793, column: 5, scope: !537)
!558 = !DILocation(line: 795, column: 11, scope: !537)
!559 = !DILocation(line: 796, column: 5, scope: !537)
!560 = !DILocation(line: 797, column: 11, scope: !537)
!561 = !DILocation(line: 798, column: 5, scope: !537)
!562 = !DILocation(line: 799, column: 11, scope: !537)
!563 = !DILocation(line: 800, column: 5, scope: !537)
!564 = !DILocation(line: 801, column: 11, scope: !537)
!565 = !DILocation(line: 802, column: 5, scope: !537)
!566 = !DILocation(line: 803, column: 11, scope: !537)
!567 = !DILocation(line: 804, column: 5, scope: !537)
!568 = !DILocation(line: 805, column: 11, scope: !537)
!569 = !DILocation(line: 806, column: 5, scope: !537)
!570 = !DILocation(line: 807, column: 5, scope: !537)
