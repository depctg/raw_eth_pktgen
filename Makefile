OBJECTS := common.o packet.o net.o
TARGETS := gen_send gen_recv gen_simple_send gen_simple_recv

LD       := gcc
LIBS     := -libverbs -lnng
CFLAGS   := -O3 -g -std=c11 -Wall -D_POSIX_C_SOURCE=199309L

.PHONY: all clean

all: $(TARGETS)


gen_%: gen_%.o $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm *.o $(TARGETS)



