OBJECTS := common.o packet.o
TARGETS := gen_send gen_recv

LD       := gcc
LIBS     := -libverbs
CFLAGS   := -O2 -g -std=c11 -Wall

.PHONY: all clean

all: $(TARGETS)


gen_%: gen_%.o $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm *.o $(TARGETS)



