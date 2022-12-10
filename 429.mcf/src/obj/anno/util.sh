function search_remote_lower() {
  pldisagg --search-rmem $1.o.mlir -o search/$1.search.mlir 
  pldisagg --convert-target-to-remote search/$1.search.mlir -o remote/$1.remote.mlir 
  pldisagg --lower-rmem-to-llvm remote/$1.remote.mlir -o lower/$1.lower.mlir
}