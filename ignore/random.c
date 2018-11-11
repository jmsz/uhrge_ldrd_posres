/*==> Random generator library <==================================//

Functions for generation of random numbers.

//--- Version ----------------------------------------------------//

Version: 1.0
Date:    2012

//--- Copyright --------------------------------------------------//

Copyright (C) by MIZZI Computer Software GmbH, Germany.

This software is furnished under a license and may be used
and copied only in accordance with the terms of such
license and with the inclusion of the above copyright
notice.

This software or any other copies thereof may not be
provided or otherwise made available to any other person.
No title to and ownership of the software is hereby
transferred.

The information in this software is subject to change
without notice and should not be construed as a commitment
by the holder of the copyright.

//----------------------------------------------------------------*/

/*--- Include ----------------------------------------------------*/

//#include "random.h"

/*----------------------------------------------------------------*/

/*================================================================*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//static int	verbose=0; 		/* 0=none 1=one 2=more */

/*==> Basic functions  <==========================================*/

/*=== Function ===================================================*/

void RandomSeed(int seed)

/*--- Description ------------------------------------------------//

Sets the seed of the random generators.
Sets its argument as the seed for a new sequence of pseudo-random
number to be returned by this library.

//----------------------------------------------------------------*/
{
  srandom(seed);
}


/*=== Function ===================================================*/

double RandomNumber(double from, double to)

/*--- Description ------------------------------------------------//

Returns a random number in the range from to.

//----------------------------------------------------------------*/
{
  double x=random();
  return x/RAND_MAX*(to-from)+from;
}


/*=== Function ===================================================*/

double RandomWindow(double old, double from, double to, double resolution)

/*--- Description ------------------------------------------------//

Returns a subsequent number by incrementing the old value
with the resolution and limits the value with to the range
given by from and to.

This is a fast function. It returns not real random numbers rather
than an equally spaced range of subsequent numbers.

//----------------------------------------------------------------*/
{
  // fprintf(stderr,"%g %g %g %g \n",old,from,to,resolution);
  if(old<from || old>to) return from;
  old=old+resolution;
  if(old>to) return from;
  return old;
}

/*=== Function ===================================================*/

int RandomIntWindow(int old, int from, int to)

/*--- Description ------------------------------------------------//

Returns an integer in the range set by from and to.

//----------------------------------------------------------------*/
{
  if(old<from || old>to) return from;
  old=old+1;
  if(old>to) return from;
  return old;
}

/*==> Functions for random number distribution <==================*/

/*=== Function ===================================================*/

double RandomGauss(double mean, double sigma)

/*--- Description ------------------------------------------------//

Retruns a random numbers for a Gaussian distribution with mean
and sigma.

//----------------------------------------------------------------*/
{
  double r;
  double v1,v2,fac,val;
  static int set=0;
  static double nextGauss=0;

  if (set)
  {
    set = 0;
    return mean+sigma*nextGauss;
  }

  do
  {
    v1 = 2.0 * RandomNumber(0,1) - 1.0;
    v2 = 2.0 * RandomNumber(0,1) - 1.0;
    r = v1*v1 + v2*v2;
  } while ( r > 1.0 );

  fac = sqrt(-2.0*log(r)/r);
  val = v1*fac;
  nextGauss = val;
  set = 1;
  return mean+sigma*v2*fac;
}



// --------------------------------------------------------------
// Utility methods to get/set attributes for Poisson distribution
// --------------------------------------------------------------

static double RandomPoisson_rstatus[3];
static double RandomPoisson_oldm = -1.0;
static double RandomPoisson_meanMax = 2.0e9;

static void RandomPoisson_setOldMean( double val )
{
  RandomPoisson_oldm = val;
}

static double RandomPoisson_getOldMean()
{
  return RandomPoisson_oldm;
}

static double RandomPoisson_getMaxMean()
{
  return RandomPoisson_meanMax;
}

static void RandomPoisson_setPStatus(double sq, double alxm, double g)
{
  RandomPoisson_rstatus[0] = sq; RandomPoisson_rstatus[1] = alxm; RandomPoisson_rstatus[2] = g;
}

static double* RandomPoisson_getPStatus() {return RandomPoisson_rstatus;}

/**
 *  @short Returns the value ln(Gamma(xx) for xx > 0.
 *  Full accuracy is obtained for
 *  xx > 1. For 0 < xx < 1. the reflection formula (6.1.4) can be used first.
 *  (Adapted from Numerical Recipes in C)
 */

static double gammln(double xx)
{
  static double cof[6] =
  {
    76.18009172947146,-86.50532032941677,
    24.01409824083091, -1.231739572450155,
    0.1208650973866179e-2, -0.5395239384953e-5
  };
  register int j;
  double x, tmp, ser;

  x = xx - 1.0;
  tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  ser = 1.000000000190015;

  for ( j = 0; j <= 5; j++ ) {
    x += 1.0;
    ser += cof[j]/x;
  }
  return -tmp + log(2.5066282746310005*ser);
}

/* -------------------------- RandomPoisson ----------------------*/

/*
 *  short Generate random numbers following Poisson distribution.
 *
 *  Returns as a floating-point number an integer value that is a random
 *  deviation drawn from a Poisson distribution of mean xm, using flat()
 *  as a source of uniform random numbers.
 *  (Adapted from Numerical Recipes in C)
 *
 *  param xm  Mean value of Poisson distribution.
 *
 *  return Random number (integer>, >= 0)
 */

/*=== Function ===================================================*/

long RandomPoisson(double xm)

/*--- Description ------------------------------------------------//


Retruns a random numbers for a Poisson distribution with the mean
given by xm.

//----------------------------------------------------------------*/
{
  register double em;
  double t, y;
  double sq, alxm, g;
  double om = RandomPoisson_getOldMean();

  double* pstatus = RandomPoisson_getPStatus();
  sq = pstatus[0];
  alxm = pstatus[1];
  g = pstatus[2];

  if( xm < 12.0 )
  {
    if( xm != om )
    {
      RandomPoisson_setOldMean(xm);
      g = exp(-xm);
    }
    em = -1;
    t = 1.0;
    do
    {
      em += 1.0;
      t *= RandomNumber(0,1);
    } while( t > g );
  }
  else if ( xm < RandomPoisson_getMaxMean() )
  {
    if ( xm != om )
    {
      RandomPoisson_setOldMean(xm);
      sq = sqrt(2.0*xm);
      alxm = log(xm);
      g = xm*alxm - gammln(xm + 1.0);
    }
    do
    {
      do
      {
              y = tan(M_PI*RandomNumber(0,1));
              em = sq*y + xm;
      } while( em < 0.0 );
      em = floor(em);
      t = 0.9*(1.0 + y*y)* exp(em*alxm - gammln(em + 1.0) - g);
    } while(RandomNumber(0,1) > t );
  }
  else
  {
    if ( xm != om )
    {
      RandomPoisson_setOldMean(xm);
      sq = sqrt(2.0*xm);
      alxm = log(xm);
      g = xm*alxm - gammln(xm + 1.0);
    }
    em = xm;
  }
  RandomPoisson_setPStatus(sq,alxm,g);
  return (long)em;
}

//int main(){return 0;}
