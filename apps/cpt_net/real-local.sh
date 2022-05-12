batch_size=$((16 * 2 ** 20))
iter_ahead=1
sed "s/constexpr static int iter_ahead = .*/constexpr static int iter_ahead = $iter_ahead;/g" -i local.cpp
sed "s/constexpr static uint64_t batch_size = .*/constexpr static uint64_t batch_size = $batch_size;/g" -i local.cpp
cd ../../build/
make cpt_net
cd bin/
# ./cpt_net-local tcp://128.110.219.11:2333 | sudo tee log.$batch_size.$iter_ahead
./cpt_net-local tcp://128.110.219.11:2333 