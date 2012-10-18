#include <iostream>
#include "fasta.hh"
#include <iomanip>
using namespace std;

inline int max2(int a, int b)
{
  return (a > b) ? a : b;
}

inline int max3(int a, int b, int c)
{
  int maxl = (a > b) ? a : b;
  return (maxl > c) ? maxl : c;
}

inline int max4(int a, int b, int c, int d)
{
  int maxl = (a > b) ? a : b;
  int maxr = (c > d) ? c : d;

  return (maxl > maxr) ? maxl : maxr;
}

int s(char a, char b, int match_score, int mismatch_score)
{
  if (a == b)
  {
    return match_score;
  }
  
  return mismatch_score;
}

struct aligned_t
{
  char s;
  char t;
  int score;
};
typedef struct aligned_t aligned_t;



void align(const string & S, 
           const string & T,
           int match_score,
           int mismatch_score,
           int gap_open,
           int gap_extension)
{
  int m = S.length()-1;
  int n = T.length()-1;

  int * E = new int[(m+2) * (n+2)];
  int * F = new int[(m+2) * (n+2)];
  int * G = new int[(m+2) * (n+2)];
  int * V = new int[(m+2) * (n+2)];

  if (!E || !F || !G || !V)
  {
    fprintf(stderr, "strings too long!\n");
    exit(1);
  }

  fprintf(stderr, "Initializing arrays...");

  // initialize matrices
  for (int i = 0; i <= m; i++)
  {
    //E[i][0] = -gap_open + -gap_extension*i;
    E[i*(n+2) + 0] = 0;
    F[i*(n+2) + 0] = 0;
    G[i*(n+2) + 0] = 0;
    V[i*(n+2) + 0] = E[i*(n+2)+0];
  }

  fprintf(stderr, " ...");

  for (int j = 0; j <= n; j++)
  {
    //F[0][j] = 0;
    F[0+j] = -gap_open + -gap_extension*j;
    E[0+j] = 0;
    G[0+j] = 0;
    V[0+j] = F[0+j];
  }

  fprintf(stderr, " done.\n");

  G[0] = 0;
  V[0] = 0;

  int maxi = 0;
  int maxj = 0;
  int maxv = 0;

  // compute the matrix
  for (int i = 1; i <= m; i++)
  {
    for (int j = 1; j <= n; j++)
    {
      E[i*(n+2)+j] = max2(E[(i-1)*(n+2) + j] - gap_extension,
                          V[(i-1)*(n+2) + j] - gap_open - gap_extension);

      F[i*(n+2)+j] = max2(F[(i-1)*(n+2) + j] - gap_extension,
                          V[(i-1)*(n+2) + j] - gap_open - gap_extension);

      G[i*(n+2)+j] = V[(i-1) * (n+2) + j-1] + s(S[i], T[j], match_score, mismatch_score);

      V[i*(n+2)+j] = max3(E[i*(n+2) + j], 
                          F[i*(n+2) + j], 
                          G[i*(n+2) + j]);

      if (V[i*(n+2)+j] > maxv)
      {
        maxv = V[i*(n+2)+j];
        maxi = i;
        maxj = j;
      }
    }
  }

  maxv = -1;
  // find the max along the last row (m)
  for (int j = 0; j <= n; j++)
  {
    if (V[m*(n+2)+j] > maxv)
    {
      maxv = V[m*(n+2)+j];
      maxi = m;
      maxj = j;
    }
  }


  // // print the scores
  // cout << "V" << endl;
  // cout << "=======================================" << endl;
  // for (int i = 0; i <= m; i++)
  // {
  //   if (i == 0)
  //   {
  //     cout << " \t";
  //     for (int j = 0; j <= n; j++) { cout << T[j] << "\t"; }
  //     cout << endl;
  //   }
  // 
  //   cout << S[i] << "\t";
  //   for (int j = 0; j <= n; j++)
  //   {
  //     cout << V[i][j];
  //     //if (V[i][j] == maxv) { cout << "*"; }
  //     if (i == maxi && j == maxj) { cout << "*"; }
  //     cout << "\t";
  //   }
  // 
  // 
  //   cout << endl;
  // }
  
  // cout << endl << endl;
  // cout << "E" << endl;
  // cout << "=======================================" << endl;
  // for (int i = 0; i <= m; i++)
  // {
  //   if (i == 0)
  //   {
  //     cout << " \t";
  //     for (int j = 0; j <= n; j++) { cout << T[j] << "\t"; }
  //     cout << endl;
  //   }
  // 
  //   cout << S[i] << "\t";
  //   for (int j = 0; j <= n; j++)
  //   {
  //     cout << E[i][j];
  //     cout << "\t";
  //   }
  // 
  //   cout << endl;
  // }
  // 
  // cout << endl << endl;
  // cout << "F" << endl;
  // cout << "=======================================" << endl;
  // for (int i = 0; i <= m; i++)
  // {
  //   if (i == 0)
  //   {
  //     cout << " \t";
  //     for (int j = 0; j <= n; j++) { cout << T[j] << "\t"; }
  //     cout << endl;
  //   }
  // 
  //   cout << S[i] << "\t";
  //   for (int j = 0; j <= n; j++)
  //   {
  //     cout << F[i][j];
  //     cout << "\t";
  //   }
  // 
  //   cout << endl;
  // }
  // 
  // cout << endl << endl;
  // cout << "G" << endl;
  // cout << "=======================================" << endl;
  // for (int i = 0; i <= m; i++)
  // {
  //   if (i == 0)
  //   {
  //     cout << " \t";
  //     for (int j = 0; j <= n; j++) { cout << T[j] << "\t"; }
  //     cout << endl;
  //   }
  // 
  //   cout << S[i] << "\t";
  //   for (int j = 0; j <= n; j++)
  //   {
  //     cout << G[i][j];
  //     cout << "\t";
  //   }
  // 
  //   cout << endl;
  // }
  
  
  cout << "max score[" << maxi << "," << maxj << "]:" << maxv << endl;


  // print the alignment
  aligned_t trace[m+n];
  //int i = m; int j = n;
  int i = maxi, j = maxj;
  int tlen = 0;

  int numbasesaligned = 0;
  int numbasessame    = 0;

  while (i > 0 && j > 0)
  {
    int v = V[i*(n+2)+j];
    trace[tlen].score = v;

    numbasesaligned++;

    if (v == G[i*(n+2)+j])
    {
      if (S[i] == T[j])
      {
        numbasessame++;
      }

      trace[tlen].s = S[i];
      trace[tlen].t = T[j];
      i--;
      j--;
    }
    else if (v == E[i*(n+2)+j])
    {
      trace[tlen].s = S[i];
      trace[tlen].t = '-';
      i--;
    }
    else if (v == F[i*(n+2)+j])
    {
      trace[tlen].s = '-';
      trace[tlen].t = T[j];
      j--;
    }
    else // v == 0;
    {
      break;
    }

    tlen++;
  }

  for (int k = tlen - 1; k >= 0; k--)
  {
    cout << "   " << trace[k].s;
  }
  cout << endl;
  
  for (int k = tlen - 1; k >= 0; k--)
  {
    cout << "   " << trace[k].t;
  }
  cout << endl;
  
  for (int k = tlen - 1; k >= 0; k--)
  {
    //printf(" %3d", trace[k].score);
    cout << setw(3) << trace[k].score << endl;
  }
  cout << endl;

  int s_s, s_e;
  int t_s, t_e;

  s_s = i+1;
  s_e = maxi;

  t_s = 1;
  t_e = maxj;

  float perc_id = (float)(numbasessame * 100) / numbasesaligned;

  cout << "S: 1 [" << s_s << " " << s_e << "]" << endl;
  cout << "T:   [" << t_s << " " << t_e << "] " << n << endl;
  cout << numbasessame << "/" << numbasesaligned << " bases aligned : " << perc_id << " percent id" << endl;

  for (int i = 1; i < s_s; i++)     { cout << S[i]; }
  for (int k = tlen-1; k >= 0; k--) { cout << trace[k].s; }
  cout << endl;

  for (int j = 1; j < s_s; j++)     { cout << " "; }
  for (int k = tlen-1; k >= 0; k--) { cout << trace[k].t; }
  for (int j = t_e; j <=n; j++)     { cout << T[j]; }
  cout << endl;
}


int main(int argc, char ** argv)
{
  //string S = "sACAAAAATTTGGTC";
  //string T =       "sATGGTTCCATGA";

  if (argc < 3)
  {
    fprintf(stderr, "USAGE: overlap-align fasta1 fasta2\n");
    exit(1);
  }

  FILE * f1 = fopen(argv[1], "r");
  FILE * f2 = fopen(argv[2], "r");

  if (!f1) { fprintf(stderr, "ERROR: Couldn't open %s\n", argv[1]); exit(1); }
  if (!f2) { fprintf(stderr, "ERROR: Couldn't open %s\n", argv[2]); exit(1); }

  string S, S_header;
  string T, T_header;

  Fasta_Read(f1, S, S_header);
  Fasta_Read(f2, T, T_header);

  S.insert(0, 1, 's');
  T.insert(0, 1, 's');
  
  int match_score = 10;
  int mismatch_score = -2;
  int gap_open = 4;
  int gap_extension = 1;

  cout << "S: " << S << " " << S.length()-1 << endl;
  cout << "T: " << T << " " << T.length()-1 << endl;

  align(S,T, match_score, mismatch_score, gap_open, gap_extension);

  return 0;
}


