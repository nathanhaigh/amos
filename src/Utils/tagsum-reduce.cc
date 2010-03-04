#include  <string>
#include  <iostream>
using namespace std;

int  main (int argc, char * argv [])
{
  string mykmer, kmer;
  unsigned long long int mycount, count;
  
  // read initial line
  cin >> mykmer;
  cin >> mycount;

  while(cin >> kmer) {
    cin >> count;

    // if same as last, increment count
    if(kmer == mykmer) {
      mycount += count;

    // else print last, initialize new
    } else {
      cout << mykmer << "\t" << mycount << endl;
      mykmer = kmer;
      mycount = count;
    }
  }

  // print last
  cout << mykmer << "\t" << mycount << endl;

  return 0;
}
