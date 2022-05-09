#ifndef _APP_H_
#define _APP_H_

#define ARRAY_SIZE (1024 * 1024)
#define BATCH_SIZE (256)
#define PRE_STRIDE (2)

struct req {
    int index;
    int size;

    unsigned long long cycles_to_sleep;
};

#endif 
