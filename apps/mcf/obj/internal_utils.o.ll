; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@str2 = internal constant [13 x i8] c"%ld %ld %ld\0A\00"
@str1 = internal constant [9 x i8] c"%ld %ld\0A\00"
@instring = internal global [201 x i8] undef
@str0 = internal constant [2 x i8] c"r\00"
@in = internal global ptr null

declare ptr @malloc(i64)

declare void @free(ptr)

declare i32 @fclose(ptr)

declare i32 @__isoc99_sscanf(ptr, ptr, ...)

declare ptr @fgets(ptr, i32, ptr)

declare ptr @fopen(ptr, ptr)

define i32 @init_file(ptr %0) !dbg !3 {
  %2 = call ptr @fopen(ptr %0, ptr @str0), !dbg !7
  store ptr %2, ptr @in, align 8, !dbg !9
  %3 = load ptr, ptr @in, align 8, !dbg !10
  %4 = icmp eq ptr %3, null, !dbg !11
  %5 = xor i1 %4, true, !dbg !12
  %6 = select i1 %4, i32 0, i32 undef, !dbg !13
  %7 = select i1 %5, i32 1, i32 %6, !dbg !14
  ret i32 %7, !dbg !15
}

define i32 @read_l_l(ptr %0, ptr %1) !dbg !16 {
  %3 = load ptr, ptr @in, align 8, !dbg !17
  %4 = call ptr @fgets(ptr @instring, i32 200, ptr %3), !dbg !19
  %5 = call i32 (ptr, ptr, ...) @__isoc99_sscanf(ptr @instring, ptr @str1, ptr %0, ptr %1), !dbg !20
  %6 = icmp ne i32 %5, 2, !dbg !21
  %7 = icmp eq i32 %5, 2, !dbg !22
  %8 = select i1 %6, i32 0, i32 undef, !dbg !23
  %9 = select i1 %7, i32 1, i32 %8, !dbg !24
  ret i32 %9, !dbg !25
}

define i32 @read_l_l_l(ptr %0, ptr %1, ptr %2) !dbg !26 {
  %4 = load ptr, ptr @in, align 8, !dbg !27
  %5 = call ptr @fgets(ptr @instring, i32 200, ptr %4), !dbg !29
  %6 = call i32 (ptr, ptr, ...) @__isoc99_sscanf(ptr @instring, ptr @str2, ptr %0, ptr %1, ptr %2), !dbg !30
  %7 = icmp ne i32 %6, 3, !dbg !31
  %8 = icmp eq i32 %6, 3, !dbg !32
  %9 = select i1 %7, i32 0, i32 undef, !dbg !33
  %10 = select i1 %8, i32 1, i32 %9, !dbg !34
  ret i32 %10, !dbg !35
}

define void @finish_reading() !dbg !36 {
  %1 = load ptr, ptr @in, align 8, !dbg !37
  %2 = call i32 @fclose(ptr %1), !dbg !39
  ret void, !dbg !40
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "init_file", linkageName: "init_file", scope: null, file: !4, line: 17, type: !5, scopeLine: 17, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/internal_utils.o_llvm.mlir", directory: "/home/wuklab/spec-eval/429.mcf/src")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 25, column: 10, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 26, column: 5, scope: !8)
!10 = !DILocation(line: 29, column: 11, scope: !8)
!11 = !DILocation(line: 30, column: 11, scope: !8)
!12 = !DILocation(line: 31, column: 11, scope: !8)
!13 = !DILocation(line: 32, column: 11, scope: !8)
!14 = !DILocation(line: 33, column: 11, scope: !8)
!15 = !DILocation(line: 34, column: 5, scope: !8)
!16 = distinct !DISubprogram(name: "read_l_l", linkageName: "read_l_l", scope: null, file: !4, line: 36, type: !5, scopeLine: 36, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!17 = !DILocation(line: 45, column: 10, scope: !18)
!18 = !DILexicalBlockFile(scope: !16, file: !4, discriminator: 0)
!19 = !DILocation(line: 46, column: 10, scope: !18)
!20 = !DILocation(line: 49, column: 11, scope: !18)
!21 = !DILocation(line: 50, column: 11, scope: !18)
!22 = !DILocation(line: 51, column: 11, scope: !18)
!23 = !DILocation(line: 52, column: 11, scope: !18)
!24 = !DILocation(line: 53, column: 11, scope: !18)
!25 = !DILocation(line: 54, column: 5, scope: !18)
!26 = distinct !DISubprogram(name: "read_l_l_l", linkageName: "read_l_l_l", scope: null, file: !4, line: 56, type: !5, scopeLine: 56, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!27 = !DILocation(line: 65, column: 10, scope: !28)
!28 = !DILexicalBlockFile(scope: !26, file: !4, discriminator: 0)
!29 = !DILocation(line: 66, column: 10, scope: !28)
!30 = !DILocation(line: 69, column: 11, scope: !28)
!31 = !DILocation(line: 70, column: 11, scope: !28)
!32 = !DILocation(line: 71, column: 11, scope: !28)
!33 = !DILocation(line: 72, column: 11, scope: !28)
!34 = !DILocation(line: 73, column: 11, scope: !28)
!35 = !DILocation(line: 74, column: 5, scope: !28)
!36 = distinct !DISubprogram(name: "finish_reading", linkageName: "finish_reading", scope: null, file: !4, line: 76, type: !5, scopeLine: 76, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!37 = !DILocation(line: 78, column: 10, scope: !38)
!38 = !DILexicalBlockFile(scope: !36, file: !4, discriminator: 0)
!39 = !DILocation(line: 79, column: 10, scope: !38)
!40 = !DILocation(line: 80, column: 5, scope: !38)
