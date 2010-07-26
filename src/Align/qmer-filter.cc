#include  <string>
#include  <iostream>
#include  <cstdlib>

using namespace std;

int  main (int argc, char * argv [])
{
  if (argc != 2)
  {
    cerr << "Usage: qmer-filter threshold < in > out\n";
    return 0;
  }

  double threshold = atof(argv[1]);
  cerr << "Qmer threshold: " << threshold << endl;

  string kmer;
  double count;

  while(cin >> kmer) 
  {
    cin >> count;

    if (count >= threshold)
    {
      cout << kmer << "\t" << count << "\n";
    }
  }

  return 0;
}
