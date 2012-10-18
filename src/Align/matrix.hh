//  A. L. Delcher
//
//  File:  matrix.hh
//
//  Last Modified:  12 September 2003
//
//  Define a simple 2D array class


#ifndef  __MATRIX_HH_INCLUDED
#define  __MATRIX_HH_INCLUDED


#include  <delcher.hh>



template <class DT>
class  Matrix
  {
  private:
   int  rows, cols;
   DT  * data;

  public:
   Matrix
       (int r, int c)
     {
      data = (DT *) Safe_calloc (r * c, sizeof (DT));
      rows = r;
      cols = c;
     }
   ~ Matrix
       ()
     {
      free (data);
     }
   DT &  get
       (int i, int j)
     {
      return  * (data + i * cols + j);
     }
   DT *  getRow
       (int i)
     { return  data + i * cols; }
   int  NumRows
       (void)  const
     { return rows; }
  };



#endif
