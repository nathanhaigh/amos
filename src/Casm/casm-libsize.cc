#include "foundation_AMOS.hh"
#include <cmath>
#include <climits>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

int main (int argc, char ** argv)
{
  if ( argc != 2 )
    {
      cerr << "bad args\n";
      exit(1);
    }

  string bankname = argv[1];

  Index_t mtpx;
  Index_t libx;
  mtpx . buildReadMate (bankname);
  libx . buildReadLibrary (bankname);

  ID_t red;
  Pos_t s, e;
  string line;

  istringstream ss;
  hash_map<ID_t, Pos_t> reads;
  hash_map<ID_t, vector<Size_t> > libs;

  bool good = true;

  while ( cin )
    {
      getline (cin, line);

      if ( ! cin || line . empty() || line[0] == '>' )
        {
          hash_map<ID_t, Pos_t>::iterator i;
          hash_map<ID_t, Pos_t>::iterator j;
          for ( i = reads . begin( ); i != reads . end( ); ++ i )
            {
              if ( i->second == -INT_MAX )
                continue;
              j = reads.find (mtpx.lookup (i->first));
              if ( j != reads . end( ) )
                {
                    if ( labs (i->second - j->second) < 20000 )
                    {
                      ID_t lib = libx.lookup (i->first);
                      libs[lib].push_back(labs (i->second - j->second));
                    }

                  cout << i->first << '\t' << j->first << '\t'
                       << labs (i->second - j->second) << endl;

                  i->second = -INT_MAX;
                  j->second = -INT_MAX;
                }
            }

          reads . clear( );
        }
      else
        {
          ss . str (line);
          ss >> red >> s;
          reads . insert (make_pair(red, s));
        }
    }

  hash_map<ID_t, vector<Size_t> >::iterator i;
  for ( i = libs . begin( ); i != libs . end( ); ++ i )
    {
      long sum = 0;
      vector<Size_t>::iterator j;

      for ( j = i->second.begin(); j != i->second.end(); ++ j )
        sum += *j;
      long int mean = sum / i->second.size();

      sort (i->second.begin(), i->second.end());
      long int median = i->second[i->second.size() / 2];

      cerr << "lib\t" << i->first << '\t' << mean << '\t' << median << endl;
    }

  return 0;
}
