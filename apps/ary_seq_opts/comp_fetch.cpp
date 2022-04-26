#include "common.h"
#include "packet.h"
#include "app.h"
#include <chrono>
#include <iostream>
#include <string.h>

using namespace std;
int main(int argc, char * argv[]) {
	init(TRANS_TYPE_RC, argv[1]);
    struct req *reqs = (struct req*) sbuf;
    unsigned long long sum = 0;
    auto start = chrono::steady_clock::now();
    
    // fetch the entire array back
    int ts = sizeof(int) * ARRAY_SIZE;
    reqs[0].index = 0;
    reqs[0].size = ts;
    send(reqs, sizeof(struct req));
    recv(rbuf, ts);
    int *a = (int*) rbuf;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sum += *a++;
    }
    auto end = chrono::steady_clock::now();
    std::cout << "SUM " << sum 
              << ", ns: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << std::endl;
    return 0;
}

