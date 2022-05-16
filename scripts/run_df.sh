exe_path="./build/bin/"
data_path=$exe_path
port_init=9090
port=$port_init
remote_mem=2147483648

local_mems=(10240 12288 15360 20480)
cache_line=(512 1024 2048)
index_size=(2000 5000)
n_prefetch=(0 1 2 5 8 10 15 20 25 30 35 40 50 70 80 100 120 150)

kill -9 `pgrep -f dataframe-`
for idx_size in "${index_size[@]}"
do
  for local_mem in "${local_mems[@]}"
  do
    for cline in "${cache_line[@]}"
    do
        if [ "$cline" -gt "$local_mem" ]; then
            continue
        fi
        for n_p in "${n_prefetch[@]}"
        do
            ${exe_path}/dataframe-vec_remote -addr "tcp://*:${port}" -cache_size $remote_mem -cache_line_size $cline &>/dev/null &
            to_kill=$!
            sleep 1
            ${exe_path}/dataframe-main -addr "tcp://localhost:${port}" -index_size $idx_size -cache_size $local_mem -cache_line_size $cline -prefetch_n $n_p 2>> "${data_path}/df_${idx_size}_${local_mem}_${cline}.csv"
            ((port=port+1))
            kill -9 $to_kill
        done
        port=$port_init
    done
  done

done

echo "Done"