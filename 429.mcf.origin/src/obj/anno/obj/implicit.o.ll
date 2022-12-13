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

declare ptr @cache_access_mut(i128)

declare ptr @cache_access(i128)

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

declare void @replace_weaker_arc(ptr, ptr, ptr, ptr, i64, i64)

define i64 @price_out_impl(ptr %0) !dbg !72 {
  %2 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 18, !dbg !73
  %3 = load i64, ptr %2, align 8, !dbg !75
  %4 = add i64 %3, -15, !dbg !76
  %5 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 3, !dbg !77
  %6 = load i64, ptr %5, align 8, !dbg !78
  %7 = icmp sle i64 %6, 15000, !dbg !79
  br i1 %7, label %8, label %44, !dbg !80

8:                                                ; preds = %1
  %9 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !81
  %10 = load i64, ptr %9, align 8, !dbg !82
  %11 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 9, !dbg !83
  %12 = load i64, ptr %11, align 8, !dbg !84
  %13 = add i64 %10, %12, !dbg !85
  %14 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 4, !dbg !86
  %15 = load i64, ptr %14, align 8, !dbg !87
  %16 = icmp sgt i64 %13, %15, !dbg !88
  br i1 %16, label %17, label %38, !dbg !89

17:                                               ; preds = %8
  %18 = load i64, ptr %5, align 8, !dbg !90
  %19 = mul i64 %18, %18, !dbg !91
  %20 = sdiv i64 %19, 2, !dbg !92
  %21 = load i64, ptr %9, align 8, !dbg !93
  %22 = add i64 %20, %21, !dbg !94
  %23 = load i64, ptr %14, align 8, !dbg !95
  %24 = icmp sgt i64 %22, %23, !dbg !96
  br i1 %24, label %25, label %33, !dbg !97

25:                                               ; preds = %17
  %26 = call i64 @resize_prob(ptr %0), !dbg !98
  %27 = icmp ne i64 %26, 0, !dbg !99
  %28 = icmp eq i64 %26, 0, !dbg !100
  %29 = select i1 %27, i64 -1, i64 undef, !dbg !101
  br i1 %27, label %30, label %31, !dbg !102

30:                                               ; preds = %25
  br label %32, !dbg !103

31:                                               ; preds = %25
  call void @refresh_neighbour_lists(ptr %0), !dbg !104
  br label %32, !dbg !105

32:                                               ; preds = %30, %31
  br label %34, !dbg !106

33:                                               ; preds = %17
  br label %34, !dbg !107

34:                                               ; preds = %32, %33
  %35 = phi i1 [ true, %33 ], [ %28, %32 ]
  %36 = phi i64 [ undef, %33 ], [ %29, %32 ]
  br label %37, !dbg !108

37:                                               ; preds = %34
  br label %39, !dbg !109

38:                                               ; preds = %8
  br label %39, !dbg !110

39:                                               ; preds = %37, %38
  %40 = phi i1 [ true, %38 ], [ %35, %37 ]
  %41 = phi i64 [ undef, %38 ], [ %36, %37 ]
  %42 = phi i1 [ false, %38 ], [ %24, %37 ]
  br label %43, !dbg !111

43:                                               ; preds = %39
  br label %45, !dbg !112

44:                                               ; preds = %1
  br label %45, !dbg !113

45:                                               ; preds = %43, %44
  %46 = phi i1 [ true, %44 ], [ %40, %43 ]
  %47 = phi i64 [ undef, %44 ], [ %41, %43 ]
  %48 = phi i1 [ false, %44 ], [ %42, %43 ]
  br label %49, !dbg !114

49:                                               ; preds = %45
  br i1 %46, label %50, label %293, !dbg !115

50:                                               ; preds = %49
  %51 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !116
  %52 = load ptr, ptr %51, align 8, !dbg !117
  %53 = load i64, ptr %5, align 8, !dbg !118
  %54 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !119
  %55 = load ptr, ptr %54, align 8, !dbg !120
  br label %56, !dbg !121

56:                                               ; preds = %81, %50
  %57 = phi i64 [ %79, %81 ], [ 0, %50 ]
  %58 = phi ptr [ %80, %81 ], [ %55, %50 ]
  %59 = icmp slt i64 %57, %53, !dbg !122
  br i1 %59, label %60, label %76, !dbg !123

60:                                               ; preds = %56
  %61 = getelementptr %_Lowered_rarc, ptr %58, i32 1, !dbg !124
  %62 = getelementptr %_Lowered_rarc, ptr %61, i32 0, i32 3, !dbg !125
  %63 = ptrtoint ptr %62 to i64, !dbg !126
  %64 = call i128 @cache_request(i64 %63), !dbg !127
  %65 = call ptr @cache_access(i128 %64), !dbg !128
  %66 = load i32, ptr %65, align 4, !dbg !129
  %67 = icmp eq i32 %66, -1, !dbg !130
  br i1 %67, label %68, label %71, !dbg !131

68:                                               ; preds = %60
  %69 = add i64 %57, 1, !dbg !132
  %70 = getelementptr %_Lowered_rarc, ptr %58, i32 3, !dbg !133
  br label %72, !dbg !134

71:                                               ; preds = %60
  br label %72, !dbg !135

72:                                               ; preds = %68, %71
  %73 = phi i64 [ %57, %71 ], [ %69, %68 ]
  %74 = phi ptr [ %58, %71 ], [ %70, %68 ]
  br label %75, !dbg !136

75:                                               ; preds = %72
  br label %77, !dbg !137

76:                                               ; preds = %56
  br label %77, !dbg !138

77:                                               ; preds = %75, %76
  %78 = phi i1 [ false, %76 ], [ %67, %75 ]
  %79 = phi i64 [ %57, %76 ], [ %73, %75 ]
  %80 = phi ptr [ %58, %76 ], [ %74, %75 ]
  br label %81, !dbg !139

81:                                               ; preds = %77
  br i1 %78, label %56, label %82, !dbg !140

82:                                               ; preds = %81
  %83 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !141
  %84 = getelementptr %_Lowered_rarc, ptr %52, i32 0, i32 6, !dbg !142
  br label %85, !dbg !143

85:                                               ; preds = %212, %82
  %86 = phi i64 [ %214, %212 ], [ %79, %82 ]
  %87 = phi i64 [ %210, %212 ], [ undef, %82 ]
  %88 = phi i64 [ %211, %212 ], [ 0, %82 ]
  %89 = phi ptr [ %100, %212 ], [ null, %82 ]
  %90 = phi ptr [ %213, %212 ], [ %80, %82 ]
  %91 = icmp slt i64 %86, %53, !dbg !144
  br i1 %91, label %92, label %215, !dbg !145

92:                                               ; preds = %85
  %93 = getelementptr %_Lowered_rarc, ptr %90, i32 1, !dbg !146
  %94 = getelementptr %_Lowered_rarc, ptr %93, i32 0, i32 3, !dbg !147
  %95 = ptrtoint ptr %94 to i64, !dbg !148
  %96 = call i128 @cache_request(i64 %95), !dbg !149
  %97 = call ptr @cache_access(i128 %96), !dbg !150
  %98 = load i32, ptr %97, align 4, !dbg !151
  %99 = icmp ne i32 %98, -1, !dbg !152
  %100 = select i1 %99, ptr %93, ptr %89, !dbg !153
  br i1 %99, label %101, label %115, !dbg !154

101:                                              ; preds = %92
  %102 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 2, !dbg !155
  %103 = ptrtoint ptr %102 to i64, !dbg !156
  %104 = call i128 @cache_request(i64 %103), !dbg !157
  %105 = call ptr @cache_access(i128 %104), !dbg !158
  %106 = load ptr, ptr %105, align 8, !dbg !159
  %107 = getelementptr %_Lowered_node, ptr %106, i32 0, i32 7, !dbg !160
  %108 = load ptr, ptr %107, align 8, !dbg !161
  %109 = getelementptr %_Lowered_rarc, ptr %108, i32 0, i32 2, !dbg !162
  %110 = ptrtoint ptr %109 to i64, !dbg !163
  %111 = call i128 @cache_request(i64 %110), !dbg !164
  %112 = call ptr @cache_access(i128 %111), !dbg !165
  %113 = load ptr, ptr %112, align 8, !dbg !166
  %114 = getelementptr %_Lowered_node, ptr %113, i32 0, i32 9, !dbg !167
  store ptr %89, ptr %114, align 8, !dbg !168
  br label %115, !dbg !169

115:                                              ; preds = %101, %92
  %116 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 3, !dbg !170
  %117 = ptrtoint ptr %116 to i64, !dbg !171
  %118 = call i128 @cache_request(i64 %117), !dbg !172
  %119 = call ptr @cache_access(i128 %118), !dbg !173
  %120 = load i32, ptr %119, align 4, !dbg !174
  %121 = icmp eq i32 %120, -1, !dbg !175
  br i1 %121, label %122, label %123, !dbg !176

122:                                              ; preds = %115
  br label %209, !dbg !177

123:                                              ; preds = %115
  %124 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 2, !dbg !178
  %125 = ptrtoint ptr %124 to i64, !dbg !179
  %126 = call i128 @cache_request(i64 %125), !dbg !180
  %127 = call ptr @cache_access(i128 %126), !dbg !181
  %128 = load ptr, ptr %127, align 8, !dbg !182
  %129 = getelementptr %_Lowered_node, ptr %128, i32 0, i32 13, !dbg !183
  %130 = load i32, ptr %129, align 4, !dbg !184
  %131 = sext i32 %130 to i64, !dbg !185
  %132 = getelementptr %_Lowered_rarc, ptr %90, i32 0, i32 7, !dbg !186
  %133 = ptrtoint ptr %132 to i64, !dbg !187
  %134 = call i128 @cache_request(i64 %133), !dbg !188
  %135 = call ptr @cache_access(i128 %134), !dbg !189
  %136 = load i64, ptr %135, align 8, !dbg !190
  %137 = sub i64 %131, %136, !dbg !191
  %138 = add i64 %137, %4, !dbg !192
  %139 = getelementptr %_Lowered_rarc, ptr %100, i32 0, i32 1, !dbg !193
  %140 = ptrtoint ptr %139 to i64, !dbg !194
  %141 = call i128 @cache_request(i64 %140), !dbg !195
  %142 = call ptr @cache_access(i128 %141), !dbg !196
  %143 = load ptr, ptr %142, align 8, !dbg !197
  %144 = getelementptr %_Lowered_node, ptr %143, i32 0, i32 9, !dbg !198
  %145 = load ptr, ptr %144, align 8, !dbg !199
  br label %146, !dbg !200

146:                                              ; preds = %207, %123
  %147 = phi i64 [ %204, %207 ], [ %87, %123 ]
  %148 = phi i64 [ %205, %207 ], [ %88, %123 ]
  %149 = phi ptr [ %206, %207 ], [ %145, %123 ]
  %150 = icmp ne ptr %149, null, !dbg !201
  br i1 %150, label %151, label %208, !dbg !202

151:                                              ; preds = %146
  %152 = phi i64 [ %147, %146 ]
  %153 = phi i64 [ %148, %146 ]
  %154 = phi ptr [ %149, %146 ]
  %155 = getelementptr %_Lowered_rarc, ptr %154, i32 0, i32 1, !dbg !203
  %156 = ptrtoint ptr %155 to i64, !dbg !204
  %157 = call i128 @cache_request(i64 %156), !dbg !205
  %158 = call ptr @cache_access(i128 %157), !dbg !206
  %159 = load ptr, ptr %158, align 8, !dbg !207
  %160 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 13, !dbg !208
  %161 = load i32, ptr %160, align 4, !dbg !209
  %162 = sext i32 %161 to i64, !dbg !210
  %163 = getelementptr %_Lowered_rarc, ptr %154, i32 0, i32 7, !dbg !211
  %164 = ptrtoint ptr %163 to i64, !dbg !212
  %165 = call i128 @cache_request(i64 %164), !dbg !213
  %166 = call ptr @cache_access(i128 %165), !dbg !214
  %167 = load i64, ptr %166, align 8, !dbg !215
  %168 = add i64 %162, %167, !dbg !216
  %169 = icmp sgt i64 %168, %138, !dbg !217
  br i1 %169, label %170, label %173, !dbg !218

170:                                              ; preds = %151
  %171 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 9, !dbg !219
  %172 = load ptr, ptr %171, align 8, !dbg !220
  br label %203, !dbg !221

173:                                              ; preds = %151
  %174 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 0, !dbg !222
  %175 = load i64, ptr %174, align 8, !dbg !223
  %176 = sub i64 30, %175, !dbg !224
  %177 = getelementptr %_Lowered_node, ptr %128, i32 0, i32 0, !dbg !225
  %178 = load i64, ptr %177, align 8, !dbg !226
  %179 = add i64 %176, %178, !dbg !227
  %180 = icmp slt i64 %179, 0, !dbg !228
  br i1 %180, label %181, label %197, !dbg !229

181:                                              ; preds = %173
  %182 = load i64, ptr %83, align 8, !dbg !230
  %183 = icmp slt i64 %153, %182, !dbg !231
  br i1 %183, label %184, label %186, !dbg !232

184:                                              ; preds = %181
  call void @insert_new_arc(ptr %52, i64 %153, ptr %159, ptr %128, i64 30, i64 %179), !dbg !233
  %185 = add i64 %153, 1, !dbg !234
  br label %194, !dbg !235

186:                                              ; preds = %181
  %187 = ptrtoint ptr %84 to i64, !dbg !236
  %188 = call i128 @cache_request(i64 %187), !dbg !237
  %189 = call ptr @cache_access(i128 %188), !dbg !238
  %190 = load i64, ptr %189, align 8, !dbg !239
  %191 = icmp sgt i64 %190, %179, !dbg !240
  br i1 %191, label %192, label %193, !dbg !241

192:                                              ; preds = %186
  call void @replace_weaker_arc(ptr %0, ptr %52, ptr %159, ptr %128, i64 30, i64 %179), !dbg !242
  br label %193, !dbg !243

193:                                              ; preds = %192, %186
  br label %194, !dbg !244

194:                                              ; preds = %184, %193
  %195 = phi i64 [ %153, %193 ], [ %185, %184 ]
  br label %196, !dbg !245

196:                                              ; preds = %194
  br label %198, !dbg !246

197:                                              ; preds = %173
  br label %198, !dbg !247

198:                                              ; preds = %196, %197
  %199 = phi i64 [ %153, %197 ], [ %195, %196 ]
  br label %200, !dbg !248

200:                                              ; preds = %198
  %201 = getelementptr %_Lowered_node, ptr %159, i32 0, i32 9, !dbg !249
  %202 = load ptr, ptr %201, align 8, !dbg !250
  br label %203, !dbg !251

203:                                              ; preds = %170, %200
  %204 = phi i64 [ %179, %200 ], [ %152, %170 ]
  %205 = phi i64 [ %199, %200 ], [ %153, %170 ]
  %206 = phi ptr [ %202, %200 ], [ %172, %170 ]
  br label %207, !dbg !252

207:                                              ; preds = %203
  br label %146, !dbg !253

208:                                              ; preds = %146
  br label %209, !dbg !254

209:                                              ; preds = %122, %208
  %210 = phi i64 [ %147, %208 ], [ %87, %122 ]
  %211 = phi i64 [ %148, %208 ], [ %88, %122 ]
  br label %212, !dbg !255

212:                                              ; preds = %209
  %213 = getelementptr %_Lowered_rarc, ptr %90, i32 3, !dbg !256
  %214 = add i64 %86, 1, !dbg !257
  br label %85, !dbg !258

215:                                              ; preds = %85
  %216 = icmp ne i64 %88, 0, !dbg !259
  br i1 %216, label %217, label %292, !dbg !260

217:                                              ; preds = %215
  %218 = load ptr, ptr %51, align 8, !dbg !261
  %219 = getelementptr %_Lowered_rarc, ptr %218, i64 %88, !dbg !262
  store ptr %219, ptr %51, align 8, !dbg !263
  %220 = load ptr, ptr %51, align 8, !dbg !264
  br i1 %48, label %221, label %237, !dbg !265

221:                                              ; preds = %217
  br label %222, !dbg !266

222:                                              ; preds = %225, %221
  %223 = phi ptr [ %235, %225 ], [ %218, %221 ]
  %224 = icmp ne ptr %223, %220, !dbg !267
  br i1 %224, label %225, label %236, !dbg !268

225:                                              ; preds = %222
  %226 = phi ptr [ %223, %222 ]
  %227 = getelementptr %_Lowered_rarc, ptr %226, i32 0, i32 6, !dbg !269
  %228 = ptrtoint ptr %227 to i64, !dbg !270
  %229 = call i128 @cache_request(i64 %228), !dbg !271
  %230 = call ptr @cache_access_mut(i128 %229), !dbg !272
  store i64 0, ptr %230, align 8, !dbg !273
  %231 = getelementptr %_Lowered_rarc, ptr %226, i32 0, i32 3, !dbg !274
  %232 = ptrtoint ptr %231 to i64, !dbg !275
  %233 = call i128 @cache_request(i64 %232), !dbg !276
  %234 = call ptr @cache_access_mut(i128 %233), !dbg !277
  store i32 1, ptr %234, align 4, !dbg !278
  %235 = getelementptr %_Lowered_rarc, ptr %226, i32 1, !dbg !279
  br label %222, !dbg !280

236:                                              ; preds = %222
  br label %283, !dbg !281

237:                                              ; preds = %217
  br label %238, !dbg !282

238:                                              ; preds = %241, %237
  %239 = phi ptr [ %281, %241 ], [ %218, %237 ]
  %240 = icmp ne ptr %239, %220, !dbg !283
  br i1 %240, label %241, label %282, !dbg !284

241:                                              ; preds = %238
  %242 = phi ptr [ %239, %238 ]
  %243 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 6, !dbg !285
  %244 = ptrtoint ptr %243 to i64, !dbg !286
  %245 = call i128 @cache_request(i64 %244), !dbg !287
  %246 = call ptr @cache_access_mut(i128 %245), !dbg !288
  store i64 0, ptr %246, align 8, !dbg !289
  %247 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 3, !dbg !290
  %248 = ptrtoint ptr %247 to i64, !dbg !291
  %249 = call i128 @cache_request(i64 %248), !dbg !292
  %250 = call ptr @cache_access_mut(i128 %249), !dbg !293
  store i32 1, ptr %250, align 4, !dbg !294
  %251 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 4, !dbg !295
  %252 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 1, !dbg !296
  %253 = ptrtoint ptr %252 to i64, !dbg !297
  %254 = call i128 @cache_request(i64 %253), !dbg !298
  %255 = call ptr @cache_access(i128 %254), !dbg !299
  %256 = load ptr, ptr %255, align 8, !dbg !300
  %257 = getelementptr %_Lowered_node, ptr %256, i32 0, i32 7, !dbg !301
  %258 = load ptr, ptr %257, align 8, !dbg !302
  %259 = ptrtoint ptr %251 to i64, !dbg !303
  %260 = call i128 @cache_request(i64 %259), !dbg !304
  %261 = call ptr @cache_access_mut(i128 %260), !dbg !305
  store ptr %258, ptr %261, align 8, !dbg !306
  %262 = call i128 @cache_request(i64 %253), !dbg !307
  %263 = call ptr @cache_access(i128 %262), !dbg !308
  %264 = load ptr, ptr %263, align 8, !dbg !309
  %265 = getelementptr %_Lowered_node, ptr %264, i32 0, i32 7, !dbg !310
  store ptr %242, ptr %265, align 8, !dbg !311
  %266 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 5, !dbg !312
  %267 = getelementptr %_Lowered_rarc, ptr %242, i32 0, i32 2, !dbg !313
  %268 = ptrtoint ptr %267 to i64, !dbg !314
  %269 = call i128 @cache_request(i64 %268), !dbg !315
  %270 = call ptr @cache_access(i128 %269), !dbg !316
  %271 = load ptr, ptr %270, align 8, !dbg !317
  %272 = getelementptr %_Lowered_node, ptr %271, i32 0, i32 8, !dbg !318
  %273 = load ptr, ptr %272, align 8, !dbg !319
  %274 = ptrtoint ptr %266 to i64, !dbg !320
  %275 = call i128 @cache_request(i64 %274), !dbg !321
  %276 = call ptr @cache_access_mut(i128 %275), !dbg !322
  store ptr %273, ptr %276, align 8, !dbg !323
  %277 = call i128 @cache_request(i64 %268), !dbg !324
  %278 = call ptr @cache_access(i128 %277), !dbg !325
  %279 = load ptr, ptr %278, align 8, !dbg !326
  %280 = getelementptr %_Lowered_node, ptr %279, i32 0, i32 8, !dbg !327
  store ptr %242, ptr %280, align 8, !dbg !328
  %281 = getelementptr %_Lowered_rarc, ptr %242, i32 1, !dbg !329
  br label %238, !dbg !330

282:                                              ; preds = %238
  br label %283, !dbg !331

283:                                              ; preds = %236, %282
  %284 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !332
  %285 = load i64, ptr %284, align 8, !dbg !333
  %286 = add i64 %285, %88, !dbg !334
  store i64 %286, ptr %284, align 8, !dbg !335
  %287 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !336
  %288 = load i64, ptr %287, align 8, !dbg !337
  %289 = add i64 %288, %88, !dbg !338
  store i64 %289, ptr %287, align 8, !dbg !339
  %290 = load i64, ptr %83, align 8, !dbg !340
  %291 = sub i64 %290, %88, !dbg !341
  store i64 %291, ptr %83, align 8, !dbg !342
  br label %292, !dbg !343

292:                                              ; preds = %283, %215
  br label %294, !dbg !344

293:                                              ; preds = %49
  br label %294, !dbg !345

294:                                              ; preds = %292, %293
  %295 = phi i64 [ %47, %293 ], [ %88, %292 ]
  br label %296, !dbg !346

296:                                              ; preds = %294
  ret i64 %295, !dbg !347
}

declare void @refresh_neighbour_lists(ptr)

declare void @insert_new_arc(ptr, i64, ptr, ptr, i64, i64)

define i64 @suspend_impl(ptr %0, i64 %1, i64 %2) !dbg !348 {
  %4 = icmp ne i64 %2, 0, !dbg !349
  br i1 %4, label %5, label %8, !dbg !351

5:                                                ; preds = %3
  %6 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !352
  %7 = load i64, ptr %6, align 8, !dbg !353
  br label %105, !dbg !354

8:                                                ; preds = %3
  %9 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !355
  %10 = load ptr, ptr %9, align 8, !dbg !356
  %11 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 23, !dbg !357
  %12 = load ptr, ptr %11, align 8, !dbg !358
  %13 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !359
  %14 = load i64, ptr %13, align 8, !dbg !360
  %15 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !361
  %16 = load i64, ptr %15, align 8, !dbg !362
  %17 = sub i64 %14, %16, !dbg !363
  %18 = getelementptr %_Lowered_rarc, ptr %12, i64 %17, !dbg !364
  br label %19, !dbg !365

19:                                               ; preds = %102, %8
  %20 = phi ptr [ %103, %102 ], [ %18, %8 ]
  %21 = phi ptr [ %100, %102 ], [ %18, %8 ]
  %22 = phi i64 [ %101, %102 ], [ 0, %8 ]
  %23 = icmp ult ptr %20, %10, !dbg !366
  br i1 %23, label %24, label %104, !dbg !367

24:                                               ; preds = %19
  %25 = phi i64 [ %22, %19 ]
  %26 = phi ptr [ %20, %19 ]
  %27 = phi ptr [ %21, %19 ]
  %28 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 3, !dbg !368
  %29 = ptrtoint ptr %28 to i64, !dbg !369
  %30 = call i128 @cache_request(i64 %29), !dbg !370
  %31 = call ptr @cache_access(i128 %30), !dbg !371
  %32 = load i32, ptr %31, align 4, !dbg !372
  %33 = icmp eq i32 %32, 1, !dbg !373
  br i1 %33, label %34, label %56, !dbg !374

34:                                               ; preds = %24
  %35 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 0, !dbg !375
  %36 = ptrtoint ptr %35 to i64, !dbg !376
  %37 = call i128 @cache_request(i64 %36), !dbg !377
  %38 = call ptr @cache_access(i128 %37), !dbg !378
  %39 = load i64, ptr %38, align 8, !dbg !379
  %40 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !380
  %41 = ptrtoint ptr %40 to i64, !dbg !381
  %42 = call i128 @cache_request(i64 %41), !dbg !382
  %43 = call ptr @cache_access(i128 %42), !dbg !383
  %44 = load ptr, ptr %43, align 8, !dbg !384
  %45 = getelementptr %_Lowered_node, ptr %44, i32 0, i32 0, !dbg !385
  %46 = load i64, ptr %45, align 8, !dbg !386
  %47 = sub i64 %39, %46, !dbg !387
  %48 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !388
  %49 = ptrtoint ptr %48 to i64, !dbg !389
  %50 = call i128 @cache_request(i64 %49), !dbg !390
  %51 = call ptr @cache_access(i128 %50), !dbg !391
  %52 = load ptr, ptr %51, align 8, !dbg !392
  %53 = getelementptr %_Lowered_node, ptr %52, i32 0, i32 0, !dbg !393
  %54 = load i64, ptr %53, align 8, !dbg !394
  %55 = add i64 %47, %54, !dbg !395
  br label %84, !dbg !396

56:                                               ; preds = %24
  %57 = call i128 @cache_request(i64 %29), !dbg !397
  %58 = call ptr @cache_access(i128 %57), !dbg !398
  %59 = load i32, ptr %58, align 4, !dbg !399
  %60 = icmp eq i32 %59, 0, !dbg !400
  br i1 %60, label %61, label %83, !dbg !401

61:                                               ; preds = %56
  %62 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 1, !dbg !402
  %63 = ptrtoint ptr %62 to i64, !dbg !403
  %64 = call i128 @cache_request(i64 %63), !dbg !404
  %65 = call ptr @cache_access(i128 %64), !dbg !405
  %66 = load ptr, ptr %65, align 8, !dbg !406
  %67 = getelementptr %_Lowered_node, ptr %66, i32 0, i32 6, !dbg !407
  %68 = load ptr, ptr %67, align 8, !dbg !408
  %69 = icmp eq ptr %68, %26, !dbg !409
  br i1 %69, label %70, label %75, !dbg !410

70:                                               ; preds = %61
  %71 = call i128 @cache_request(i64 %63), !dbg !411
  %72 = call ptr @cache_access(i128 %71), !dbg !412
  %73 = load ptr, ptr %72, align 8, !dbg !413
  %74 = getelementptr %_Lowered_node, ptr %73, i32 0, i32 6, !dbg !414
  store ptr %27, ptr %74, align 8, !dbg !415
  br label %82, !dbg !416

75:                                               ; preds = %61
  %76 = getelementptr %_Lowered_rarc, ptr %26, i32 0, i32 2, !dbg !417
  %77 = ptrtoint ptr %76 to i64, !dbg !418
  %78 = call i128 @cache_request(i64 %77), !dbg !419
  %79 = call ptr @cache_access(i128 %78), !dbg !420
  %80 = load ptr, ptr %79, align 8, !dbg !421
  %81 = getelementptr %_Lowered_node, ptr %80, i32 0, i32 6, !dbg !422
  store ptr %27, ptr %81, align 8, !dbg !423
  br label %82, !dbg !424

82:                                               ; preds = %70, %75
  br label %83, !dbg !425

83:                                               ; preds = %82, %56
  br label %84, !dbg !426

84:                                               ; preds = %34, %83
  %85 = phi i64 [ -2, %83 ], [ %55, %34 ]
  br label %86, !dbg !427

86:                                               ; preds = %84
  %87 = icmp sgt i64 %85, %1, !dbg !428
  br i1 %87, label %88, label %90, !dbg !429

88:                                               ; preds = %86
  %89 = add i64 %25, 1, !dbg !430
  br label %99, !dbg !431

90:                                               ; preds = %86
  %91 = ptrtoint ptr %26 to i64, !dbg !432
  %92 = call i128 @cache_request(i64 %91), !dbg !433
  %93 = call ptr @cache_access(i128 %92), !dbg !434
  %94 = load %_Lowered_rarc, ptr %93, align 8, !dbg !435
  %95 = ptrtoint ptr %27 to i64, !dbg !436
  %96 = call i128 @cache_request(i64 %95), !dbg !437
  %97 = call ptr @cache_access_mut(i128 %96), !dbg !438
  store %_Lowered_rarc %94, ptr %97, align 8, !dbg !439
  %98 = getelementptr %_Lowered_rarc, ptr %27, i32 1, !dbg !440
  br label %99, !dbg !441

99:                                               ; preds = %88, %90
  %100 = phi ptr [ %98, %90 ], [ %27, %88 ]
  %101 = phi i64 [ %25, %90 ], [ %89, %88 ]
  br label %102, !dbg !442

102:                                              ; preds = %99
  %103 = getelementptr %_Lowered_rarc, ptr %26, i32 1, !dbg !443
  br label %19, !dbg !444

104:                                              ; preds = %19
  br label %105, !dbg !445

105:                                              ; preds = %5, %104
  %106 = phi i64 [ %22, %104 ], [ %7, %5 ]
  br label %107, !dbg !446

107:                                              ; preds = %105
  %108 = icmp ne i64 %106, 0, !dbg !447
  br i1 %108, label %109, label %123, !dbg !448

109:                                              ; preds = %107
  %110 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 5, !dbg !449
  %111 = load i64, ptr %110, align 8, !dbg !450
  %112 = sub i64 %111, %106, !dbg !451
  store i64 %112, ptr %110, align 8, !dbg !452
  %113 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 7, !dbg !453
  %114 = load i64, ptr %113, align 8, !dbg !454
  %115 = sub i64 %114, %106, !dbg !455
  store i64 %115, ptr %113, align 8, !dbg !456
  %116 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 24, !dbg !457
  %117 = load ptr, ptr %116, align 8, !dbg !458
  %118 = sub i64 0, %106, !dbg !459
  %119 = getelementptr %_Lowered_rarc, ptr %117, i64 %118, !dbg !460
  store ptr %119, ptr %116, align 8, !dbg !461
  %120 = getelementptr %_Lowered_network, ptr %0, i32 0, i32 8, !dbg !462
  %121 = load i64, ptr %120, align 8, !dbg !463
  %122 = add i64 %121, %106, !dbg !464
  store i64 %122, ptr %120, align 8, !dbg !465
  call void @refresh_neighbour_lists(ptr %0), !dbg !466
  br label %123, !dbg !467

123:                                              ; preds = %109, %107
  ret i64 %106, !dbg !468
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "resize_prob", linkageName: "resize_prob", scope: null, file: !4, line: 12, type: !5, scopeLine: 12, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/implicit.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf.origin/src/obj/anno")
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
!72 = distinct !DISubprogram(name: "price_out_impl", linkageName: "price_out_impl", scope: null, file: !4, line: 108, type: !5, scopeLine: 108, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!73 = !DILocation(line: 122, column: 11, scope: !74)
!74 = !DILexicalBlockFile(scope: !72, file: !4, discriminator: 0)
!75 = !DILocation(line: 123, column: 11, scope: !74)
!76 = !DILocation(line: 124, column: 11, scope: !74)
!77 = !DILocation(line: 125, column: 11, scope: !74)
!78 = !DILocation(line: 126, column: 11, scope: !74)
!79 = !DILocation(line: 127, column: 11, scope: !74)
!80 = !DILocation(line: 128, column: 5, scope: !74)
!81 = !DILocation(line: 130, column: 11, scope: !74)
!82 = !DILocation(line: 131, column: 11, scope: !74)
!83 = !DILocation(line: 132, column: 11, scope: !74)
!84 = !DILocation(line: 133, column: 11, scope: !74)
!85 = !DILocation(line: 134, column: 11, scope: !74)
!86 = !DILocation(line: 135, column: 11, scope: !74)
!87 = !DILocation(line: 136, column: 11, scope: !74)
!88 = !DILocation(line: 137, column: 11, scope: !74)
!89 = !DILocation(line: 138, column: 5, scope: !74)
!90 = !DILocation(line: 140, column: 11, scope: !74)
!91 = !DILocation(line: 141, column: 11, scope: !74)
!92 = !DILocation(line: 142, column: 11, scope: !74)
!93 = !DILocation(line: 143, column: 11, scope: !74)
!94 = !DILocation(line: 144, column: 11, scope: !74)
!95 = !DILocation(line: 145, column: 11, scope: !74)
!96 = !DILocation(line: 146, column: 11, scope: !74)
!97 = !DILocation(line: 147, column: 5, scope: !74)
!98 = !DILocation(line: 149, column: 11, scope: !74)
!99 = !DILocation(line: 150, column: 11, scope: !74)
!100 = !DILocation(line: 151, column: 11, scope: !74)
!101 = !DILocation(line: 152, column: 11, scope: !74)
!102 = !DILocation(line: 153, column: 5, scope: !74)
!103 = !DILocation(line: 155, column: 5, scope: !74)
!104 = !DILocation(line: 157, column: 5, scope: !74)
!105 = !DILocation(line: 158, column: 5, scope: !74)
!106 = !DILocation(line: 160, column: 5, scope: !74)
!107 = !DILocation(line: 162, column: 5, scope: !74)
!108 = !DILocation(line: 164, column: 5, scope: !74)
!109 = !DILocation(line: 166, column: 5, scope: !74)
!110 = !DILocation(line: 168, column: 5, scope: !74)
!111 = !DILocation(line: 170, column: 5, scope: !74)
!112 = !DILocation(line: 172, column: 5, scope: !74)
!113 = !DILocation(line: 174, column: 5, scope: !74)
!114 = !DILocation(line: 176, column: 5, scope: !74)
!115 = !DILocation(line: 178, column: 5, scope: !74)
!116 = !DILocation(line: 180, column: 11, scope: !74)
!117 = !DILocation(line: 181, column: 11, scope: !74)
!118 = !DILocation(line: 182, column: 11, scope: !74)
!119 = !DILocation(line: 183, column: 11, scope: !74)
!120 = !DILocation(line: 184, column: 11, scope: !74)
!121 = !DILocation(line: 185, column: 5, scope: !74)
!122 = !DILocation(line: 187, column: 11, scope: !74)
!123 = !DILocation(line: 188, column: 5, scope: !74)
!124 = !DILocation(line: 190, column: 11, scope: !74)
!125 = !DILocation(line: 191, column: 11, scope: !74)
!126 = !DILocation(line: 192, column: 11, scope: !74)
!127 = !DILocation(line: 193, column: 11, scope: !74)
!128 = !DILocation(line: 194, column: 11, scope: !74)
!129 = !DILocation(line: 196, column: 11, scope: !74)
!130 = !DILocation(line: 197, column: 11, scope: !74)
!131 = !DILocation(line: 198, column: 5, scope: !74)
!132 = !DILocation(line: 200, column: 11, scope: !74)
!133 = !DILocation(line: 201, column: 11, scope: !74)
!134 = !DILocation(line: 202, column: 5, scope: !74)
!135 = !DILocation(line: 204, column: 5, scope: !74)
!136 = !DILocation(line: 206, column: 5, scope: !74)
!137 = !DILocation(line: 208, column: 5, scope: !74)
!138 = !DILocation(line: 210, column: 5, scope: !74)
!139 = !DILocation(line: 212, column: 5, scope: !74)
!140 = !DILocation(line: 214, column: 5, scope: !74)
!141 = !DILocation(line: 219, column: 11, scope: !74)
!142 = !DILocation(line: 220, column: 11, scope: !74)
!143 = !DILocation(line: 221, column: 5, scope: !74)
!144 = !DILocation(line: 223, column: 11, scope: !74)
!145 = !DILocation(line: 224, column: 5, scope: !74)
!146 = !DILocation(line: 226, column: 11, scope: !74)
!147 = !DILocation(line: 227, column: 11, scope: !74)
!148 = !DILocation(line: 228, column: 11, scope: !74)
!149 = !DILocation(line: 229, column: 11, scope: !74)
!150 = !DILocation(line: 230, column: 11, scope: !74)
!151 = !DILocation(line: 232, column: 11, scope: !74)
!152 = !DILocation(line: 233, column: 11, scope: !74)
!153 = !DILocation(line: 234, column: 11, scope: !74)
!154 = !DILocation(line: 235, column: 5, scope: !74)
!155 = !DILocation(line: 237, column: 11, scope: !74)
!156 = !DILocation(line: 238, column: 11, scope: !74)
!157 = !DILocation(line: 239, column: 11, scope: !74)
!158 = !DILocation(line: 240, column: 11, scope: !74)
!159 = !DILocation(line: 242, column: 11, scope: !74)
!160 = !DILocation(line: 243, column: 11, scope: !74)
!161 = !DILocation(line: 244, column: 11, scope: !74)
!162 = !DILocation(line: 245, column: 11, scope: !74)
!163 = !DILocation(line: 246, column: 11, scope: !74)
!164 = !DILocation(line: 247, column: 11, scope: !74)
!165 = !DILocation(line: 248, column: 12, scope: !74)
!166 = !DILocation(line: 250, column: 12, scope: !74)
!167 = !DILocation(line: 251, column: 12, scope: !74)
!168 = !DILocation(line: 252, column: 5, scope: !74)
!169 = !DILocation(line: 253, column: 5, scope: !74)
!170 = !DILocation(line: 255, column: 12, scope: !74)
!171 = !DILocation(line: 256, column: 12, scope: !74)
!172 = !DILocation(line: 257, column: 12, scope: !74)
!173 = !DILocation(line: 258, column: 12, scope: !74)
!174 = !DILocation(line: 260, column: 12, scope: !74)
!175 = !DILocation(line: 261, column: 12, scope: !74)
!176 = !DILocation(line: 262, column: 5, scope: !74)
!177 = !DILocation(line: 264, column: 5, scope: !74)
!178 = !DILocation(line: 266, column: 12, scope: !74)
!179 = !DILocation(line: 267, column: 12, scope: !74)
!180 = !DILocation(line: 268, column: 12, scope: !74)
!181 = !DILocation(line: 269, column: 12, scope: !74)
!182 = !DILocation(line: 271, column: 12, scope: !74)
!183 = !DILocation(line: 272, column: 12, scope: !74)
!184 = !DILocation(line: 273, column: 12, scope: !74)
!185 = !DILocation(line: 274, column: 12, scope: !74)
!186 = !DILocation(line: 275, column: 12, scope: !74)
!187 = !DILocation(line: 276, column: 12, scope: !74)
!188 = !DILocation(line: 277, column: 12, scope: !74)
!189 = !DILocation(line: 278, column: 12, scope: !74)
!190 = !DILocation(line: 280, column: 12, scope: !74)
!191 = !DILocation(line: 281, column: 12, scope: !74)
!192 = !DILocation(line: 282, column: 12, scope: !74)
!193 = !DILocation(line: 283, column: 12, scope: !74)
!194 = !DILocation(line: 284, column: 12, scope: !74)
!195 = !DILocation(line: 285, column: 12, scope: !74)
!196 = !DILocation(line: 286, column: 12, scope: !74)
!197 = !DILocation(line: 288, column: 12, scope: !74)
!198 = !DILocation(line: 289, column: 12, scope: !74)
!199 = !DILocation(line: 290, column: 12, scope: !74)
!200 = !DILocation(line: 291, column: 5, scope: !74)
!201 = !DILocation(line: 293, column: 12, scope: !74)
!202 = !DILocation(line: 294, column: 5, scope: !74)
!203 = !DILocation(line: 296, column: 12, scope: !74)
!204 = !DILocation(line: 297, column: 12, scope: !74)
!205 = !DILocation(line: 298, column: 12, scope: !74)
!206 = !DILocation(line: 299, column: 12, scope: !74)
!207 = !DILocation(line: 301, column: 12, scope: !74)
!208 = !DILocation(line: 302, column: 12, scope: !74)
!209 = !DILocation(line: 303, column: 12, scope: !74)
!210 = !DILocation(line: 304, column: 12, scope: !74)
!211 = !DILocation(line: 305, column: 12, scope: !74)
!212 = !DILocation(line: 306, column: 12, scope: !74)
!213 = !DILocation(line: 307, column: 12, scope: !74)
!214 = !DILocation(line: 308, column: 12, scope: !74)
!215 = !DILocation(line: 310, column: 12, scope: !74)
!216 = !DILocation(line: 311, column: 12, scope: !74)
!217 = !DILocation(line: 312, column: 12, scope: !74)
!218 = !DILocation(line: 313, column: 5, scope: !74)
!219 = !DILocation(line: 315, column: 12, scope: !74)
!220 = !DILocation(line: 316, column: 12, scope: !74)
!221 = !DILocation(line: 317, column: 5, scope: !74)
!222 = !DILocation(line: 319, column: 12, scope: !74)
!223 = !DILocation(line: 320, column: 12, scope: !74)
!224 = !DILocation(line: 321, column: 12, scope: !74)
!225 = !DILocation(line: 322, column: 12, scope: !74)
!226 = !DILocation(line: 323, column: 12, scope: !74)
!227 = !DILocation(line: 324, column: 12, scope: !74)
!228 = !DILocation(line: 325, column: 12, scope: !74)
!229 = !DILocation(line: 326, column: 5, scope: !74)
!230 = !DILocation(line: 328, column: 12, scope: !74)
!231 = !DILocation(line: 329, column: 12, scope: !74)
!232 = !DILocation(line: 330, column: 5, scope: !74)
!233 = !DILocation(line: 332, column: 5, scope: !74)
!234 = !DILocation(line: 333, column: 12, scope: !74)
!235 = !DILocation(line: 334, column: 5, scope: !74)
!236 = !DILocation(line: 336, column: 12, scope: !74)
!237 = !DILocation(line: 337, column: 12, scope: !74)
!238 = !DILocation(line: 338, column: 12, scope: !74)
!239 = !DILocation(line: 340, column: 12, scope: !74)
!240 = !DILocation(line: 341, column: 12, scope: !74)
!241 = !DILocation(line: 342, column: 5, scope: !74)
!242 = !DILocation(line: 344, column: 5, scope: !74)
!243 = !DILocation(line: 345, column: 5, scope: !74)
!244 = !DILocation(line: 347, column: 5, scope: !74)
!245 = !DILocation(line: 349, column: 5, scope: !74)
!246 = !DILocation(line: 351, column: 5, scope: !74)
!247 = !DILocation(line: 353, column: 5, scope: !74)
!248 = !DILocation(line: 355, column: 5, scope: !74)
!249 = !DILocation(line: 357, column: 12, scope: !74)
!250 = !DILocation(line: 358, column: 12, scope: !74)
!251 = !DILocation(line: 359, column: 5, scope: !74)
!252 = !DILocation(line: 361, column: 5, scope: !74)
!253 = !DILocation(line: 363, column: 5, scope: !74)
!254 = !DILocation(line: 365, column: 5, scope: !74)
!255 = !DILocation(line: 367, column: 5, scope: !74)
!256 = !DILocation(line: 369, column: 12, scope: !74)
!257 = !DILocation(line: 370, column: 12, scope: !74)
!258 = !DILocation(line: 371, column: 5, scope: !74)
!259 = !DILocation(line: 373, column: 12, scope: !74)
!260 = !DILocation(line: 374, column: 5, scope: !74)
!261 = !DILocation(line: 376, column: 12, scope: !74)
!262 = !DILocation(line: 377, column: 12, scope: !74)
!263 = !DILocation(line: 378, column: 5, scope: !74)
!264 = !DILocation(line: 379, column: 12, scope: !74)
!265 = !DILocation(line: 380, column: 5, scope: !74)
!266 = !DILocation(line: 382, column: 5, scope: !74)
!267 = !DILocation(line: 384, column: 12, scope: !74)
!268 = !DILocation(line: 385, column: 5, scope: !74)
!269 = !DILocation(line: 387, column: 12, scope: !74)
!270 = !DILocation(line: 388, column: 12, scope: !74)
!271 = !DILocation(line: 389, column: 12, scope: !74)
!272 = !DILocation(line: 390, column: 12, scope: !74)
!273 = !DILocation(line: 392, column: 5, scope: !74)
!274 = !DILocation(line: 393, column: 12, scope: !74)
!275 = !DILocation(line: 394, column: 12, scope: !74)
!276 = !DILocation(line: 395, column: 12, scope: !74)
!277 = !DILocation(line: 396, column: 12, scope: !74)
!278 = !DILocation(line: 398, column: 5, scope: !74)
!279 = !DILocation(line: 399, column: 12, scope: !74)
!280 = !DILocation(line: 400, column: 5, scope: !74)
!281 = !DILocation(line: 402, column: 5, scope: !74)
!282 = !DILocation(line: 404, column: 5, scope: !74)
!283 = !DILocation(line: 406, column: 12, scope: !74)
!284 = !DILocation(line: 407, column: 5, scope: !74)
!285 = !DILocation(line: 409, column: 12, scope: !74)
!286 = !DILocation(line: 410, column: 12, scope: !74)
!287 = !DILocation(line: 411, column: 12, scope: !74)
!288 = !DILocation(line: 412, column: 12, scope: !74)
!289 = !DILocation(line: 414, column: 5, scope: !74)
!290 = !DILocation(line: 415, column: 12, scope: !74)
!291 = !DILocation(line: 416, column: 12, scope: !74)
!292 = !DILocation(line: 417, column: 12, scope: !74)
!293 = !DILocation(line: 418, column: 12, scope: !74)
!294 = !DILocation(line: 420, column: 5, scope: !74)
!295 = !DILocation(line: 421, column: 12, scope: !74)
!296 = !DILocation(line: 422, column: 12, scope: !74)
!297 = !DILocation(line: 423, column: 12, scope: !74)
!298 = !DILocation(line: 424, column: 12, scope: !74)
!299 = !DILocation(line: 425, column: 12, scope: !74)
!300 = !DILocation(line: 427, column: 12, scope: !74)
!301 = !DILocation(line: 428, column: 12, scope: !74)
!302 = !DILocation(line: 429, column: 12, scope: !74)
!303 = !DILocation(line: 430, column: 12, scope: !74)
!304 = !DILocation(line: 431, column: 12, scope: !74)
!305 = !DILocation(line: 432, column: 12, scope: !74)
!306 = !DILocation(line: 434, column: 5, scope: !74)
!307 = !DILocation(line: 435, column: 12, scope: !74)
!308 = !DILocation(line: 436, column: 12, scope: !74)
!309 = !DILocation(line: 438, column: 12, scope: !74)
!310 = !DILocation(line: 439, column: 12, scope: !74)
!311 = !DILocation(line: 440, column: 5, scope: !74)
!312 = !DILocation(line: 441, column: 12, scope: !74)
!313 = !DILocation(line: 442, column: 12, scope: !74)
!314 = !DILocation(line: 443, column: 12, scope: !74)
!315 = !DILocation(line: 444, column: 12, scope: !74)
!316 = !DILocation(line: 445, column: 12, scope: !74)
!317 = !DILocation(line: 447, column: 12, scope: !74)
!318 = !DILocation(line: 448, column: 12, scope: !74)
!319 = !DILocation(line: 449, column: 12, scope: !74)
!320 = !DILocation(line: 450, column: 12, scope: !74)
!321 = !DILocation(line: 451, column: 12, scope: !74)
!322 = !DILocation(line: 452, column: 12, scope: !74)
!323 = !DILocation(line: 454, column: 5, scope: !74)
!324 = !DILocation(line: 455, column: 12, scope: !74)
!325 = !DILocation(line: 456, column: 12, scope: !74)
!326 = !DILocation(line: 458, column: 12, scope: !74)
!327 = !DILocation(line: 459, column: 12, scope: !74)
!328 = !DILocation(line: 460, column: 5, scope: !74)
!329 = !DILocation(line: 461, column: 12, scope: !74)
!330 = !DILocation(line: 462, column: 5, scope: !74)
!331 = !DILocation(line: 464, column: 5, scope: !74)
!332 = !DILocation(line: 466, column: 12, scope: !74)
!333 = !DILocation(line: 467, column: 12, scope: !74)
!334 = !DILocation(line: 468, column: 12, scope: !74)
!335 = !DILocation(line: 469, column: 5, scope: !74)
!336 = !DILocation(line: 470, column: 12, scope: !74)
!337 = !DILocation(line: 471, column: 12, scope: !74)
!338 = !DILocation(line: 472, column: 12, scope: !74)
!339 = !DILocation(line: 473, column: 5, scope: !74)
!340 = !DILocation(line: 474, column: 12, scope: !74)
!341 = !DILocation(line: 475, column: 12, scope: !74)
!342 = !DILocation(line: 476, column: 5, scope: !74)
!343 = !DILocation(line: 477, column: 5, scope: !74)
!344 = !DILocation(line: 479, column: 5, scope: !74)
!345 = !DILocation(line: 481, column: 5, scope: !74)
!346 = !DILocation(line: 483, column: 5, scope: !74)
!347 = !DILocation(line: 485, column: 5, scope: !74)
!348 = distinct !DISubprogram(name: "suspend_impl", linkageName: "suspend_impl", scope: null, file: !4, line: 489, type: !5, scopeLine: 489, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!349 = !DILocation(line: 495, column: 10, scope: !350)
!350 = !DILexicalBlockFile(scope: !348, file: !4, discriminator: 0)
!351 = !DILocation(line: 496, column: 5, scope: !350)
!352 = !DILocation(line: 498, column: 10, scope: !350)
!353 = !DILocation(line: 499, column: 10, scope: !350)
!354 = !DILocation(line: 500, column: 5, scope: !350)
!355 = !DILocation(line: 502, column: 10, scope: !350)
!356 = !DILocation(line: 503, column: 10, scope: !350)
!357 = !DILocation(line: 504, column: 11, scope: !350)
!358 = !DILocation(line: 505, column: 11, scope: !350)
!359 = !DILocation(line: 506, column: 11, scope: !350)
!360 = !DILocation(line: 507, column: 11, scope: !350)
!361 = !DILocation(line: 508, column: 11, scope: !350)
!362 = !DILocation(line: 509, column: 11, scope: !350)
!363 = !DILocation(line: 510, column: 11, scope: !350)
!364 = !DILocation(line: 511, column: 11, scope: !350)
!365 = !DILocation(line: 512, column: 5, scope: !350)
!366 = !DILocation(line: 514, column: 11, scope: !350)
!367 = !DILocation(line: 515, column: 5, scope: !350)
!368 = !DILocation(line: 517, column: 11, scope: !350)
!369 = !DILocation(line: 518, column: 11, scope: !350)
!370 = !DILocation(line: 519, column: 11, scope: !350)
!371 = !DILocation(line: 520, column: 11, scope: !350)
!372 = !DILocation(line: 522, column: 11, scope: !350)
!373 = !DILocation(line: 523, column: 11, scope: !350)
!374 = !DILocation(line: 524, column: 5, scope: !350)
!375 = !DILocation(line: 526, column: 11, scope: !350)
!376 = !DILocation(line: 527, column: 11, scope: !350)
!377 = !DILocation(line: 528, column: 11, scope: !350)
!378 = !DILocation(line: 529, column: 11, scope: !350)
!379 = !DILocation(line: 531, column: 11, scope: !350)
!380 = !DILocation(line: 532, column: 11, scope: !350)
!381 = !DILocation(line: 533, column: 11, scope: !350)
!382 = !DILocation(line: 534, column: 11, scope: !350)
!383 = !DILocation(line: 535, column: 11, scope: !350)
!384 = !DILocation(line: 537, column: 11, scope: !350)
!385 = !DILocation(line: 538, column: 11, scope: !350)
!386 = !DILocation(line: 539, column: 11, scope: !350)
!387 = !DILocation(line: 540, column: 11, scope: !350)
!388 = !DILocation(line: 541, column: 11, scope: !350)
!389 = !DILocation(line: 542, column: 11, scope: !350)
!390 = !DILocation(line: 543, column: 11, scope: !350)
!391 = !DILocation(line: 544, column: 11, scope: !350)
!392 = !DILocation(line: 546, column: 11, scope: !350)
!393 = !DILocation(line: 547, column: 11, scope: !350)
!394 = !DILocation(line: 548, column: 11, scope: !350)
!395 = !DILocation(line: 549, column: 11, scope: !350)
!396 = !DILocation(line: 550, column: 5, scope: !350)
!397 = !DILocation(line: 552, column: 11, scope: !350)
!398 = !DILocation(line: 553, column: 11, scope: !350)
!399 = !DILocation(line: 555, column: 11, scope: !350)
!400 = !DILocation(line: 556, column: 11, scope: !350)
!401 = !DILocation(line: 557, column: 5, scope: !350)
!402 = !DILocation(line: 559, column: 11, scope: !350)
!403 = !DILocation(line: 560, column: 11, scope: !350)
!404 = !DILocation(line: 561, column: 11, scope: !350)
!405 = !DILocation(line: 562, column: 11, scope: !350)
!406 = !DILocation(line: 564, column: 11, scope: !350)
!407 = !DILocation(line: 565, column: 11, scope: !350)
!408 = !DILocation(line: 566, column: 11, scope: !350)
!409 = !DILocation(line: 567, column: 11, scope: !350)
!410 = !DILocation(line: 568, column: 5, scope: !350)
!411 = !DILocation(line: 570, column: 11, scope: !350)
!412 = !DILocation(line: 571, column: 11, scope: !350)
!413 = !DILocation(line: 573, column: 11, scope: !350)
!414 = !DILocation(line: 574, column: 11, scope: !350)
!415 = !DILocation(line: 575, column: 5, scope: !350)
!416 = !DILocation(line: 576, column: 5, scope: !350)
!417 = !DILocation(line: 578, column: 11, scope: !350)
!418 = !DILocation(line: 579, column: 11, scope: !350)
!419 = !DILocation(line: 580, column: 11, scope: !350)
!420 = !DILocation(line: 581, column: 11, scope: !350)
!421 = !DILocation(line: 583, column: 11, scope: !350)
!422 = !DILocation(line: 584, column: 11, scope: !350)
!423 = !DILocation(line: 585, column: 5, scope: !350)
!424 = !DILocation(line: 586, column: 5, scope: !350)
!425 = !DILocation(line: 588, column: 5, scope: !350)
!426 = !DILocation(line: 590, column: 5, scope: !350)
!427 = !DILocation(line: 592, column: 5, scope: !350)
!428 = !DILocation(line: 594, column: 11, scope: !350)
!429 = !DILocation(line: 595, column: 5, scope: !350)
!430 = !DILocation(line: 597, column: 11, scope: !350)
!431 = !DILocation(line: 598, column: 5, scope: !350)
!432 = !DILocation(line: 600, column: 11, scope: !350)
!433 = !DILocation(line: 601, column: 11, scope: !350)
!434 = !DILocation(line: 602, column: 11, scope: !350)
!435 = !DILocation(line: 604, column: 11, scope: !350)
!436 = !DILocation(line: 605, column: 11, scope: !350)
!437 = !DILocation(line: 606, column: 11, scope: !350)
!438 = !DILocation(line: 607, column: 11, scope: !350)
!439 = !DILocation(line: 609, column: 5, scope: !350)
!440 = !DILocation(line: 610, column: 11, scope: !350)
!441 = !DILocation(line: 611, column: 5, scope: !350)
!442 = !DILocation(line: 613, column: 5, scope: !350)
!443 = !DILocation(line: 615, column: 11, scope: !350)
!444 = !DILocation(line: 616, column: 5, scope: !350)
!445 = !DILocation(line: 618, column: 5, scope: !350)
!446 = !DILocation(line: 620, column: 5, scope: !350)
!447 = !DILocation(line: 622, column: 11, scope: !350)
!448 = !DILocation(line: 623, column: 5, scope: !350)
!449 = !DILocation(line: 625, column: 12, scope: !350)
!450 = !DILocation(line: 626, column: 12, scope: !350)
!451 = !DILocation(line: 627, column: 12, scope: !350)
!452 = !DILocation(line: 628, column: 5, scope: !350)
!453 = !DILocation(line: 629, column: 12, scope: !350)
!454 = !DILocation(line: 630, column: 12, scope: !350)
!455 = !DILocation(line: 631, column: 12, scope: !350)
!456 = !DILocation(line: 632, column: 5, scope: !350)
!457 = !DILocation(line: 633, column: 12, scope: !350)
!458 = !DILocation(line: 634, column: 12, scope: !350)
!459 = !DILocation(line: 635, column: 12, scope: !350)
!460 = !DILocation(line: 636, column: 12, scope: !350)
!461 = !DILocation(line: 637, column: 5, scope: !350)
!462 = !DILocation(line: 638, column: 12, scope: !350)
!463 = !DILocation(line: 639, column: 12, scope: !350)
!464 = !DILocation(line: 640, column: 12, scope: !350)
!465 = !DILocation(line: 641, column: 5, scope: !350)
!466 = !DILocation(line: 642, column: 5, scope: !350)
!467 = !DILocation(line: 643, column: 5, scope: !350)
!468 = !DILocation(line: 645, column: 5, scope: !350)
