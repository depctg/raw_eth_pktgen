#include <iostream>
#include "../common.h"
#include "../greeting.h"

using namespace std;


// data resides in sbuf for non-copy
// [tag] -> content

void app_init()
{
    
}

int main(int argc, char * argv[]) 
{
	init(TRANS_TYPE_RC_SERVER, argv[1]);
  cout << "Start processing requests" << endl;

  recv(rbuf, sizeof(struct req));
  struct req *r = (struct req *) rbuf;
  cout << r->addr << " " << r->size << " " << r->type << endl;
}
