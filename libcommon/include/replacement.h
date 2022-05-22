#ifndef __REPLACE__
#define __REPLACE__
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "uthash.h"
#include "mem_block.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct LRU {
  Victim *v;
} LRU;


#ifdef __cplusplus
}
#endif

#endif