#include <string.h>
#include "common.h"
#include "packet.h"
#include <chrono>
#include <iostream>

#include "app.h"

using namespace std;
int main(int argc, char * argv[]) {
	// init(TRANS_TYPE_RC, argv[1]);
    
    // local
    auto start = chrono::steady_clock::now();
    int *a = new int[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        a[i] = i;
    }
    unsigned long long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sum += a[i];
    }
    auto end = chrono::steady_clock::now();
    std::cout << "SUM " << sum 
              << ", ns: " << chrono::duration_cast<chrono::microseconds>(end - start).count()
              << std::endl;
    return 0;
}

