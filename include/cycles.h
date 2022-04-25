#ifndef _CYCLES_H_
#define _CYCLES_H_
// rdtsc
static inline unsigned long long get_cycles()
{
	unsigned low, high;
	unsigned long long val;
	asm volatile ("rdtsc" : "=a" (low), "=d" (high));
	val = high;
	val = (val << 32) | low;
	return val;
}

static inline void wait_until_cycles(unsigned long long c) {
    for (;;) if (get_cycles() >= c) break; 
}
#endif
