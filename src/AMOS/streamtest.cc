#include "foundation_AMOS.hh"
#include <iostream>
using namespace AMOS;
using namespace std;

int main (int argc, char ** argv)
{
  if ( argc != 2 )
    {
      cerr << "USAGE: " << argv[0] << " <outfile>" << endl;
      exit (-1);
    }

  const uint8_t u8c = 0x01;
  const uint16_t u16c = 0x0123;
  const uint32_t u32c = 0x01234567;
  const uint64_t u64c = 0x0123456789ABCDEFull;

  const int8_t s8c = -u8c;
  const int16_t s16c = -u16c;
  const int32_t s32c = -u32c;
  const int64_t s64c = -u64c;

  cout << "BEFORE." << endl
       << (unsigned long long)u8c << endl
       << (unsigned long long)u16c << endl
       << (unsigned long long)u32c << endl
       << (unsigned long long)u64c << endl
       << (long long)s8c << endl
       << (long long)s16c << endl
       << (long long)s32c << endl
       << (long long)s64c << endl << endl;

  uint8_t u8;
  uint16_t u16;
  uint32_t u32;
  uint64_t u64;

  int8_t s8;
  int16_t s16;
  int32_t s32;
  int64_t s64;

  fstream as;
  as . open (argv[1], ios::out | ios::binary | ios::trunc);
  writeLE (as, &u8c);
  writeLE (as, &u16c);
  writeLE (as, &u32c);
  writeLE (as, &u64c);
  writeLE (as, &s8c);
  writeLE (as, &s16c);
  writeLE (as, &s32c);
  writeLE (as, &s64c);
  as . close( );

  as . open (argv[1], ios::in | ios::binary);
  readLE (as, &u8);
  readLE (as, &u16);
  readLE (as, &u32);
  readLE (as, &u64);
  readLE (as, &s8);
  readLE (as, &s16);
  readLE (as, &s32);
  readLE (as, &s64);
  as . close( );

  cout << "AFTER." << endl
       << (unsigned long long)u8 << endl
       << (unsigned long long)u16 << endl
       << (unsigned long long)u32 << endl
       << (unsigned long long)u64 << endl
       << (long long)s8 << endl
       << (long long)s16 << endl
       << (long long)s32 << endl
       << (long long)s64 << endl << endl;

  if ( u8c == u8 && u16c == u16 && u32c == u32 && u64c == u64 &&
       s8c == s8 && s16c == s16 && s32c == s32 && s64c == s64 )
    {
      cout << "SUCCESS!" << endl;
      return 0;
    }
  else
    {
      cout << "FAILURE!" << endl;
      return -1;
    }
}
