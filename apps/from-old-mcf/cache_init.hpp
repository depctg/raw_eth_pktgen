#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cache.hpp"

const int slots = (3 << 20);
const uint64_t c1_raddr = 0;

using C1 = DirectCache<0,c1_raddr,0,slots,1024,1>;
using C1R = CacheReq<C1>;
