OBJECTS := common.o packet.o net.o
TARGETS := gen_send gen_recv gen_simple_send gen_simple_recv gen_latency_send gen_latency_recv

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
	$(CXX) $(LD_FLAGS) -o $@ $^ $(LIBS)

# batch_fetch.o : cpp_ary_seq/batch_fetch.cpp
# 	$(CXX) $(LD_FLAGS) -c -o $@ $^
# batch_fetch: $(OBJECTS) batch_fetch.o
# 	$(CXX) $(LD_FLAGS) -o $@ $^ $(LIBS)

bench_%: bench_%.o $(OBJECTS)
	$(LD) $(CXXFLAGS) $(LD_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm *.o $(TARGETS)



