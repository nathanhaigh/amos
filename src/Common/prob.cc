//  A. L. Delcher
//
//  File:  prob.cc
//
//  Last Modified:  30 December 2002
//
//  Routines to do probability things



#include  "prob.hh"



int  Binomial_Cutoff
    (int n, double p, double e_prob)

//  Return  k  such that the probability of  >= k  successes in
//  n  trials is  < e_prob , where  p  is the probability
//  of success of each trial.

  {
   double  lambda, target, sum, term, q;
   int  i;

   if  (n <= 0)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  n = %d <= 0", n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   lambda = n * p;
   q = 1.0 - p;

   if  (Verbose > 3)
       fprintf (stderr, "Binomial Cutoff:  n = %d  p = %e  lambda = %e\n",
                n, p, lambda);

   if  (n >= 30.0)
       {
        if  (lambda <= 5.0)
            {  
             // use Poisson approximation
             if  (Verbose > 4)
                fprintf (stderr, "Binomial Cutoff: using Poisson approximation\n");

             target = (1.0 - e_prob) * exp (lambda);
             sum = term = 1.0;
             for  (i = 1;  sum <= target && i < 50;  i ++)
               {
                term = term * lambda / (double) i;
                sum += term;
               }

             if  (sum <= target)
                 fprintf (stderr, "Binomial Cutoff: sum <= target... Is this a problem?\n");

            }
        else // lambda > 5.0
            {
             // use Normal approximation
             if  (Verbose > 4)
                 fprintf (stderr, "Binomial Cutoff: using Normal approximation\n");

             double  t, x, y;

             if  (e_prob <= 0.5)
                 t = sqrt (- 2.0 * log (e_prob));
               else
                 t = sqrt (- 2.0 * log ((1.0 - e_prob)));

             y = t - (((0.010328 * t + 0.802853) * t + 2.515517)
                         /
                     (((0.001308 * t + 0.189269) * t + 1.432788) * t + 1.0));

             if  (e_prob <= 0.5)
                 x = y;
               else
                 x = -y;

             i = int (ceil (x * sqrt (lambda * q) + lambda));
            }
       }
     else
       {
        // brute force
        if  (Verbose > 4)
           fprintf (stderr, "Binomial Cutoff: using brute force method\n");

        target = 1.0 - e_prob;
        sum = term = pow (q, n);
        for  (i = 1;  sum <= target && i < n;  i ++)
          {
           term *= double (n + 1 - i) / i;
           term *= p / q;
           sum += term;
          }
       }
   return i;
  }

