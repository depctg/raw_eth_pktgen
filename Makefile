OBJECTS := common.o packet.o net.o cache.o
TARGETS := bench_arrsum/bench_send_arrsum bench_arrsum/bench_recv_arrsum

LD       := g++
LIBS     := -libverbs -lnng
CFLAGS   := -O3 -g -flto -std=c11 -Wall -D_POSIX_C_SOURCE=199309L
CXXFLAGS  := -O3 -g -flto -std=c++11 -Wall -D_POSIX_C_SOURCE=199309L
LD_FLAGS := -flto

.PHONY: all clean

all: $(TARGETS) 

gen_%: gen_%.o $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

ary_seq_opts/%.o : ary_seq_opts/%.cpp
	$(CXX) $(LD_FLAGS) -c -o $@ $^
ary_seq_opts/% : ary_seq_opts/%.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)

prefetch_batch : ary_seq_opts/general_recv ary_seq_opts/batch_fetch ary_seq_opts/pre_fetch ary_seq_opts/comp_fetch ary_seq_opts/async_fetch
clean_ary_seq: 
	sudo rm ary_seq_opts/general_recv ary_seq_opts/batch_fetch ary_seq_opts/pre_fetch ary_seq_opts/comp_fetch ary_seq_opts/async_fetch
# batch_fetch.o : cpp_ary_seq/batch_fetch.cpp
# 	$(CXX) $(LD_FLAGS) -c -o $@ $^
# batch_fetch: $(OBJECTS) batch_fetch.o
# 	$(CXX) $(LD_FLAGS) -o $@ $^ $(LIBS)

test_arrsum: bench_arrsum/bench_send_arrsum bench_arrsum/bench_recv_arrsum 
	echo "Benchmark array sequantial sum"

bench_arrsum/%.o: bench_arrsum/%.cpp
	$(CXX) $(LD_FLAGS) -c -o $@ $^
bench_arrsum/%: bench_arrsum/%.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)

cachesR: cache_Rstem/cache_test cache_Rstem/remote_test
	echo "Cache tests"
cache_Rstem/%.o: cache_Rstem/%.cpp
	$(CXX) $(LD_FLAGS) -c -o $@ $^
cache_Rstem/%: cache_Rstem/%.o $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)
clean_cacheR: 
	sudo rm cache_Rstem/cache_test cache_Rstem/remote_test

AIFMCOMP_seq: AIFM_COMP_seq/ary_sum_local AIFM_COMP_seq/keepC_local AIFM_COMP_seq/cache_remote
	echo "Compare AIFM"
AIFM_COMP_seq/%.o: AIFM_COMP_seq/%.cpp
	$(CXX) $(LD_FLAGS) -c -o $@ $^
AIFM_COMP_seq/%: AIFM_COMP_seq/%.o $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)

AIFMCOMP_rand: AIFM_COMP_rand/rand_local AIFM_COMP_rand/cache_remote AIFM_COMP_rand/keepC_local
	echo "Compare AIFM"
AIFM_COMP_rand/%.o: AIFM_COMP_rand/%.cpp
	$(CXX) $(LD_FLAGS) -c -o $@ $^
AIFM_COMP_rand/%: AIFM_COMP_rand/%.o $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)

clean_AIFMCOMP: 
	sudo rm AIFM_COMP_seq/ary_sum_local AIFM_COMP_seq/keepC_local AIFM_COMP_seq/cache_remote AIFM_COMP_rand/rand_local AIFM_COMP_rand/cache_remote AIFM_COMP_rand/keepC_local

clean:
	rm *.o $(TARGETS)



