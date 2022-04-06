#ifndef _APP_H_
#define _APP_H_

#define ARRAY_SIZE (1024 * 1024 * 64)
#define BATCH_SIZE (128 * 4)
#define PRE_STRIDE (8)

struct req {
    int index;
    int size;
};

#endif 
