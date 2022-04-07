#ifndef _APP_H_
#define _APP_H_

#define ARRAY_SIZE (1024 * 1024 * 4)
#define BATCH_SIZE (256)
#define PRE_STRIDE (4)

struct req {
    int index;
    int size;
};

#endif 
