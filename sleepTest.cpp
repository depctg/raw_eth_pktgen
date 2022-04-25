#include <iostream>
#include <thread>
#include <chrono>
#include "sleepus.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
  auto start = chrono::steady_clock::now();
  // this_thread::sleep_for(chrono::microseconds(1));
  // wait_until_cycles(40 * CPU_F);
  wait_until_us(3);
  auto end = chrono::steady_clock::now();
  cout << "ms: " << chrono::duration_cast<chrono::microseconds>(end-start).count() << endl;
}