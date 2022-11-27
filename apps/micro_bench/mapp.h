#define OBJ_SIZE 64
#define OUTER 1024
#define INNER (1<<20)
#define MEMORY_COMPONENT 64
#define COMPUTE_COMPONENT 30

typedef struct Dat {
  char payload[OBJ_SIZE];
} dat_t;