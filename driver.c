#include "cdecl.h"

int PRE_CDECL bmain( void ) POST_CDECL;

int main()
{
  int ret_status;
  ret_status = bmain();
  return ret_status;
}
