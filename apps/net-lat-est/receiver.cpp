#include <iostream>
#include <chrono>
#include "common.h"
#include "greeting.h"
#include <memory>

constexpr static uint64_t packet_size = 4194304;
using namespace std;

int main(int argc, char **argv)
{
  init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing reqs"<< endl;

  while (1)
  {
    recv(rbuf, packet_size);
    send(sbuf, packet_size);
  }
}