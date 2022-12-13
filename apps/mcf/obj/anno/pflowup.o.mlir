module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic", rmem.type_rule = {
  struct.node = !llvm.struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>,

  struct.arc = !llvm.struct<"rarc", (i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>,

  struct.network = !llvm.struct<"network", (array<200 x i8>, array<200 x i8>, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, f64, i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i64)>,

  struct.basket = !llvm.struct<"basket", (!rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc", (i64, ptr<!llvm.struct<"node", (i64, i32, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, i64, i64, i32, i32)>>, ptr<!llvm.struct<"node", (i64, i32, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, ptr<!llvm.struct<"node">>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, i64, i64, i32, i32)>>, i32, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<!llvm.struct<"rarc">>>, i64, i64)>>>, i64, i64)>
}} {
  func.func @primal_update_flow(%arg0: !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>, %arg1: !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>, %arg2: !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>)  attributes {
    llvm.linkage = #llvm.linkage<external>,
    operand_types = [!llvm.ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>>, !llvm.ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>>, !llvm.ptr<struct<"node", (i64, i32, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, ptr<struct<"node">>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc", (i64, ptr<struct<"node">>, ptr<struct<"node">>, i32, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, !rmem.rmref<1, !llvm.ptr<struct<"rarc">>>, i64, i64)>>>, i64, i64, i32, i32)>>],
    remote_target = 1
} {
    %c0_i64 = arith.constant 0 : i64
    %c0_i32 = arith.constant 0 : i32
    %c1_i64 = arith.constant 1 : i64
    %0 = scf.while (%arg3 = %arg0) : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>> {
      %2 = llvm.icmp "ne" %arg3, %arg2 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
      scf.condition(%2) %arg3 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
    } do {
    ^bb0(%arg3: !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>):
      %2 = llvm.getelementptr %arg3[0, 1] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i32>
      %3 = llvm.load %2 : !llvm.ptr<i32>
      %4 = arith.cmpi ne, %3, %c0_i32 : i32
      scf.if %4 {
        %7 = llvm.getelementptr %arg3[0, 10] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i64>
        llvm.store %c0_i64, %7 : !llvm.ptr<i64>
      } else {
        %7 = llvm.getelementptr %arg3[0, 10] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i64>
        llvm.store %c1_i64, %7 : !llvm.ptr<i64>
      }
      %5 = llvm.getelementptr %arg3[0, 3] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>>
      %6 = llvm.load %5 : !llvm.ptr<ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>>
      scf.yield %6 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
    }
    %1 = scf.while (%arg3 = %arg1) : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>> {
      %2 = llvm.icmp "ne" %arg3, %arg2 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
      scf.condition(%2) %arg3 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
    } do {
    ^bb0(%arg3: !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>):
      %2 = llvm.getelementptr %arg3[0, 1] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i32>
      %3 = llvm.load %2 : !llvm.ptr<i32>
      %4 = arith.cmpi ne, %3, %c0_i32 : i32
      scf.if %4 {
        %7 = llvm.getelementptr %arg3[0, 10] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i64>
        llvm.store %c1_i64, %7 : !llvm.ptr<i64>
      } else {
        %7 = llvm.getelementptr %arg3[0, 10] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<i64>
        llvm.store %c0_i64, %7 : !llvm.ptr<i64>
      }
      %5 = llvm.getelementptr %arg3[0, 3] : (!llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>) -> !llvm.ptr<ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>>
      %6 = llvm.load %5 : !llvm.ptr<ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>>
      scf.yield %6 : !llvm.ptr<struct<"struct.node", (i64, i32, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, ptr<struct<"struct.arc", (i64, ptr<struct<"struct.node">>, ptr<struct<"struct.node">>, i32, ptr<struct<"struct.arc">>, ptr<struct<"struct.arc">>, i64, i64)>>, i64, i64, i32, i32)>>
    }
    return
  }
}