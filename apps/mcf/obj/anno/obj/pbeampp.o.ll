; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_basket = type { ptr, i64, i64 }
%_Lowered_rarc = type { i64, ptr, ptr, i32, ptr, ptr, i64, i64 }
%_Lowered_node = type { i64, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i32, i32 }

@basket_size = internal global i64 undef
@group_pos = internal global i64 undef
@nr_group = internal global i64 undef
@basket = internal global [351 x %_Lowered_basket] undef
@initialize = internal global i64 1
@perm = internal global [351 x ptr] undef

declare ptr @malloc(i64)

declare void @free(ptr)

declare ptr @cache_access(i128)

declare i128 @cache_request(i64)

define i32 @bea_is_dual_infeasible(ptr %0, i64 %1) !dbg !3 {
  %3 = icmp slt i64 %1, 0, !dbg !7
  br i1 %3, label %4, label %11, !dbg !9

4:                                                ; preds = %2
  %5 = getelementptr %_Lowered_rarc, ptr %0, i32 0, i32 3, !dbg !10
  %6 = ptrtoint ptr %5 to i64, !dbg !11
  %7 = call i128 @cache_request(i64 %6), !dbg !12
  %8 = call ptr @cache_access(i128 %7), !dbg !13
  %9 = load i32, ptr %8, align 4, !dbg !14
  %10 = icmp eq i32 %9, 1, !dbg !15
  br label %12, !dbg !16

11:                                               ; preds = %2
  br label %12, !dbg !17

12:                                               ; preds = %4, %11
  %13 = phi i1 [ false, %11 ], [ %10, %4 ]
  br label %14, !dbg !18

14:                                               ; preds = %12
  br i1 %13, label %15, label %16, !dbg !19

15:                                               ; preds = %14
  br label %29, !dbg !20

16:                                               ; preds = %14
  %17 = icmp sgt i64 %1, 0, !dbg !21
  br i1 %17, label %18, label %25, !dbg !22

18:                                               ; preds = %16
  %19 = getelementptr %_Lowered_rarc, ptr %0, i32 0, i32 3, !dbg !23
  %20 = ptrtoint ptr %19 to i64, !dbg !24
  %21 = call i128 @cache_request(i64 %20), !dbg !25
  %22 = call ptr @cache_access(i128 %21), !dbg !26
  %23 = load i32, ptr %22, align 4, !dbg !27
  %24 = icmp eq i32 %23, 2, !dbg !28
  br label %26, !dbg !29

25:                                               ; preds = %16
  br label %26, !dbg !30

26:                                               ; preds = %18, %25
  %27 = phi i1 [ false, %25 ], [ %24, %18 ]
  br label %28, !dbg !31

28:                                               ; preds = %26
  br label %29, !dbg !32

29:                                               ; preds = %15, %28
  %30 = phi i1 [ %27, %28 ], [ true, %15 ]
  br label %31, !dbg !33

31:                                               ; preds = %29
  %32 = sext i1 %30 to i32, !dbg !34
  ret i32 %32, !dbg !35
}

define void @sort_basket(i64 %0, i64 %1) !dbg !36 {
  %3 = add i64 %0, %1, !dbg !37
  %4 = sdiv i64 %3, 2, !dbg !39
  %5 = getelementptr ptr, ptr @perm, i64 %4, !dbg !40
  %6 = load ptr, ptr %5, align 8, !dbg !41
  %7 = getelementptr %_Lowered_basket, ptr %6, i32 0, i32 2, !dbg !42
  %8 = load i64, ptr %7, align 8, !dbg !43
  br label %9, !dbg !44

9:                                                ; preds = %53, %2
  %10 = phi i64 [ %54, %53 ], [ %1, %2 ]
  %11 = phi i64 [ %55, %53 ], [ %0, %2 ]
  %12 = phi i64 [ %54, %53 ], [ undef, %2 ]
  %13 = phi i64 [ %55, %53 ], [ undef, %2 ]
  br label %14, !dbg !45

14:                                               ; preds = %21, %9
  %15 = phi i64 [ %23, %21 ], [ %11, %9 ]
  %16 = getelementptr ptr, ptr @perm, i64 %15, !dbg !46
  %17 = load ptr, ptr %16, align 8, !dbg !47
  %18 = getelementptr %_Lowered_basket, ptr %17, i32 0, i32 2, !dbg !48
  %19 = load i64, ptr %18, align 8, !dbg !49
  %20 = icmp sgt i64 %19, %8, !dbg !50
  br i1 %20, label %21, label %24, !dbg !51

21:                                               ; preds = %14
  %22 = phi i64 [ %15, %14 ]
  %23 = add i64 %22, 1, !dbg !52
  br label %14, !dbg !53

24:                                               ; preds = %14
  br label %25, !dbg !54

25:                                               ; preds = %32, %24
  %26 = phi i64 [ %34, %32 ], [ %10, %24 ]
  %27 = getelementptr ptr, ptr @perm, i64 %26, !dbg !55
  %28 = load ptr, ptr %27, align 8, !dbg !56
  %29 = getelementptr %_Lowered_basket, ptr %28, i32 0, i32 2, !dbg !57
  %30 = load i64, ptr %29, align 8, !dbg !58
  %31 = icmp sgt i64 %8, %30, !dbg !59
  br i1 %31, label %32, label %35, !dbg !60

32:                                               ; preds = %25
  %33 = phi i64 [ %26, %25 ]
  %34 = add i64 %33, -1, !dbg !61
  br label %25, !dbg !62

35:                                               ; preds = %25
  %36 = icmp slt i64 %15, %26, !dbg !63
  br i1 %36, label %37, label %42, !dbg !64

37:                                               ; preds = %35
  %38 = getelementptr ptr, ptr @perm, i64 %15, !dbg !65
  %39 = load ptr, ptr %38, align 8, !dbg !66
  %40 = getelementptr ptr, ptr @perm, i64 %26, !dbg !67
  %41 = load ptr, ptr %40, align 8, !dbg !68
  store ptr %41, ptr %38, align 8, !dbg !69
  store ptr %39, ptr %40, align 8, !dbg !70
  br label %42, !dbg !71

42:                                               ; preds = %37, %35
  %43 = icmp sle i64 %15, %26, !dbg !72
  br i1 %43, label %44, label %47, !dbg !73

44:                                               ; preds = %42
  %45 = add i64 %15, 1, !dbg !74
  %46 = add i64 %26, -1, !dbg !75
  br label %48, !dbg !76

47:                                               ; preds = %42
  br label %48, !dbg !77

48:                                               ; preds = %44, %47
  %49 = phi i64 [ %26, %47 ], [ %46, %44 ]
  %50 = phi i64 [ %15, %47 ], [ %45, %44 ]
  br label %51, !dbg !78

51:                                               ; preds = %48
  %52 = icmp sle i64 %50, %49, !dbg !79
  br i1 %52, label %53, label %56, !dbg !80

53:                                               ; preds = %51
  %54 = phi i64 [ %49, %51 ]
  %55 = phi i64 [ %50, %51 ]
  br label %9, !dbg !81

56:                                               ; preds = %51
  %57 = icmp slt i64 %0, %49, !dbg !82
  br i1 %57, label %58, label %59, !dbg !83

58:                                               ; preds = %56
  call void @sort_basket(i64 %0, i64 %49), !dbg !84
  br label %59, !dbg !85

59:                                               ; preds = %58, %56
  %60 = icmp slt i64 %50, %1, !dbg !86
  %61 = icmp sle i64 %50, 50, !dbg !87
  %62 = and i1 %60, %61, !dbg !88
  br i1 %62, label %63, label %64, !dbg !89

63:                                               ; preds = %59
  call void @sort_basket(i64 %50, i64 %1), !dbg !90
  br label %64, !dbg !91

64:                                               ; preds = %63, %59
  ret void, !dbg !92
}

define ptr @primal_bea_mpp(i64 %0, ptr %1, ptr %2, ptr %3) !dbg !93 {
  %5 = load i64, ptr @initialize, align 8, !dbg !94
  %6 = icmp ne i64 %5, 0, !dbg !96
  br i1 %6, label %7, label %19, !dbg !97

7:                                                ; preds = %4
  br label %8, !dbg !98

8:                                                ; preds = %11, %7
  %9 = phi i64 [ %14, %11 ], [ 1, %7 ]
  %10 = icmp slt i64 %9, 351, !dbg !99
  br i1 %10, label %11, label %15, !dbg !100

11:                                               ; preds = %8
  %12 = getelementptr ptr, ptr @perm, i64 %9, !dbg !101
  %13 = getelementptr %_Lowered_basket, ptr @basket, i64 %9, !dbg !102
  store ptr %13, ptr %12, align 8, !dbg !103
  %14 = add i64 %9, 1, !dbg !104
  br label %8, !dbg !105

15:                                               ; preds = %8
  %16 = add i64 %0, -1, !dbg !106
  %17 = sdiv i64 %16, 300, !dbg !107
  %18 = add i64 %17, 1, !dbg !108
  store i64 %18, ptr @nr_group, align 8, !dbg !109
  store i64 0, ptr @group_pos, align 8, !dbg !110
  store i64 0, ptr @basket_size, align 8, !dbg !111
  store i64 0, ptr @initialize, align 8, !dbg !112
  br label %118, !dbg !113

19:                                               ; preds = %4
  br label %20, !dbg !114

20:                                               ; preds = %116, %19
  %21 = phi i64 [ %113, %116 ], [ undef, %19 ]
  %22 = phi i64 [ %114, %116 ], [ 0, %19 ]
  %23 = phi i64 [ %115, %116 ], [ 2, %19 ]
  %24 = icmp sle i64 %23, 50, !dbg !115
  br i1 %24, label %25, label %110, !dbg !116

25:                                               ; preds = %20
  %26 = load i64, ptr @basket_size, align 8, !dbg !117
  %27 = icmp sle i64 %23, %26, !dbg !118
  br i1 %27, label %28, label %104, !dbg !119

28:                                               ; preds = %25
  %29 = getelementptr ptr, ptr @perm, i64 %23, !dbg !120
  %30 = load ptr, ptr %29, align 8, !dbg !121
  %31 = getelementptr %_Lowered_basket, ptr %30, i32 0, i32 0, !dbg !122
  %32 = load ptr, ptr %31, align 8, !dbg !123
  %33 = getelementptr %_Lowered_rarc, ptr %32, i32 0, i32 0, !dbg !124
  %34 = ptrtoint ptr %33 to i64, !dbg !125
  %35 = call i128 @cache_request(i64 %34), !dbg !126
  %36 = call ptr @cache_access(i128 %35), !dbg !127
  %37 = load i64, ptr %36, align 8, !dbg !128
  %38 = getelementptr %_Lowered_rarc, ptr %32, i32 0, i32 1, !dbg !129
  %39 = ptrtoint ptr %38 to i64, !dbg !130
  %40 = call i128 @cache_request(i64 %39), !dbg !131
  %41 = call ptr @cache_access(i128 %40), !dbg !132
  %42 = load ptr, ptr %41, align 8, !dbg !133
  %43 = getelementptr %_Lowered_node, ptr %42, i32 0, i32 0, !dbg !134
  %44 = load i64, ptr %43, align 8, !dbg !135
  %45 = sub i64 %37, %44, !dbg !136
  %46 = getelementptr %_Lowered_rarc, ptr %32, i32 0, i32 2, !dbg !137
  %47 = ptrtoint ptr %46 to i64, !dbg !138
  %48 = call i128 @cache_request(i64 %47), !dbg !139
  %49 = call ptr @cache_access(i128 %48), !dbg !140
  %50 = load ptr, ptr %49, align 8, !dbg !141
  %51 = getelementptr %_Lowered_node, ptr %50, i32 0, i32 0, !dbg !142
  %52 = load i64, ptr %51, align 8, !dbg !143
  %53 = add i64 %45, %52, !dbg !144
  %54 = icmp slt i64 %53, 0, !dbg !145
  br i1 %54, label %55, label %62, !dbg !146

55:                                               ; preds = %28
  %56 = getelementptr %_Lowered_rarc, ptr %32, i32 0, i32 3, !dbg !147
  %57 = ptrtoint ptr %56 to i64, !dbg !148
  %58 = call i128 @cache_request(i64 %57), !dbg !149
  %59 = call ptr @cache_access(i128 %58), !dbg !150
  %60 = load i32, ptr %59, align 4, !dbg !151
  %61 = icmp eq i32 %60, 1, !dbg !152
  br label %63, !dbg !153

62:                                               ; preds = %28
  br label %63, !dbg !154

63:                                               ; preds = %55, %62
  %64 = phi i1 [ false, %62 ], [ %61, %55 ]
  br label %65, !dbg !155

65:                                               ; preds = %63
  br i1 %64, label %66, label %67, !dbg !156

66:                                               ; preds = %65
  br label %80, !dbg !157

67:                                               ; preds = %65
  %68 = icmp sgt i64 %53, 0, !dbg !158
  br i1 %68, label %69, label %76, !dbg !159

69:                                               ; preds = %67
  %70 = getelementptr %_Lowered_rarc, ptr %32, i32 0, i32 3, !dbg !160
  %71 = ptrtoint ptr %70 to i64, !dbg !161
  %72 = call i128 @cache_request(i64 %71), !dbg !162
  %73 = call ptr @cache_access(i128 %72), !dbg !163
  %74 = load i32, ptr %73, align 4, !dbg !164
  %75 = icmp eq i32 %74, 2, !dbg !165
  br label %77, !dbg !166

76:                                               ; preds = %67
  br label %77, !dbg !167

77:                                               ; preds = %69, %76
  %78 = phi i1 [ false, %76 ], [ %75, %69 ]
  br label %79, !dbg !168

79:                                               ; preds = %77
  br label %80, !dbg !169

80:                                               ; preds = %66, %79
  %81 = phi i1 [ %78, %79 ], [ true, %66 ]
  br label %82, !dbg !170

82:                                               ; preds = %80
  br i1 %81, label %83, label %99, !dbg !171

83:                                               ; preds = %82
  %84 = add i64 %22, 1, !dbg !172
  %85 = getelementptr ptr, ptr @perm, i64 %84, !dbg !173
  %86 = load ptr, ptr %85, align 8, !dbg !174
  %87 = getelementptr %_Lowered_basket, ptr %86, i32 0, i32 0, !dbg !175
  store ptr %32, ptr %87, align 8, !dbg !176
  %88 = load ptr, ptr %85, align 8, !dbg !177
  %89 = getelementptr %_Lowered_basket, ptr %88, i32 0, i32 1, !dbg !178
  store i64 %53, ptr %89, align 8, !dbg !179
  %90 = load ptr, ptr %85, align 8, !dbg !180
  %91 = getelementptr %_Lowered_basket, ptr %90, i32 0, i32 2, !dbg !181
  %92 = icmp sge i64 %53, 0, !dbg !182
  br i1 %92, label %93, label %94, !dbg !183

93:                                               ; preds = %83
  br label %96, !dbg !184

94:                                               ; preds = %83
  %95 = sub i64 0, %53, !dbg !185
  br label %96, !dbg !186

96:                                               ; preds = %93, %94
  %97 = phi i64 [ %95, %94 ], [ %53, %93 ]
  br label %98, !dbg !187

98:                                               ; preds = %96
  store i64 %97, ptr %91, align 8, !dbg !188
  br label %100, !dbg !189

99:                                               ; preds = %82
  br label %100, !dbg !190

100:                                              ; preds = %98, %99
  %101 = phi i64 [ %22, %99 ], [ %84, %98 ]
  br label %102, !dbg !191

102:                                              ; preds = %100
  %103 = add i64 %23, 1, !dbg !192
  br label %105, !dbg !193

104:                                              ; preds = %25
  br label %105, !dbg !194

105:                                              ; preds = %102, %104
  %106 = phi i64 [ %21, %104 ], [ %53, %102 ]
  %107 = phi i64 [ %22, %104 ], [ %101, %102 ]
  %108 = phi i64 [ %23, %104 ], [ %103, %102 ]
  br label %109, !dbg !195

109:                                              ; preds = %105
  br label %111, !dbg !196

110:                                              ; preds = %20
  br label %111, !dbg !197

111:                                              ; preds = %109, %110
  %112 = phi i1 [ false, %110 ], [ %27, %109 ]
  %113 = phi i64 [ %21, %110 ], [ %106, %109 ]
  %114 = phi i64 [ %22, %110 ], [ %107, %109 ]
  %115 = phi i64 [ %23, %110 ], [ %108, %109 ]
  br label %116, !dbg !198

116:                                              ; preds = %111
  br i1 %112, label %20, label %117, !dbg !199

117:                                              ; preds = %116
  store i64 %114, ptr @basket_size, align 8, !dbg !200
  br label %118, !dbg !201

118:                                              ; preds = %15, %117
  %119 = phi i64 [ %113, %117 ], [ undef, %15 ]
  br label %120, !dbg !202

120:                                              ; preds = %118
  %121 = load i64, ptr @group_pos, align 8, !dbg !203
  br label %122, !dbg !204

122:                                              ; preds = %208, %120
  %123 = phi i64 [ %127, %208 ], [ %119, %120 ]
  %124 = load i64, ptr @group_pos, align 8, !dbg !205
  %125 = getelementptr %_Lowered_rarc, ptr %1, i64 %124, !dbg !206
  br label %126, !dbg !207

126:                                              ; preds = %189, %122
  %127 = phi i64 [ %188, %189 ], [ %123, %122 ]
  %128 = phi ptr [ %191, %189 ], [ %125, %122 ]
  %129 = icmp ult ptr %128, %2, !dbg !208
  br i1 %129, label %130, label %192, !dbg !209

130:                                              ; preds = %126
  %131 = phi i64 [ %127, %126 ]
  %132 = phi ptr [ %128, %126 ]
  %133 = getelementptr %_Lowered_rarc, ptr %132, i32 0, i32 3, !dbg !210
  %134 = ptrtoint ptr %133 to i64, !dbg !211
  %135 = call i128 @cache_request(i64 %134), !dbg !212
  %136 = call ptr @cache_access(i128 %135), !dbg !213
  %137 = load i32, ptr %136, align 4, !dbg !214
  %138 = icmp sgt i32 %137, 0, !dbg !215
  br i1 %138, label %139, label %186, !dbg !216

139:                                              ; preds = %130
  %140 = getelementptr %_Lowered_rarc, ptr %132, i32 0, i32 0, !dbg !217
  %141 = ptrtoint ptr %140 to i64, !dbg !218
  %142 = call i128 @cache_request(i64 %141), !dbg !219
  %143 = call ptr @cache_access(i128 %142), !dbg !220
  %144 = load i64, ptr %143, align 8, !dbg !221
  %145 = getelementptr %_Lowered_rarc, ptr %132, i32 0, i32 1, !dbg !222
  %146 = ptrtoint ptr %145 to i64, !dbg !223
  %147 = call i128 @cache_request(i64 %146), !dbg !224
  %148 = call ptr @cache_access(i128 %147), !dbg !225
  %149 = load ptr, ptr %148, align 8, !dbg !226
  %150 = getelementptr %_Lowered_node, ptr %149, i32 0, i32 0, !dbg !227
  %151 = load i64, ptr %150, align 8, !dbg !228
  %152 = sub i64 %144, %151, !dbg !229
  %153 = getelementptr %_Lowered_rarc, ptr %132, i32 0, i32 2, !dbg !230
  %154 = ptrtoint ptr %153 to i64, !dbg !231
  %155 = call i128 @cache_request(i64 %154), !dbg !232
  %156 = call ptr @cache_access(i128 %155), !dbg !233
  %157 = load ptr, ptr %156, align 8, !dbg !234
  %158 = getelementptr %_Lowered_node, ptr %157, i32 0, i32 0, !dbg !235
  %159 = load i64, ptr %158, align 8, !dbg !236
  %160 = add i64 %152, %159, !dbg !237
  %161 = call i32 @bea_is_dual_infeasible(ptr %132, i64 %160), !dbg !238
  %162 = icmp ne i32 %161, 0, !dbg !239
  br i1 %162, label %163, label %185, !dbg !240

163:                                              ; preds = %139
  %164 = load i64, ptr @basket_size, align 8, !dbg !241
  %165 = add i64 %164, 1, !dbg !242
  store i64 %165, ptr @basket_size, align 8, !dbg !243
  %166 = load i64, ptr @basket_size, align 8, !dbg !244
  %167 = getelementptr ptr, ptr @perm, i64 %166, !dbg !245
  %168 = load ptr, ptr %167, align 8, !dbg !246
  %169 = getelementptr %_Lowered_basket, ptr %168, i32 0, i32 0, !dbg !247
  store ptr %132, ptr %169, align 8, !dbg !248
  %170 = load i64, ptr @basket_size, align 8, !dbg !249
  %171 = getelementptr ptr, ptr @perm, i64 %170, !dbg !250
  %172 = load ptr, ptr %171, align 8, !dbg !251
  %173 = getelementptr %_Lowered_basket, ptr %172, i32 0, i32 1, !dbg !252
  store i64 %160, ptr %173, align 8, !dbg !253
  %174 = load i64, ptr @basket_size, align 8, !dbg !254
  %175 = getelementptr ptr, ptr @perm, i64 %174, !dbg !255
  %176 = load ptr, ptr %175, align 8, !dbg !256
  %177 = getelementptr %_Lowered_basket, ptr %176, i32 0, i32 2, !dbg !257
  %178 = icmp sge i64 %160, 0, !dbg !258
  br i1 %178, label %179, label %180, !dbg !259

179:                                              ; preds = %163
  br label %182, !dbg !260

180:                                              ; preds = %163
  %181 = sub i64 0, %160, !dbg !261
  br label %182, !dbg !262

182:                                              ; preds = %179, %180
  %183 = phi i64 [ %181, %180 ], [ %160, %179 ]
  br label %184, !dbg !263

184:                                              ; preds = %182
  store i64 %183, ptr %177, align 8, !dbg !264
  br label %185, !dbg !265

185:                                              ; preds = %184, %139
  br label %187, !dbg !266

186:                                              ; preds = %130
  br label %187, !dbg !267

187:                                              ; preds = %185, %186
  %188 = phi i64 [ %131, %186 ], [ %160, %185 ]
  br label %189, !dbg !268

189:                                              ; preds = %187
  %190 = load i64, ptr @nr_group, align 8, !dbg !269
  %191 = getelementptr %_Lowered_rarc, ptr %132, i64 %190, !dbg !270
  br label %126, !dbg !271

192:                                              ; preds = %126
  %193 = load i64, ptr @group_pos, align 8, !dbg !272
  %194 = add i64 %193, 1, !dbg !273
  store i64 %194, ptr @group_pos, align 8, !dbg !274
  %195 = load i64, ptr @nr_group, align 8, !dbg !275
  %196 = load i64, ptr @group_pos, align 8, !dbg !276
  %197 = icmp eq i64 %196, %195, !dbg !277
  br i1 %197, label %198, label %199, !dbg !278

198:                                              ; preds = %192
  store i64 0, ptr @group_pos, align 8, !dbg !279
  br label %199, !dbg !280

199:                                              ; preds = %198, %192
  %200 = load i64, ptr @basket_size, align 8, !dbg !281
  %201 = icmp slt i64 %200, 50, !dbg !282
  br i1 %201, label %202, label %205, !dbg !283

202:                                              ; preds = %199
  %203 = load i64, ptr @group_pos, align 8, !dbg !284
  %204 = icmp ne i64 %203, %121, !dbg !285
  br label %206, !dbg !286

205:                                              ; preds = %199
  br label %206, !dbg !287

206:                                              ; preds = %202, %205
  %207 = phi i1 [ false, %205 ], [ %204, %202 ]
  br label %208, !dbg !288

208:                                              ; preds = %206
  br i1 %207, label %122, label %209, !dbg !289

209:                                              ; preds = %208
  %210 = load i64, ptr @basket_size, align 8, !dbg !290
  %211 = icmp eq i64 %210, 0, !dbg !291
  br i1 %211, label %212, label %213, !dbg !292

212:                                              ; preds = %209
  store i64 1, ptr @initialize, align 8, !dbg !293
  store i64 0, ptr %3, align 8, !dbg !294
  br label %221, !dbg !295

213:                                              ; preds = %209
  %214 = load i64, ptr @basket_size, align 8, !dbg !296
  call void @sort_basket(i64 1, i64 %214), !dbg !297
  %215 = load ptr, ptr getelementptr inbounds (ptr, ptr @perm, i32 1), align 8, !dbg !298
  %216 = getelementptr %_Lowered_basket, ptr %215, i32 0, i32 1, !dbg !299
  %217 = load i64, ptr %216, align 8, !dbg !300
  store i64 %217, ptr %3, align 8, !dbg !301
  %218 = load ptr, ptr getelementptr inbounds (ptr, ptr @perm, i32 1), align 8, !dbg !302
  %219 = getelementptr %_Lowered_basket, ptr %218, i32 0, i32 0, !dbg !303
  %220 = load ptr, ptr %219, align 8, !dbg !304
  br label %221, !dbg !305

221:                                              ; preds = %212, %213
  %222 = phi ptr [ %220, %213 ], [ null, %212 ]
  br label %223, !dbg !306

223:                                              ; preds = %221
  ret ptr %222, !dbg !307
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "bea_is_dual_infeasible", linkageName: "bea_is_dual_infeasible", scope: null, file: !4, line: 28, type: !5, scopeLine: 28, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/pbeampp.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 34, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 35, column: 5, scope: !8)
!10 = !DILocation(line: 37, column: 10, scope: !8)
!11 = !DILocation(line: 38, column: 10, scope: !8)
!12 = !DILocation(line: 39, column: 10, scope: !8)
!13 = !DILocation(line: 40, column: 10, scope: !8)
!14 = !DILocation(line: 42, column: 11, scope: !8)
!15 = !DILocation(line: 43, column: 11, scope: !8)
!16 = !DILocation(line: 44, column: 5, scope: !8)
!17 = !DILocation(line: 46, column: 5, scope: !8)
!18 = !DILocation(line: 48, column: 5, scope: !8)
!19 = !DILocation(line: 50, column: 5, scope: !8)
!20 = !DILocation(line: 52, column: 5, scope: !8)
!21 = !DILocation(line: 54, column: 11, scope: !8)
!22 = !DILocation(line: 55, column: 5, scope: !8)
!23 = !DILocation(line: 57, column: 11, scope: !8)
!24 = !DILocation(line: 58, column: 11, scope: !8)
!25 = !DILocation(line: 59, column: 11, scope: !8)
!26 = !DILocation(line: 60, column: 11, scope: !8)
!27 = !DILocation(line: 62, column: 11, scope: !8)
!28 = !DILocation(line: 63, column: 11, scope: !8)
!29 = !DILocation(line: 64, column: 5, scope: !8)
!30 = !DILocation(line: 66, column: 5, scope: !8)
!31 = !DILocation(line: 68, column: 5, scope: !8)
!32 = !DILocation(line: 70, column: 5, scope: !8)
!33 = !DILocation(line: 72, column: 5, scope: !8)
!34 = !DILocation(line: 74, column: 11, scope: !8)
!35 = !DILocation(line: 75, column: 5, scope: !8)
!36 = distinct !DISubprogram(name: "sort_basket", linkageName: "sort_basket", scope: null, file: !4, line: 77, type: !5, scopeLine: 77, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!37 = !DILocation(line: 84, column: 10, scope: !38)
!38 = !DILexicalBlockFile(scope: !36, file: !4, discriminator: 0)
!39 = !DILocation(line: 85, column: 10, scope: !38)
!40 = !DILocation(line: 86, column: 10, scope: !38)
!41 = !DILocation(line: 87, column: 10, scope: !38)
!42 = !DILocation(line: 88, column: 11, scope: !38)
!43 = !DILocation(line: 89, column: 11, scope: !38)
!44 = !DILocation(line: 91, column: 5, scope: !38)
!45 = !DILocation(line: 93, column: 5, scope: !38)
!46 = !DILocation(line: 95, column: 11, scope: !38)
!47 = !DILocation(line: 96, column: 11, scope: !38)
!48 = !DILocation(line: 97, column: 11, scope: !38)
!49 = !DILocation(line: 98, column: 11, scope: !38)
!50 = !DILocation(line: 99, column: 11, scope: !38)
!51 = !DILocation(line: 100, column: 5, scope: !38)
!52 = !DILocation(line: 102, column: 11, scope: !38)
!53 = !DILocation(line: 103, column: 5, scope: !38)
!54 = !DILocation(line: 105, column: 5, scope: !38)
!55 = !DILocation(line: 107, column: 11, scope: !38)
!56 = !DILocation(line: 108, column: 11, scope: !38)
!57 = !DILocation(line: 109, column: 11, scope: !38)
!58 = !DILocation(line: 110, column: 11, scope: !38)
!59 = !DILocation(line: 111, column: 11, scope: !38)
!60 = !DILocation(line: 112, column: 5, scope: !38)
!61 = !DILocation(line: 114, column: 11, scope: !38)
!62 = !DILocation(line: 115, column: 5, scope: !38)
!63 = !DILocation(line: 117, column: 11, scope: !38)
!64 = !DILocation(line: 118, column: 5, scope: !38)
!65 = !DILocation(line: 120, column: 11, scope: !38)
!66 = !DILocation(line: 121, column: 11, scope: !38)
!67 = !DILocation(line: 122, column: 11, scope: !38)
!68 = !DILocation(line: 123, column: 11, scope: !38)
!69 = !DILocation(line: 124, column: 5, scope: !38)
!70 = !DILocation(line: 125, column: 5, scope: !38)
!71 = !DILocation(line: 126, column: 5, scope: !38)
!72 = !DILocation(line: 128, column: 11, scope: !38)
!73 = !DILocation(line: 129, column: 5, scope: !38)
!74 = !DILocation(line: 131, column: 11, scope: !38)
!75 = !DILocation(line: 132, column: 11, scope: !38)
!76 = !DILocation(line: 133, column: 5, scope: !38)
!77 = !DILocation(line: 135, column: 5, scope: !38)
!78 = !DILocation(line: 137, column: 5, scope: !38)
!79 = !DILocation(line: 139, column: 11, scope: !38)
!80 = !DILocation(line: 140, column: 5, scope: !38)
!81 = !DILocation(line: 142, column: 5, scope: !38)
!82 = !DILocation(line: 144, column: 11, scope: !38)
!83 = !DILocation(line: 145, column: 5, scope: !38)
!84 = !DILocation(line: 147, column: 5, scope: !38)
!85 = !DILocation(line: 148, column: 5, scope: !38)
!86 = !DILocation(line: 150, column: 11, scope: !38)
!87 = !DILocation(line: 151, column: 11, scope: !38)
!88 = !DILocation(line: 152, column: 11, scope: !38)
!89 = !DILocation(line: 153, column: 5, scope: !38)
!90 = !DILocation(line: 155, column: 5, scope: !38)
!91 = !DILocation(line: 156, column: 5, scope: !38)
!92 = !DILocation(line: 158, column: 5, scope: !38)
!93 = distinct !DISubprogram(name: "primal_bea_mpp", linkageName: "primal_bea_mpp", scope: null, file: !4, line: 160, type: !5, scopeLine: 160, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!94 = !DILocation(line: 176, column: 11, scope: !95)
!95 = !DILexicalBlockFile(scope: !93, file: !4, discriminator: 0)
!96 = !DILocation(line: 177, column: 11, scope: !95)
!97 = !DILocation(line: 178, column: 5, scope: !95)
!98 = !DILocation(line: 184, column: 5, scope: !95)
!99 = !DILocation(line: 186, column: 11, scope: !95)
!100 = !DILocation(line: 187, column: 5, scope: !95)
!101 = !DILocation(line: 189, column: 11, scope: !95)
!102 = !DILocation(line: 190, column: 11, scope: !95)
!103 = !DILocation(line: 191, column: 5, scope: !95)
!104 = !DILocation(line: 192, column: 11, scope: !95)
!105 = !DILocation(line: 193, column: 5, scope: !95)
!106 = !DILocation(line: 196, column: 11, scope: !95)
!107 = !DILocation(line: 197, column: 11, scope: !95)
!108 = !DILocation(line: 198, column: 11, scope: !95)
!109 = !DILocation(line: 199, column: 5, scope: !95)
!110 = !DILocation(line: 201, column: 5, scope: !95)
!111 = !DILocation(line: 203, column: 5, scope: !95)
!112 = !DILocation(line: 204, column: 5, scope: !95)
!113 = !DILocation(line: 205, column: 5, scope: !95)
!114 = !DILocation(line: 207, column: 5, scope: !95)
!115 = !DILocation(line: 209, column: 11, scope: !95)
!116 = !DILocation(line: 210, column: 5, scope: !95)
!117 = !DILocation(line: 213, column: 11, scope: !95)
!118 = !DILocation(line: 214, column: 11, scope: !95)
!119 = !DILocation(line: 215, column: 5, scope: !95)
!120 = !DILocation(line: 219, column: 11, scope: !95)
!121 = !DILocation(line: 220, column: 11, scope: !95)
!122 = !DILocation(line: 221, column: 11, scope: !95)
!123 = !DILocation(line: 222, column: 11, scope: !95)
!124 = !DILocation(line: 223, column: 11, scope: !95)
!125 = !DILocation(line: 224, column: 11, scope: !95)
!126 = !DILocation(line: 225, column: 11, scope: !95)
!127 = !DILocation(line: 226, column: 11, scope: !95)
!128 = !DILocation(line: 228, column: 11, scope: !95)
!129 = !DILocation(line: 229, column: 11, scope: !95)
!130 = !DILocation(line: 230, column: 11, scope: !95)
!131 = !DILocation(line: 231, column: 11, scope: !95)
!132 = !DILocation(line: 232, column: 11, scope: !95)
!133 = !DILocation(line: 234, column: 11, scope: !95)
!134 = !DILocation(line: 235, column: 11, scope: !95)
!135 = !DILocation(line: 236, column: 11, scope: !95)
!136 = !DILocation(line: 237, column: 11, scope: !95)
!137 = !DILocation(line: 238, column: 11, scope: !95)
!138 = !DILocation(line: 239, column: 11, scope: !95)
!139 = !DILocation(line: 240, column: 11, scope: !95)
!140 = !DILocation(line: 241, column: 11, scope: !95)
!141 = !DILocation(line: 243, column: 11, scope: !95)
!142 = !DILocation(line: 244, column: 11, scope: !95)
!143 = !DILocation(line: 245, column: 11, scope: !95)
!144 = !DILocation(line: 246, column: 11, scope: !95)
!145 = !DILocation(line: 247, column: 11, scope: !95)
!146 = !DILocation(line: 248, column: 5, scope: !95)
!147 = !DILocation(line: 250, column: 11, scope: !95)
!148 = !DILocation(line: 251, column: 11, scope: !95)
!149 = !DILocation(line: 252, column: 11, scope: !95)
!150 = !DILocation(line: 253, column: 11, scope: !95)
!151 = !DILocation(line: 255, column: 11, scope: !95)
!152 = !DILocation(line: 256, column: 11, scope: !95)
!153 = !DILocation(line: 257, column: 5, scope: !95)
!154 = !DILocation(line: 259, column: 5, scope: !95)
!155 = !DILocation(line: 261, column: 5, scope: !95)
!156 = !DILocation(line: 263, column: 5, scope: !95)
!157 = !DILocation(line: 265, column: 5, scope: !95)
!158 = !DILocation(line: 267, column: 11, scope: !95)
!159 = !DILocation(line: 268, column: 5, scope: !95)
!160 = !DILocation(line: 270, column: 11, scope: !95)
!161 = !DILocation(line: 271, column: 11, scope: !95)
!162 = !DILocation(line: 272, column: 11, scope: !95)
!163 = !DILocation(line: 273, column: 11, scope: !95)
!164 = !DILocation(line: 275, column: 11, scope: !95)
!165 = !DILocation(line: 276, column: 11, scope: !95)
!166 = !DILocation(line: 277, column: 5, scope: !95)
!167 = !DILocation(line: 279, column: 5, scope: !95)
!168 = !DILocation(line: 281, column: 5, scope: !95)
!169 = !DILocation(line: 283, column: 5, scope: !95)
!170 = !DILocation(line: 285, column: 5, scope: !95)
!171 = !DILocation(line: 287, column: 5, scope: !95)
!172 = !DILocation(line: 289, column: 11, scope: !95)
!173 = !DILocation(line: 290, column: 11, scope: !95)
!174 = !DILocation(line: 291, column: 11, scope: !95)
!175 = !DILocation(line: 292, column: 11, scope: !95)
!176 = !DILocation(line: 293, column: 5, scope: !95)
!177 = !DILocation(line: 294, column: 11, scope: !95)
!178 = !DILocation(line: 295, column: 11, scope: !95)
!179 = !DILocation(line: 296, column: 5, scope: !95)
!180 = !DILocation(line: 297, column: 11, scope: !95)
!181 = !DILocation(line: 298, column: 11, scope: !95)
!182 = !DILocation(line: 299, column: 11, scope: !95)
!183 = !DILocation(line: 300, column: 5, scope: !95)
!184 = !DILocation(line: 302, column: 5, scope: !95)
!185 = !DILocation(line: 304, column: 11, scope: !95)
!186 = !DILocation(line: 305, column: 5, scope: !95)
!187 = !DILocation(line: 307, column: 5, scope: !95)
!188 = !DILocation(line: 309, column: 5, scope: !95)
!189 = !DILocation(line: 310, column: 5, scope: !95)
!190 = !DILocation(line: 312, column: 5, scope: !95)
!191 = !DILocation(line: 314, column: 5, scope: !95)
!192 = !DILocation(line: 316, column: 12, scope: !95)
!193 = !DILocation(line: 317, column: 5, scope: !95)
!194 = !DILocation(line: 319, column: 5, scope: !95)
!195 = !DILocation(line: 321, column: 5, scope: !95)
!196 = !DILocation(line: 323, column: 5, scope: !95)
!197 = !DILocation(line: 325, column: 5, scope: !95)
!198 = !DILocation(line: 327, column: 5, scope: !95)
!199 = !DILocation(line: 329, column: 5, scope: !95)
!200 = !DILocation(line: 332, column: 5, scope: !95)
!201 = !DILocation(line: 333, column: 5, scope: !95)
!202 = !DILocation(line: 335, column: 5, scope: !95)
!203 = !DILocation(line: 338, column: 12, scope: !95)
!204 = !DILocation(line: 341, column: 5, scope: !95)
!205 = !DILocation(line: 343, column: 12, scope: !95)
!206 = !DILocation(line: 344, column: 12, scope: !95)
!207 = !DILocation(line: 345, column: 5, scope: !95)
!208 = !DILocation(line: 347, column: 12, scope: !95)
!209 = !DILocation(line: 348, column: 5, scope: !95)
!210 = !DILocation(line: 350, column: 12, scope: !95)
!211 = !DILocation(line: 351, column: 12, scope: !95)
!212 = !DILocation(line: 352, column: 12, scope: !95)
!213 = !DILocation(line: 353, column: 12, scope: !95)
!214 = !DILocation(line: 355, column: 12, scope: !95)
!215 = !DILocation(line: 356, column: 12, scope: !95)
!216 = !DILocation(line: 357, column: 5, scope: !95)
!217 = !DILocation(line: 359, column: 12, scope: !95)
!218 = !DILocation(line: 360, column: 12, scope: !95)
!219 = !DILocation(line: 361, column: 12, scope: !95)
!220 = !DILocation(line: 362, column: 12, scope: !95)
!221 = !DILocation(line: 364, column: 12, scope: !95)
!222 = !DILocation(line: 365, column: 12, scope: !95)
!223 = !DILocation(line: 366, column: 12, scope: !95)
!224 = !DILocation(line: 367, column: 12, scope: !95)
!225 = !DILocation(line: 368, column: 12, scope: !95)
!226 = !DILocation(line: 370, column: 12, scope: !95)
!227 = !DILocation(line: 371, column: 12, scope: !95)
!228 = !DILocation(line: 372, column: 12, scope: !95)
!229 = !DILocation(line: 373, column: 12, scope: !95)
!230 = !DILocation(line: 374, column: 12, scope: !95)
!231 = !DILocation(line: 375, column: 12, scope: !95)
!232 = !DILocation(line: 376, column: 12, scope: !95)
!233 = !DILocation(line: 377, column: 12, scope: !95)
!234 = !DILocation(line: 379, column: 12, scope: !95)
!235 = !DILocation(line: 380, column: 12, scope: !95)
!236 = !DILocation(line: 381, column: 12, scope: !95)
!237 = !DILocation(line: 382, column: 12, scope: !95)
!238 = !DILocation(line: 383, column: 12, scope: !95)
!239 = !DILocation(line: 384, column: 12, scope: !95)
!240 = !DILocation(line: 385, column: 5, scope: !95)
!241 = !DILocation(line: 387, column: 12, scope: !95)
!242 = !DILocation(line: 388, column: 12, scope: !95)
!243 = !DILocation(line: 389, column: 5, scope: !95)
!244 = !DILocation(line: 392, column: 12, scope: !95)
!245 = !DILocation(line: 393, column: 12, scope: !95)
!246 = !DILocation(line: 394, column: 12, scope: !95)
!247 = !DILocation(line: 395, column: 12, scope: !95)
!248 = !DILocation(line: 396, column: 5, scope: !95)
!249 = !DILocation(line: 397, column: 12, scope: !95)
!250 = !DILocation(line: 398, column: 12, scope: !95)
!251 = !DILocation(line: 399, column: 12, scope: !95)
!252 = !DILocation(line: 400, column: 12, scope: !95)
!253 = !DILocation(line: 401, column: 5, scope: !95)
!254 = !DILocation(line: 402, column: 12, scope: !95)
!255 = !DILocation(line: 403, column: 12, scope: !95)
!256 = !DILocation(line: 404, column: 12, scope: !95)
!257 = !DILocation(line: 405, column: 12, scope: !95)
!258 = !DILocation(line: 406, column: 12, scope: !95)
!259 = !DILocation(line: 407, column: 5, scope: !95)
!260 = !DILocation(line: 409, column: 5, scope: !95)
!261 = !DILocation(line: 411, column: 12, scope: !95)
!262 = !DILocation(line: 412, column: 5, scope: !95)
!263 = !DILocation(line: 414, column: 5, scope: !95)
!264 = !DILocation(line: 416, column: 5, scope: !95)
!265 = !DILocation(line: 417, column: 5, scope: !95)
!266 = !DILocation(line: 419, column: 5, scope: !95)
!267 = !DILocation(line: 421, column: 5, scope: !95)
!268 = !DILocation(line: 423, column: 5, scope: !95)
!269 = !DILocation(line: 425, column: 12, scope: !95)
!270 = !DILocation(line: 426, column: 12, scope: !95)
!271 = !DILocation(line: 427, column: 5, scope: !95)
!272 = !DILocation(line: 429, column: 12, scope: !95)
!273 = !DILocation(line: 430, column: 12, scope: !95)
!274 = !DILocation(line: 431, column: 5, scope: !95)
!275 = !DILocation(line: 432, column: 12, scope: !95)
!276 = !DILocation(line: 433, column: 12, scope: !95)
!277 = !DILocation(line: 434, column: 12, scope: !95)
!278 = !DILocation(line: 435, column: 5, scope: !95)
!279 = !DILocation(line: 437, column: 5, scope: !95)
!280 = !DILocation(line: 438, column: 5, scope: !95)
!281 = !DILocation(line: 440, column: 12, scope: !95)
!282 = !DILocation(line: 441, column: 12, scope: !95)
!283 = !DILocation(line: 442, column: 5, scope: !95)
!284 = !DILocation(line: 444, column: 12, scope: !95)
!285 = !DILocation(line: 445, column: 12, scope: !95)
!286 = !DILocation(line: 446, column: 5, scope: !95)
!287 = !DILocation(line: 448, column: 5, scope: !95)
!288 = !DILocation(line: 450, column: 5, scope: !95)
!289 = !DILocation(line: 452, column: 5, scope: !95)
!290 = !DILocation(line: 454, column: 12, scope: !95)
!291 = !DILocation(line: 455, column: 12, scope: !95)
!292 = !DILocation(line: 456, column: 5, scope: !95)
!293 = !DILocation(line: 458, column: 5, scope: !95)
!294 = !DILocation(line: 459, column: 5, scope: !95)
!295 = !DILocation(line: 461, column: 5, scope: !95)
!296 = !DILocation(line: 463, column: 12, scope: !95)
!297 = !DILocation(line: 464, column: 5, scope: !95)
!298 = !DILocation(line: 468, column: 12, scope: !95)
!299 = !DILocation(line: 469, column: 12, scope: !95)
!300 = !DILocation(line: 470, column: 12, scope: !95)
!301 = !DILocation(line: 471, column: 5, scope: !95)
!302 = !DILocation(line: 472, column: 12, scope: !95)
!303 = !DILocation(line: 473, column: 12, scope: !95)
!304 = !DILocation(line: 474, column: 12, scope: !95)
!305 = !DILocation(line: 475, column: 5, scope: !95)
!306 = !DILocation(line: 477, column: 5, scope: !95)
!307 = !DILocation(line: 479, column: 5, scope: !95)
