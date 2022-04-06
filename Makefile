OBJECTS := common.o packet.o net.o
TARGETS := gen_send gen_recv gen_simple_send gen_simple_recv gen_latency_send gen_latency_recv cpp_array_seq/naive_local cpp_array_seq/comp_fetch_local

LD       := gcc
LIBS     := -libverbs -lnng
CFLAGS   := -O3 -g -flto -std=c11 -Wall -D_POSIX_C_SOURCE=199309L
CXXFLAGS  := -O3 -g -flto -std=c++11 -Wall -D_POSIX_C_SOURCE=199309L
LD_FLAGS := -flto

.PHONY: all clean

all: $(TARGETS)


gen_%: gen_%.o $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

cpp_% : cpp_%.o $(OBJECTS)
	$(CXX) $(LD_FLAGS) -o $@ $^ $(LIBS)
bench_%: bench_%.o $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm *.o $(TARGETS)



