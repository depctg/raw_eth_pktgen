; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

@stdout = external global ptr
@str3 = internal constant [31 x i8] c"network %s: not enough memory\0A\00"
@str2 = internal constant [30 x i8] c"long resize_prob(network_t *)\00"
@str1 = internal constant [11 x i8] c"implicit.c\00"
@str0 = internal constant [20 x i8] c"net->max_new_m >= 3\00"

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access(i128)

declare ptr @cache_access_mut(i128)

declare i128 @cache_request(i64)

declare i32 @printf(ptr, ...)

declare void @__assert_fail(ptr, ptr, i32, ptr)

define i64 @resize_prob(ptr %0) !dbg !3 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 9, !dbg !7
  %3 = load i64, ptr %2, align 8, !dbg !9
  %4 = icmp sge i64 %3, 3, !dbg !10
  br i1 %4, label %5, label %6, !dbg !11

5:                                                ; preds = %1
  br label %7, !dbg !12

6:                                                ; preds = %1
  call void @__assert_fail(ptr @str0, ptr @str1, i32 38, ptr @str2), !dbg !13
  br label %7, !dbg !14

7:                                                ; preds = %5, %6
  %8 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 4, !dbg !15
  %9 = load i64, ptr %2, align 8, !dbg !16
  %10 = load i64, ptr %8, align 8, !dbg !17
  %11 = add i64 %10, %9, !dbg !18
  store i64 %11, ptr %8, align 8, !dbg !19
  %12 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !20
  %13 = load i64, ptr %2, align 8, !dbg !21
  %14 = load i64, ptr %12, align 8, !dbg !22
  %15 = add i64 %14, %13, !dbg !23
  store i64 %15, ptr %12, align 8, !dbg !24
  %16 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !25
  %17 = load ptr, ptr %16, align 8, !dbg !26
  %18 = icmp eq ptr %17, null, !dbg !27
  %19 = xor i1 %18, true, !dbg !28
  %20 = select i1 %18, i64 -1, i64 undef, !dbg !29
  br i1 %18, label %21, label %27, !dbg !30

21:                                               ; preds = %7
  %22 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 0, !dbg !31
  %23 = getelementptr [200 x i8], ptr %22, i32 0, i32 0, !dbg !32
  %24 = call i32 (ptr, ...) @printf(ptr @str3, ptr %23), !dbg !33
  %25 = load ptr, ptr @stdout, align 8, !dbg !34
  %26 = call i32 @fflush(ptr %25), !dbg !35
  br label %27, !dbg !36

27:                                               ; preds = %21, %7
  %28 = select i1 %19, i64 0, i64 %20, !dbg !37
  br i1 %19, label %29, label %60, !dbg !38

29:                                               ; preds = %27
  %30 = ptrtoint ptr %17 to i64, !dbg !39
  %31 = load ptr, ptr %16, align 8, !dbg !40
  %32 = ptrtoint ptr %31 to i64, !dbg !41
  %33 = sub i64 %30, %32, !dbg !42
  store ptr %17, ptr %16, align 8, !dbg !43
  %34 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !44
  %35 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !45
  %36 = load i64, ptr %35, align 8, !dbg !46
  %37 = getelementptr %_Lowered_rarc, ptr %17, i64 %36, !dbg !47
  store ptr %37, ptr %34, align 8, !dbg !48
  %38 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 21, !dbg !49
  %39 = load ptr, ptr %38, align 8, !dbg !50
  %40 = getelementptr %_Lowered_node, ptr %39, i32 1, !dbg !51
  %41 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 22, !dbg !52
  %42 = load ptr, ptr %41, align 8, !dbg !53
  br label %43, !dbg !54

43:                                               ; preds = %57, %29
  %44 = phi ptr [ %58, %57 ], [ %40, %29 ]
  %45 = icmp ult ptr %44, %42, !dbg !55
  br i1 %45, label %46, label %59, !dbg !56

46:                                               ; preds = %43
  %47 = phi ptr [ %44, %43 ]
  %48 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 3, !dbg !57
  %49 = load ptr, ptr %48, align 8, !dbg !58
  %50 = icmp ne ptr %49, %39, !dbg !59
  br i1 %50, label %51, label %57, !dbg !60

51:                                               ; preds = %46
  %52 = getelementptr %_Lowered_node, ptr %47, i32 0, i32 6, !dbg !61
  %53 = load ptr, ptr %52, align 8, !dbg !62
  %54 = ptrtoint ptr %53 to i64, !dbg !63
  %55 = add i64 %54, %33, !dbg !64
  %56 = inttoptr i64 %55 to ptr, !dbg !65
  store ptr %56, ptr %52, align 8, !dbg !66
  br label %57, !dbg !67

57:                                               ; preds = %51, %46
  %58 = getelementptr %_Lowered_node, ptr %47, i32 1, !dbg !68
  br label %43, !dbg !69

59:                                               ; preds = %43
  br label %60, !dbg !70

60:                                               ; preds = %59, %27
  ret i64 %28, !dbg !71
}

declare i32 @fflush(ptr)

define void @replace_weaker_arc(ptr %0, ptr %1, ptr %2, ptr %3, i64 %4, i64 %5) !dbg !72 {
  %7 = getelementptr %_Lowered_rarc, ptr %1, i32 0, i32 1, !dbg !73
  %8 = ptrtoint ptr %7 to i64, !dbg !75
  %9 = call i128 @cache_request(i64 %8), !dbg !76
  %10 = call ptr @cache_access_mut(i128 %9), !dbg !77
  store ptr %2, ptr %10, align 8, !dbg !78
  %11 = getelementptr %_Lowered_rarc, ptr %1, i32 0, i32 2, !dbg !79
  %12 = ptrtoint ptr %11 to i64, !dbg !80
  %13 = call i128 @cache_request(i64 %12), !dbg !81
  %14 = call ptr @cache_access_mut(i128 %13), !dbg !82
  store ptr %3, ptr %14, align 8, !dbg !83
  %15 = getelementptr %_Lowered_rarc, ptr %1, i32 0, i32 7, !dbg !84
  %16 = ptrtoint ptr %15 to i64, !dbg !85
  %17 = call i128 @cache_request(i64 %16), !dbg !86
  %18 = call ptr @cache_access_mut(i128 %17), !dbg !87
  store i64 %4, ptr %18, align 8, !dbg !88
  %19 = getelementptr %_Lowered_rarc, ptr %1, i32 0, i32 0, !dbg !89
  %20 = ptrtoint ptr %19 to i64, !dbg !90
  %21 = call i128 @cache_request(i64 %20), !dbg !91
  %22 = call ptr @cache_access_mut(i128 %21), !dbg !92
  store i64 %4, ptr %22, align 8, !dbg !93
  %23 = getelementptr %_Lowered_rarc, ptr %1, i32 0, i32 6, !dbg !94
  %24 = ptrtoint ptr %23 to i64, !dbg !95
  %25 = call i128 @cache_request(i64 %24), !dbg !96
  %26 = call ptr @cache_access_mut(i128 %25), !dbg !97
  store i64 %5, ptr %26, align 8, !dbg !98
  %27 = getelementptr %_Lowered_rarc, ptr %1, i32 1, !dbg !99
  %28 = getelementptr %_Lowered_rarc, ptr %27, i32 0, i32 6, !dbg !100
  %29 = ptrtoint ptr %28 to i64, !dbg !101
  %30 = call i128 @cache_request(i64 %29), !dbg !102
  %31 = call ptr @cache_access(i128 %30), !dbg !103
  %32 = load i64, ptr %31, align 8, !dbg !104
  %33 = getelementptr %_Lowered_rarc, ptr %1, i32 2, !dbg !105
  %34 = getelementptr %_Lowered_rarc, ptr %33, i32 0, i32 6, !dbg !106
  %35 = ptrtoint ptr %34 to i64, !dbg !107
  %36 = call i128 @cache_request(i64 %35), !dbg !108
  %37 = call ptr @cache_access(i128 %36), !dbg !109
  %38 = load i64, ptr %37, align 8, !dbg !110
  %39 = icmp sgt i64 %32, %38, !dbg !111
  %40 = select i1 %39, i32 2, i32 3, !dbg !112
  %41 = sext i32 %40 to i64, !dbg !113
  %42 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !114
  br label %43, !dbg !115

43:                                               ; preds = %147, %6
  %44 = phi i64 [ %145, %147 ], [ %41, %6 ]
  %45 = phi i64 [ %146, %147 ], [ 1, %6 ]
  %46 = load i64, ptr %42, align 8, !dbg !116
  %47 = icmp sle i64 %44, %46, !dbg !117
  br i1 %47, label %48, label %142, !dbg !118

48:                                               ; preds = %43
  %49 = add i64 %44, -1, !dbg !119
  %50 = getelementptr %_Lowered_rarc, ptr %1, i64 %49, !dbg !120
  %51 = getelementptr %_Lowered_rarc, ptr %50, i32 0, i32 6, !dbg !121
  %52 = ptrtoint ptr %51 to i64, !dbg !122
  %53 = call i128 @cache_request(i64 %52), !dbg !123
  %54 = call ptr @cache_access(i128 %53), !dbg !124
  %55 = load i64, ptr %54, align 8, !dbg !125
  %56 = icmp slt i64 %5, %55, !dbg !126
  %57 = select i1 %56, i64 %44, i64 %45, !dbg !127
  br i1 %56, label %58, label %138, !dbg !128

58:                                               ; preds = %48
  %59 = add i64 %45, -1, !dbg !129
  %60 = getelementptr %_Lowered_rarc, ptr %1, i64 %59, !dbg !130
  %61 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 1, !dbg !131
  %62 = getelementptr %_Lowered_rarc, ptr %50, i32 0, i32 1, !dbg !132
  %63 = ptrtoint ptr %62 to i64, !dbg !133
  %64 = call i128 @cache_request(i64 %63), !dbg !134
  %65 = call ptr @cache_access(i128 %64), !dbg !135
  %66 = load ptr, ptr %65, align 8, !dbg !136
  %67 = ptrtoint ptr %61 to i64, !dbg !137
  %68 = call i128 @cache_request(i64 %67), !dbg !138
  %69 = call ptr @cache_access_mut(i128 %68), !dbg !139
  store ptr %66, ptr %69, align 8, !dbg !140
  %70 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 2, !dbg !141
  %71 = getelementptr %_Lowered_rarc, ptr %50, i32 0, i32 2, !dbg !142
  %72 = ptrtoint ptr %71 to i64, !dbg !143
  %73 = call i128 @cache_request(i64 %72), !dbg !144
  %74 = call ptr @cache_access(i128 %73), !dbg !145
  %75 = load ptr, ptr %74, align 8, !dbg !146
  %76 = ptrtoint ptr %70 to i64, !dbg !147
  %77 = call i128 @cache_request(i64 %76), !dbg !148
  %78 = call ptr @cache_access_mut(i128 %77), !dbg !149
  store ptr %75, ptr %78, align 8, !dbg !150
  %79 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 0, !dbg !151
  %80 = getelementptr %_Lowered_rarc, ptr %50, i32 0, i32 0, !dbg !152
  %81 = ptrtoint ptr %80 to i64, !dbg !153
  %82 = call i128 @cache_request(i64 %81), !dbg !154
  %83 = call ptr @cache_access(i128 %82), !dbg !155
  %84 = load i64, ptr %83, align 8, !dbg !156
  %85 = ptrtoint ptr %79 to i64, !dbg !157
  %86 = call i128 @cache_request(i64 %85), !dbg !158
  %87 = call ptr @cache_access_mut(i128 %86), !dbg !159
  store i64 %84, ptr %87, align 8, !dbg !160
  %88 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 7, !dbg !161
  %89 = call i128 @cache_request(i64 %81), !dbg !162
  %90 = call ptr @cache_access(i128 %89), !dbg !163
  %91 = load i64, ptr %90, align 8, !dbg !164
  %92 = ptrtoint ptr %88 to i64, !dbg !165
  %93 = call i128 @cache_request(i64 %92), !dbg !166
  %94 = call ptr @cache_access_mut(i128 %93), !dbg !167
  store i64 %91, ptr %94, align 8, !dbg !168
  %95 = getelementptr %_Lowered_rarc, ptr %60, i32 0, i32 6, !dbg !169
  %96 = call i128 @cache_request(i64 %52), !dbg !170
  %97 = call ptr @cache_access(i128 %96), !dbg !171
  %98 = load i64, ptr %97, align 8, !dbg !172
  %99 = ptrtoint ptr %95 to i64, !dbg !173
  %100 = call i128 @cache_request(i64 %99), !dbg !174
  %101 = call ptr @cache_access_mut(i128 %100), !dbg !175
  store i64 %98, ptr %101, align 8, !dbg !176
  %102 = call i128 @cache_request(i64 %63), !dbg !177
  %103 = call ptr @cache_access_mut(i128 %102), !dbg !178
  store ptr %2, ptr %103, align 8, !dbg !179
  %104 = call i128 @cache_request(i64 %72), !dbg !180
  %105 = call ptr @cache_access_mut(i128 %104), !dbg !181
  store ptr %3, ptr %105, align 8, !dbg !182
  %106 = call i128 @cache_request(i64 %81), !dbg !183
  %107 = call ptr @cache_access_mut(i128 %106), !dbg !184
  store i64 %4, ptr %107, align 8, !dbg !185
  %108 = getelementptr %_Lowered_rarc, ptr %50, i32 0, i32 7, !dbg !186
  %109 = ptrtoint ptr %108 to i64, !dbg !187
  %110 = call i128 @cache_request(i64 %109), !dbg !188
  %111 = call ptr @cache_access_mut(i128 %110), !dbg !189
  store i64 %4, ptr %111, align 8, !dbg !190
  %112 = call i128 @cache_request(i64 %52), !dbg !191
  %113 = call ptr @cache_access_mut(i128 %112), !dbg !192
  store i64 %5, ptr %113, align 8, !dbg !193
  %114 = mul i64 %44, 2, !dbg !194
  %115 = add i64 %114, 1, !dbg !195
  %116 = load i64, ptr %42, align 8, !dbg !196
  %117 = icmp sle i64 %115, %116, !dbg !197
  br i1 %117, label %118, label %134, !dbg !198

118:                                              ; preds = %58
  %119 = add i64 %114, -1, !dbg !199
  %120 = getelementptr %_Lowered_rarc, ptr %1, i64 %119, !dbg !200
  %121 = getelementptr %_Lowered_rarc, ptr %120, i32 0, i32 6, !dbg !201
  %122 = ptrtoint ptr %121 to i64, !dbg !202
  %123 = call i128 @cache_request(i64 %122), !dbg !203
  %124 = call ptr @cache_access(i128 %123), !dbg !204
  %125 = load i64, ptr %124, align 8, !dbg !205
  %126 = getelementptr %_Lowered_rarc, ptr %1, i64 %114, !dbg !206
  %127 = getelementptr %_Lowered_rarc, ptr %126, i32 0, i32 6, !dbg !207
  %128 = ptrtoint ptr %127 to i64, !dbg !208
  %129 = call i128 @cache_request(i64 %128), !dbg !209
  %130 = call ptr @cache_access(i128 %129), !dbg !210
  %131 = load i64, ptr %130, align 8, !dbg !211
  %132 = icmp slt i64 %125, %131, !dbg !212
  %133 = select i1 %132, i64 %115, i64 %114, !dbg !213
  br label %135, !dbg !214

134:                                              ; preds = %58
  br label %135, !dbg !215

135:                                              ; preds = %118, %134
  %136 = phi i64 [ %114, %134 ], [ %133, %118 ]
  br label %137, !dbg !216

137:                                              ; preds = %135
  br label %139, !dbg !217

138:                                              ; preds = %48
  br label %139, !dbg !218

139:                                              ; preds = %137, %138
  %140 = phi i64 [ %44, %138 ], [ %136, %137 ]
  br label %141, !dbg !219

141:                                              ; preds = %139
  br label %143, !dbg !220

142:                                              ; preds = %43
  br label %143, !dbg !221

143:                                              ; preds = %141, %142
  %144 = phi i1 [ false, %142 ], [ %56, %141 ]
  %145 = phi i64 [ %44, %142 ], [ %140, %141 ]
  %146 = phi i64 [ %45, %142 ], [ %57, %141 ]
  br label %147, !dbg !222

147:                                              ; preds = %143
  br i1 %144, label %43, label %148, !dbg !223

148:                                              ; preds = %147
  ret void, !dbg !224
}

define i64 @price_out_impl(ptr %0) !dbg !225 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !226
  %3 = load i64, ptr %2, align 8, !dbg !228
  %4 = add i64 %3, -15, !dbg !229
  %5 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 3, !dbg !230
  %6 = load i64, ptr %5, align 8, !dbg !231
  %7 = icmp sle i64 %6, 15000, !dbg !232
  br i1 %7, label %8, label %44, !dbg !233

8:                                                ; preds = %1
  %9 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !234
  %10 = load i64, ptr %9, align 8, !dbg !235
  %11 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 9, !dbg !236
  %12 = load i64, ptr %11, align 8, !dbg !237
  %13 = add i64 %10, %12, !dbg !238
  %14 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 4, !dbg !239
  %15 = load i64, ptr %14, align 8, !dbg !240
  %16 = icmp sgt i64 %13, %15, !dbg !241
  br i1 %16, label %17, label %38, !dbg !242

17:                                               ; preds = %8
  %18 = load i64, ptr %5, align 8, !dbg !243
  %19 = mul i64 %18, %18, !dbg !244
  %20 = sdiv i64 %19, 2, !dbg !245
  %21 = load i64, ptr %9, align 8, !dbg !246
  %22 = add i64 %20, %21, !dbg !247
  %23 = load i64, ptr %14, align 8, !dbg !248
  %24 = icmp sgt i64 %22, %23, !dbg !249
  br i1 %24, label %25, label %33, !dbg !250

25:                                               ; preds = %17
  %26 = call i64 @resize_prob(ptr %0), !dbg !251
  %27 = icmp ne i64 %26, 0, !dbg !252
  %28 = icmp eq i64 %26, 0, !dbg !253
  %29 = select i1 %27, i64 -1, i64 undef, !dbg !254
  br i1 %27, label %30, label %31, !dbg !255

30:                                               ; preds = %25
  br label %32, !dbg !256

31:                                               ; preds = %25
  call void @refresh_neighbour_lists(ptr %0), !dbg !257
  br label %32, !dbg !258

32:                                               ; preds = %30, %31
  br label %34, !dbg !259

33:                                               ; preds = %17
  br label %34, !dbg !260

34:                                               ; preds = %32, %33
  %35 = phi i1 [ true, %33 ], [ %28, %32 ]
  %36 = phi i64 [ undef, %33 ], [ %29, %32 ]
  br label %37, !dbg !261

37:                                               ; preds = %34
  br label %39, !dbg !262

38:                                               ; preds = %8
  br label %39, !dbg !263

39:                                               ; preds = %37, %38
  %40 = phi i1 [ true, %38 ], [ %35, %37 ]
  %41 = phi i64 [ undef, %38 ], [ %36, %37 ]
  %42 = phi i1 [ false, %38 ], [ %24, %37 ]
  br label %43, !dbg !264

43:                                               ; preds = %39
  br label %45, !dbg !265

44:                                               ; preds = %1
  br label %45, !dbg !266

45:                                               ; preds = %43, %44
  %46 = phi i1 [ true, %44 ], [ %40, %43 ]
  %47 = phi i64 [ undef, %44 ], [ %41, %43 ]
  %48 = phi i1 [ false, %44 ], [ %42, %43 ]
  br label %49, !dbg !267

49:                                               ; preds = %45
  br i1 %46, label %50, label %293, !dbg !268

50:                                               ; preds = %49
  %51 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !269
  %52 = load ptr, ptr %51, align 8, !dbg !270
  %53 = load i64, ptr %5, align 8, !dbg !271
  %54 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !272
  %55 = load ptr, ptr %54, align 8, !dbg !273
  br label %56, !dbg !274

56:                                               ; preds = %81, %50
  %57 = phi i64 [ %79, %81 ], [ 0, %50 ]
  %58 = phi ptr [ %80, %81 ], [ %55, %50 ]
  %59 = icmp slt i64 %57, %53, !dbg !275
  br i1 %59, label %60, label %76, !dbg !276

60:                                               ; preds = %56
  %61 = getelementptr %_Lowered_rarc, ptr %58, i32 1, !dbg !277
  %62 = getelementptr %_Lowered_rarc, ptr %61, i32 0, i32 3, !dbg !278
  %63 = ptrtoint ptr %62 to i64, !dbg !279
  %64 = call i128 @cache_request(i64 %63), !dbg !280
  %65 = call ptr @cache_access(i128 %64), !dbg !281
  %66 = load i32, ptr %65, align 4, !dbg !282
  %67 = icmp eq i32 %66, -1, !dbg !283
  br i1 %67, label %68, label %71, !dbg !284

68:                                               ; preds = %60
  %69 = add i64 %57, 1, !dbg !285
  %70 = getelementptr %_Lowered_rarc, ptr %58, i32 3, !dbg !286
  br label %72, !dbg !287

71:                                               ; preds = %60
  br label %72, !dbg !288

72:                                               ; preds = %68, %71
  %73 = phi i64 [ %57, %71 ], [ %69, %68 ]
  %74 = phi ptr [ %58, %71 ], [ %70, %68 ]
  br label %75, !dbg !289

75:                                               ; preds = %72
  br label %77, !dbg !290

76:                                               ; preds = %56
  br label %77, !dbg !291

77:                                               ; preds = %75, %76
  %78 = phi i1 [ false, %76 ], [ %67, %75 ]
  %79 = phi i64 [ %57, %76 ], [ %73, %75 ]
  %80 = phi ptr [ %58, %76 ], [ %74, %75 ]
  br label %81, !dbg !292

81:                                               ; preds = %77
  br i1 %78, label %56, label %82, !dbg !293

82:                                               ; preds = %81
  %83 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !294
  %84 = getelementptr %_Lowered_rarc, ptr %52, i32 0, i32 6, !dbg !295
  br label %85, !dbg !296

85:                                               ; preds = %212, %82
  %86 = phi i64 [ %214, %212 ], [ %79, %82 ]
  %87 = phi i64 [ %210, %212 ], [ undef, %82 ]
  %88 = phi i64 [ %211, %212 ], [ 0, %82 ]
  %89 = phi ptr [ %100, %212 ], [ null, %82 ]
  %90 = phi ptr [ %213, %212 ], [ %80, %82 ]
  %91 = icmp slt i64 %86, %53, !dbg !297
  br i1 %91, label %92, label %215, !dbg !298

92:                                               ; preds = %85
  %93 = getelementptr %_Lowered_rarc, ptr %90, i32 1, !dbg !299
  %94 = getelementptr %_Lowered_rarc, ptr %93, i32 0, i32 3, !dbg !300
  %95 = ptrtoint ptr %94 to i64, !dbg !301
  %96 = call i128 @cache_request(i64 %95), !dbg !302
  %97 = call ptr @cache_access(i128 %96), !dbg !303
  %98 = load i32, ptr %97, align 4, !dbg !304
  %99 = icmp ne i32 %98, -1, !dbg !305
  %100 = select i1 %99, ptr %93, ptr %89, !dbg !306
  br i1 %99, label %101, label %115, !dbg !307

101:                                              ; preds = %92
  %102 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 2, !dbg !308
  %103 = ptrtoint ptr %102 to i64, !dbg !309
  %104 = call i128 @cache_request(i64 %103), !dbg !310
  %105 = call ptr @cache_access(i128 %104), !dbg !311
  %106 = load ptr, ptr %105, align 8, !dbg !312
  %107 = getelementptr %_Lowered_node, ptr %106, i32 0, i32 7, !dbg !313
  %108 = load ptr, ptr %107, align 8, !dbg !314
  %109 = getelementptr %_Lowered_rarc, ptr %108, i32 0, i32 2, !dbg !315
  %110 = ptrtoint ptr %109 to i64, !dbg !316
  %111 = call i128 @cache_request(i64 %110), !dbg !317
  %112 = call ptr @cache_access(i128 %111), !dbg !318
  %113 = load ptr, ptr %112, align 8, !dbg !319
  %114 = getelementptr %_Lowered_node, ptr %113, i32 0, i32 9, !dbg !320
  store ptr %89, ptr %114, align 8, !dbg !321
  br label %115, !dbg !322

115:                                              ; preds = %101, %92
  %116 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 3, !dbg !323
  %117 = ptrtoint ptr %116 to i64, !dbg !324
  %118 = call i128 @cache_request(i64 %117), !dbg !325
  %119 = call ptr @cache_access(i128 %118), !dbg !326
  %120 = load i32, ptr %119, align 4, !dbg !327
  %121 = icmp eq i32 %120, -1, !dbg !328
  br i1 %121, label %122, label %123, !dbg !329

122:                                              ; preds = %115
  br label %209, !dbg !330

123:                                              ; preds = %115
  %124 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 2, !dbg !331
  %125 = ptrtoint ptr %124 to i64, !dbg !332
  %126 = call i128 @cache_request(i64 %125), !dbg !333
  %127 = call ptr @cache_access(i128 %126), !dbg !334
  %128 = load ptr, ptr %127, align 8, !dbg !335
  %129 = getelementptr %_Lowered_node, ptr %128, i32 0, i32 13, !dbg !336
  %130 = load i32, ptr %129, align 4, !dbg !337
  %131 = sext i32 %130 to i64, !dbg !338
  %132 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 7, !dbg !339
  %133 = ptrtoint ptr %132 to i64, !dbg !340
  %134 = call i128 @cache_request(i64 %133), !dbg !341
  %135 = call ptr @cache_access(i128 %134), !dbg !342
  %136 = load i64, ptr %135, align 8, !dbg !343
  %137 = sub i64 %131, %136, !dbg !344
  %138 = add i64 %137, %4, !dbg !345
  %139 = getelementptr %_Lowered_rarc, ptr %100, i32 0, i32 1, !dbg !346
  %140 = ptrtoint ptr %139 to i64, !dbg !347
  %141 = call i128 @cache_request(i64 %140), !dbg !348
  %142 = call ptr @cache_access(i128 %141), !dbg !349
  %143 = load ptr, ptr %142, align 8, !dbg !350
  %144 = getelementptr %_Lowered_node, ptr %143, i32 0, i32 9, !dbg !351
  %145 = load ptr, ptr %144, align 8, !dbg !352
  br label %146, !dbg !353

146:                                              ; preds = %207, %123
  %147 = phi i64 [ %204, %207 ], [ %87, %123 ]
  %148 = phi i64 [ %205, %207 ], [ %88, %123 ]
  %149 = phi ptr [ %206, %207 ], [ %145, %123 ]
  %150 = icmp ne ptr %149, null, !dbg !354
  br i1 %150, label %151, label %208, !dbg !355

151:                                              ; preds = %146
  %152 = phi i64 [ %147, %146 ]
  %153 = phi i64 [ %148, %146 ]
  %154 = phi ptr [ %149, %146 ]
  %155 = getelementptr %_Lowered_rarc, ptr %154, i32 0, i32 1, !dbg !356
  %156 = ptrtoint ptr %155 to i64, !dbg !357
  %157 = call i128 @cache_request(i64 %156), !dbg !358
  %158 = call ptr @cache_access(i128 %157), !dbg !359
  %159 = load ptr, ptr %158, align 8, !dbg !360
  %160 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 13, !dbg !361
  %161 = load i32, ptr %160, align 4, !dbg !362
  %162 = sext i32 %161 to i64, !dbg !363
  %163 = getelementptr %_Lowered_rarc, ptr %154, i32 0, i32 7, !dbg !364
  %164 = ptrtoint ptr %163 to i64, !dbg !365
  %165 = call i128 @cache_request(i64 %164), !dbg !366
  %166 = call ptr @cache_access(i128 %165), !dbg !367
  %167 = load i64, ptr %166, align 8, !dbg !368
  %168 = add i64 %162, %167, !dbg !369
  %169 = icmp sgt i64 %168, %138, !dbg !370
  br i1 %169, label %170, label %173, !dbg !371

170:                                              ; preds = %151
  %171 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 9, !dbg !372
  %172 = load ptr, ptr %171, align 8, !dbg !373
  br label %203, !dbg !374

173:                                              ; preds = %151
  %174 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 0, !dbg !375
  %175 = load i64, ptr %174, align 8, !dbg !376
  %176 = sub i64 30, %175, !dbg !377
  %177 = getelementptr %_Lowered_node, ptr %128, i32 0, i32 0, !dbg !378
  %178 = load i64, ptr %177, align 8, !dbg !379
  %179 = add i64 %176, %178, !dbg !380
  %180 = icmp slt i64 %179, 0, !dbg !381
  br i1 %180, label %181, label %197, !dbg !382

181:                                              ; preds = %173
  %182 = load i64, ptr %83, align 8, !dbg !383
  %183 = icmp slt i64 %153, %182, !dbg !384
  br i1 %183, label %184, label %186, !dbg !385

184:                                              ; preds = %181
  call void @insert_new_arc(ptr %52, i64 %153, ptr %159, ptr %128, i64 30, i64 %179), !dbg !386
  %185 = add i64 %153, 1, !dbg !387
  br label %194, !dbg !388

186:                                              ; preds = %181
  %187 = ptrtoint ptr %84 to i64, !dbg !389
  %188 = call i128 @cache_request(i64 %187), !dbg !390
  %189 = call ptr @cache_access(i128 %188), !dbg !391
  %190 = load i64, ptr %189, align 8, !dbg !392
  %191 = icmp sgt i64 %190, %179, !dbg !393
  br i1 %191, label %192, label %193, !dbg !394

192:                                              ; preds = %186
  call void @replace_weaker_arc(ptr %0, ptr %52, ptr %159, ptr %128, i64 30, i64 %179), !dbg !395
  br label %193, !dbg !396

193:                                              ; preds = %192, %186
  br label %194, !dbg !397

194:                                              ; preds = %184, %193
  %195 = phi i64 [ %153, %193 ], [ %185, %184 ]
  br label %196, !dbg !398

196:                                              ; preds = %194
  br label %198, !dbg !399

197:                                              ; preds = %173
  br label %198, !dbg !400

198:                                              ; preds = %196, %197
  %199 = phi i64 [ %153, %197 ], [ %195, %196 ]
  br label %200, !dbg !401

200:                                              ; preds = %198
  %201 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 9, !dbg !402
  %202 = load ptr, ptr %201, align 8, !dbg !403
  br label %203, !dbg !404

203:                                              ; preds = %170, %200
  %204 = phi i64 [ %179, %200 ], [ %152, %170 ]
  %205 = phi i64 [ %199, %200 ], [ %153, %170 ]
  %206 = phi ptr [ %202, %200 ], [ %172, %170 ]
  br label %207, !dbg !405

207:                                              ; preds = %203
  br label %146, !dbg !406

208:                                              ; preds = %146
  br label %209, !dbg !407

209:                                              ; preds = %122, %208
  %210 = phi i64 [ %147, %208 ], [ %87, %122 ]
  %211 = phi i64 [ %148, %208 ], [ %88, %122 ]
  br label %212, !dbg !408

212:                                              ; preds = %209
  %213 = getelementptr %_Lowered_rarc, ptr %90, i32 3, !dbg !409
  %214 = add i64 %86, 1, !dbg !410
  br label %85, !dbg !411

215:                                              ; preds = %85
  %216 = icmp ne i64 %88, 0, !dbg !412
  br i1 %216, label %217, label %292, !dbg !413

217:                                              ; preds = %215
  %218 = load ptr, ptr %51, align 8, !dbg !414
  %219 = getelementptr %_Lowered_rarc, ptr %218, i64 %88, !dbg !415
  store ptr %219, ptr %51, align 8, !dbg !416
  %220 = load ptr, ptr %51, align 8, !dbg !417
  br i1 %48, label %221, label %237, !dbg !418

221:                                              ; preds = %217
  br label %222, !dbg !419

222:                                              ; preds = %225, %221
  %223 = phi ptr [ %235, %225 ], [ %218, %221 ]
  %224 = icmp ne ptr %223, %220, !dbg !420
  br i1 %224, label %225, label %236, !dbg !421

225:                                              ; preds = %222
  %226 = phi ptr [ %223, %222 ]
  %227 = getelementptr %_Lowered_rarc, ptr %226, i32 0, i32 6, !dbg !422
  %228 = ptrtoint ptr %227 to i64, !dbg !423
  %229 = call i128 @cache_request(i64 %228), !dbg !424
  %230 = call ptr @cache_access_mut(i128 %229), !dbg !425
  store i64 0, ptr %230, align 8, !dbg !426
  %231 = getelementptr %_Lowered_rarc, ptr %226, i32 0, i32 3, !dbg !427
  %232 = ptrtoint ptr %231 to i64, !dbg !428
  %233 = call i128 @cache_request(i64 %232), !dbg !429
  %234 = call ptr @cache_access_mut(i128 %233), !dbg !430
  store i32 1, ptr %234, align 4, !dbg !431
  %235 = getelementptr %_Lowered_rarc, ptr %226, i32 1, !dbg !432
  br label %222, !dbg !433

236:                                              ; preds = %222
  br label %283, !dbg !434

237:                                              ; preds = %217
  br label %238, !dbg !435

238:                                              ; preds = %241, %237
  %239 = phi ptr [ %281, %241 ], [ %218, %237 ]
  %240 = icmp ne ptr %239, %220, !dbg !436
  br i1 %240, label %241, label %282, !dbg !437

241:                                              ; preds = %238
  %242 = phi ptr [ %239, %238 ]
  %243 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 6, !dbg !438
  %244 = ptrtoint ptr %243 to i64, !dbg !439
  %245 = call i128 @cache_request(i64 %244), !dbg !440
  %246 = call ptr @cache_access_mut(i128 %245), !dbg !441
  store i64 0, ptr %246, align 8, !dbg !442
  %247 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 3, !dbg !443
  %248 = ptrtoint ptr %247 to i64, !dbg !444
  %249 = call i128 @cache_request(i64 %248), !dbg !445
  %250 = call ptr @cache_access_mut(i128 %249), !dbg !446
  store i32 1, ptr %250, align 4, !dbg !447
  %251 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 4, !dbg !448
  %252 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 1, !dbg !449
  %253 = ptrtoint ptr %252 to i64, !dbg !450
  %254 = call i128 @cache_request(i64 %253), !dbg !451
  %255 = call ptr @cache_access(i128 %254), !dbg !452
  %256 = load ptr, ptr %255, align 8, !dbg !453
  %257 = getelementptr %_Lowered_node, ptr %256, i32 0, i32 7, !dbg !454
  %258 = load ptr, ptr %257, align 8, !dbg !455
  %259 = ptrtoint ptr %251 to i64, !dbg !456
  %260 = call i128 @cache_request(i64 %259), !dbg !457
  %261 = call ptr @cache_access_mut(i128 %260), !dbg !458
  store ptr %258, ptr %261, align 8, !dbg !459
  %262 = call i128 @cache_request(i64 %253), !dbg !460
  %263 = call ptr @cache_access(i128 %262), !dbg !461
  %264 = load ptr, ptr %263, align 8, !dbg !462
  %265 = getelementptr %_Lowered_node, ptr %264, i32 0, i32 7, !dbg !463
  store ptr %242, ptr %265, align 8, !dbg !464
  %266 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 5, !dbg !465
  %267 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 2, !dbg !466
  %268 = ptrtoint ptr %267 to i64, !dbg !467
  %269 = call i128 @cache_request(i64 %268), !dbg !468
  %270 = call ptr @cache_access(i128 %269), !dbg !469
  %271 = load ptr, ptr %270, align 8, !dbg !470
  %272 = getelementptr %_Lowered_node, ptr %271, i32 0, i32 8, !dbg !471
  %273 = load ptr, ptr %272, align 8, !dbg !472
  %274 = ptrtoint ptr %266 to i64, !dbg !473
  %275 = call i128 @cache_request(i64 %274), !dbg !474
  %276 = call ptr @cache_access_mut(i128 %275), !dbg !475
  store ptr %273, ptr %276, align 8, !dbg !476
  %277 = call i128 @cache_request(i64 %268), !dbg !477
  %278 = call ptr @cache_access(i128 %277), !dbg !478
  %279 = load ptr, ptr %278, align 8, !dbg !479
  %280 = getelementptr %_Lowered_node, ptr %279, i32 0, i32 8, !dbg !480
  store ptr %242, ptr %280, align 8, !dbg !481
  %281 = getelementptr %_Lowered_rarc, ptr %242, i32 1, !dbg !482
  br label %238, !dbg !483

282:                                              ; preds = %238
  br label %283, !dbg !484

283:                                              ; preds = %236, %282
  %284 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !485
  %285 = load i64, ptr %284, align 8, !dbg !486
  %286 = add i64 %285, %88, !dbg !487
  store i64 %286, ptr %284, align 8, !dbg !488
  %287 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !489
  %288 = load i64, ptr %287, align 8, !dbg !490
  %289 = add i64 %288, %88, !dbg !491
  store i64 %289, ptr %287, align 8, !dbg !492
  %290 = load i64, ptr %83, align 8, !dbg !493
  %291 = sub i64 %290, %88, !dbg !494
  store i64 %291, ptr %83, align 8, !dbg !495
  br label %292, !dbg !496

292:                                              ; preds = %283, %215
  br label %294, !dbg !497

293:                                              ; preds = %49
  br label %294, !dbg !498

294:                                              ; preds = %292, %293
  %295 = phi i64 [ %47, %293 ], [ %88, %292 ]
  br label %296, !dbg !499

296:                                              ; preds = %294
  ret i64 %295, !dbg !500
}

declare void @refresh_neighbour_lists(ptr)

define void @insert_new_arc(ptr %0, i64 %1, ptr %2, ptr %3, i64 %4, i64 %5) !dbg !501 {
  %7 = getelementptr %_Lowered_rarc, ptr %0, i64 %1, !dbg !502
  %8 = getelementptr %_Lowered_rarc, ptr %7, i32 0, i32 1, !dbg !504
  %9 = ptrtoint ptr %8 to i64, !dbg !505
  %10 = call i128 @cache_request(i64 %9), !dbg !506
  %11 = call ptr @cache_access_mut(i128 %10), !dbg !507
  store ptr %2, ptr %11, align 8, !dbg !508
  %12 = getelementptr %_Lowered_rarc, ptr %7, i32 0, i32 2, !dbg !509
  %13 = ptrtoint ptr %12 to i64, !dbg !510
  %14 = call i128 @cache_request(i64 %13), !dbg !511
  %15 = call ptr @cache_access_mut(i128 %14), !dbg !512
  store ptr %3, ptr %15, align 8, !dbg !513
  %16 = getelementptr %_Lowered_rarc, ptr %7, i32 0, i32 7, !dbg !514
  %17 = ptrtoint ptr %16 to i64, !dbg !515
  %18 = call i128 @cache_request(i64 %17), !dbg !516
  %19 = call ptr @cache_access_mut(i128 %18), !dbg !517
  store i64 %4, ptr %19, align 8, !dbg !518
  %20 = getelementptr %_Lowered_rarc, ptr %7, i32 0, i32 0, !dbg !519
  %21 = ptrtoint ptr %20 to i64, !dbg !520
  %22 = call i128 @cache_request(i64 %21), !dbg !521
  %23 = call ptr @cache_access_mut(i128 %22), !dbg !522
  store i64 %4, ptr %23, align 8, !dbg !523
  %24 = getelementptr %_Lowered_rarc, ptr %7, i32 0, i32 6, !dbg !524
  %25 = ptrtoint ptr %24 to i64, !dbg !525
  %26 = call i128 @cache_request(i64 %25), !dbg !526
  %27 = call ptr @cache_access_mut(i128 %26), !dbg !527
  store i64 %5, ptr %27, align 8, !dbg !528
  %28 = add i64 %1, 1, !dbg !529
  br label %29, !dbg !530

29:                                               ; preds = %104, %6
  %30 = phi i64 [ %103, %104 ], [ %28, %6 ]
  %31 = add i64 %30, -1, !dbg !531
  %32 = icmp ne i64 %31, 0, !dbg !532
  br i1 %32, label %33, label %100, !dbg !533

33:                                               ; preds = %29
  %34 = sdiv i64 %30, 2, !dbg !534
  %35 = add i64 %34, -1, !dbg !535
  %36 = getelementptr %_Lowered_rarc, ptr %0, i64 %35, !dbg !536
  %37 = getelementptr %_Lowered_rarc, ptr %36, i32 0, i32 6, !dbg !537
  %38 = ptrtoint ptr %37 to i64, !dbg !538
  %39 = call i128 @cache_request(i64 %38), !dbg !539
  %40 = call ptr @cache_access(i128 %39), !dbg !540
  %41 = load i64, ptr %40, align 8, !dbg !541
  %42 = icmp sgt i64 %5, %41, !dbg !542
  %43 = select i1 %42, i64 %34, i64 %30, !dbg !543
  br i1 %42, label %44, label %99, !dbg !544

44:                                               ; preds = %33
  %45 = getelementptr %_Lowered_rarc, ptr %0, i64 %31, !dbg !545
  %46 = getelementptr %_Lowered_rarc, ptr %45, i32 0, i32 1, !dbg !546
  %47 = getelementptr %_Lowered_rarc, ptr %36, i32 0, i32 1, !dbg !547
  %48 = ptrtoint ptr %47 to i64, !dbg !548
  %49 = call i128 @cache_request(i64 %48), !dbg !549
  %50 = call ptr @cache_access(i128 %49), !dbg !550
  %51 = load ptr, ptr %50, align 8, !dbg !551
  %52 = ptrtoint ptr %46 to i64, !dbg !552
  %53 = call i128 @cache_request(i64 %52), !dbg !553
  %54 = call ptr @cache_access_mut(i128 %53), !dbg !554
  store ptr %51, ptr %54, align 8, !dbg !555
  %55 = getelementptr %_Lowered_rarc, ptr %45, i32 0, i32 2, !dbg !556
  %56 = getelementptr %_Lowered_rarc, ptr %36, i32 0, i32 2, !dbg !557
  %57 = ptrtoint ptr %56 to i64, !dbg !558
  %58 = call i128 @cache_request(i64 %57), !dbg !559
  %59 = call ptr @cache_access(i128 %58), !dbg !560
  %60 = load ptr, ptr %59, align 8, !dbg !561
  %61 = ptrtoint ptr %55 to i64, !dbg !562
  %62 = call i128 @cache_request(i64 %61), !dbg !563
  %63 = call ptr @cache_access_mut(i128 %62), !dbg !564
  store ptr %60, ptr %63, align 8, !dbg !565
  %64 = getelementptr %_Lowered_rarc, ptr %45, i32 0, i32 0, !dbg !566
  %65 = getelementptr %_Lowered_rarc, ptr %36, i32 0, i32 0, !dbg !567
  %66 = ptrtoint ptr %65 to i64, !dbg !568
  %67 = call i128 @cache_request(i64 %66), !dbg !569
  %68 = call ptr @cache_access(i128 %67), !dbg !570
  %69 = load i64, ptr %68, align 8, !dbg !571
  %70 = ptrtoint ptr %64 to i64, !dbg !572
  %71 = call i128 @cache_request(i64 %70), !dbg !573
  %72 = call ptr @cache_access_mut(i128 %71), !dbg !574
  store i64 %69, ptr %72, align 8, !dbg !575
  %73 = getelementptr %_Lowered_rarc, ptr %45, i32 0, i32 7, !dbg !576
  %74 = call i128 @cache_request(i64 %66), !dbg !577
  %75 = call ptr @cache_access(i128 %74), !dbg !578
  %76 = load i64, ptr %75, align 8, !dbg !579
  %77 = ptrtoint ptr %73 to i64, !dbg !580
  %78 = call i128 @cache_request(i64 %77), !dbg !581
  %79 = call ptr @cache_access_mut(i128 %78), !dbg !582
  store i64 %76, ptr %79, align 8, !dbg !583
  %80 = getelementptr %_Lowered_rarc, ptr %45, i32 0, i32 6, !dbg !584
  %81 = call i128 @cache_request(i64 %38), !dbg !585
  %82 = call ptr @cache_access(i128 %81), !dbg !586
  %83 = load i64, ptr %82, align 8, !dbg !587
  %84 = ptrtoint ptr %80 to i64, !dbg !588
  %85 = call i128 @cache_request(i64 %84), !dbg !589
  %86 = call ptr @cache_access_mut(i128 %85), !dbg !590
  store i64 %83, ptr %86, align 8, !dbg !591
  %87 = call i128 @cache_request(i64 %48), !dbg !592
  %88 = call ptr @cache_access_mut(i128 %87), !dbg !593
  store ptr %2, ptr %88, align 8, !dbg !594
  %89 = call i128 @cache_request(i64 %57), !dbg !595
  %90 = call ptr @cache_access_mut(i128 %89), !dbg !596
  store ptr %3, ptr %90, align 8, !dbg !597
  %91 = call i128 @cache_request(i64 %66), !dbg !598
  %92 = call ptr @cache_access_mut(i128 %91), !dbg !599
  store i64 %4, ptr %92, align 8, !dbg !600
  %93 = getelementptr %_Lowered_rarc, ptr %36, i32 0, i32 7, !dbg !601
  %94 = ptrtoint ptr %93 to i64, !dbg !602
  %95 = call i128 @cache_request(i64 %94), !dbg !603
  %96 = call ptr @cache_access_mut(i128 %95), !dbg !604
  store i64 %4, ptr %96, align 8, !dbg !605
  %97 = call i128 @cache_request(i64 %38), !dbg !606
  %98 = call ptr @cache_access_mut(i128 %97), !dbg !607
  store i64 %5, ptr %98, align 8, !dbg !608
  br label %99, !dbg !609

99:                                               ; preds = %44, %33
  br label %101, !dbg !610

100:                                              ; preds = %29
  br label %101, !dbg !611

101:                                              ; preds = %99, %100
  %102 = phi i1 [ false, %100 ], [ %42, %99 ]
  %103 = phi i64 [ %30, %100 ], [ %43, %99 ]
  br label %104, !dbg !612

104:                                              ; preds = %101
  br i1 %102, label %29, label %105, !dbg !613

105:                                              ; preds = %104
  ret void, !dbg !614
}

define i64 @suspend_impl(ptr %0, i64 %1, i64 %2) !dbg !615 {
  %4 = icmp ne i64 %2, 0, !dbg !616
  br i1 %4, label %5, label %8, !dbg !618

5:                                                ; preds = %3
  %6 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !619
  %7 = load i64, ptr %6, align 8, !dbg !620
  br label %105, !dbg !621

8:                                                ; preds = %3
  %9 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !622
  %10 = load ptr, ptr %9, align 8, !dbg !623
  %11 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !624
  %12 = load ptr, ptr %11, align 8, !dbg !625
  %13 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !626
  %14 = load i64, ptr %13, align 8, !dbg !627
  %15 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !628
  %16 = load i64, ptr %15, align 8, !dbg !629
  %17 = sub i64 %14, %16, !dbg !630
  %18 = getelementptr %_Lowered_rarc, ptr %12, i64 %17, !dbg !631
  br label %19, !dbg !632

19:                                               ; preds = %102, %8
  %20 = phi ptr [ %103, %102 ], [ %18, %8 ]
  %21 = phi ptr [ %100, %102 ], [ %18, %8 ]
  %22 = phi i64 [ %101, %102 ], [ 0, %8 ]
  %23 = icmp ult ptr %20, %10, !dbg !633
  br i1 %23, label %24, label %104, !dbg !634

24:                                               ; preds = %19
  %25 = phi i64 [ %22, %19 ]
  %26 = phi ptr [ %20, %19 ]
  %27 = phi ptr [ %21, %19 ]
  %28 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 3, !dbg !635
  %29 = ptrtoint ptr %28 to i64, !dbg !636
  %30 = call i128 @cache_request(i64 %29), !dbg !637
  %31 = call ptr @cache_access(i128 %30), !dbg !638
  %32 = load i32, ptr %31, align 4, !dbg !639
  %33 = icmp eq i32 %32, 1, !dbg !640
  br i1 %33, label %34, label %56, !dbg !641

34:                                               ; preds = %24
  %35 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 0, !dbg !642
  %36 = ptrtoint ptr %35 to i64, !dbg !643
  %37 = call i128 @cache_request(i64 %36), !dbg !644
  %38 = call ptr @cache_access(i128 %37), !dbg !645
  %39 = load i64, ptr %38, align 8, !dbg !646
  %40 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !647
  %41 = ptrtoint ptr %40 to i64, !dbg !648
  %42 = call i128 @cache_request(i64 %41), !dbg !649
  %43 = call ptr @cache_access(i128 %42), !dbg !650
  %44 = load ptr, ptr %43, align 8, !dbg !651
  %45 = getelementptr %_Lowered_node, ptr %44, i32 0, i32 0, !dbg !652
  %46 = load i64, ptr %45, align 8, !dbg !653
  %47 = sub i64 %39, %46, !dbg !654
  %48 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !655
  %49 = ptrtoint ptr %48 to i64, !dbg !656
  %50 = call i128 @cache_request(i64 %49), !dbg !657
  %51 = call ptr @cache_access(i128 %50), !dbg !658
  %52 = load ptr, ptr %51, align 8, !dbg !659
  %53 = getelementptr %_Lowered_node, ptr %52, i32 0, i32 0, !dbg !660
  %54 = load i64, ptr %53, align 8, !dbg !661
  %55 = add i64 %47, %54, !dbg !662
  br label %84, !dbg !663

56:                                               ; preds = %24
  %57 = call i128 @cache_request(i64 %29), !dbg !664
  %58 = call ptr @cache_access(i128 %57), !dbg !665
  %59 = load i32, ptr %58, align 4, !dbg !666
  %60 = icmp eq i32 %59, 0, !dbg !667
  br i1 %60, label %61, label %83, !dbg !668

61:                                               ; preds = %56
  %62 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !669
  %63 = ptrtoint ptr %62 to i64, !dbg !670
  %64 = call i128 @cache_request(i64 %63), !dbg !671
  %65 = call ptr @cache_access(i128 %64), !dbg !672
  %66 = load ptr, ptr %65, align 8, !dbg !673
  %67 = getelementptr %_Lowered_node, ptr %66, i32 0, i32 6, !dbg !674
  %68 = load ptr, ptr %67, align 8, !dbg !675
  %69 = icmp eq ptr %68, %26, !dbg !676
  br i1 %69, label %70, label %75, !dbg !677

70:                                               ; preds = %61
  %71 = call i128 @cache_request(i64 %63), !dbg !678
  %72 = call ptr @cache_access(i128 %71), !dbg !679
  %73 = load ptr, ptr %72, align 8, !dbg !680
  %74 = getelementptr %_Lowered_node, ptr %73, i32 0, i32 6, !dbg !681
  store ptr %27, ptr %74, align 8, !dbg !682
  br label %82, !dbg !683

75:                                               ; preds = %61
  %76 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !684
  %77 = ptrtoint ptr %76 to i64, !dbg !685
  %78 = call i128 @cache_request(i64 %77), !dbg !686
  %79 = call ptr @cache_access(i128 %78), !dbg !687
  %80 = load ptr, ptr %79, align 8, !dbg !688
  %81 = getelementptr %_Lowered_node, ptr %80, i32 0, i32 6, !dbg !689
  store ptr %27, ptr %81, align 8, !dbg !690
  br label %82, !dbg !691

82:                                               ; preds = %70, %75
  br label %83, !dbg !692

83:                                               ; preds = %82, %56
  br label %84, !dbg !693

84:                                               ; preds = %34, %83
  %85 = phi i64 [ -2, %83 ], [ %55, %34 ]
  br label %86, !dbg !694

86:                                               ; preds = %84
  %87 = icmp sgt i64 %85, %1, !dbg !695
  br i1 %87, label %88, label %90, !dbg !696

88:                                               ; preds = %86
  %89 = add i64 %25, 1, !dbg !697
  br label %99, !dbg !698

90:                                               ; preds = %86
  %91 = ptrtoint ptr %26 to i64, !dbg !699
  %92 = call i128 @cache_request(i64 %91), !dbg !700
  %93 = call ptr @cache_access(i128 %92), !dbg !701
  %94 = load %_Lowered_rarc, ptr %93, align 8, !dbg !702
  %95 = ptrtoint ptr %27 to i64, !dbg !703
  %96 = call i128 @cache_request(i64 %95), !dbg !704
  %97 = call ptr @cache_access_mut(i128 %96), !dbg !705
  store %_Lowered_rarc %94, ptr %97, align 8, !dbg !706
  %98 = getelementptr %_Lowered_rarc, ptr %27, i32 1, !dbg !707
  br label %99, !dbg !708

99:                                               ; preds = %88, %90
  %100 = phi ptr [ %98, %90 ], [ %27, %88 ]
  %101 = phi i64 [ %25, %90 ], [ %89, %88 ]
  br label %102, !dbg !709

102:                                              ; preds = %99
  %103 = getelementptr %_Lowered_rarc, ptr %26, i32 1, !dbg !710
  br label %19, !dbg !711

104:                                              ; preds = %19
  br label %105, !dbg !712

105:                                              ; preds = %5, %104
  %106 = phi i64 [ %22, %104 ], [ %7, %5 ]
  br label %107, !dbg !713

107:                                              ; preds = %105
  %108 = icmp ne i64 %106, 0, !dbg !714
  br i1 %108, label %109, label %123, !dbg !715

109:                                              ; preds = %107
  %110 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !716
  %111 = load i64, ptr %110, align 8, !dbg !717
  %112 = sub i64 %111, %106, !dbg !718
  store i64 %112, ptr %110, align 8, !dbg !719
  %113 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !720
  %114 = load i64, ptr %113, align 8, !dbg !721
  %115 = sub i64 %114, %106, !dbg !722
  store i64 %115, ptr %113, align 8, !dbg !723
  %116 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !724
  %117 = load ptr, ptr %116, align 8, !dbg !725
  %118 = sub i64 0, %106, !dbg !726
  %119 = getelementptr %_Lowered_rarc, ptr %117, i64 %118, !dbg !727
  store ptr %119, ptr %116, align 8, !dbg !728
  %120 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !729
  %121 = load i64, ptr %120, align 8, !dbg !730
  %122 = add i64 %121, %106, !dbg !731
  store i64 %122, ptr %120, align 8, !dbg !732
  call void @refresh_neighbour_lists(ptr %0), !dbg !733
  br label %123, !dbg !734

123:                                              ; preds = %109, %107
  ret i64 %106, !dbg !735
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "resize_prob", linkageName: "resize_prob", scope: null, file: !4, line: 12, type: !5, scopeLine: 12, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/implicit.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 19, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 20, column: 10, scope: !8)
!10 = !DILocation(line: 21, column: 10, scope: !8)
!11 = !DILocation(line: 22, column: 5, scope: !8)
!12 = !DILocation(line: 24, column: 5, scope: !8)
!13 = !DILocation(line: 32, column: 5, scope: !8)
!14 = !DILocation(line: 33, column: 5, scope: !8)
!15 = !DILocation(line: 35, column: 11, scope: !8)
!16 = !DILocation(line: 36, column: 11, scope: !8)
!17 = !DILocation(line: 37, column: 11, scope: !8)
!18 = !DILocation(line: 38, column: 11, scope: !8)
!19 = !DILocation(line: 39, column: 5, scope: !8)
!20 = !DILocation(line: 40, column: 11, scope: !8)
!21 = !DILocation(line: 41, column: 11, scope: !8)
!22 = !DILocation(line: 42, column: 11, scope: !8)
!23 = !DILocation(line: 43, column: 11, scope: !8)
!24 = !DILocation(line: 44, column: 5, scope: !8)
!25 = !DILocation(line: 45, column: 11, scope: !8)
!26 = !DILocation(line: 46, column: 11, scope: !8)
!27 = !DILocation(line: 48, column: 11, scope: !8)
!28 = !DILocation(line: 49, column: 11, scope: !8)
!29 = !DILocation(line: 50, column: 11, scope: !8)
!30 = !DILocation(line: 51, column: 5, scope: !8)
!31 = !DILocation(line: 55, column: 11, scope: !8)
!32 = !DILocation(line: 56, column: 11, scope: !8)
!33 = !DILocation(line: 57, column: 11, scope: !8)
!34 = !DILocation(line: 59, column: 11, scope: !8)
!35 = !DILocation(line: 60, column: 11, scope: !8)
!36 = !DILocation(line: 61, column: 5, scope: !8)
!37 = !DILocation(line: 63, column: 11, scope: !8)
!38 = !DILocation(line: 64, column: 5, scope: !8)
!39 = !DILocation(line: 66, column: 11, scope: !8)
!40 = !DILocation(line: 67, column: 11, scope: !8)
!41 = !DILocation(line: 68, column: 11, scope: !8)
!42 = !DILocation(line: 69, column: 11, scope: !8)
!43 = !DILocation(line: 70, column: 5, scope: !8)
!44 = !DILocation(line: 71, column: 11, scope: !8)
!45 = !DILocation(line: 72, column: 11, scope: !8)
!46 = !DILocation(line: 73, column: 11, scope: !8)
!47 = !DILocation(line: 74, column: 11, scope: !8)
!48 = !DILocation(line: 75, column: 5, scope: !8)
!49 = !DILocation(line: 76, column: 11, scope: !8)
!50 = !DILocation(line: 77, column: 11, scope: !8)
!51 = !DILocation(line: 78, column: 11, scope: !8)
!52 = !DILocation(line: 79, column: 11, scope: !8)
!53 = !DILocation(line: 80, column: 11, scope: !8)
!54 = !DILocation(line: 81, column: 5, scope: !8)
!55 = !DILocation(line: 83, column: 11, scope: !8)
!56 = !DILocation(line: 84, column: 5, scope: !8)
!57 = !DILocation(line: 86, column: 11, scope: !8)
!58 = !DILocation(line: 87, column: 11, scope: !8)
!59 = !DILocation(line: 88, column: 11, scope: !8)
!60 = !DILocation(line: 89, column: 5, scope: !8)
!61 = !DILocation(line: 91, column: 11, scope: !8)
!62 = !DILocation(line: 92, column: 11, scope: !8)
!63 = !DILocation(line: 93, column: 11, scope: !8)
!64 = !DILocation(line: 94, column: 11, scope: !8)
!65 = !DILocation(line: 95, column: 11, scope: !8)
!66 = !DILocation(line: 96, column: 5, scope: !8)
!67 = !DILocation(line: 97, column: 5, scope: !8)
!68 = !DILocation(line: 99, column: 11, scope: !8)
!69 = !DILocation(line: 100, column: 5, scope: !8)
!70 = !DILocation(line: 102, column: 5, scope: !8)
!71 = !DILocation(line: 104, column: 5, scope: !8)
!72 = distinct !DISubprogram(name: "replace_weaker_arc", linkageName: "replace_weaker_arc", scope: null, file: !4, line: 107, type: !5, scopeLine: 107, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!73 = !DILocation(line: 114, column: 10, scope: !74)
!74 = !DILexicalBlockFile(scope: !72, file: !4, discriminator: 0)
!75 = !DILocation(line: 115, column: 10, scope: !74)
!76 = !DILocation(line: 116, column: 10, scope: !74)
!77 = !DILocation(line: 117, column: 10, scope: !74)
!78 = !DILocation(line: 119, column: 5, scope: !74)
!79 = !DILocation(line: 120, column: 11, scope: !74)
!80 = !DILocation(line: 121, column: 11, scope: !74)
!81 = !DILocation(line: 122, column: 11, scope: !74)
!82 = !DILocation(line: 123, column: 11, scope: !74)
!83 = !DILocation(line: 125, column: 5, scope: !74)
!84 = !DILocation(line: 126, column: 11, scope: !74)
!85 = !DILocation(line: 127, column: 11, scope: !74)
!86 = !DILocation(line: 128, column: 11, scope: !74)
!87 = !DILocation(line: 129, column: 11, scope: !74)
!88 = !DILocation(line: 131, column: 5, scope: !74)
!89 = !DILocation(line: 132, column: 11, scope: !74)
!90 = !DILocation(line: 133, column: 11, scope: !74)
!91 = !DILocation(line: 134, column: 11, scope: !74)
!92 = !DILocation(line: 135, column: 11, scope: !74)
!93 = !DILocation(line: 137, column: 5, scope: !74)
!94 = !DILocation(line: 138, column: 11, scope: !74)
!95 = !DILocation(line: 139, column: 11, scope: !74)
!96 = !DILocation(line: 140, column: 11, scope: !74)
!97 = !DILocation(line: 141, column: 11, scope: !74)
!98 = !DILocation(line: 143, column: 5, scope: !74)
!99 = !DILocation(line: 144, column: 11, scope: !74)
!100 = !DILocation(line: 145, column: 11, scope: !74)
!101 = !DILocation(line: 146, column: 11, scope: !74)
!102 = !DILocation(line: 147, column: 11, scope: !74)
!103 = !DILocation(line: 148, column: 11, scope: !74)
!104 = !DILocation(line: 150, column: 11, scope: !74)
!105 = !DILocation(line: 151, column: 11, scope: !74)
!106 = !DILocation(line: 152, column: 11, scope: !74)
!107 = !DILocation(line: 153, column: 11, scope: !74)
!108 = !DILocation(line: 154, column: 11, scope: !74)
!109 = !DILocation(line: 155, column: 11, scope: !74)
!110 = !DILocation(line: 157, column: 11, scope: !74)
!111 = !DILocation(line: 158, column: 11, scope: !74)
!112 = !DILocation(line: 159, column: 11, scope: !74)
!113 = !DILocation(line: 160, column: 11, scope: !74)
!114 = !DILocation(line: 161, column: 11, scope: !74)
!115 = !DILocation(line: 162, column: 5, scope: !74)
!116 = !DILocation(line: 164, column: 11, scope: !74)
!117 = !DILocation(line: 165, column: 11, scope: !74)
!118 = !DILocation(line: 166, column: 5, scope: !74)
!119 = !DILocation(line: 168, column: 11, scope: !74)
!120 = !DILocation(line: 169, column: 11, scope: !74)
!121 = !DILocation(line: 170, column: 11, scope: !74)
!122 = !DILocation(line: 171, column: 11, scope: !74)
!123 = !DILocation(line: 172, column: 11, scope: !74)
!124 = !DILocation(line: 173, column: 11, scope: !74)
!125 = !DILocation(line: 175, column: 11, scope: !74)
!126 = !DILocation(line: 176, column: 11, scope: !74)
!127 = !DILocation(line: 177, column: 11, scope: !74)
!128 = !DILocation(line: 178, column: 5, scope: !74)
!129 = !DILocation(line: 180, column: 11, scope: !74)
!130 = !DILocation(line: 181, column: 11, scope: !74)
!131 = !DILocation(line: 182, column: 11, scope: !74)
!132 = !DILocation(line: 183, column: 11, scope: !74)
!133 = !DILocation(line: 184, column: 11, scope: !74)
!134 = !DILocation(line: 185, column: 11, scope: !74)
!135 = !DILocation(line: 186, column: 11, scope: !74)
!136 = !DILocation(line: 188, column: 11, scope: !74)
!137 = !DILocation(line: 189, column: 11, scope: !74)
!138 = !DILocation(line: 190, column: 11, scope: !74)
!139 = !DILocation(line: 191, column: 11, scope: !74)
!140 = !DILocation(line: 193, column: 5, scope: !74)
!141 = !DILocation(line: 194, column: 11, scope: !74)
!142 = !DILocation(line: 195, column: 11, scope: !74)
!143 = !DILocation(line: 196, column: 11, scope: !74)
!144 = !DILocation(line: 197, column: 11, scope: !74)
!145 = !DILocation(line: 198, column: 11, scope: !74)
!146 = !DILocation(line: 200, column: 11, scope: !74)
!147 = !DILocation(line: 201, column: 11, scope: !74)
!148 = !DILocation(line: 202, column: 11, scope: !74)
!149 = !DILocation(line: 203, column: 11, scope: !74)
!150 = !DILocation(line: 205, column: 5, scope: !74)
!151 = !DILocation(line: 206, column: 11, scope: !74)
!152 = !DILocation(line: 207, column: 11, scope: !74)
!153 = !DILocation(line: 208, column: 11, scope: !74)
!154 = !DILocation(line: 209, column: 11, scope: !74)
!155 = !DILocation(line: 210, column: 11, scope: !74)
!156 = !DILocation(line: 212, column: 11, scope: !74)
!157 = !DILocation(line: 213, column: 11, scope: !74)
!158 = !DILocation(line: 214, column: 11, scope: !74)
!159 = !DILocation(line: 215, column: 11, scope: !74)
!160 = !DILocation(line: 217, column: 5, scope: !74)
!161 = !DILocation(line: 218, column: 11, scope: !74)
!162 = !DILocation(line: 219, column: 11, scope: !74)
!163 = !DILocation(line: 220, column: 12, scope: !74)
!164 = !DILocation(line: 222, column: 12, scope: !74)
!165 = !DILocation(line: 223, column: 12, scope: !74)
!166 = !DILocation(line: 224, column: 12, scope: !74)
!167 = !DILocation(line: 225, column: 12, scope: !74)
!168 = !DILocation(line: 227, column: 5, scope: !74)
!169 = !DILocation(line: 228, column: 12, scope: !74)
!170 = !DILocation(line: 229, column: 12, scope: !74)
!171 = !DILocation(line: 230, column: 12, scope: !74)
!172 = !DILocation(line: 232, column: 12, scope: !74)
!173 = !DILocation(line: 233, column: 12, scope: !74)
!174 = !DILocation(line: 234, column: 12, scope: !74)
!175 = !DILocation(line: 235, column: 12, scope: !74)
!176 = !DILocation(line: 237, column: 5, scope: !74)
!177 = !DILocation(line: 238, column: 12, scope: !74)
!178 = !DILocation(line: 239, column: 12, scope: !74)
!179 = !DILocation(line: 241, column: 5, scope: !74)
!180 = !DILocation(line: 242, column: 12, scope: !74)
!181 = !DILocation(line: 243, column: 12, scope: !74)
!182 = !DILocation(line: 245, column: 5, scope: !74)
!183 = !DILocation(line: 246, column: 12, scope: !74)
!184 = !DILocation(line: 247, column: 12, scope: !74)
!185 = !DILocation(line: 249, column: 5, scope: !74)
!186 = !DILocation(line: 250, column: 12, scope: !74)
!187 = !DILocation(line: 251, column: 12, scope: !74)
!188 = !DILocation(line: 252, column: 12, scope: !74)
!189 = !DILocation(line: 253, column: 12, scope: !74)
!190 = !DILocation(line: 255, column: 5, scope: !74)
!191 = !DILocation(line: 256, column: 12, scope: !74)
!192 = !DILocation(line: 257, column: 12, scope: !74)
!193 = !DILocation(line: 259, column: 5, scope: !74)
!194 = !DILocation(line: 260, column: 12, scope: !74)
!195 = !DILocation(line: 261, column: 12, scope: !74)
!196 = !DILocation(line: 262, column: 12, scope: !74)
!197 = !DILocation(line: 263, column: 12, scope: !74)
!198 = !DILocation(line: 264, column: 5, scope: !74)
!199 = !DILocation(line: 266, column: 12, scope: !74)
!200 = !DILocation(line: 267, column: 12, scope: !74)
!201 = !DILocation(line: 268, column: 12, scope: !74)
!202 = !DILocation(line: 269, column: 12, scope: !74)
!203 = !DILocation(line: 270, column: 12, scope: !74)
!204 = !DILocation(line: 271, column: 12, scope: !74)
!205 = !DILocation(line: 273, column: 12, scope: !74)
!206 = !DILocation(line: 274, column: 12, scope: !74)
!207 = !DILocation(line: 275, column: 12, scope: !74)
!208 = !DILocation(line: 276, column: 12, scope: !74)
!209 = !DILocation(line: 277, column: 12, scope: !74)
!210 = !DILocation(line: 278, column: 12, scope: !74)
!211 = !DILocation(line: 280, column: 12, scope: !74)
!212 = !DILocation(line: 281, column: 12, scope: !74)
!213 = !DILocation(line: 282, column: 12, scope: !74)
!214 = !DILocation(line: 283, column: 5, scope: !74)
!215 = !DILocation(line: 285, column: 5, scope: !74)
!216 = !DILocation(line: 287, column: 5, scope: !74)
!217 = !DILocation(line: 289, column: 5, scope: !74)
!218 = !DILocation(line: 291, column: 5, scope: !74)
!219 = !DILocation(line: 293, column: 5, scope: !74)
!220 = !DILocation(line: 295, column: 5, scope: !74)
!221 = !DILocation(line: 297, column: 5, scope: !74)
!222 = !DILocation(line: 299, column: 5, scope: !74)
!223 = !DILocation(line: 301, column: 5, scope: !74)
!224 = !DILocation(line: 303, column: 5, scope: !74)
!225 = distinct !DISubprogram(name: "price_out_impl", linkageName: "price_out_impl", scope: null, file: !4, line: 305, type: !5, scopeLine: 305, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!226 = !DILocation(line: 319, column: 11, scope: !227)
!227 = !DILexicalBlockFile(scope: !225, file: !4, discriminator: 0)
!228 = !DILocation(line: 320, column: 11, scope: !227)
!229 = !DILocation(line: 321, column: 11, scope: !227)
!230 = !DILocation(line: 322, column: 11, scope: !227)
!231 = !DILocation(line: 323, column: 11, scope: !227)
!232 = !DILocation(line: 324, column: 11, scope: !227)
!233 = !DILocation(line: 325, column: 5, scope: !227)
!234 = !DILocation(line: 327, column: 11, scope: !227)
!235 = !DILocation(line: 328, column: 11, scope: !227)
!236 = !DILocation(line: 329, column: 11, scope: !227)
!237 = !DILocation(line: 330, column: 11, scope: !227)
!238 = !DILocation(line: 331, column: 11, scope: !227)
!239 = !DILocation(line: 332, column: 11, scope: !227)
!240 = !DILocation(line: 333, column: 11, scope: !227)
!241 = !DILocation(line: 334, column: 11, scope: !227)
!242 = !DILocation(line: 335, column: 5, scope: !227)
!243 = !DILocation(line: 337, column: 11, scope: !227)
!244 = !DILocation(line: 338, column: 11, scope: !227)
!245 = !DILocation(line: 339, column: 11, scope: !227)
!246 = !DILocation(line: 340, column: 11, scope: !227)
!247 = !DILocation(line: 341, column: 11, scope: !227)
!248 = !DILocation(line: 342, column: 11, scope: !227)
!249 = !DILocation(line: 343, column: 11, scope: !227)
!250 = !DILocation(line: 344, column: 5, scope: !227)
!251 = !DILocation(line: 346, column: 11, scope: !227)
!252 = !DILocation(line: 347, column: 11, scope: !227)
!253 = !DILocation(line: 348, column: 11, scope: !227)
!254 = !DILocation(line: 349, column: 11, scope: !227)
!255 = !DILocation(line: 350, column: 5, scope: !227)
!256 = !DILocation(line: 352, column: 5, scope: !227)
!257 = !DILocation(line: 354, column: 5, scope: !227)
!258 = !DILocation(line: 355, column: 5, scope: !227)
!259 = !DILocation(line: 357, column: 5, scope: !227)
!260 = !DILocation(line: 359, column: 5, scope: !227)
!261 = !DILocation(line: 361, column: 5, scope: !227)
!262 = !DILocation(line: 363, column: 5, scope: !227)
!263 = !DILocation(line: 365, column: 5, scope: !227)
!264 = !DILocation(line: 367, column: 5, scope: !227)
!265 = !DILocation(line: 369, column: 5, scope: !227)
!266 = !DILocation(line: 371, column: 5, scope: !227)
!267 = !DILocation(line: 373, column: 5, scope: !227)
!268 = !DILocation(line: 375, column: 5, scope: !227)
!269 = !DILocation(line: 377, column: 11, scope: !227)
!270 = !DILocation(line: 378, column: 11, scope: !227)
!271 = !DILocation(line: 379, column: 11, scope: !227)
!272 = !DILocation(line: 380, column: 11, scope: !227)
!273 = !DILocation(line: 381, column: 11, scope: !227)
!274 = !DILocation(line: 382, column: 5, scope: !227)
!275 = !DILocation(line: 384, column: 11, scope: !227)
!276 = !DILocation(line: 385, column: 5, scope: !227)
!277 = !DILocation(line: 387, column: 11, scope: !227)
!278 = !DILocation(line: 388, column: 11, scope: !227)
!279 = !DILocation(line: 389, column: 11, scope: !227)
!280 = !DILocation(line: 390, column: 11, scope: !227)
!281 = !DILocation(line: 391, column: 11, scope: !227)
!282 = !DILocation(line: 393, column: 11, scope: !227)
!283 = !DILocation(line: 394, column: 11, scope: !227)
!284 = !DILocation(line: 395, column: 5, scope: !227)
!285 = !DILocation(line: 397, column: 11, scope: !227)
!286 = !DILocation(line: 398, column: 11, scope: !227)
!287 = !DILocation(line: 399, column: 5, scope: !227)
!288 = !DILocation(line: 401, column: 5, scope: !227)
!289 = !DILocation(line: 403, column: 5, scope: !227)
!290 = !DILocation(line: 405, column: 5, scope: !227)
!291 = !DILocation(line: 407, column: 5, scope: !227)
!292 = !DILocation(line: 409, column: 5, scope: !227)
!293 = !DILocation(line: 411, column: 5, scope: !227)
!294 = !DILocation(line: 416, column: 11, scope: !227)
!295 = !DILocation(line: 417, column: 11, scope: !227)
!296 = !DILocation(line: 418, column: 5, scope: !227)
!297 = !DILocation(line: 420, column: 11, scope: !227)
!298 = !DILocation(line: 421, column: 5, scope: !227)
!299 = !DILocation(line: 423, column: 11, scope: !227)
!300 = !DILocation(line: 424, column: 11, scope: !227)
!301 = !DILocation(line: 425, column: 11, scope: !227)
!302 = !DILocation(line: 426, column: 11, scope: !227)
!303 = !DILocation(line: 427, column: 11, scope: !227)
!304 = !DILocation(line: 429, column: 11, scope: !227)
!305 = !DILocation(line: 430, column: 11, scope: !227)
!306 = !DILocation(line: 431, column: 11, scope: !227)
!307 = !DILocation(line: 432, column: 5, scope: !227)
!308 = !DILocation(line: 434, column: 11, scope: !227)
!309 = !DILocation(line: 435, column: 11, scope: !227)
!310 = !DILocation(line: 436, column: 11, scope: !227)
!311 = !DILocation(line: 437, column: 11, scope: !227)
!312 = !DILocation(line: 439, column: 11, scope: !227)
!313 = !DILocation(line: 440, column: 11, scope: !227)
!314 = !DILocation(line: 441, column: 11, scope: !227)
!315 = !DILocation(line: 442, column: 11, scope: !227)
!316 = !DILocation(line: 443, column: 11, scope: !227)
!317 = !DILocation(line: 444, column: 11, scope: !227)
!318 = !DILocation(line: 445, column: 12, scope: !227)
!319 = !DILocation(line: 447, column: 12, scope: !227)
!320 = !DILocation(line: 448, column: 12, scope: !227)
!321 = !DILocation(line: 449, column: 5, scope: !227)
!322 = !DILocation(line: 450, column: 5, scope: !227)
!323 = !DILocation(line: 452, column: 12, scope: !227)
!324 = !DILocation(line: 453, column: 12, scope: !227)
!325 = !DILocation(line: 454, column: 12, scope: !227)
!326 = !DILocation(line: 455, column: 12, scope: !227)
!327 = !DILocation(line: 457, column: 12, scope: !227)
!328 = !DILocation(line: 458, column: 12, scope: !227)
!329 = !DILocation(line: 459, column: 5, scope: !227)
!330 = !DILocation(line: 461, column: 5, scope: !227)
!331 = !DILocation(line: 463, column: 12, scope: !227)
!332 = !DILocation(line: 464, column: 12, scope: !227)
!333 = !DILocation(line: 465, column: 12, scope: !227)
!334 = !DILocation(line: 466, column: 12, scope: !227)
!335 = !DILocation(line: 468, column: 12, scope: !227)
!336 = !DILocation(line: 469, column: 12, scope: !227)
!337 = !DILocation(line: 470, column: 12, scope: !227)
!338 = !DILocation(line: 471, column: 12, scope: !227)
!339 = !DILocation(line: 472, column: 12, scope: !227)
!340 = !DILocation(line: 473, column: 12, scope: !227)
!341 = !DILocation(line: 474, column: 12, scope: !227)
!342 = !DILocation(line: 475, column: 12, scope: !227)
!343 = !DILocation(line: 477, column: 12, scope: !227)
!344 = !DILocation(line: 478, column: 12, scope: !227)
!345 = !DILocation(line: 479, column: 12, scope: !227)
!346 = !DILocation(line: 480, column: 12, scope: !227)
!347 = !DILocation(line: 481, column: 12, scope: !227)
!348 = !DILocation(line: 482, column: 12, scope: !227)
!349 = !DILocation(line: 483, column: 12, scope: !227)
!350 = !DILocation(line: 485, column: 12, scope: !227)
!351 = !DILocation(line: 486, column: 12, scope: !227)
!352 = !DILocation(line: 487, column: 12, scope: !227)
!353 = !DILocation(line: 488, column: 5, scope: !227)
!354 = !DILocation(line: 490, column: 12, scope: !227)
!355 = !DILocation(line: 491, column: 5, scope: !227)
!356 = !DILocation(line: 493, column: 12, scope: !227)
!357 = !DILocation(line: 494, column: 12, scope: !227)
!358 = !DILocation(line: 495, column: 12, scope: !227)
!359 = !DILocation(line: 496, column: 12, scope: !227)
!360 = !DILocation(line: 498, column: 12, scope: !227)
!361 = !DILocation(line: 499, column: 12, scope: !227)
!362 = !DILocation(line: 500, column: 12, scope: !227)
!363 = !DILocation(line: 501, column: 12, scope: !227)
!364 = !DILocation(line: 502, column: 12, scope: !227)
!365 = !DILocation(line: 503, column: 12, scope: !227)
!366 = !DILocation(line: 504, column: 12, scope: !227)
!367 = !DILocation(line: 505, column: 12, scope: !227)
!368 = !DILocation(line: 507, column: 12, scope: !227)
!369 = !DILocation(line: 508, column: 12, scope: !227)
!370 = !DILocation(line: 509, column: 12, scope: !227)
!371 = !DILocation(line: 510, column: 5, scope: !227)
!372 = !DILocation(line: 512, column: 12, scope: !227)
!373 = !DILocation(line: 513, column: 12, scope: !227)
!374 = !DILocation(line: 514, column: 5, scope: !227)
!375 = !DILocation(line: 516, column: 12, scope: !227)
!376 = !DILocation(line: 517, column: 12, scope: !227)
!377 = !DILocation(line: 518, column: 12, scope: !227)
!378 = !DILocation(line: 519, column: 12, scope: !227)
!379 = !DILocation(line: 520, column: 12, scope: !227)
!380 = !DILocation(line: 521, column: 12, scope: !227)
!381 = !DILocation(line: 522, column: 12, scope: !227)
!382 = !DILocation(line: 523, column: 5, scope: !227)
!383 = !DILocation(line: 525, column: 12, scope: !227)
!384 = !DILocation(line: 526, column: 12, scope: !227)
!385 = !DILocation(line: 527, column: 5, scope: !227)
!386 = !DILocation(line: 529, column: 5, scope: !227)
!387 = !DILocation(line: 530, column: 12, scope: !227)
!388 = !DILocation(line: 531, column: 5, scope: !227)
!389 = !DILocation(line: 533, column: 12, scope: !227)
!390 = !DILocation(line: 534, column: 12, scope: !227)
!391 = !DILocation(line: 535, column: 12, scope: !227)
!392 = !DILocation(line: 537, column: 12, scope: !227)
!393 = !DILocation(line: 538, column: 12, scope: !227)
!394 = !DILocation(line: 539, column: 5, scope: !227)
!395 = !DILocation(line: 541, column: 5, scope: !227)
!396 = !DILocation(line: 542, column: 5, scope: !227)
!397 = !DILocation(line: 544, column: 5, scope: !227)
!398 = !DILocation(line: 546, column: 5, scope: !227)
!399 = !DILocation(line: 548, column: 5, scope: !227)
!400 = !DILocation(line: 550, column: 5, scope: !227)
!401 = !DILocation(line: 552, column: 5, scope: !227)
!402 = !DILocation(line: 554, column: 12, scope: !227)
!403 = !DILocation(line: 555, column: 12, scope: !227)
!404 = !DILocation(line: 556, column: 5, scope: !227)
!405 = !DILocation(line: 558, column: 5, scope: !227)
!406 = !DILocation(line: 560, column: 5, scope: !227)
!407 = !DILocation(line: 562, column: 5, scope: !227)
!408 = !DILocation(line: 564, column: 5, scope: !227)
!409 = !DILocation(line: 566, column: 12, scope: !227)
!410 = !DILocation(line: 567, column: 12, scope: !227)
!411 = !DILocation(line: 568, column: 5, scope: !227)
!412 = !DILocation(line: 570, column: 12, scope: !227)
!413 = !DILocation(line: 571, column: 5, scope: !227)
!414 = !DILocation(line: 573, column: 12, scope: !227)
!415 = !DILocation(line: 574, column: 12, scope: !227)
!416 = !DILocation(line: 575, column: 5, scope: !227)
!417 = !DILocation(line: 576, column: 12, scope: !227)
!418 = !DILocation(line: 577, column: 5, scope: !227)
!419 = !DILocation(line: 579, column: 5, scope: !227)
!420 = !DILocation(line: 581, column: 12, scope: !227)
!421 = !DILocation(line: 582, column: 5, scope: !227)
!422 = !DILocation(line: 584, column: 12, scope: !227)
!423 = !DILocation(line: 585, column: 12, scope: !227)
!424 = !DILocation(line: 586, column: 12, scope: !227)
!425 = !DILocation(line: 587, column: 12, scope: !227)
!426 = !DILocation(line: 589, column: 5, scope: !227)
!427 = !DILocation(line: 590, column: 12, scope: !227)
!428 = !DILocation(line: 591, column: 12, scope: !227)
!429 = !DILocation(line: 592, column: 12, scope: !227)
!430 = !DILocation(line: 593, column: 12, scope: !227)
!431 = !DILocation(line: 595, column: 5, scope: !227)
!432 = !DILocation(line: 596, column: 12, scope: !227)
!433 = !DILocation(line: 597, column: 5, scope: !227)
!434 = !DILocation(line: 599, column: 5, scope: !227)
!435 = !DILocation(line: 601, column: 5, scope: !227)
!436 = !DILocation(line: 603, column: 12, scope: !227)
!437 = !DILocation(line: 604, column: 5, scope: !227)
!438 = !DILocation(line: 606, column: 12, scope: !227)
!439 = !DILocation(line: 607, column: 12, scope: !227)
!440 = !DILocation(line: 608, column: 12, scope: !227)
!441 = !DILocation(line: 609, column: 12, scope: !227)
!442 = !DILocation(line: 611, column: 5, scope: !227)
!443 = !DILocation(line: 612, column: 12, scope: !227)
!444 = !DILocation(line: 613, column: 12, scope: !227)
!445 = !DILocation(line: 614, column: 12, scope: !227)
!446 = !DILocation(line: 615, column: 12, scope: !227)
!447 = !DILocation(line: 617, column: 5, scope: !227)
!448 = !DILocation(line: 618, column: 12, scope: !227)
!449 = !DILocation(line: 619, column: 12, scope: !227)
!450 = !DILocation(line: 620, column: 12, scope: !227)
!451 = !DILocation(line: 621, column: 12, scope: !227)
!452 = !DILocation(line: 622, column: 12, scope: !227)
!453 = !DILocation(line: 624, column: 12, scope: !227)
!454 = !DILocation(line: 625, column: 12, scope: !227)
!455 = !DILocation(line: 626, column: 12, scope: !227)
!456 = !DILocation(line: 627, column: 12, scope: !227)
!457 = !DILocation(line: 628, column: 12, scope: !227)
!458 = !DILocation(line: 629, column: 12, scope: !227)
!459 = !DILocation(line: 631, column: 5, scope: !227)
!460 = !DILocation(line: 632, column: 12, scope: !227)
!461 = !DILocation(line: 633, column: 12, scope: !227)
!462 = !DILocation(line: 635, column: 12, scope: !227)
!463 = !DILocation(line: 636, column: 12, scope: !227)
!464 = !DILocation(line: 637, column: 5, scope: !227)
!465 = !DILocation(line: 638, column: 12, scope: !227)
!466 = !DILocation(line: 639, column: 12, scope: !227)
!467 = !DILocation(line: 640, column: 12, scope: !227)
!468 = !DILocation(line: 641, column: 12, scope: !227)
!469 = !DILocation(line: 642, column: 12, scope: !227)
!470 = !DILocation(line: 644, column: 12, scope: !227)
!471 = !DILocation(line: 645, column: 12, scope: !227)
!472 = !DILocation(line: 646, column: 12, scope: !227)
!473 = !DILocation(line: 647, column: 12, scope: !227)
!474 = !DILocation(line: 648, column: 12, scope: !227)
!475 = !DILocation(line: 649, column: 12, scope: !227)
!476 = !DILocation(line: 651, column: 5, scope: !227)
!477 = !DILocation(line: 652, column: 12, scope: !227)
!478 = !DILocation(line: 653, column: 12, scope: !227)
!479 = !DILocation(line: 655, column: 12, scope: !227)
!480 = !DILocation(line: 656, column: 12, scope: !227)
!481 = !DILocation(line: 657, column: 5, scope: !227)
!482 = !DILocation(line: 658, column: 12, scope: !227)
!483 = !DILocation(line: 659, column: 5, scope: !227)
!484 = !DILocation(line: 661, column: 5, scope: !227)
!485 = !DILocation(line: 663, column: 12, scope: !227)
!486 = !DILocation(line: 664, column: 12, scope: !227)
!487 = !DILocation(line: 665, column: 12, scope: !227)
!488 = !DILocation(line: 666, column: 5, scope: !227)
!489 = !DILocation(line: 667, column: 12, scope: !227)
!490 = !DILocation(line: 668, column: 12, scope: !227)
!491 = !DILocation(line: 669, column: 12, scope: !227)
!492 = !DILocation(line: 670, column: 5, scope: !227)
!493 = !DILocation(line: 671, column: 12, scope: !227)
!494 = !DILocation(line: 672, column: 12, scope: !227)
!495 = !DILocation(line: 673, column: 5, scope: !227)
!496 = !DILocation(line: 674, column: 5, scope: !227)
!497 = !DILocation(line: 676, column: 5, scope: !227)
!498 = !DILocation(line: 678, column: 5, scope: !227)
!499 = !DILocation(line: 680, column: 5, scope: !227)
!500 = !DILocation(line: 682, column: 5, scope: !227)
!501 = distinct !DISubprogram(name: "insert_new_arc", linkageName: "insert_new_arc", scope: null, file: !4, line: 685, type: !5, scopeLine: 685, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!502 = !DILocation(line: 691, column: 10, scope: !503)
!503 = !DILexicalBlockFile(scope: !501, file: !4, discriminator: 0)
!504 = !DILocation(line: 692, column: 10, scope: !503)
!505 = !DILocation(line: 693, column: 10, scope: !503)
!506 = !DILocation(line: 694, column: 10, scope: !503)
!507 = !DILocation(line: 695, column: 10, scope: !503)
!508 = !DILocation(line: 697, column: 5, scope: !503)
!509 = !DILocation(line: 698, column: 11, scope: !503)
!510 = !DILocation(line: 699, column: 11, scope: !503)
!511 = !DILocation(line: 700, column: 11, scope: !503)
!512 = !DILocation(line: 701, column: 11, scope: !503)
!513 = !DILocation(line: 703, column: 5, scope: !503)
!514 = !DILocation(line: 704, column: 11, scope: !503)
!515 = !DILocation(line: 705, column: 11, scope: !503)
!516 = !DILocation(line: 706, column: 11, scope: !503)
!517 = !DILocation(line: 707, column: 11, scope: !503)
!518 = !DILocation(line: 709, column: 5, scope: !503)
!519 = !DILocation(line: 710, column: 11, scope: !503)
!520 = !DILocation(line: 711, column: 11, scope: !503)
!521 = !DILocation(line: 712, column: 11, scope: !503)
!522 = !DILocation(line: 713, column: 11, scope: !503)
!523 = !DILocation(line: 715, column: 5, scope: !503)
!524 = !DILocation(line: 716, column: 11, scope: !503)
!525 = !DILocation(line: 717, column: 11, scope: !503)
!526 = !DILocation(line: 718, column: 11, scope: !503)
!527 = !DILocation(line: 719, column: 11, scope: !503)
!528 = !DILocation(line: 721, column: 5, scope: !503)
!529 = !DILocation(line: 722, column: 11, scope: !503)
!530 = !DILocation(line: 723, column: 5, scope: !503)
!531 = !DILocation(line: 725, column: 11, scope: !503)
!532 = !DILocation(line: 726, column: 11, scope: !503)
!533 = !DILocation(line: 727, column: 5, scope: !503)
!534 = !DILocation(line: 729, column: 11, scope: !503)
!535 = !DILocation(line: 730, column: 11, scope: !503)
!536 = !DILocation(line: 731, column: 11, scope: !503)
!537 = !DILocation(line: 732, column: 11, scope: !503)
!538 = !DILocation(line: 733, column: 11, scope: !503)
!539 = !DILocation(line: 734, column: 11, scope: !503)
!540 = !DILocation(line: 735, column: 11, scope: !503)
!541 = !DILocation(line: 737, column: 11, scope: !503)
!542 = !DILocation(line: 738, column: 11, scope: !503)
!543 = !DILocation(line: 739, column: 11, scope: !503)
!544 = !DILocation(line: 740, column: 5, scope: !503)
!545 = !DILocation(line: 742, column: 11, scope: !503)
!546 = !DILocation(line: 743, column: 11, scope: !503)
!547 = !DILocation(line: 744, column: 11, scope: !503)
!548 = !DILocation(line: 745, column: 11, scope: !503)
!549 = !DILocation(line: 746, column: 11, scope: !503)
!550 = !DILocation(line: 747, column: 11, scope: !503)
!551 = !DILocation(line: 749, column: 11, scope: !503)
!552 = !DILocation(line: 750, column: 11, scope: !503)
!553 = !DILocation(line: 751, column: 11, scope: !503)
!554 = !DILocation(line: 752, column: 11, scope: !503)
!555 = !DILocation(line: 754, column: 5, scope: !503)
!556 = !DILocation(line: 755, column: 11, scope: !503)
!557 = !DILocation(line: 756, column: 11, scope: !503)
!558 = !DILocation(line: 757, column: 11, scope: !503)
!559 = !DILocation(line: 758, column: 11, scope: !503)
!560 = !DILocation(line: 759, column: 11, scope: !503)
!561 = !DILocation(line: 761, column: 11, scope: !503)
!562 = !DILocation(line: 762, column: 11, scope: !503)
!563 = !DILocation(line: 763, column: 11, scope: !503)
!564 = !DILocation(line: 764, column: 11, scope: !503)
!565 = !DILocation(line: 766, column: 5, scope: !503)
!566 = !DILocation(line: 767, column: 11, scope: !503)
!567 = !DILocation(line: 768, column: 11, scope: !503)
!568 = !DILocation(line: 769, column: 11, scope: !503)
!569 = !DILocation(line: 770, column: 11, scope: !503)
!570 = !DILocation(line: 771, column: 11, scope: !503)
!571 = !DILocation(line: 773, column: 11, scope: !503)
!572 = !DILocation(line: 774, column: 11, scope: !503)
!573 = !DILocation(line: 775, column: 11, scope: !503)
!574 = !DILocation(line: 776, column: 11, scope: !503)
!575 = !DILocation(line: 778, column: 5, scope: !503)
!576 = !DILocation(line: 779, column: 11, scope: !503)
!577 = !DILocation(line: 780, column: 11, scope: !503)
!578 = !DILocation(line: 781, column: 11, scope: !503)
!579 = !DILocation(line: 783, column: 11, scope: !503)
!580 = !DILocation(line: 784, column: 11, scope: !503)
!581 = !DILocation(line: 785, column: 11, scope: !503)
!582 = !DILocation(line: 786, column: 11, scope: !503)
!583 = !DILocation(line: 788, column: 5, scope: !503)
!584 = !DILocation(line: 789, column: 11, scope: !503)
!585 = !DILocation(line: 790, column: 11, scope: !503)
!586 = !DILocation(line: 791, column: 11, scope: !503)
!587 = !DILocation(line: 793, column: 11, scope: !503)
!588 = !DILocation(line: 794, column: 11, scope: !503)
!589 = !DILocation(line: 795, column: 11, scope: !503)
!590 = !DILocation(line: 796, column: 11, scope: !503)
!591 = !DILocation(line: 798, column: 5, scope: !503)
!592 = !DILocation(line: 799, column: 11, scope: !503)
!593 = !DILocation(line: 800, column: 11, scope: !503)
!594 = !DILocation(line: 802, column: 5, scope: !503)
!595 = !DILocation(line: 803, column: 12, scope: !503)
!596 = !DILocation(line: 804, column: 12, scope: !503)
!597 = !DILocation(line: 806, column: 5, scope: !503)
!598 = !DILocation(line: 807, column: 12, scope: !503)
!599 = !DILocation(line: 808, column: 12, scope: !503)
!600 = !DILocation(line: 810, column: 5, scope: !503)
!601 = !DILocation(line: 811, column: 12, scope: !503)
!602 = !DILocation(line: 812, column: 12, scope: !503)
!603 = !DILocation(line: 813, column: 12, scope: !503)
!604 = !DILocation(line: 814, column: 12, scope: !503)
!605 = !DILocation(line: 816, column: 5, scope: !503)
!606 = !DILocation(line: 817, column: 12, scope: !503)
!607 = !DILocation(line: 818, column: 12, scope: !503)
!608 = !DILocation(line: 820, column: 5, scope: !503)
!609 = !DILocation(line: 821, column: 5, scope: !503)
!610 = !DILocation(line: 823, column: 5, scope: !503)
!611 = !DILocation(line: 825, column: 5, scope: !503)
!612 = !DILocation(line: 827, column: 5, scope: !503)
!613 = !DILocation(line: 829, column: 5, scope: !503)
!614 = !DILocation(line: 831, column: 5, scope: !503)
!615 = distinct !DISubprogram(name: "suspend_impl", linkageName: "suspend_impl", scope: null, file: !4, line: 833, type: !5, scopeLine: 833, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!616 = !DILocation(line: 839, column: 10, scope: !617)
!617 = !DILexicalBlockFile(scope: !615, file: !4, discriminator: 0)
!618 = !DILocation(line: 840, column: 5, scope: !617)
!619 = !DILocation(line: 842, column: 10, scope: !617)
!620 = !DILocation(line: 843, column: 10, scope: !617)
!621 = !DILocation(line: 844, column: 5, scope: !617)
!622 = !DILocation(line: 846, column: 10, scope: !617)
!623 = !DILocation(line: 847, column: 10, scope: !617)
!624 = !DILocation(line: 848, column: 11, scope: !617)
!625 = !DILocation(line: 849, column: 11, scope: !617)
!626 = !DILocation(line: 850, column: 11, scope: !617)
!627 = !DILocation(line: 851, column: 11, scope: !617)
!628 = !DILocation(line: 852, column: 11, scope: !617)
!629 = !DILocation(line: 853, column: 11, scope: !617)
!630 = !DILocation(line: 854, column: 11, scope: !617)
!631 = !DILocation(line: 855, column: 11, scope: !617)
!632 = !DILocation(line: 856, column: 5, scope: !617)
!633 = !DILocation(line: 858, column: 11, scope: !617)
!634 = !DILocation(line: 859, column: 5, scope: !617)
!635 = !DILocation(line: 861, column: 11, scope: !617)
!636 = !DILocation(line: 862, column: 11, scope: !617)
!637 = !DILocation(line: 863, column: 11, scope: !617)
!638 = !DILocation(line: 864, column: 11, scope: !617)
!639 = !DILocation(line: 866, column: 11, scope: !617)
!640 = !DILocation(line: 867, column: 11, scope: !617)
!641 = !DILocation(line: 868, column: 5, scope: !617)
!642 = !DILocation(line: 870, column: 11, scope: !617)
!643 = !DILocation(line: 871, column: 11, scope: !617)
!644 = !DILocation(line: 872, column: 11, scope: !617)
!645 = !DILocation(line: 873, column: 11, scope: !617)
!646 = !DILocation(line: 875, column: 11, scope: !617)
!647 = !DILocation(line: 876, column: 11, scope: !617)
!648 = !DILocation(line: 877, column: 11, scope: !617)
!649 = !DILocation(line: 878, column: 11, scope: !617)
!650 = !DILocation(line: 879, column: 11, scope: !617)
!651 = !DILocation(line: 881, column: 11, scope: !617)
!652 = !DILocation(line: 882, column: 11, scope: !617)
!653 = !DILocation(line: 883, column: 11, scope: !617)
!654 = !DILocation(line: 884, column: 11, scope: !617)
!655 = !DILocation(line: 885, column: 11, scope: !617)
!656 = !DILocation(line: 886, column: 11, scope: !617)
!657 = !DILocation(line: 887, column: 11, scope: !617)
!658 = !DILocation(line: 888, column: 11, scope: !617)
!659 = !DILocation(line: 890, column: 11, scope: !617)
!660 = !DILocation(line: 891, column: 11, scope: !617)
!661 = !DILocation(line: 892, column: 11, scope: !617)
!662 = !DILocation(line: 893, column: 11, scope: !617)
!663 = !DILocation(line: 894, column: 5, scope: !617)
!664 = !DILocation(line: 896, column: 11, scope: !617)
!665 = !DILocation(line: 897, column: 11, scope: !617)
!666 = !DILocation(line: 899, column: 11, scope: !617)
!667 = !DILocation(line: 900, column: 11, scope: !617)
!668 = !DILocation(line: 901, column: 5, scope: !617)
!669 = !DILocation(line: 903, column: 11, scope: !617)
!670 = !DILocation(line: 904, column: 11, scope: !617)
!671 = !DILocation(line: 905, column: 11, scope: !617)
!672 = !DILocation(line: 906, column: 11, scope: !617)
!673 = !DILocation(line: 908, column: 11, scope: !617)
!674 = !DILocation(line: 909, column: 11, scope: !617)
!675 = !DILocation(line: 910, column: 11, scope: !617)
!676 = !DILocation(line: 911, column: 11, scope: !617)
!677 = !DILocation(line: 912, column: 5, scope: !617)
!678 = !DILocation(line: 914, column: 11, scope: !617)
!679 = !DILocation(line: 915, column: 11, scope: !617)
!680 = !DILocation(line: 917, column: 11, scope: !617)
!681 = !DILocation(line: 918, column: 11, scope: !617)
!682 = !DILocation(line: 919, column: 5, scope: !617)
!683 = !DILocation(line: 920, column: 5, scope: !617)
!684 = !DILocation(line: 922, column: 11, scope: !617)
!685 = !DILocation(line: 923, column: 11, scope: !617)
!686 = !DILocation(line: 924, column: 11, scope: !617)
!687 = !DILocation(line: 925, column: 11, scope: !617)
!688 = !DILocation(line: 927, column: 11, scope: !617)
!689 = !DILocation(line: 928, column: 11, scope: !617)
!690 = !DILocation(line: 929, column: 5, scope: !617)
!691 = !DILocation(line: 930, column: 5, scope: !617)
!692 = !DILocation(line: 932, column: 5, scope: !617)
!693 = !DILocation(line: 934, column: 5, scope: !617)
!694 = !DILocation(line: 936, column: 5, scope: !617)
!695 = !DILocation(line: 938, column: 11, scope: !617)
!696 = !DILocation(line: 939, column: 5, scope: !617)
!697 = !DILocation(line: 941, column: 11, scope: !617)
!698 = !DILocation(line: 942, column: 5, scope: !617)
!699 = !DILocation(line: 944, column: 11, scope: !617)
!700 = !DILocation(line: 945, column: 11, scope: !617)
!701 = !DILocation(line: 946, column: 11, scope: !617)
!702 = !DILocation(line: 948, column: 11, scope: !617)
!703 = !DILocation(line: 949, column: 11, scope: !617)
!704 = !DILocation(line: 950, column: 11, scope: !617)
!705 = !DILocation(line: 951, column: 11, scope: !617)
!706 = !DILocation(line: 953, column: 5, scope: !617)
!707 = !DILocation(line: 954, column: 11, scope: !617)
!708 = !DILocation(line: 955, column: 5, scope: !617)
!709 = !DILocation(line: 957, column: 5, scope: !617)
!710 = !DILocation(line: 959, column: 11, scope: !617)
!711 = !DILocation(line: 960, column: 5, scope: !617)
!712 = !DILocation(line: 962, column: 5, scope: !617)
!713 = !DILocation(line: 964, column: 5, scope: !617)
!714 = !DILocation(line: 966, column: 11, scope: !617)
!715 = !DILocation(line: 967, column: 5, scope: !617)
!716 = !DILocation(line: 969, column: 12, scope: !617)
!717 = !DILocation(line: 970, column: 12, scope: !617)
!718 = !DILocation(line: 971, column: 12, scope: !617)
!719 = !DILocation(line: 972, column: 5, scope: !617)
!720 = !DILocation(line: 973, column: 12, scope: !617)
!721 = !DILocation(line: 974, column: 12, scope: !617)
!722 = !DILocation(line: 975, column: 12, scope: !617)
!723 = !DILocation(line: 976, column: 5, scope: !617)
!724 = !DILocation(line: 977, column: 12, scope: !617)
!725 = !DILocation(line: 978, column: 12, scope: !617)
!726 = !DILocation(line: 979, column: 12, scope: !617)
!727 = !DILocation(line: 980, column: 12, scope: !617)
!728 = !DILocation(line: 981, column: 5, scope: !617)
!729 = !DILocation(line: 982, column: 12, scope: !617)
!730 = !DILocation(line: 983, column: 12, scope: !617)
!731 = !DILocation(line: 984, column: 12, scope: !617)
!732 = !DILocation(line: 985, column: 5, scope: !617)
!733 = !DILocation(line: 986, column: 5, scope: !617)
!734 = !DILocation(line: 987, column: 5, scope: !617)
!735 = !DILocation(line: 989, column: 5, scope: !617)
