// $Id$

#ifndef BAMBUS_BUNDLER_HH
#define BAMBUS_BUNDLER_HH

#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include <string>
#include <map>

using namespace AMOS;
using namespace std;

typedef multimap<pair<ID_t, ID_t> , ContigLink_t > LinkMap_t;
typedef map<ID_t, int > ContigLen_t;
typedef map<string, ID_t> IdMap_t;

#endif // BAMBUS_BUNDLER_HH
