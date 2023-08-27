module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  llvm.func @printf(!llvm.ptr<i8>, ...) -> i32
  llvm.mlir.global internal constant @str0("%d\0A\00") {addr_space = 0 : i32}
  llvm.func @main() -> i32 {
    %0 = llvm.mlir.constant(6 : i32) : i32
    %1 = llvm.mlir.constant(5 : i32) : i32
    %2 = llvm.mlir.constant(4 : i32) : i32
    %3 = llvm.mlir.constant(3 : i32) : i32
    %4 = llvm.mlir.constant(2 : i32) : i32
    %5 = llvm.mlir.constant(1 : i32) : i32
    %6 = llvm.mlir.constant(0 : i32) : i32
    %7 = llvm.mlir.constant(1 : i64) : i64
    %8 = llvm.alloca %7 x !llvm.array<5 x i32> : (i64) -> !llvm.ptr<array<5 x i32>>
    %9 = llvm.alloca %7 x !llvm.array<5 x i32> : (i64) -> !llvm.ptr<array<5 x i32>>
    %10 = llvm.getelementptr %9[0, 0] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %5, %10 : !llvm.ptr<i32>
    %11 = llvm.getelementptr %9[0, 1] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %4, %11 : !llvm.ptr<i32>
    %12 = llvm.getelementptr %9[0, 2] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %3, %12 : !llvm.ptr<i32>
    %13 = llvm.getelementptr %9[0, 3] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %2, %13 : !llvm.ptr<i32>
    %14 = llvm.getelementptr %9[0, 4] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %1, %14 : !llvm.ptr<i32>
    %15 = llvm.getelementptr %8[0, 0] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %4, %15 : !llvm.ptr<i32>
    %16 = llvm.getelementptr %8[0, 1] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %3, %16 : !llvm.ptr<i32>
    %17 = llvm.getelementptr %8[0, 2] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %2, %17 : !llvm.ptr<i32>
    %18 = llvm.getelementptr %8[0, 3] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %1, %18 : !llvm.ptr<i32>
    %19 = llvm.getelementptr %8[0, 4] : (!llvm.ptr<array<5 x i32>>) -> !llvm.ptr<i32>
    llvm.store %0, %19 : !llvm.ptr<i32>
    llvm.call @driver(%10, %15) : (!llvm.ptr<i32>, !llvm.ptr<i32>) -> ()
    llvm.return %6 : i32
  }
  llvm.func @driver(%arg0: !llvm.ptr<i32>, %arg1: !llvm.ptr<i32>) {
    %0 = llvm.mlir.constant(5 : index) : i64
    %1 = llvm.mlir.constant(0 : index) : i64
    %2 = llvm.mlir.constant(1 : index) : i64
    %3 = llvm.mlir.constant(1 : i64) : i64
    %4 = llvm.alloca %3 x !llvm.array<5 x i64> : (i64) -> !llvm.ptr<array<5 x i64>>
    %5 = llvm.alloca %3 x !llvm.array<5 x struct<"struct.Pack", (ptr<i32>, ptr<i32>)>> : (i64) -> !llvm.ptr<array<5 x struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>>
    %6 = llvm.getelementptr %5[0, 0] : (!llvm.ptr<array<5 x struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>>) -> !llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>
    %7 = llvm.getelementptr %4[0, 0] : (!llvm.ptr<array<5 x i64>>) -> !llvm.ptr<i64>
    %8 = llvm.mlir.null : !llvm.ptr<struct<"union.pthread_attr_t", (i64, array<48 x i8>)>>
    %9 = llvm.mlir.addressof @task : !llvm.ptr<func<ptr<i8> (ptr<i8>)>>
    llvm.br ^bb1(%1 : i64)
  ^bb1(%10: i64):  // 2 preds: ^bb0, ^bb2
    %11 = llvm.icmp "slt" %10, %0 : i64
    llvm.cond_br %11, ^bb2, ^bb3
  ^bb2:  // pred: ^bb1
    %12 = llvm.trunc %10 : i64 to i32
    %13 = llvm.getelementptr %6[%10] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>, i64) -> !llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>
    %14 = llvm.getelementptr %13[0, 0] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>) -> !llvm.ptr<ptr<i32>>
    %15 = llvm.getelementptr %arg0[%12] : (!llvm.ptr<i32>, i32) -> !llvm.ptr<i32>
    llvm.store %15, %14 : !llvm.ptr<ptr<i32>>
    %16 = llvm.getelementptr %13[0, 1] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>) -> !llvm.ptr<ptr<i32>>
    %17 = llvm.getelementptr %arg1[%12] : (!llvm.ptr<i32>, i32) -> !llvm.ptr<i32>
    llvm.store %17, %16 : !llvm.ptr<ptr<i32>>
    %18 = llvm.getelementptr %7[%12] : (!llvm.ptr<i64>, i32) -> !llvm.ptr<i64>
    %19 = llvm.getelementptr %6[%12] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>, i32) -> !llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>
    %20 = llvm.bitcast %19 : !llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>> to !llvm.ptr<i8>
    %21 = llvm.call @pthread_create(%18, %8, %9, %20) : (!llvm.ptr<i64>, !llvm.ptr<struct<"union.pthread_attr_t", (i64, array<48 x i8>)>>, !llvm.ptr<func<ptr<i8> (ptr<i8>)>>, !llvm.ptr<i8>) -> i32
    %22 = llvm.add %10, %2  : i64
    llvm.br ^bb1(%22 : i64)
  ^bb3:  // pred: ^bb1
    %23 = llvm.mlir.null : !llvm.ptr<ptr<i8>>
    llvm.br ^bb4(%1 : i64)
  ^bb4(%24: i64):  // 2 preds: ^bb3, ^bb5
    %25 = llvm.icmp "slt" %24, %0 : i64
    llvm.cond_br %25, ^bb5, ^bb6
  ^bb5:  // pred: ^bb4
    %26 = llvm.getelementptr %7[%24] : (!llvm.ptr<i64>, i64) -> !llvm.ptr<i64>
    %27 = llvm.load %26 : !llvm.ptr<i64>
    %28 = llvm.call @pthread_join(%27, %23) : (i64, !llvm.ptr<ptr<i8>>) -> i32
    %29 = llvm.add %24, %2  : i64
    llvm.br ^bb4(%29 : i64)
  ^bb6:  // pred: ^bb4
    llvm.return
  }
  llvm.func @pthread_create(!llvm.ptr<i64>, !llvm.ptr<struct<"union.pthread_attr_t", (i64, array<48 x i8>)>>, !llvm.ptr<func<ptr<i8> (ptr<i8>)>>, !llvm.ptr<i8>) -> i32 attributes {sym_visibility = "private"}
  llvm.func @task(%arg0: !llvm.ptr<i8>) -> !llvm.ptr<i8> {
    %0 = llvm.bitcast %arg0 : !llvm.ptr<i8> to !llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>
    %1 = llvm.getelementptr %0[0, 0] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>) -> !llvm.ptr<ptr<i32>>
    %2 = llvm.load %1 : !llvm.ptr<ptr<i32>>
    %3 = llvm.getelementptr %0[0, 1] : (!llvm.ptr<struct<"struct.Pack", (ptr<i32>, ptr<i32>)>>) -> !llvm.ptr<ptr<i32>>
    %4 = llvm.load %3 : !llvm.ptr<ptr<i32>>
    llvm.call @foo(%2, %4) : (!llvm.ptr<i32>, !llvm.ptr<i32>) -> ()
    %5 = llvm.mlir.null : !llvm.ptr<i8>
    llvm.return %5 : !llvm.ptr<i8>
  }
  llvm.func @pthread_join(i64, !llvm.ptr<ptr<i8>>) -> i32 attributes {sym_visibility = "private"}
  llvm.func @foo(%arg0: !llvm.ptr<i32>, %arg1: !llvm.ptr<i32>) {
    %0 = llvm.mlir.addressof @str0 : !llvm.ptr<array<4 x i8>>
    %1 = llvm.getelementptr %0[0, 0] : (!llvm.ptr<array<4 x i8>>) -> !llvm.ptr<i8>
    %2 = llvm.load %arg0 : !llvm.ptr<i32>
    %3 = llvm.load %arg1 : !llvm.ptr<i32>
    %4 = llvm.add %2, %3  : i32
    %5 = llvm.call @printf(%1, %4) : (!llvm.ptr<i8>, i32) -> i32
    llvm.return
  }
}
