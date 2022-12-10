; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%_Lowered_network = type { [200 x i8], [200 x i8], i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double, i64, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }
%struct.timespec = type { i64, i64 }

@str18 = internal constant [8 x i8] c"mcf.out\00"
@str17 = internal constant [6 x i8] c"done\0A\00"
@str16 = internal constant [34 x i8] c"nodes                      : %ld\0A\00"
@str15 = internal constant [18 x i8] c"read error, exit\0A\00"
@str14 = internal constant [2 x i8] c"\0A\00"
@str13 = internal constant [40 x i8] c"Copyright (c) 2003-2005 Andreas Loebel\0A\00"
@str12 = internal constant [46 x i8] c"Copyright (c) 2000-2002 Andreas Loebel & ZIB\0A\00"
@str11 = internal constant [52 x i8] c"Copyright (c) 1998-2000 Zuse Institut Berlin (ZIB)\0A\00"
@str10 = internal constant [32 x i8] c"\0AMCF SPEC CPU2006 version 1.10\0A\00"
@str9 = internal constant [34 x i8] c"checksum                   : %ld\0A\00"
@str8 = internal constant [29 x i8] c"not enough memory, exit(-1)\0A\00"
@str7 = internal constant [17 x i8] c"price out %f us\0A\00"
@str6 = internal constant [34 x i8] c"new implicit arcs          : %ld\0A\00"
@str5 = internal constant [15 x i8] c"suspend %f us\0A\00"
@str4 = internal constant [34 x i8] c"erased arcs                : %ld\0A\00"
@str3 = internal constant [15 x i8] c"simplex %f us\0A\00"
@str2 = internal constant [36 x i8] c"objective value            : %0.0f\0A\00"
@str1 = internal constant [34 x i8] c"simplex iterations         : %ld\0A\00"
@str0 = internal constant [34 x i8] c"active arcs                : %ld\0A\00"
@net = global %_Lowered_network undef

declare ptr @malloc(i64)

declare void @free(ptr)

declare void @channel_init()

declare void @cache_init()

declare void @init_client()

declare i32 @clock_gettime(i32, ptr)

declare ptr @strcpy(ptr, ptr)

declare i32 @printf(ptr, ...)

define i64 @global_opt() !dbg !3 {
  br label %1, !dbg !7

1:                                                ; preds = %74, %0
  %2 = phi i1 [ %31, %74 ], [ true, %0 ]
  %3 = phi i64 [ %72, %74 ], [ undef, %0 ]
  %4 = phi i64 [ %70, %74 ], [ undef, %0 ]
  %5 = phi i64 [ %69, %74 ], [ undef, %0 ]
  %6 = phi i64 [ %22, %74 ], [ undef, %0 ]
  %7 = phi i64 [ %18, %74 ], [ undef, %0 ]
  %8 = phi i64 [ %73, %74 ], [ 5, %0 ]
  %9 = phi i64 [ %71, %74 ], [ -1, %0 ]
  %10 = icmp ne i64 %9, 0, !dbg !9
  %11 = and i1 %10, %2, !dbg !10
  br i1 %11, label %12, label %75, !dbg !11

12:                                               ; preds = %1
  %13 = phi i64 [ %3, %1 ]
  %14 = phi i64 [ %4, %1 ]
  %15 = phi i64 [ %5, %1 ]
  %16 = phi i64 [ %8, %1 ]
  %17 = phi i64 [ %9, %1 ]
  %18 = call i64 @getCurNs(), !dbg !12
  %19 = load i64, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 5), align 8, !dbg !13
  %20 = call i32 (ptr, ...) @printf(ptr @str0, i64 %19), !dbg !14
  %21 = call i64 @primal_net_simplex(ptr @net), !dbg !15
  %22 = call i64 @getCurNs(), !dbg !16
  %23 = load i64, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 27), align 8, !dbg !17
  %24 = call i32 (ptr, ...) @printf(ptr @str1, i64 %23), !dbg !18
  %25 = call double @flow_cost(ptr @net), !dbg !19
  %26 = call i32 (ptr, ...) @printf(ptr @str2, double %25), !dbg !20
  %27 = sub i64 %22, %18, !dbg !21
  %28 = sitofp i64 %27 to double, !dbg !22
  %29 = fdiv double %28, 1.000000e+03, !dbg !23
  %30 = call i32 (ptr, ...) @printf(ptr @str3, double %29), !dbg !24
  %31 = icmp ne i64 %16, 0, !dbg !25
  br i1 %31, label %32, label %67, !dbg !26

32:                                               ; preds = %12
  %33 = load i64, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 7), align 8, !dbg !27
  %34 = icmp ne i64 %33, 0, !dbg !28
  br i1 %34, label %35, label %46, !dbg !29

35:                                               ; preds = %32
  %36 = call i64 @suspend_impl(ptr @net, i64 -1, i64 0), !dbg !30
  %37 = call i64 @getCurNs(), !dbg !31
  %38 = icmp ne i64 %36, 0, !dbg !32
  br i1 %38, label %39, label %45, !dbg !33

39:                                               ; preds = %35
  %40 = call i32 (ptr, ...) @printf(ptr @str4, i64 %36), !dbg !34
  %41 = sub i64 %37, %22, !dbg !35
  %42 = sitofp i64 %41 to double, !dbg !36
  %43 = fdiv double %42, 1.000000e+03, !dbg !37
  %44 = call i32 (ptr, ...) @printf(ptr @str5, double %43), !dbg !38
  br label %45, !dbg !39

45:                                               ; preds = %39, %35
  br label %47, !dbg !40

46:                                               ; preds = %32
  br label %47, !dbg !41

47:                                               ; preds = %45, %46
  %48 = phi i64 [ %15, %46 ], [ %37, %45 ]
  br label %49, !dbg !42

49:                                               ; preds = %47
  %50 = call i64 @getCurNs(), !dbg !43
  %51 = call i64 @price_out_impl(ptr @net), !dbg !44
  %52 = call i64 @getCurNs(), !dbg !45
  %53 = icmp ne i64 %51, 0, !dbg !46
  br i1 %53, label %54, label %60, !dbg !47

54:                                               ; preds = %49
  %55 = call i32 (ptr, ...) @printf(ptr @str6, i64 %51), !dbg !48
  %56 = sub i64 %52, %50, !dbg !49
  %57 = sitofp i64 %56 to double, !dbg !50
  %58 = fdiv double %57, 1.000000e+03, !dbg !51
  %59 = call i32 (ptr, ...) @printf(ptr @str7, double %58), !dbg !52
  br label %60, !dbg !53

60:                                               ; preds = %54, %49
  %61 = icmp slt i64 %51, 0, !dbg !54
  br i1 %61, label %62, label %64, !dbg !55

62:                                               ; preds = %60
  %63 = call i32 (ptr, ...) @printf(ptr @str8), !dbg !56
  call void @exit(i32 -1), !dbg !57
  br label %64, !dbg !58

64:                                               ; preds = %62, %60
  %65 = call i64 @getCurNs(), !dbg !59
  %66 = add i64 %16, -1, !dbg !60
  br label %68, !dbg !61

67:                                               ; preds = %12
  br label %68, !dbg !62

68:                                               ; preds = %64, %67
  %69 = phi i64 [ %15, %67 ], [ %48, %64 ]
  %70 = phi i64 [ %14, %67 ], [ %50, %64 ]
  %71 = phi i64 [ %17, %67 ], [ %51, %64 ]
  %72 = phi i64 [ %13, %67 ], [ %52, %64 ]
  %73 = phi i64 [ %16, %67 ], [ %66, %64 ]
  br label %74, !dbg !63

74:                                               ; preds = %68
  br label %1, !dbg !64

75:                                               ; preds = %1
  %76 = load i64, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 29), align 8, !dbg !65
  %77 = call i32 (ptr, ...) @printf(ptr @str9, i64 %76), !dbg !66
  ret i64 0, !dbg !67
}

define internal i64 @getCurNs() !dbg !68 {
  %1 = alloca %struct.timespec, i64 1, align 8, !dbg !69
  %2 = call i32 @clock_gettime(i32 0, ptr %1), !dbg !71
  %3 = getelementptr %struct.timespec, ptr %1, i32 0, i32 0, !dbg !72
  %4 = load i64, ptr %3, align 8, !dbg !73
  %5 = mul i64 %4, 1000, !dbg !74
  %6 = mul i64 %5, 1000, !dbg !75
  %7 = mul i64 %6, 1000, !dbg !76
  %8 = getelementptr %struct.timespec, ptr %1, i32 0, i32 1, !dbg !77
  %9 = load i64, ptr %8, align 8, !dbg !78
  %10 = add i64 %7, %9, !dbg !79
  ret i64 %10, !dbg !80
}

declare i64 @primal_net_simplex(ptr)

declare double @flow_cost(ptr)

declare i64 @suspend_impl(ptr, i64, i64)

declare i64 @price_out_impl(ptr)

declare void @exit(i32)

define i32 @main(i32 %0, ptr %1) !dbg !81 {
  call void @init_client(), !dbg !82
  call void @cache_init(), !dbg !84
  call void @channel_init(), !dbg !85
  %3 = icmp slt i32 %0, 2, !dbg !86
  br i1 %3, label %4, label %5, !dbg !87

4:                                                ; preds = %2
  br label %36, !dbg !88

5:                                                ; preds = %2
  %6 = call i32 (ptr, ...) @printf(ptr @str10), !dbg !89
  %7 = call i32 (ptr, ...) @printf(ptr @str11), !dbg !90
  %8 = call i32 (ptr, ...) @printf(ptr @str12), !dbg !91
  %9 = call i32 (ptr, ...) @printf(ptr @str13), !dbg !92
  %10 = call i32 (ptr, ...) @printf(ptr @str14), !dbg !93
  call void @llvm.memset.p0.i64(ptr @net, i8 0, i64 624, i1 false), !dbg !94
  store i64 10000000, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 18), align 8, !dbg !95
  %11 = getelementptr ptr, ptr %1, i32 1, !dbg !96
  %12 = load ptr, ptr %11, align 8, !dbg !97
  %13 = call ptr @strcpy(ptr @net, ptr %12), !dbg !98
  %14 = call i64 @read_min(ptr @net), !dbg !99
  %15 = icmp ne i64 %14, 0, !dbg !100
  br i1 %15, label %16, label %19, !dbg !101

16:                                               ; preds = %5
  %17 = call i32 (ptr, ...) @printf(ptr @str15), !dbg !102
  %18 = call i64 @getfree(ptr @net), !dbg !103
  br label %32, !dbg !104

19:                                               ; preds = %5
  %20 = load i64, ptr getelementptr inbounds (%_Lowered_network, ptr @net, i32 0, i32 3), align 8, !dbg !105
  %21 = call i32 (ptr, ...) @printf(ptr @str16, i64 %20), !dbg !106
  %22 = call i64 @primal_start_artificial(ptr @net), !dbg !107
  %23 = call i64 @global_opt(), !dbg !108
  %24 = call i32 (ptr, ...) @printf(ptr @str17), !dbg !109
  %25 = call i64 @write_circulations(ptr @str18, ptr @net), !dbg !110
  %26 = icmp ne i64 %25, 0, !dbg !111
  %27 = icmp eq i64 %25, 0, !dbg !112
  %28 = select i1 %26, i32 -1, i32 undef, !dbg !113
  br i1 %26, label %29, label %31, !dbg !114

29:                                               ; preds = %19
  %30 = call i64 @getfree(ptr @net), !dbg !115
  br label %31, !dbg !116

31:                                               ; preds = %29, %19
  br label %32, !dbg !117

32:                                               ; preds = %16, %31
  %33 = phi i1 [ %27, %31 ], [ false, %16 ]
  %34 = phi i32 [ %28, %31 ], [ -1, %16 ]
  br label %35, !dbg !118

35:                                               ; preds = %32
  br label %36, !dbg !119

36:                                               ; preds = %4, %35
  %37 = phi i1 [ %33, %35 ], [ false, %4 ]
  %38 = phi i32 [ %34, %35 ], [ -1, %4 ]
  br label %39, !dbg !120

39:                                               ; preds = %36
  %40 = select i1 %37, i32 0, i32 %38, !dbg !121
  br i1 %37, label %41, label %43, !dbg !122

41:                                               ; preds = %39
  %42 = call i64 @getfree(ptr @net), !dbg !123
  br label %43, !dbg !124

43:                                               ; preds = %41, %39
  ret i32 %40, !dbg !125
}

declare i64 @read_min(ptr)

declare i64 @getfree(ptr)

declare i64 @primal_start_artificial(ptr)

declare i64 @write_circulations(ptr, ptr)

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #0

attributes #0 = { argmemonly nocallback nofree nounwind willreturn writeonly }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "global_opt", linkageName: "global_opt", scope: null, file: !4, line: 31, type: !5, scopeLine: 31, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "obj/mcf.o_llvm.mlir", directory: "/users/Zijian/raw_eth_pktgen/429.mcf/src/obj/anno")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 50, column: 5, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 52, column: 11, scope: !8)
!10 = !DILocation(line: 53, column: 11, scope: !8)
!11 = !DILocation(line: 54, column: 5, scope: !8)
!12 = !DILocation(line: 56, column: 11, scope: !8)
!13 = !DILocation(line: 57, column: 11, scope: !8)
!14 = !DILocation(line: 58, column: 11, scope: !8)
!15 = !DILocation(line: 59, column: 11, scope: !8)
!16 = !DILocation(line: 60, column: 11, scope: !8)
!17 = !DILocation(line: 61, column: 11, scope: !8)
!18 = !DILocation(line: 62, column: 11, scope: !8)
!19 = !DILocation(line: 63, column: 11, scope: !8)
!20 = !DILocation(line: 64, column: 11, scope: !8)
!21 = !DILocation(line: 65, column: 11, scope: !8)
!22 = !DILocation(line: 66, column: 11, scope: !8)
!23 = !DILocation(line: 67, column: 11, scope: !8)
!24 = !DILocation(line: 68, column: 11, scope: !8)
!25 = !DILocation(line: 69, column: 11, scope: !8)
!26 = !DILocation(line: 70, column: 5, scope: !8)
!27 = !DILocation(line: 73, column: 11, scope: !8)
!28 = !DILocation(line: 74, column: 11, scope: !8)
!29 = !DILocation(line: 75, column: 5, scope: !8)
!30 = !DILocation(line: 77, column: 11, scope: !8)
!31 = !DILocation(line: 78, column: 11, scope: !8)
!32 = !DILocation(line: 79, column: 11, scope: !8)
!33 = !DILocation(line: 80, column: 5, scope: !8)
!34 = !DILocation(line: 84, column: 11, scope: !8)
!35 = !DILocation(line: 87, column: 11, scope: !8)
!36 = !DILocation(line: 88, column: 11, scope: !8)
!37 = !DILocation(line: 89, column: 11, scope: !8)
!38 = !DILocation(line: 90, column: 11, scope: !8)
!39 = !DILocation(line: 91, column: 5, scope: !8)
!40 = !DILocation(line: 93, column: 5, scope: !8)
!41 = !DILocation(line: 95, column: 5, scope: !8)
!42 = !DILocation(line: 97, column: 5, scope: !8)
!43 = !DILocation(line: 99, column: 11, scope: !8)
!44 = !DILocation(line: 100, column: 11, scope: !8)
!45 = !DILocation(line: 101, column: 11, scope: !8)
!46 = !DILocation(line: 102, column: 11, scope: !8)
!47 = !DILocation(line: 103, column: 5, scope: !8)
!48 = !DILocation(line: 107, column: 11, scope: !8)
!49 = !DILocation(line: 110, column: 11, scope: !8)
!50 = !DILocation(line: 111, column: 11, scope: !8)
!51 = !DILocation(line: 112, column: 11, scope: !8)
!52 = !DILocation(line: 113, column: 11, scope: !8)
!53 = !DILocation(line: 114, column: 5, scope: !8)
!54 = !DILocation(line: 116, column: 11, scope: !8)
!55 = !DILocation(line: 117, column: 5, scope: !8)
!56 = !DILocation(line: 121, column: 11, scope: !8)
!57 = !DILocation(line: 122, column: 5, scope: !8)
!58 = !DILocation(line: 123, column: 5, scope: !8)
!59 = !DILocation(line: 125, column: 11, scope: !8)
!60 = !DILocation(line: 126, column: 11, scope: !8)
!61 = !DILocation(line: 127, column: 5, scope: !8)
!62 = !DILocation(line: 129, column: 5, scope: !8)
!63 = !DILocation(line: 131, column: 5, scope: !8)
!64 = !DILocation(line: 133, column: 5, scope: !8)
!65 = !DILocation(line: 138, column: 11, scope: !8)
!66 = !DILocation(line: 139, column: 11, scope: !8)
!67 = !DILocation(line: 140, column: 5, scope: !8)
!68 = distinct !DISubprogram(name: "getCurNs", linkageName: "getCurNs", scope: null, file: !4, line: 142, type: !5, scopeLine: 142, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!69 = !DILocation(line: 146, column: 10, scope: !70)
!70 = !DILexicalBlockFile(scope: !68, file: !4, discriminator: 0)
!71 = !DILocation(line: 147, column: 10, scope: !70)
!72 = !DILocation(line: 148, column: 10, scope: !70)
!73 = !DILocation(line: 149, column: 10, scope: !70)
!74 = !DILocation(line: 150, column: 10, scope: !70)
!75 = !DILocation(line: 151, column: 10, scope: !70)
!76 = !DILocation(line: 152, column: 10, scope: !70)
!77 = !DILocation(line: 153, column: 11, scope: !70)
!78 = !DILocation(line: 154, column: 11, scope: !70)
!79 = !DILocation(line: 155, column: 11, scope: !70)
!80 = !DILocation(line: 156, column: 5, scope: !70)
!81 = distinct !DISubprogram(name: "main", linkageName: "main", scope: null, file: !4, line: 163, type: !5, scopeLine: 163, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!82 = !DILocation(line: 172, column: 5, scope: !83)
!83 = !DILexicalBlockFile(scope: !81, file: !4, discriminator: 0)
!84 = !DILocation(line: 173, column: 5, scope: !83)
!85 = !DILocation(line: 174, column: 5, scope: !83)
!86 = !DILocation(line: 176, column: 10, scope: !83)
!87 = !DILocation(line: 177, column: 5, scope: !83)
!88 = !DILocation(line: 179, column: 5, scope: !83)
!89 = !DILocation(line: 183, column: 11, scope: !83)
!90 = !DILocation(line: 186, column: 11, scope: !83)
!91 = !DILocation(line: 189, column: 11, scope: !83)
!92 = !DILocation(line: 192, column: 11, scope: !83)
!93 = !DILocation(line: 195, column: 11, scope: !83)
!94 = !DILocation(line: 198, column: 5, scope: !83)
!95 = !DILocation(line: 200, column: 5, scope: !83)
!96 = !DILocation(line: 203, column: 11, scope: !83)
!97 = !DILocation(line: 204, column: 11, scope: !83)
!98 = !DILocation(line: 205, column: 11, scope: !83)
!99 = !DILocation(line: 206, column: 11, scope: !83)
!100 = !DILocation(line: 207, column: 11, scope: !83)
!101 = !DILocation(line: 208, column: 5, scope: !83)
!102 = !DILocation(line: 212, column: 11, scope: !83)
!103 = !DILocation(line: 213, column: 11, scope: !83)
!104 = !DILocation(line: 214, column: 5, scope: !83)
!105 = !DILocation(line: 219, column: 11, scope: !83)
!106 = !DILocation(line: 220, column: 11, scope: !83)
!107 = !DILocation(line: 221, column: 11, scope: !83)
!108 = !DILocation(line: 222, column: 11, scope: !83)
!109 = !DILocation(line: 225, column: 11, scope: !83)
!110 = !DILocation(line: 228, column: 11, scope: !83)
!111 = !DILocation(line: 229, column: 11, scope: !83)
!112 = !DILocation(line: 230, column: 11, scope: !83)
!113 = !DILocation(line: 231, column: 11, scope: !83)
!114 = !DILocation(line: 232, column: 5, scope: !83)
!115 = !DILocation(line: 234, column: 11, scope: !83)
!116 = !DILocation(line: 235, column: 5, scope: !83)
!117 = !DILocation(line: 237, column: 5, scope: !83)
!118 = !DILocation(line: 239, column: 5, scope: !83)
!119 = !DILocation(line: 241, column: 5, scope: !83)
!120 = !DILocation(line: 243, column: 5, scope: !83)
!121 = !DILocation(line: 245, column: 11, scope: !83)
!122 = !DILocation(line: 246, column: 5, scope: !83)
!123 = !DILocation(line: 249, column: 11, scope: !83)
!124 = !DILocation(line: 250, column: 5, scope: !83)
!125 = !DILocation(line: 252, column: 5, scope: !83)
