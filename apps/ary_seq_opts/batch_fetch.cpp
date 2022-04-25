#include "../common.h"
#include "../greeting.h"
#include <chrono>
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);

	// batch optimization
    int batch_size = sizeof(int) * BATCH_SIZE;
	struct req *reqs = (struct req *)sbuf;

    uint64_t sum = 0;
    // Send first request
    int buf_ofst = 0;
	reqs[0].index = 0;
	reqs[0].size = batch_size;
    auto start = chrono::steady_clock::now();
	for (int i = 0; i < ARRAY_SIZE; i += BATCH_SIZE) {

	    // fetch batch
        send(reqs, sizeof(struct req));
        recv((int*)rbuf + buf_ofst*BATCH_SIZE, batch_size);
        int * arr = (int *)rbuf + buf_ofst * BATCH_SIZE;

	    // get data from buffer
        for (int j = 0; j < BATCH_SIZE; j++)
            sum += *arr++;
        buf_ofst ++;
        reqs[0].index = buf_ofst * batch_size;
        reqs[0].size = batch_size;
	}

    auto end = chrono::steady_clock::now();
    std::cout << "SUM " << sum 
              << ", ns: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << std::endl;
    return 0;
}

