#ifndef _APP_H_
#define _APP_H_

//#define ARRAY_SIZE (1024)
//#define BATCH_SIZE (64)
//#define PRE_STRIDE (4)

struct req {
    int index;
    int size;

    unsigned long long target_cycles;
};

#endif 
