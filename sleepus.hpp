#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

inline void wait_until_us(unsigned d)
{
  auto spin_start = high_resolution_clock::now();
  while (duration_cast<microseconds>(high_resolution_clock::now() - spin_start).count() < d);
}
