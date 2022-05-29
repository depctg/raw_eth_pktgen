exe_path="./build/bin/" # assumes running from project root
data_path="$exe_path/df_run.csv"
port_init=9090
port=$port_init
remote_mem=2147483648

local_mems=(5120 10240 12288 15360 20480)
cache_line=(128 256 512 1024 2048)
index_size=(2000 5000)
n_prefetch=(0 1 2)

kill -9 `pgrep -f dataframe-`
for idx_size in "${index_size[@]}"
do
  for local_mem in "${local_mems[@]}"
  do
    for cline in "${cache_line[@]}"
    do
        for n_p in "${n_prefetch[@]}"
        do
            if [ "$cline" -gt "$local_mem" ]; then
                continue
            fi
            ${exe_path}/dataframe-vec_remote -addr "tcp://*:${port}" -cache_size $remote_mem -cache_line_size $cline &>/dev/null &
            #${exe_path}/dataframe-vec_remote -addr "tcp://*:${port}" -cache_size $remote_mem -cache_line_size $cline &>>"${exe_path}/remote-output" &
            to_kill=$!
            sleep 2
            ${exe_path}/dataframe-main -addr "tcp://localhost:${port}" -index_size $idx_size -cache_size $local_mem -cache_line_size $cline -prefetch_nline $n_p 2>> "${data_path}"
            ((port=port+1))
            kill -9 $to_kill &>/dev/null
        done
        port=$port_init
    done
  done
done

echo "Done"
