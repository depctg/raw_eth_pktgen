#include "patterns.h"

#include <stdio.h>

int main() {
    int n = 100;
    // vector<size_t> a = gen_access_pattern_uniform(n, 100, 5, 321);
    vector<size_t> a = gen_access_pattern_normal(n, 100, 5, 512, 5.0, 321);
    for (int i = 0; i < n; ++i) {
        printf("%lu ", a[i]);
    }
    printf("\n");
    return 0;
}