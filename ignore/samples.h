/*==> Samples Library  <======================================//

Functions for filtering and analysing arrays of doubles.

//--- Version ------------------------------------------------//

Version: 3.2
Date:    2017
Updated by: Marco Salathe (marco.salathe@mpi-hd.mpg.de)

//--- Copyright ----------------------------------------------//

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

//------------------------------------------------------------*/

/*--- Include ------------------------------------------------*/

//#include "samples.h"

/*------------------------------------------------------------*/

/*============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef ONCE
#define ONCE 1

static const double Euler=2.7182818284590452354;
static const double Pi=3.14159265358979323846;

static int debug=0;
#endif

static int LastHeaderMode__=0;

/*==> Debugging <=============================================*/

/*=== Function ===============================================*/

int DebugSamples(int level)

/*--- Description --------------------------------------------//

Set debug level for print outs:

0  ... no debug messagens
1  ... errors
2  ... warnings
3  ... i/o
4  ... info
>4 ... debugging

Not all functions prints info messages
Returns the old debug level
Logging output is written to stderr

//------------------------------------------------------------*/
{
  int tmp=debug;
  debug=level;
  return tmp;
}


/*==> Allocation <============================================*/

/*=== Function ===============================================*/

double *AllocateSamples(int nx)

/*--- Description --------------------------------------------//

Allocates an array of doubles.
Returns the pointer to the array on success, 0 on error.

//------------------------------------------------------------*/
{
  return (double *)calloc(nx,sizeof(double));
}


/*=== Function ===============================================*/

int FreeSamples(double *data)

/*--- Description --------------------------------------------//

Free the array of doubles.
Returns 0 if data is a 0 pointer or 1 if data was freed.

//------------------------------------------------------------*/
{
  if(data==(void*)0) return 0;
  free(data);
  return 1;
}


/*=== Function ===============================================*/

double **Allocate2DSamples(int ny, int nx)

/*--- Description --------------------------------------------//

Allocates an 2 dim array of doubles.
Returns the pointers to the arrays on success, 0 on error.
The array of all double is consecutive.

//------------------------------------------------------------*/
{
  int i;
  double **yp=(double**)calloc(ny,sizeof(void*));
  double *dp=(double*)calloc(nx*ny,sizeof(double));
  for(i=0; i<ny; i++) yp[i]= &dp[i*nx];
  return yp;
}

/*=== Function ===============================================*/

int Free2DSamples(double **data)

/*--- Description --------------------------------------------//

Free the 2dim. array of doubles.
Returns 0 if data is a 0 pointer or 1 if data was freed.

//------------------------------------------------------------*/
{
  if(!data) return 0;
  free(data[0]);
  free(data);
  return 1;
}

/*=== Function ===============================================*/

double ***Allocate3DSamples(int nz, int ny, int nx)

/*--- Description --------------------------------------------//

Allocates an 3 dim array of doubles.
Returns the pointers to the arrays on success, 0 on error.
The array of all double is consecutive.

//------------------------------------------------------------*/
{
  int i;
  double ***zp=(double***)calloc(nz,sizeof(void*));
  double **yp=(double**)calloc(ny*nz,sizeof(void*));
  double *dp=(double*)calloc(nx*ny*nz,sizeof(double));
  for(i=0; i<ny*nz; i++) yp[i]= &dp[i*nx];
  for(i=0; i<nz; i++) zp[i]=&yp[i*ny];
  return zp;
}


/*=== Function ===============================================*/

int Free3DSamples(double ***data)

/*--- Description --------------------------------------------//

Free the 2dim. array of doubles.
Returns 0 if data is a 0 pointer or 1 if data was freed.

//------------------------------------------------------------*/
{
  if(!data) return 0;
  free(data[0][0]);
  free(data[0]);
  free(data);
  return 1;
}

/*==> Basic functions <=======================================*/

/*=== Function ===============================================*/

int ClearSamples(int n, double *in)

/*--- Description --------------------------------------------//

Clears an array of n doubles (set values to 0).
Returns n.

//------------------------------------------------------------*/
{
  int i;
  for(i=0; i<n; i++) in[i]=0;
  if(debug>2) fprintf(stderr,"ClearSamples: %d samples set to zero\n",n);
  return n;
}


/*=== Function ===============================================*/

int Clear2DSamples(int ny, int nx, double **in)

/*--- Description --------------------------------------------//

Clears an array of n doubles (set values to 0).
Returns n.

//------------------------------------------------------------*/
{
  int i,j;
  for(j=0;j<ny;j++) for(i=0; i<nx; i++) in[j][i]=0;
  if(debug>2) fprintf(stderr,"Clear2DSamples: %d samples set to zero\n",ny*nx);
  return nx*ny;
}

/*=== Function ===============================================*/

int Clear3DSamples(int nz, int ny, int nx, double ***in)

/*--- Description --------------------------------------------//

Clears an array of n doubles (set values to 0).
Returns n.

//------------------------------------------------------------*/
{
  int i,j;
  for(j=0;j<nz;j++) for(j=0;j<ny;j++) for(i=0; i<nx; i++) in[j][i]=0;
  if(debug>2) fprintf(stderr,"Clear3DSamples: %d samples set to zero\n",nz*ny*nx);
  return nx*ny*nz;
}

/*=== Function ===============================================*/

int CopySamples(int n, double *in, double *out)

/*--- Description --------------------------------------------//

Copy an array of n doubles from in to out.
Returns the number of samples.

//------------------------------------------------------------*/
{
  int i;
  if(in==out) return n;
  for(i=0; i<n; i++) out[i]=in[i];
  return n;
}


/*=== Function ===============================================*/

int Copy2DSamples(int ny, int nx, double **in, double **out)

/*--- Description --------------------------------------------//

Copy an array of nx*ny doubles from in to out.
Returns the number of samples copied.

//------------------------------------------------------------*/
{
  int i,j;
  for(i=0; i<ny; i++) for(j=0; j<nx; j++) out[i][j]=in[i][j];
  return nx*ny;
}


/*=== Function ===============================================*/

int MultiplyAddSamples(int n, double *in, double *out,
    double m1, double offset)

/*--- Description --------------------------------------------//

Multiply an array of doubles given by in1 by m1,
add a constant offset, and store the result in out.

Returns n.

//------------------------------------------------------------*/
{
  int i;
  for(i=0; i<n; i++) out[i]=m1*in[i]+offset;
  return n;
}


/*=== Function ===============================================*/

int MultiplyAdd2DSamples(int ny, int nx, double **in, double **out,
    double m1, double offset)

/*--- Description --------------------------------------------//

Multiply an array of doubles given by in1 by m1,
add a constant offset, and store the result in out.

Returns n.

//------------------------------------------------------------*/
{
  int i,j;
  for(i=0; i<ny; i++) for(j=0; j<nx; j++) out[i][j]=m1*in[i][j]+offset;
  return nx*ny;
}


/*=== Function ===============================================*/

int AddTwoSamples(int n, double *in1, double *in2, double *out,
    double m1, double m2)

/*--- Description --------------------------------------------//

Multiply an array in1 with m1 and in2 with m2 and add both arrays
element by element.

The result is stored in out.
Returns n.

//------------------------------------------------------------*/
{
  int i;
  for(i=0;i<n;i++) out[i]=m1*in1[i]+m2*in2[i];
  return n;
}


/*=== Function ===============================================*/

int AddTwo2DSamples(int ny, int nx, double **in1, double **in2,
    double **out, double m1, double m2)

/*--- Description --------------------------------------------//

Multiply an array in1 with m1 and in2 with m2 and add both arrays
element by element.

The result is stored in out.
Returns n.

//------------------------------------------------------------*/
{
  int i,j;
  for(i=0; i<ny; i++) for(j=0; j<nx; j++) out[i][j]=m1*in1[i][j]+m2*in2[i][j];
  return nx*ny;
}


/*=== Function ===============================================*/

int CompressSamples(int n, double *in, double *out, int c)

/*--- Description --------------------------------------------//

Store in out the sum of c double values of array in.
retains the number of new samples written to out (n/c).

//------------------------------------------------------------*/
{
int i,ic;
double sum;
int newn=n/c;
for(i=0; i<newn; i++)
{
  for(sum=0, ic=0; ic<c; ic++) sum+=(*in++);
  *out++=sum;
}
return newn;
}


/*=== Function ===============================================*/

int ShiftSamples(int n, double *in, double *out, int shift)

/*--- Description --------------------------------------------//

Shifts data elements in array in by shift elements.
If shift > 0 elements are shifted to higher elements
if shift < 0 elements are shifted to lower elements
output is stored in out and not shifted values are set to 0.
Returns 0

//------------------------------------------------------------*/
{
  int i;
  if(shift<=0)
  {
    for(shift=-shift, i=shift; i<n; i++)  out[i-shift]=in[i];
    while(i-shift <n)  out[i-shift]=0,i++;
  }
  else if(shift>0)
  {
    for(i=n-shift; i>=0; i--) out[i+shift]=in[i];
    while(i+shift >=0)  out[i+shift]=0,i--;
  }
  return n;
}


/*==> Conversions <===========================================*/

/*=== Function ===============================================*/

int CompressRangedShortToSamples
(
  int n, unsigned short *in, double *out,
  int min, int max, int c, double gain, double offset
)
/*--- Description --------------------------------------------//

Convert data array of unsigned shorts given by in to an array
of doubles (out).

The elements of in are limited by min and max and c elements
are added to form one output element. Each output element is then
multiplied by gain and and offset is added to the final value.

Returns the number of output elements (n/c).

//------------------------------------------------------------*/
{

  int i,ic;
  double v;
  int of=0;
  int uf=0;
  double sum;
  int newn=n/c;
  double *o=out;
  for(i=0; i<newn; i++)
  {
    for(sum=0, ic=0; ic<c; ic++)
    {
      v=(*in++);
      if(v>=max) v=max,of++;
      else if(v<=min) v=min,uf++;
      sum+=v;
    }
    *o++=sum;
  }
  for(o=out,i=0;i<newn;i++) o[i]=o[i]*gain+offset;
  // if(of || uf) return -newn;
  return newn;

}


/*=== Function ===============================================*/

int ConvertFloatToSamples(int n, float *in, double *out)

/*--- Description --------------------------------------------//

Converts n floats to doubles.

Returns n.

//------------------------------------------------------------*/
{
  int i;
  for(i=0; i<n; i++) out[i]=in[i];
  return n;
}


/*==> Filling and retrieving <================================*/

/*=== Function ===============================================*/

double Fill1Samples(int n, double *in, double pos, double value)

/*--- Description --------------------------------------------//

Add to in[pos+0.5] the number given by value.
Returns the integer position.

//------------------------------------------------------------*/
{
  double ipos=floor(pos+0.5);
  if(ipos>=0 && ipos<n)  in[(int)ipos]+=value;
  if(debug>4) fprintf(stderr,"Fill1Samples: pos %g value %g element %g  \n",pos,value,ipos);
  return ipos;
}


/*=== Function ===============================================*/

double Fill2Samples(int n, double *in, double pos, double value)

/*--- Description --------------------------------------------//

Fill around in[pos] two samples that the integral and center
of gravity is retained.

Returns pos.

//------------------------------------------------------------*/
{
  double ipos,f,tmp[2];
  ipos=floor(pos);
  f=pos-ipos;
  tmp[0]=value*(1-f);
  tmp[1]=value*(f);
  if(ipos>=0 && ipos<n) in[(int)ipos]+=tmp[0];
  ipos+=1;
  if(ipos>=0 && ipos<n) in[(int)ipos]+=tmp[1];
  if(debug>4) fprintf(stderr,"Fill2Samples: pos %g value %g elements %g %g\n",
     pos,value,tmp[0],tmp[1]);
  return pos;
}


/*=== Function ===============================================*/

double Fill3Samples(int n, double *in, double pos, double value)

/*--- Description --------------------------------------------//

Fill around in[pos] three samples that the integral and center
of gravity is retained. The maximum is then 0.5 * value.

Returns pos.

//------------------------------------------------------------*/
{
  double ipos,f,tmp[3];
  pos+=0.5;
  ipos=floor(pos);
  f=pos-ipos;
  tmp[0]=value*(1-f);
  tmp[1]=value*(f);
  ipos-=1;
  if(ipos>=0 && ipos<n) in[(int)ipos]+=tmp[0]/2;
  ipos+=1;
  if(ipos>=0 && ipos<n) in[(int)ipos]+=value/2;
  ipos+=1;
  if(ipos>=0 && ipos<n) in[(int)ipos]+=tmp[1]/2;
  if(debug>4) fprintf(stderr,"Fill3Samples: pos %g value %g elements %g %g %g\n",
     pos-0.5,value,tmp[0]/2,value/2,tmp[1]/2);
  return pos;
}


/*=== Function ===============================================*/

int Fill2D1Samples(int ny, int nx, double **in, double ypos,
    double xpos, double value)

/*--- Description --------------------------------------------//

Add to in[pos+0.5] the number given by value.

//------------------------------------------------------------*/
{
  double xipos=floor(xpos+0.5);
  double yipos=floor(ypos+0.5);
  if(xipos>=0 && xipos<nx && yipos>=0 && yipos<ny)
    in[(int)yipos][(int)xipos]+=value;
  if(debug>4) fprintf(stderr,"Fill2D1Samples: ypos %g xpos %g value %g elements %g %g \n",ypos,xpos,value,yipos,xipos);
  return 1;
}


/*=== Function ===============================================*/

int Fill2D2Samples(int ny, int nx, double **in, double ypos,
    double xpos, double value)

/*--- Description --------------------------------------------//

Fill around in[pos] two samples that the integral and center
of gravity is retained.

Returns pos.

//------------------------------------------------------------*/
{
  double yipos, xipos,yf,xf,tmp[4];
  yipos=floor(ypos); xipos=floor(xpos);
  yf=ypos-yipos; xf=xpos-xipos;
  tmp[0]=value*(1-yf)*(1-xf);
  tmp[1]=value*(yf)*(1-xf);
  tmp[2]=value*(1-yf)*(xf);
  tmp[3]=value*(yf)*(xf);
  if(xipos>=0 && xipos<nx && yipos>=0 && yipos<ny) in[(int)yipos][(int)xipos]+=tmp[0];
  yipos+=1;
  if(xipos>=0 && xipos<nx && yipos>=0 && yipos<ny) in[(int)yipos][(int)xipos]+=tmp[1];
  yipos-=1;
  xipos+=1;
  if(xipos>=0 && xipos<nx && yipos>=0 && yipos<ny) in[(int)yipos][(int)xipos]+=tmp[2];
  yipos+=1;
  if(xipos>=0 && xipos<nx && yipos>=0 && yipos<ny) in[(int)yipos][(int)xipos]+=tmp[3];
  if(debug>4) fprintf(stderr,"Fill2D2Samples: ypos %g xpos %g value %g elements %g %g %g %g\n",
     ypos,xpos,value,tmp[0],tmp[1],tmp[2],tmp[3]);
  return 1;
}


/*=== Function ===============================================*/

int FillSamples(int n, double *in, double pos,double value, double width)

/*--- Description --------------------------------------------//

Fills a sample in the range width centered at pos such that the
amount filled into a certain sample corresponds to the overlap
with the input width. If width=0 the function is the same as
Fill1Samples if width=1 the same as Fill2Samples and if width=2
the function is the same as Fill3Samples.

Returns pos.

//------------------------------------------------------------*/
{
//short cuts for speeding up the function
  if(value==0) return pos;

  width=fabs(width);
  if(width>0) value/=width;

  pos+=0.5; width*=0.5;
  double le=pos-width;
  double he=pos+width;
  double ile=floor(le);
  double ihe=floor(he);
  double fle=1.0+ile-le;
  double fhe=he-ihe;

  int i,delta=ihe-ile;

  if(ile==ihe && width) fle=width, fhe=width;
  if(ile>=0 && ile<n) in[(int)(ile)]+=fle*value;
  if(ihe>=0 && ihe<n) in[(int)(ihe)]+=fhe*value;
  for(i=1;i<delta;i++) if(i+ile>=0 && i+ile<n) in[(int)(i+ile)]+=value;
  if(debug>4) fprintf(stderr,"FillSamples: pos %.10g i %g %g value %.10g width %.10g elements %.10g %.10g %d\n",
     pos,ile,ihe,value,width,fle,fhe,delta);

  return pos;
}


/*=== Function ===============================================*/

int Fill2DSamples(int ny, int nx, double **in, double ypos,
    double xpos, double value, double ywidth, double xwidth)

/*--- Description --------------------------------------------//

Fills a sample in the area xwidth*ywidth centered at
(ypos,xpos) such that the amount filled into a certain sample
corresponds to the overlap with the input area.
If width=0 the function is the same as Fill1Samples if width=1
the same as Fill2Samples and if width=2 the function is the same
as Fill3Samples.

Returns always 1.

//------------------------------------------------------------*/
{
//short cuts for speeding up the function
  if(value==0) return 1;
//  if(xwidth==0 && ywidth==0) return Fill2D1Samples(ny,nx,in,ypos,xpos,value);

  xwidth=fabs(xwidth); ywidth=fabs(ywidth);
  if(xwidth>0) value/=xwidth;
  if(ywidth>0) value/=ywidth;

  xpos+=0.5; ypos+=0.5; xwidth*=0.5; ywidth*=0.5;
  double xle=xpos-xwidth;
  double yle=ypos-ywidth;
  double xhe=xpos+xwidth;
  double yhe=ypos+ywidth;
  double xile=floor(xle);
  double yile=floor(yle);
  double xihe=floor(xhe);
  double yihe=floor(yhe);
  double xfle=1.0+xile-xle;
  double yfle=1.0+yile-yle;
  double xfhe=xhe-xihe;
  double yfhe=yhe-yihe;

  int xdelta=xihe-xile;
  int ydelta=yihe-yile;
  int xi,yi;

  if(xile==xihe && xwidth) xfle=xwidth, xfhe=xwidth;
  if(yile==yihe && ywidth) yfle=ywidth, yfhe=ywidth;

  if(xile>=0 && yile>=0 && xile<nx && yile<ny) in[(int)yile][(int)xile]+=xfle*yfle*value;
  if(xihe>=0 && yile>=0 && xihe<nx && yile<ny) in[(int)yile][(int)xihe]+=xfhe*yfle*value;
  for(xi=1;xi<xdelta;xi++) if(xi+xile>=0 && yile>=0 && xi+xile<nx && yile<ny) in[(int)yile][(int)(xi+xile)]+=yfle*value;

  for(yi=1;yi<ydelta;yi++)
  {
    if(xile>=0 && yi+yile>=0 && xile<nx && yi+yile<ny) in[yi+(int)yile][(int)xile]+=xfle*value;
    if(xihe>=0 && yi+yile>=0 && xihe<nx && yi+yile<ny) in[yi+(int)yile][(int)xihe]+=xfhe*value;
    for(xi=1;xi<xdelta;xi++) if(xi+xile>=0 && yi+yile>=0 && xi+xile<nx && yi+yile<ny) in[(int)(yi+yile)][(int)(xi+xile)]+=value;
  }

  if(xile>=0 && yihe>=0 && xile<nx && yihe<ny) in[(int)yihe][(int)xile]+=xfle*yfhe*value;
  if(xihe>=0 && yihe>=0 && xihe<nx && yihe<ny) in[(int)yihe][(int)xihe]+=xfhe*yfhe*value;
  for(xi=1;xi<xdelta;xi++) if(xi+xile>=0 && yihe>=0 && xi+xile<nx && yihe<ny) in[(int)yihe][(int)(xi+xile)]+=yfhe*value;

  if(debug>4) fprintf(stderr,"Fill2DSamples: ypos %.9g xpos %.9g value %.9g ywidth %.9g xwidth %.9g elements %.9g %.9g %.9g %.9g %d %d\n",
     ypos,xpos,value,ywidth,xwidth,yfle,yfhe,xfle,xfhe,ydelta,xdelta);

  return 1;
}


/*=== Function ===============================================*/

double InterpolateSamples(int n, double *in, double pos, double width)

/*--- Description --------------------------------------------//

Interpolate values of the in-array with size n around the position
pos using width and returns that value.

If width is 0 a linear interpolation between samples is used.

//------------------------------------------------------------*/
{
  if(width==0)
  {
  double fraction,v;
  int c1,c2;
  if(pos<0) return in[0];
  if(pos>=n-1) return in[n-1];
  c1=(int)pos;
  c2=c1+1;
  fraction=pos-c1;
  v=(1-fraction)*in[c1]+(fraction)*in[c2];
  if(debug>3) fprintf(stderr,"InterpolateSamples: pos %g => weight %d/%g %d/%g result %g\n",
     pos,c1,(1-fraction),c2,fraction,v);
  return v;
  }
  else if(width>0)
  {
    int i,x;
    double f;
    double sum=0;
    double start;
    double stop;
    start=pos-width/2+0.5;
    stop=pos+width/2+0.5;
    if(debug>4) fprintf(stderr,"InterpolateSamples: pos %g width %g start %g stop %g\n",pos, width, start,stop);
    if(start<0) sum+=(-start*in[0]), start=0;
    if(start>n) start=n;
    if(stop>n) sum+=(stop-n)*in[n-1], stop=n;

    if(debug>4) fprintf(stderr,"InterpolateSamples: sum after check %g start %g stop %g\n",sum,start,stop);
    x=(int)start;
    f=(x-start);
    sum+=in[x]*f;
    if(debug>4) fprintf(stderr,"InterpolateSamples: cor first index %d %f %g sum %g \n",x,f,in[x],sum);
    for(i=x;i<stop-1;i++) sum+=in[i];
    if(debug>4) fprintf(stderr,"InterpolateSamples: add mid   index %d %d sum %g \n",x,(int)(stop-1),sum);
    x=i;
    f=(stop-x);
    sum+=in[x]*f;
    if(debug>4) fprintf(stderr,"InterpolateSamples: add last  index %d %f %g sum %g \n",x,f,in[x],sum);
    if(debug>3) fprintf(stderr,"InterpolateSamples: pos %g width %g => sum %g result %g\n",
       pos,width,sum,sum/(width));
    return sum/(width);
  }
  return 0.0;
}


/*=== Function ===============================================*/

double Interpolate2DSamples(int ny, int nx, double **in,
       double ypos, double xpos, double ywidth, double xwidth)

/*--- Description --------------------------------------------//

Interpolate values of the in-array with size ny,nx around the position
(ypos,xpos) using ywidth,xwidth and return that value.

If width is 0 a linear interpolation between samples is used.

//------------------------------------------------------------*/
{
  if(ywidth==0)
  {
  double fraction,v;
  int c1,c2;
  if(ypos<0) return InterpolateSamples(nx,in[0],xpos,xwidth);
  if(ypos>=ny-1) return InterpolateSamples(nx,in[ny-1],xpos,xwidth);
  c1=(int)ypos;
  c2=c1+1;
  fraction=ypos-c1;
  v=(1-fraction)*InterpolateSamples(nx,in[c1],xpos,xwidth)+(fraction)*InterpolateSamples(nx,in[c2],xpos,xwidth);
  return v;
  }
  else if(ywidth>0)
  {
    int i,x;
    double f;
    double sum=0;
    double start;
    double stop;
    start=ypos-ywidth/2+0.5;
    stop=ypos+ywidth/2+0.5;
    if(start<0) sum+=(-start*InterpolateSamples(nx,in[0],xpos,xwidth)), start=0;
    if(start>ny) start=ny;
    if(stop>ny) sum+=(stop-ny)*InterpolateSamples(nx,in[ny-1],xpos,xwidth), stop=ny;

    x=(int)start;
    f=(x-start);
    sum+=InterpolateSamples(nx,in[x],xpos,xwidth)*f;
    for(i=x;i<stop-1;i++) sum+=InterpolateSamples(nx,in[i],xpos,xwidth);
    x=i;
    f=(stop-x);
    sum+=InterpolateSamples(nx,in[x],xpos,xwidth)*f;
    return sum/(ywidth);
  }
  return 0.0;
}


/*=== Function ===============================================*/

int ResizeSamples(int n, double *in, int newsize, double *out,
    double from , double to)

/*--- Description --------------------------------------------//

Resize array in (of size n) and store it in out. out contains
newsize values, in the range from ... to.

This function uses FillSamples and assures the cog by filling
at least 2 samples.

Caution: If the sample sizes are not a multiple of each other
unwanted artefacts can appear.

Returns newsize.

//------------------------------------------------------------*/
{
int i;
/*
double step=(to-from)/newsize;
for(i=0;i<newsize; i++)
  out[i]=InterpolateSamples(n,in,(i+0.5)*step-0.5,step)*step;
*/
double step=newsize/(to-from);
double width=step; if(width<1) width=1;
if(to<0) to=0;
if(from>=n) from=n-1;
for(i=from;i<to; i++)
  FillSamples(newsize,out,(i-from+0.5)*step-0.5,in[i],width);
return newsize;
}


/*=== Function ===============================================*/

int Resize2DSamples(int ny, int nx, double **in, int newsizey,
    int newsizex, double **out, double fromy, double toy,
    double fromx, double tox)

/*--- Description --------------------------------------------//

Resize array in (of size n) and store it in out. out contains
newsize values, in the range from ... to.

This function uses FillSamples or InterpolateSamples.

Caution: If the sample sizes are not a multiple of each other
unwanted artefacts can appear.

Returns newsizex*newsizey.

//------------------------------------------------------------*/
{
int xi,yi;
double xstep=newsizex/(tox-fromx);
double xwidth=xstep; if(xwidth<1) xwidth=1;
double ystep=newsizey/(toy-fromy);
double ywidth=ystep; if(ywidth<1) ywidth=1;
if(tox<0) tox=0;
if(fromx>=nx) fromx=nx-1;
if(toy<0) toy=0;
if(fromy>=ny) fromy=ny-1;
for(xi=fromx;xi<tox; xi++)
  for(yi=fromy;yi<toy;yi++)
    Fill2DSamples(newsizey,newsizex,out,(yi-fromy+0.5)*ystep-0.5,(xi-fromx+0.5)*xstep-0.5,in[yi][xi],ywidth,xwidth);
return newsizex*newsizey;
}


/*==> Properties of arrays <==================================//

At index based operations first channel is 0,
if are specified with start stop the elements of start and stop
are included in the range.

//------------------------------------------------------------*/

/*=== Function ===============================================*/

double IntegrateSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Return the sum of array elements within of the range start
and and stop.

//------------------------------------------------------------*/
{
  int i;
  double sum=0;
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;
  for(i=start;i<=stop;i++) sum+=in[i];
  return sum;
}


/*=== Function ===============================================*/

double CenterOfGravityOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Returns the center of gravity (cog) of the range start and stop.
All element must be greater than 0. there is no check within
of the function that this is fulfilled.

If the sum of the given range is 0 the function returns 0.


//------------------------------------------------------------*/

// data  must be >0

{
  int i;
  double isum=0;
  double sum=0;
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;
  for(i=start;i<=stop;i++) isum+=in[i]*i,sum+=in[i];
  if(sum!=0) return isum/sum;
  return 0;
}

/*=== Function ===============================================*/

double PositiveCogOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Returns the center of gravity (cog) of the range start and stop.
All element less than 0 are replaced by 0.

If the sum of the given range is 0 the function returns 0.

//------------------------------------------------------------*/
{
  int i;
  double isum=0;
  double sum=0;
  double v,min;
  if(start<0) start=0;
  if(start>n-1) start=n-1;
  if(stop>n-1) stop=n-1;
  min=in[start];
  for(i=start;i<=stop;i++) min=(in[i]<min)?in[i]:min;
  for(i=start;i<=stop;i++)
  {
    v=in[i]-min;
    // if(v<0) v=0;
    if(debug>5) fprintf(stderr,"PositiveCenterOfGravity: i %d in %g v %g\n",i,in[i],v);
    isum+=v*i;
    sum+=v;
  }
  if(sum!=0) v=isum/sum;
  else v=0;
  if(debug>4) fprintf(stderr,"PositiveCenterOfGravity: start %d stop %d sum %g isum %g cog %g\n",
     start,stop,sum,isum,v);
  return v;
}


/*=== Function ===============================================*/

int MomentsOfSamples(int n, double *data, int start, int stop,
    double *sum, double *cog, double *rms)

/*--- Description --------------------------------------------//

Return the first three moments of a distribution given as array
in data (size n). The moments sum cog (center of gravity) are
calculated from the positive data values. Negative values are
set to zero.

If sum cog or rms is 0 pointer the corresponding value is not
returned.

On error (sum is 0 ...) 0 is returned, on success n.

//------------------------------------------------------------*/

{
  int i;
  double isum=0;
  double icog=0;
  double icogrms=0;
  double psum=0;
  double pcog=0;
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;

  for(i=start;i<=stop;i++)
  {
    double x=data[i];
    if(x<0) x=0;
    psum+=x, pcog+=x*i;
  }
  // fprintf(stderr,"mos start stop %d %d sum %g psum %g nsum %g \n",start,stop,isum,psum,nsum);
  if(psum<=0) goto error;
  icog=pcog/psum;
  isum=psum;

  for(i=start;i<=stop;i++)
  {
    double x=data[i];
    if(x<0) x=0;
    icogrms+=(i-icog)*(i-icog)*x;
  }
  icogrms=sqrt(icogrms/isum);

  if(debug>3) fprintf(stderr,"MomentsOfSamples: from %d to %d sum %g cog %g rms %g\n",
     start,stop,isum,icog,icogrms);
  if(sum) *sum=isum;
  if(rms) *rms=icogrms;
  if(cog) *cog=icog;
  return n;

  error:
  if(sum) *sum=0;
  if(rms) *rms=0;
  if(cog) *cog=0;
  return 0;
}


/*=== Function ===============================================*/

int SteadyMomentsOfSamples(int n, double *data, double start, double stop,
    double *sum, double *cog, double *rms)

/*--- Description --------------------------------------------//

Return the first three moments of a distribution given as array
in data (size n). The moments sum cog (center of gravity) are
calculated from the positive data values. Negative values are
set to zero.

This function makes sure that start/stop can be double and
that it returns a corrected value for this situation. Particular
for the rms this can be crucial in certain situatio to avoid
a discret distribution.

If sum cog or rms is 0 pointer the corresponding value is not
returned.

On error (sum is 0 ...) 0 is returned, on success n.

//------------------------------------------------------------*/

{
  int i;
  double isum=0;
  double icog=0;
  double icogrms=0;
  double psum=0;
  double pcog=0;
  if(start<0) start=0;
  if(stop<0) stop=0;
  if(start>n-1) start=n-1;
  if(stop>n-1) stop=n-1;
  if(stop<start) stop=start;

  int istart=start;
  int istop=stop;
  double fstart=1.0+(double)istart-start;
  double fstop=stop-(double)istop;

  for(i=istart+1; i<istop; i++)
  {
    double x=data[i];
    if(x<0) x=0;
    psum+=x, pcog+=x*i;
  }

  psum+=fstart*data[istart]+fstop*data[istop];
  pcog+=fstart*data[istart]*(istart)+fstop*data[istop]*(istop);

  if(psum<=0) goto error;
  icog=pcog/psum;
  isum=psum;

  //fprintf(stderr,"mos start stop %g %g istart istop %d %d fstart fstop %g %g sum %g psum %g cog %g pcog %g \n",start,stop,istart, istop, fstart,fstop,isum,psum,icog,pcog);

  for(i=istart+1;i<istop;i++)
  {
    double x=data[i];
    if(x<0) x=0;
    icogrms+=(i-icog)*(i-icog)*x;
  }

  icogrms+=fstart*data[istart]*(istart-icog)*(istart-icog)+fstop*data[istop]*(istop-icog)*(istop-icog);

  icogrms=sqrt(icogrms/isum);

  if(sum) *sum=isum;
  if(rms) *rms=icogrms;
  if(cog) *cog=icog;
  return n;

  error:
  if(sum) *sum=0;
  if(rms) *rms=0;
  if(cog) *cog=0;
  return 0;
}


/*=== Function ===============================================*/

int FindMaximumOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Find the maximum of array in (size n) in the range start stop
and return it. If the maximum is flat the middle of the flat region
is returned.

The function returns the index position of the element.

//------------------------------------------------------------*/
{
  int i;
  double max;
  int imax;
  if(start<0) start=0;
  if(start>n-1) start=n-1;
  if(stop>n-1) stop=n-1;
  if(debug>3) fprintf(stderr,"FindMaximumOfSamples: from %d to %d \n",start,stop);
  max=in[start];
  imax=start;
  for(i=start;i<=stop;i++) if(in[i]>max) max=in[i],imax=i;
  for(i=imax+1;i<=stop;i++) if(in[i]!=max) break;
  i--;
  if(i!=imax)
  {
    int meanmax=(i+imax)/2;
    if(debug>2) fprintf(stderr,"FindMaximumOfSamples: flat maximum from %d to %d => %d \n",
      imax,i,meanmax);
    imax=meanmax;
  }
  return imax;
}


/*=== Function ===============================================*/

int FindMinimumOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Find the mininum of array in (size n) in the range start stop
and return it. If the minimum is flat the middle of the flat region
is returned.

The function returns the index position of the element.

//------------------------------------------------------------*/
{
  int i;
  double max;
  int imax;
  if(start<0) start=0;
  if(start>n-1) start=n-1;
  if(stop>n-1) stop=n-1;
  max=in[start];
  imax=start;
  for(i=start;i<=stop;i++) if(in[i]<max) max=in[i],imax=i;
  return imax;
}


/*=== Function ===============================================*/

int FindLocalMaximumOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Find the first local maximum of array in (size n) in the
range start stop.

The function returns the index position of the element.

//------------------------------------------------------------*/
{
  int i=0;
  int imax=0;
  if(start<1) start=1;
  if(stop<1) stop=1;
  if(start>n-2) start=n-2;
  if(stop>n-2) stop=n-2;
  if(start==stop) return stop;
  else if(stop>start)
  {
    for(i=start;i<=stop;i++) if(in[i-1]<in[i] && in[i+1]<in[i]) break;
  }
  else if(stop<start)
  {
     for(i=start;i>=stop;i--) if(in[i-1]<in[i] && in[i+1]<in[i]) break;
  }
  imax=i;
  return imax;
}


/*=== Function ===============================================*/

int FindLocalMinimumOfSamples(int n, double *in, int start, int stop)

/*--- Description --------------------------------------------//

Find the first local minimum of array in (size n) in the
range start stop.

The function returns the index position of the element.

//------------------------------------------------------------*/
{
  int i=0;
  int imin=0;
  if(start<1) start=1;
  if(stop<1) stop=1;
  if(start>n-2) start=n-2;
  if(stop>n-2) stop=n-2;
  if(start==stop) return stop;
  else if(stop>start)
  {
    for(i=start;i<=stop;i++) if(in[i-1]>=in[i] && in[i+1]>in[i]) break;
  }
  else if(stop<start)
  {

    for(i=start;i>=stop;i--) if(in[i-1]>in[i] && in[i+1]>=in[i]) break;
  }
  imin=i;
  return imin;
}


/*=== Function ===============================================*/

double IntegralRangeOfSamples(int n,double *in,
       double low, double hig, double *lop, double *hip)

/*--- Description --------------------------------------------//

Finds the position in the array in of size n, where the integral
reaches low and high as a fraction of the total integral of the
array (no normalization required). If passed the exact positions
are stored to lop (low) and hip (high). If the total integral is
negative lop and hip are switched.

The exact position is interpolated linearly. For more accurate
results, first the high position is found and then low backwards
searched.

If array in is the derivative of a continously increasing
function then the return value is the risetime from low to high.

The function returns the difference between the two positions
(hip-lop), which is negative if the total interal is negative.

//------------------------------------------------------------*/
{
  double ll=0, hl=n;
  int kk=0, sig=1; //round down min
  double igr=IntegrateSamples(n,in,0,n);
  if(hig<0){ hig=-hig; }
  if(low<0){ low=-low; }
  if(igr<0){ sig=-1; igr=-igr; }
  low*=igr; hig*=igr; igr=0;

  //lets go forward find hig first much saver
  for(kk=0; kk<n; kk++){ igr+=sig*in[kk]; if(igr>hig) break; }
  if(kk>n-1) kk=n-1;
  igr-=sig*in[kk];
  hl=kk-1.0+(hig-igr)/in[kk];


  //now lets go back and find low
  for(; kk>=0; kk--){ igr-=sig*in[kk]; if(igr<low ) break; }
  if(kk<0) kk=0;
  ll=kk-1.0+(low-igr)/in[kk];

 //if negative pulse return negative rise time
  if(sig<0){ double tmp=ll; ll=hl, hl=tmp; }
  if(lop) *lop=ll;
  if(hip) *hip=hl;
  return hl-ll;
}


/*=== Function ===============================================*/

double FindLowerBoundOfSamples(int n, double *in, double low, double max)

/*--- Description --------------------------------------------//

Finds the position in the array in of size n, where the integral
reaches low. It requires that this position is below max.
It works with positive and negative pulses.

The exact position is interpolated linearly.

If array in is the derivative of a continously increasing
function then the return value is the time where the pulse crosses
low as a fraction of the total pulse height.

//------------------------------------------------------------*/
{
  int kk, sig=1, imax=(int)(max+1.0); //roung up max
  if(imax<1) imax=1;
  if(imax>n) imax=n;
  double igr=IntegrateSamples(n,in,0,n);
  if(igr==0) return 0;
  if(low<0){ low=-low; }
  if(igr<0){ sig=-1; igr=-igr; }
  low*=igr;
  for(kk=n-1; kk>imax; kk--) igr-=sig*in[kk];
  for(;kk>=0;kk--){ igr-=sig*in[kk]; if(igr<low ) break; }
  if(kk<0) kk=0;
  if(debug>5) fprintf(stderr, "kk %d low %g igr %g in %g ll %g\n", kk-1, low, igr, in[kk], (low-igr)/in[kk]);
  return kk-1.0+(low-igr)/in[kk]; //in[kk]=y2-y1; y1=igr; low=y3; x1=0; x2=1; we require in[kk]>0 but that is necessary to reach the stop condition above, if in[kk]==0 then igr does not change
}


/*=== Function ===============================================*/

double FindUpperBoundOfSamples(int n,double *in, double hig, double min)

/*--- Description --------------------------------------------//

Finds the position in the array in of size n, where the integral
reaches hig as a fraction of the total integral of the array (no
normalization required). It requires that this position is above
min. It works with positive and negative pulses.

The exact position is interpolated linearly.

If array in is the derivative of a continously increasing
function then the return value is the time where the pulse crosses
hig as a fraction of total the pulse height.

//------------------------------------------------------------*/
{
  int kk, sig=1, imin=(int)(min); //round down min
  if(imin<0) imin=0;
  if(imin>n) imin=n-1;
  double igr=IntegrateSamples(n,in,0,n);
  if(igr==0) return n;
  if(hig<0){ hig=-hig; }
  if(igr<0){ sig=-1; igr=-igr; }
  hig*=igr; igr=0;

  //lets go forward
  for(kk=0; kk<imin; kk++) igr+=sig*in[kk];
  for(; kk<n; kk++){ igr+=sig*in[kk]; if(igr>hig) break; }
  if(kk>n-1) kk=n-1;
  igr-=sig*in[kk];
  if(debug>5) fprintf(stderr, "kk %d hig %g igr %g in %g ll %g\n",kk-1, hig, igr, in[kk], (hig-igr)/in[kk]);
  return kk-1.0+(hig-igr)/in[kk];
}


/*=== Function ===============================================*/

double FwhmOfSamples(int n, double *in, int peak)

/*--- Description --------------------------------------------//

Returns the FWHM of a peak in array in specified peak.

//------------------------------------------------------------*/
{
  int i;
  double right,left;
  if(peak<1) return 0;
  if(peak>n-2) return 0;
  double max=in[peak]/2;
  if(max<0) return 0;

  for(right=n,i=peak; i<n;  i++) if(in[i]<max)
  {
    double up=in[i-1];
    double down=in[i];
    right=i-1+(max-up)/(down-up);
    break;
  }
  for(left=0,i=peak; i>=0; i--) if(in[i]<max)
  {
    double up=in[i+1];
    double down=in[i];
    left=i+1-(max-up)/(down-up);
    break;
  }
  if(right-left < 0)
  {
    if(debug) fprintf(stderr,"FwhmOfSamples/ERROR:  peak left right  %d %g %g\n",peak,left,right);
  }
  return right-left;
}


/*=== Function ===============================================*/

int FwxmOfSamples(int n, double *in, int start, int stop,
    double fraction, double *m, double *l, double *r)

/*--- Description --------------------------------------------//

Returns the full with x maximum of a peak in the range of start
and stop of the array (size n) given by in.

fraction must be >0 and <1.

The function searches for the maximum in the range of start
and stop, multiplies the found maximum with fraction. This
value is then used to find the values on the left edge and right
edge of the data values.

The found maximum is returned in m, the left and right edge in
l and r.

The functions returns to position where the maximum is found.

E.g.: calculate the full width tenth maximum of data samples

int pos=FwxmOfSamples(1000,data,400,600,0.1,&m,&l,&r);

//------------------------------------------------------------*/
{
  int i;
  double right,left;
  int peak=FindMaximumOfSamples(n,in,start,stop);
  if(peak<1) return 0;
  if(peak>n-2) return 0;
  double max=in[peak]*fraction;
  if(max<0) return 0;

  for(right=n,i=peak; i<n;  i++) if(in[i]<max)
  {
    double up=in[i-1];
    double down=in[i];
    right=i-1+(max-up)/(down-up);
    break;
  }
  for(left=0,i=peak; i>=0; i--) if(in[i]<max)
  {
    double up=in[i+1];
    double down=in[i];
    left=i+1-(max-up)/(down-up);
    break;
  }
  if(right-left < 0)
  {
    if(debug) fprintf(stderr,"FwxmOfSamples/ERROR:  peak left right  %d %g %g\n",peak,left,right);
    return 0;
  }
  if(l) *l=left;
  if(r) *r=right;
  if(m) *m=in[peak];
  return peak;
}


/*=== Function ===============================================*/

int MeanAndRmsOfSamples(int n, double *data, int start,
    int stop, double *mean, double *rms)

/*--- Description --------------------------------------------//

Returns in mean and rms for a given range (start, stop)
the mean and rms of array data (size n).

If mean or rms is set to 0 pointer the corresponding value is
not returned.

The function returns 1;

//------------------------------------------------------------*/
{
  int i;
  double sum=0;
  double imean=0;
  double irms=0;
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;
  if(stop<start) stop=start; //division by zero

  for(sum=0,i=start;i<=stop; i++) sum+=data[i];
  imean=sum/(stop-start+1);
  for(irms=0,i=start;i<=stop; i++) irms+=(data[i]-imean)*(data[i]-imean);
  irms=sqrt(irms/(stop-start+1));
  if(mean) *mean=imean;
  if(rms) *rms=irms;
  // fprintf(stderr,"sum %g mean %g rms %g \n",sum,imean,irms);
  return 1;
}


/*=== Function ===============================================*/

int SamplesToAverageAndRMS(int n, double x, double *avg,
     double *rms)

/*--- Description --------------------------------------------//

This function computes from a continuous subset of values (x)
the average and rms (avg, rms) or values x.

n ist the number of the value you add to the subset and must be
incremented by each call and must start with a value of 1.

The function returns n.

//------------------------------------------------------------*/
{
   double irms,iavg;
   double a=*avg;
   double r=*rms;
   if(n<=1) { *avg=x, *rms=0; return n; }
   iavg=(a)*(n-1);              // old x-sum
   irms=(r*r+a*a)*(n-1);        // old x*x-sum
   //fprintf(stderr,"old-rs n %d x %g iavg %g irms %g\n",n-1,x,iavg,irms);
   iavg+=x;
   irms+=x*x;
   //fprintf(stderr,"new-rs n %d x %g iavg %g irms %g\n",n,x,iavg,irms);
   *avg=iavg/n;
   *rms=((irms)/n-(*avg)*(*avg) );
   if(*rms<=0) *rms=0;
   *rms=sqrt(*rms);
   //fprintf(stderr,"new n %d avg %g rms %g\n",n,*avg,*rms);
   return n;
}


/*==> Triggers (searching edges) <============================*/

/*=== Function ===============================================*/

int TriggerSamplesLeadingEdge(int n, double *in, int start,
    double thr)

/*--- Description --------------------------------------------//

Scan array in (size n) and find first element < thr.
Start from this position to find an element >=thr.

Returns the position of the element, or 0 if no element is found.

//------------------------------------------------------------*/
{
  int i;
  // fprintf(stderr,"letrigger at %d thr %g\n",start,thr);
  if(start<0) start=0;
  for (i=start; i<n; i++) if(in[i]<thr) break;
  for (; i<n; i++) if(in[i]>=thr) break;
  if(i>=n) return 0; // no trigger found
  return i;
}


/*=== Function ===============================================*/

int TriggerSamplesFallingEdge(int n, double *in, int start,
    double thr)

/*--- Description --------------------------------------------//

Scan array in (size n) and find first element > thr.
Start from this position to find an element <=thr.

Returns the position of the element, or 0 if no element is found.

//------------------------------------------------------------*/
{
  int i;
  // fprintf(stderr,"fetrigger at %d thr %g\n",start,thr);
  if(start<0) start=0;
  for (i=start; i<n; i++) if(in[i]>thr) break;
  for (; i<n; i++) if(in[i]<=thr) break;
  if(i>=n) return 0; // no trigger found
  return i;
}


/*=== Function ===============================================*/

int TriggerSamplesMaxLeadingEdge(int n, double *in, int start,
    double thr)

/*--- Description --------------------------------------------//

Scan array in (size n) and find first element < thr.
Start from this position to find an element >=thr.
This position is used to search the maximum >thr.

Returns the position of this element, or 0 if no element is found.

//------------------------------------------------------------*/
{
  int i;
  double max=thr;
  int maxi;
  if(start<0) start=0;
  // fprintf(stderr,"letrigger at %d thr %g\n",start,thr);
  for (i=start; i<n; i++) if(in[i]<thr) break;
  for (; i<n; i++) if(in[i]>=thr) break;
  if(i>=n) return 0; // no trigger found
  for(maxi=i; i<n; i++)
  {
    if(in[i]<thr) break;
    if(in[i]>=max) { max=in[i]; maxi=i; }
  }
  if(i>=n) return 0; // no trigger found
  return maxi;
}


/*=== Function ===============================================*/

double CFDFilter(int n, double *in, int d, double f, int stop)

/*--- Description --------------------------------------------//

Constant fraction discriminator. It requires a differentiated
and smoothed signal (in) and calculates the zero crossing of a
signal overlayed with a delayed copy (d) of itself scaled by -f.
The filter looks for the absolute maximum and thus can handle
both positive and negative pulses. The zero crossing has to be
to the left of stop (or slightly to the right if stop is just
before the cross over).

It interpolates around the zero crossing and returns the
zero crossing value.

The filter fires at d/(1-f) according to H. Spieler
(doi:10.1093/acprof:oso/9780198527848.001.0001), with the
requirement d<(1-f)tr and an optimal shaping of its input
is roughly the rise time of the signal.

//------------------------------------------------------------*/
{
  int i=0, max=n-1, sig=1; double mval=0, cval=0;
  stop+=d;
  for(i=n-1; i>=d; i--)
  {
    cval=(in[i-d]-f*in[i]);
    if(cval>mval){ mval=cval; max=i; }
    else if(cval<-mval){ mval=-cval; max=i; }
  }
  if(in[max-d]-f*in[max]<0) sig=-1;
  if(stop>0 && stop<n)
  {
    while(sig*(in[stop-d]-f*in[stop])<0) stop++;
    if(stop<max) max=stop;
  }
  //mval=0; for(i=d; i<max-2*d; i++) mval+=sig*in[i]; //find baseline for baseline correction
  //for(i=max; i>=3*d; i--)
  for(i=max; i>=d; i--)
  {
    if(sig*(in[i-d]-f*in[i])<0){ break; } //mval*(1-f)/(i-3*d)) { break; }
    //mval-=sig*in[i-2*d];
  }
  return i - (in[i-d]-f*in[i])/(in[i-d+1]-f*in[i+1]-in[i-d]+f*in[i]) - d/(1-f);
}


/*=== Function ===============================================*/

int CFDShaper(int n, double *in, double *out, int d, double f)

/*--- Description --------------------------------------------//

Constant fraction discriminator shaping filter. It requires a
differentiated and smoothed signal (in) and calculates the
signal overlayed with a delayed copy (d) of itself scaled by -f.
The filter looks for the absolute maximum and thus can handle
both positive and negative pulses. The zero crossing has to be
to the left of stop (or slightly to the right if stop is just
before the cross over).

//------------------------------------------------------------*/
{
  int i=0, max=n-1, sig=1, offset=d/(1.0-f); double mval=0, cval=0;
  for(i=n-1; i>=d; i--)
  {
    cval=(in[i-d] - f*in[i]);
    if(cval>mval){ mval=cval; max=i; }
    else if(cval<-mval){ mval=-cval; max=i; }
  }
  if(in[max-d] - f*in[max] < 0) sig=-1;
  for(i=n-1; i>=d; i-- ) out[i]=sig*(in[i-d] - f*in[i]);
  for(i=offset; i<n; i++) out[i-offset]=out[i];
  return n-d;
}

/*=== Function ===============================================*/

int DifferenceOfSamples(int n, double *in, double *out)

/*--- Description --------------------------------------------//

Differentiate the array in (size n) and store it in out.

WARNING: The first element of out is always the first element
of in, hence contains the information about the offset and
should be ignored for most applications.

The function returns n.

//------------------------------------------------------------*/
{
  int i;
  double last=0;
  double next=0;
  for(i=0; i<n; i++) next=in[i],out[i]=next-last,last=next;
  return n;
}

/*=== Function ===============================================*/

int IntegralOfSamples(int n, double *in, double *out)

/*--- Description --------------------------------------------//

Integrate the array in (size n) and store the values in out.

The function returns n.

//------------------------------------------------------------*/
{
  int i;
  double sum=0;
  for(i=0; i<n; i++) sum+=in[i],out[i]=sum;
  return n;
}


/*==> Smoothing Filters <=====================================*/

/*=== Function ===============================================*/

int SmoothSamples(int n, double *input, double *output,
    int m, double w)

/*--- Description --------------------------------------------//

Multi path smooth array in (size n) m times by the width w.

w can be any fractional part of a channel >1. This allows
subchannel smoothing.

The function is slow and returns n.

//------------------------------------------------------------*/
{
  int i;
  double *in=input;
  double *out=output;
  double sum,f,*l,*r;
  int iw,is,start,stop,o;
  double ledge,redge;
  if(w<1) w=1;
  iw=floor((w-1)/2);
  is=2*iw+1;
  f=(w-is)/2;
  // printf("f=%g\n",f);
  // if(f<=0) f=1,iw--,is-=2; // enable if you dont want int chan shaping

  if(debug>3) fprintf(stderr,"SmoothSamples: loops %d width %g isum %d\n",m,w,iw);

  if(w>n-1)
  {
    for(sum=i=0; i<n; i++) sum+=in[i];
    for(i=0; i<n; i++) out[i]=sum/n;
    return n;
  }

  while(m-->0)
  {
  ledge=InterpolateSamples(n,in,0,w);
  redge=InterpolateSamples(n,in,n-1,w);
  if(f>0) // subchannel shaping
  {
    start=iw+1;
    stop=n-iw-2;
    if(debug>3) fprintf(stderr,"SmoothSamples: ... width %g start stop %d %d +-win %d isum %d lrfracs %g\n",
      w,start,stop,iw,is,f);
    for(sum=0,i=start-iw; i<=start+iw; i++) sum+=in[i]; // init sum
    for(o=0,i=start,l=&in[i-iw-1],r=&in[i+iw+1]; i<=stop; i++) // run the middle fast
    {
      out[o++]=(sum+((*l++)+(*r))*f)/w;
      sum=sum+(*r++)-*l;
    }
    for(i=stop;i>=0;i--) out[i+iw+1]=out[i]; // shift
    // for(i=0;i<start;i++) out[i]=out[start]; // leftedge
    for(i=0;i<start;i++) out[i]=(ledge*(start-i)+out[start]*i)/start; // leftedge
    // for(i=stop+1;i<n;i++) out[i]=out[stop]; // right edge
    for(i=stop+1;i<n;i++) out[i]=(out[stop]*(n-i-1)+redge*(i-stop))/(n-stop-1); // right edge
  }
  else // faster integer channel shaping 1,3,5,....
  {
    start=iw;
    stop=n-iw-1;
    if(debug>3) fprintf(stderr,"SmoothSamples: ... width %g start stop %d %d +-win %d isum %d lrfracs %g\n",
       w,start,stop,iw,is,f);
    for(sum=0,i=start-iw; i<=start+iw; i++) sum+=in[i]; // init sum
    for(o=0,i=start,l=&in[i-iw],r=&in[i+iw+1]; i<=stop; i++) // run the middle fast
    {
      double v=sum/w;
      sum=sum+(*r++)-*l++;
      out[o++]=v;
    }
    for(i=stop;i>=0;i--) out[i+iw]=out[i]; // shift
    // for(i=0;i<start; i++) out[i]=out[start]; // leftedge
    for(i=0;i<start;i++) out[i]=(ledge*(start-i)+out[start]*i)/start; // leftedge
    // for(i=stop+1;i<n; i++) out[i]=out[stop]; // right edge
    for(i=stop+1;i<n;i++) out[i]=(out[stop]*(n-i-1)+redge*(i-stop))/(n-stop-1); // right edge
  }
  in=out;
  }
  return n;

}


/*=== Function ===============================================*/

int SmoothSamplesSlow(int n, double *input, double *output,
    int m, double w)

/*--- Description --------------------------------------------//

Same as SmoothSamples but much slower algo with better edges.

//------------------------------------------------------------*/

{
  int i=0;
  double *tmp=(double*)malloc(n*sizeof(double));
  double *in=input;
  double *out=tmp;
  if(debug>3) fprintf(stderr,"SmoothSamplesSlow: loops %d width %g \n",m,w);
  while(m-->0)
  {
    for(i=0;i<n; i++) out[i]=InterpolateSamples(n,in,i,w);
    CopySamples(n,tmp,output);
    in=output;
  }
  free(tmp);
  return n;
}


/*=== Function ===============================================*/

int Smooth2DSamples(int ny, int nx,  double **input,
    double **output, int m, double wy, double wx)

/*--- Description --------------------------------------------//

Multi path smooth array in (size nx and size ny) m times by the width w.
The function is slow and returns n.

//------------------------------------------------------------*/
{
  int i,j;
  double *ay=AllocateSamples(ny);
  // fprintf(stderr,"**** nx %d ny %d wx %g wy %g m %d\n",nx,ny,wx,wy,m);
  if(wx) for(j=0; j<ny; j++) SmoothSamples(nx,input[j],output[j],m,wx);
  if(wy)
  {
    for(i=0; i<nx; i++)
    {
      for(j=0; j<ny; j++) ay[j]=output[j][i];
      SmoothSamples(ny,ay,ay,m,wy);
      for(j=0; j<ny; j++) output[j][i]=ay[j];
    }
  }
  FreeSamples(ay);
  return 1;
}


/*=== Function ===============================================*/

int Smooth2DSamplesSlow(int ny, int nx,  double **input,
    double **output, int m, double wy, double wx)

/*--- Description --------------------------------------------//

Multi path smooth array in (size nx and size ny) m times by the width w.
The function is slow and returns n.

//------------------------------------------------------------*/
{
  int i,j;
  double *ay=AllocateSamples(ny);
  // fprintf(stderr,"**** nx %d ny %d wx %g wy %g m %d\n",nx,ny,wx,wy,m);
  for(j=0; j<ny; j++) SmoothSamplesSlow(nx, input[j], output[j], m, wx);
  for(i=0; i<nx; i++)
  {
    for(j=0; j<ny; j++) ay[j]=output[j][i];
    SmoothSamplesSlow(ny, ay, ay , m, wy);
    for(j=0; j<ny; j++) output[j][i]=ay[j];
  }
  FreeSamples(ay);
  return 1;
}

/*=== Function ===============================================*/

int CausalAverageSamplesFilter(int n, double *in, double *out, int m, int r)

/*--- Description --------------------------------------------//

Multi path smooth array in (size n) m times by the width r. The filter
only moves from the right to the left and thus conserves causiality.

r must be an integer. No subchannel smoothing allowed.
Outside elements of the array, which needs to be known for computation,
are replaced by the edges of the in array and are linear interpolated
to the first and last computable element.

The function returns n.

//------------------------------------------------------------*/
{
  int i;
  int w=r;
  double sum=0;
  double newv=0;
  if(w<1) return 0;
  if(debug>3) fprintf(stderr,"CausalSamplesFilter: loops %d width %d \n",m,w);
  double le;
  while(m-->0)
  {
  for(le=i=0;i<w;i++) le+=in[i]/w;
  for(sum=0,i=n-1; i>n-1-w; i--) sum+=out[i];
  for(i=n-1; i>w-1; i--) { newv=sum/w; sum+=out[i-w]-out[i]; out[i]=newv; }
  for(i=0;i<w;i++) out[i]=(le*(w-i)+out[w]*i)/w; // leftedge
  in=out;
  }
  return n;
}

/*=== Function ===============================================*/

int AverageSamplesFilter(int n, double *input, double *output,
    int m, int w)

/*--- Description --------------------------------------------//

Multi path smooth array in (size n) m times by the width w.

w must be an integer. No subchannel smoothing allowed.

The function returns n-m*w. The maximum number of channels
which can be computed without knowing the edges.

//------------------------------------------------------------*/
{
  int i;
  double *in=input;
  double *out=output;
  double sum;
  double last;
  while(m-->0)
  {
  // fprintf(stderr,"loop  %d n %d \n",m,n);
  for(sum=0,i=0; i<w-1; i++) sum+=in[i];
  for(last=0,i=0; i<n-w; i++)
  {
    last=in[i];
    sum+=in[i+w-1];
    out[i]=sum/w;
    sum-=last;
  }
  for(;i<n;i++) out[i]=0;
  n-=w;
  in=output;
  }
  return n-w;
}


/*=== Function ===============================================*/

int LRAverageSamplesFilter(int n, double *in, double *out,
    int m, int r)

/*--- Description --------------------------------------------//

Multi path left right smooth array in (size n) m times by the width r.
Smoothing is applied first from left to right and then from right to left.
This retains the center of gravity of signals.

r must be an integer. No subchannel smoothing allowed.
Outside elements of the array, which needs to be known for computation,
are replaced by the edges of the in array and are linear interpolated
to the first an last computable element.

The function returns n.

//------------------------------------------------------------*/
{
  int i,ie;
  int w=r;
  double sum=0;
  double newv=0;
  if(w<1) return 0;
  if(debug>3) fprintf(stderr,"LRAverageSamples: loops %d width %d \n",m,w);
  double le,re;
  while(m-->0)
  {
  for(le=i=0;i<w;i++) le+=in[i]/w;
  for(re=i=0;i<w;i++) re+=in[n-i-1]/w;
  for(sum=0,i=0; i<w; i++) sum+=in[i];
  for(i=0; i<n-w; i++) { newv=sum/w; sum+=in[i+w]-in[i]; out[i]=newv; }
  ie=i-1;
  for(sum=0,i=ie; i>ie-w; i--) sum+=out[i];
  for(i=ie; i>w-1; i--) { newv=sum/w; sum+=out[i-w]-out[i]; out[i]=newv; }
  for(i=0;i<w;i++) out[i]=(le*(w-i)+out[w]*i)/w; // leftedge
  for(i=0;i<w;i++) out[n-1-i]=(re*(w-i)+out[n-1-w]*i)/w; // rightedge
  in=out;
  }
  return n;
}


/*=== Function ===============================================*/

int LRAverageSamplesFilterFlatEdges(int n, double *in, double *out,
    int m, int r)

/*--- Description --------------------------------------------//

Multi path left right smooth array in (size n) m times by the width r.
Smoothing is applied first from left to right and then from right to left.
This retains the center of gravity of signals.

r must be an integer. No subchannel smoothing allowed.
Outside elements of the array, which needs to be known for computation,
are replaced by a constant value of the first and last computable element.

The function returns n.

//------------------------------------------------------------*/
{
  int i,ie;
  int w=r;
  double sum=0;
  double newv=0;
  if(w<1) return 0;
  if(debug>3) fprintf(stderr,"LRAverageSamples: loops %d width %d \n",m,w);
  while(m-->0)
  {
  for(sum=0,i=0; i<w; i++) sum+=in[i];
  for(i=0; i<n-w; i++) { newv=sum/w; sum+=in[i+w]-in[i]; out[i]=newv; }
  ie=i-1;
  for(sum=0,i=ie; i>ie-w; i--) sum+=out[i];
  for(i=ie; i>w-1; i--) { newv=sum/w; sum+=out[i-w]-out[i]; out[i]=newv; }
  for(;i>=0;i--) out[i]=out[w];
  for(i=ie+1;i<n;i++) out[i]=out[ie];
  in=out;
  }
  return n;
}


/*=== Function ===============================================*/

int LRSumSamplesFilter(int n, double *in, double *out, int m, int r)

/*--- Description --------------------------------------------//

Same as LRAverageSamplesFilter, using a running sum instead of
average.

Outside elements of the array, which needs to be known for computation,
are replaced by 0. This saves time in computation and if not sufficient
you must extend you in array by samples you may think they are the best.

The function is slightly faster than the average filters.
The function returns n.

//------------------------------------------------------------*/

{
  int i;
  int w=r;
  double sum=0;
  double newv=0;
  if(w<1) return 0;
  if(debug>3) fprintf(stderr,"LRAverageSamples: loops %d width %d \n",m,w);
  while(m-->0)
  {
  for(sum=0,i=0; i<w; i++) sum+=in[i];
  for(i=0; i<n-w-1; i++) newv=sum, sum+=in[i+w]-in[i], out[i]=newv;
  for(; i<n; i++)        newv=sum, sum+=-in[i], out[i]=newv;
  for(sum=0,i=n-1; i>n-1-w; i--) sum+=out[i];
  for(i=n-1; i>=w; i--)  newv=sum, sum+=out[i-w]-out[i],out[i]=newv;
  for(; i>=0; i--)       newv=sum, sum+=-out[i],out[i]=newv;
  in=out;
  }
  return n;
}


/*==> Natural Filters <=======================================*/

/*=== Function ===============================================*/

double DecaySamplesFilter(double f)

/*--- Description --------------------------------------------//

Returns the lowpass decay constant for a frequency f.

//------------------------------------------------------------*/
{
  return pow(Euler,-2*Pi*f);
}


/*=== Function ===============================================*/

double CutOffSamplesFilter(double d)

/*--- Description --------------------------------------------//

Returns the cutoff frequency of decay d.

//------------------------------------------------------------*/
{
  return -log(d)/(2*Pi);
}


/*=== Function ===============================================*/

double LowPassSamplesFilter(int n, double *in, double *out,
       double d, double offset)

/*--- Description --------------------------------------------//

Apply a lowpass filter to the array in (size n) and store it in out.
d is the decay constant (see DSP books). offset is the previous
value in front of the trace. This allows to compute output arrays
in a consecutive way because the last value of the out array can
be used as offset.

The function returns the last element of the out array.

//------------------------------------------------------------*/
{
  int i;
  double a0=1-d;
  double b1=d;
  double last=offset;
  for(i=0; i<n; i++) last=out[i]=a0*in[i]+b1*last;
  return last;
}


/*=== Function ===============================================*/

double InverseLowPassSamplesFilter(int n, double *in, double *out,
       double d, double offset)

/*--- Description --------------------------------------------//

Same as LowPassSamplesFilter but inverts the function.

//------------------------------------------------------------*/
{
  int i;
  double last;
  double gain=(1/(1-d)-1);
  out[0]=in[0]+gain*(in[0]-offset);

  for(last=in[0],i=1; i<n; i++)
  {
    double newv=in[i]+gain*(in[i]-last);
    last=in[i];
    out[i]=newv;
  }
  return last;

  // non reentrant code
  // for(i=1; i<n; i++) out[i]=in[i]+gain*(in[i]-in[i-1]);
  // return in[i-1];
}


/*=== Function ===============================================*/

int MultiLowPassSamplesFilter(int n, double *in, double *out,
    int pass, double d, double offset)

/*--- Description --------------------------------------------//

Same as LowPassSamplesFilter but runs multiple lowpass filters
on array in and store result in out.

pass is the number of times the filter is applied (poles).

Returns the number of pass+1.

//------------------------------------------------------------*/
{
  int i;
  LowPassSamplesFilter(n,in,out,d,offset);
  for(i=0;i<pass-1;i++)   LowPassSamplesFilter(n,out,out,d,offset);
  return i+1;
}


/*=== Function ===============================================*/

int LRLowPassSamplesFilter(int n, double *in, double *out, double d)

/*--- Description --------------------------------------------//

Run a single Left right right left low pass filter with the decay
constant d.

This is the non causal CUSP function.

Returns 1.

//------------------------------------------------------------*/
{
  int i;
  double a0=1-d;
  double b1=d;
  double last=in[0];
  for(i=0; i<n; i++) last=out[i]=a0*in[i]+b1*last;
  last=in[n-1];
  for(i=n-1; i>=0; i--) last=out[i]=a0*out[i]+b1*last;
  return 1;
}


/*=== Function ===============================================*/

double HighPassSamplesFilter(int n, double *in, double *out,
       double d, double offset)

/*--- Description --------------------------------------------//

Same as LowPassSamplesFilter but applies a highpass filter.

//------------------------------------------------------------*/
{
  int i;
  double a0=(1+d)/2;
  double a1=-a0;
  double b1=d;
  double lastout=0;
  double lastin=offset;
  for(i=0; i<n; i++)
  {
    lastout=a0*in[i] + a1*lastin + b1*lastout;
    lastin=in[i];
    out[i]=lastout;
  }
  return (lastin*a1+b1*lastout)/a1;
}


/*=== Function ===============================================*/

double InverseHighPassSamplesFilter(int n, double *in, double *out,
       double d, double offset)

/*--- Description --------------------------------------------//

Same as InverseLowPassSamplesFilter but applies an inverse
highpass filter.

//------------------------------------------------------------*/
{
  int i;
  double gain=d/(1-d);
  double mult=2*(1-d)/(1+d);
  double sum=offset/mult;
  for(i=0; i<n; i++)
  {
    double v=in[i];
    sum+=v;
    out[i]=(gain*v+sum)*mult;
  }
  // fprintf(stderr,"mult %g sum %g sum/mult %g sum*mult %g\n",mult,sum,sum/mult,sum*mult);
  return sum*mult;
}


/*=== Function ===============================================*/

int MultiHighPassSamplesFilter(int n, double *in, double *out,
    int pass, double d, double offset)

/*--- Description --------------------------------------------//

Same as MultiLowPassSamplesFilter but applies an
highpass filter.

//------------------------------------------------------------*/
{
  int i;
  HighPassSamplesFilter(n,in,out,d,offset);
  for(i=0;i<pass-1;i++)   HighPassSamplesFilter(n,out,out,d,offset);
  return i+1;
}


/*==> Linear Interpolation <==================================*/

/*=== Function ===============================================*/

int LinearInterpolateSamples(int n, double *in, double *out,
    int start, int stop)

/*--- Description --------------------------------------------//

Computes between start and stop element of the array given by in
(size n)  a linear interpolation and stores values in out.

Returns n.

//------------------------------------------------------------*/
{
  int i;
  int w=stop-start;
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;
  if((stop-start)<1) return 0;
  double l=in[start];
  double r=in[stop];
  for(i=start; i<=stop; i++) out[i]=(l+(r-l)/w*(i-start));
  return n;
}


/*=== Function ===============================================*/

int LinearSlopeAndOffsetOfSamples(int n, double *d,
    int start, int stop, double *slope, double *offset)

/*--- Description --------------------------------------------//

Interpolates between the start and stop element of the array given by in
(size n) and returns the corresponding slop and offset.

The function returns 1.

//------------------------------------------------------------*/
{
  if(start<0) start=0;
  if(stop>n-1) stop=n-1;
  if(stop-start < 1) return 0;

  int blmid=(start+stop)/2+0.5;
  int bl1min=start;
  int bl1max=blmid;
  int bl2min=blmid+1;
  int bl2max=stop;
  if((start-stop)%2==0) bl2min--;

  double bl1=IntegrateSamples(n,d,bl1min,bl1max)/(bl1max-bl1min+1);
  double bl2=IntegrateSamples(n,d,bl2min,bl2max)/(bl2max-bl2min+1);
  double bl1g=(bl1max+bl1min)/2.0;
  double bl2g=(bl2max+bl2min)/2.0;

  *slope=(bl1-bl2)/(bl1g-bl2g);
  *offset=bl1-(*slope)*bl1g;

   if(0)
   {
   fprintf(stderr,"so: start %d stop %d bl1 %d %d %d %g bl2 %d %d %d %g\n",
     start,stop,bl1min,bl1max,bl1max-bl1min+1,bl1g,bl2min,bl2max,bl2max-bl2min+1,bl2g);
   fprintf(stderr,"so: start %d stop %d bl %g %g slope %g offset %g\n",
     start,stop,bl1,bl2,*slope,*offset);
   }
   return 1;

}


/*==> DFT Discrete Fourier Transformation <===================*/

/*=== Function ===============================================*/

int ConvertPolarToComplexSamples(int n,  double *mag, double *phase,
    double *re, double *im)

/*--- Description --------------------------------------------//

Convert a n element complex array containing magnitude and phase
data to real and imaginary part.

returns n.

//------------------------------------------------------------*/
{
  int k;
  for (k=0; k<n; k++)
  {
    re[k] = mag[k] * cos( phase[k] );
    im[k] = mag[k] * sin( phase[k] );
  }
  return n;
}


/*=== Function ===============================================*/

int ConvertComplexToPolarSamples(int n, double *re, double *im,
    double *mag, double *phase)

/*--- Description --------------------------------------------//

Convert a n element complex array containing real and imaginary
data to magnitude and phase part.

returns n.

//------------------------------------------------------------*/
{
  int k;
  double m,p,r,i;
  for (k=0; k<n; k++)
  {
    r=re[k];
    i=im[k];
    m=sqrt(r*r + i*i);
    if(r==0) r=1E-20;
    p=atan(i/r);
    if(r<0 && i<0)   p=p-Pi;
    if(r<0 && i>= 0) p=p+Pi;
    mag[k]=m;
    phase[k]=p;
  }
  return n;
}


/*=== Function ===============================================*/

int MultiplyComplexSamples(int n, double *re1, double *im1,
    double *re2, double *im2,  double *reo, double *imo)

/*--- Description --------------------------------------------//

Multiply two complex arrays (re1,im1,re2,im2) with size n and store
output in reo and imo.

Returns n.

//------------------------------------------------------------*/
{
  int f;
  double r1,r2,i1,i2;
  for (f=0; f<n; f++)
  {
    r1=re1[f];
    r2=re2[f];
    i1=im1[f];
    i2=im2[f];
    reo [ f ] = r1 * r2 - i1 * i2;
    imo [ f ] = i1 * r2 + r1 * i2;
  }
  return n;
}


/*=== Function ===============================================*/

int DivideComplexSamples(int n, double *re1, double *im1,
    double *re2, double *im2,  double *reo, double *imo)

/*--- Description --------------------------------------------//

Divide two complex arrays (re1,im1,re2,im2) with size n and store
output in reo and imo.

Returns n.

//------------------------------------------------------------*/
{
  int f;
  double r1,r2,i1,i2;
  for (f=0; f<n; f++)
  {
    r1=re1[f];
    r2=re2[f];
    i1=im1[f];
    i2=im2[f];
    reo[f] = (r1*r2 + i1*i2) / (r2*r2 + i2*i2);
    imo[f] = (i1*r2 - r1*i2) / (r2*r2 + i2*i2);
  }
  return n;
}


/*=== Function ===============================================*/

int DFTOfSamples(int n, double *in, double *re, double *im)

/*--- Description --------------------------------------------//

Computes a DFT (Discrete Fourier Transformation) of n (must be
even) elements given by in and store result in re and im.

re and im contains n/2+1 elements.
n must be even out holds n/2+1 samples.

//------------------------------------------------------------*/
{
  int i,j;
  int on;
  if(n%2==1) n--;          // if n not even make it even
  on=n/2+1;                // output samples hold n/2+1 elements
  double norm=2.0/n;
  for(i=0;i<on;i++) re[i]=im[i]=0;
  for(i=0;i<on;i++)
  {
    for(j=0;j<n;j++)
    {
      re[i] += in[j] * cos(2*Pi*i*j/n);
      im[i] -= in[j] * sin(2*Pi*i*j/n);
    }
    re[i]*=norm;
    im[i]*=norm;
  }
  return on;
}


/*=== Function ===============================================*/

int InverseDFTOfSamples(int n, double *re, double *im, double *out)

/*--- Description --------------------------------------------//

Computes a inverse DFT (Discrete Fourier Transformation) of n
(must be odd) elements given by re and im and store result out.

out contains(n-1)*2 elements.

Returns n (or the number of samples used if not odd).

n must be odd out contains (n-1)*2 values

//------------------------------------------------------------*/
{
  int i,k;
  int on;
  if(n%2==0) n--; // if n not odd make it odd
  on=(n-1)*2;    // output contains
  for(i=0;i<on;i++) out[i] = 0;
  for(k=0; k<n; k++)
  {
    double rp=re[k];
    double ip=-im[k];
    if(k==0) rp=rp/2;
    if(k==(n-1)) rp=rp/2;
    for(i=0;i<on; i++)
    {
      out[i] = out[i] + rp * cos(2*Pi*k*i/on);
      out[i] = out[i] + ip * sin(2*Pi*k*i/on);
    }
  }
  return on;
}


/*=== Function ===============================================*/

int PolarDFTOfSamples(int n, double *in, double *a, double *p)

/*--- Description --------------------------------------------//

Same as DFTOfSamples but transformation to polar coordinates
(a for amplitude and p for phase).

returns n/2+1.
n must be even out holds n/2+1 samples
a is normalized to the amplitude of the input signal.

//------------------------------------------------------------*/
{
  int i,j;
  double m,ph,re,im;
  int on;
  if(n%2==1) n--;          // if n not even make it even
  on=n/2+1;                // output samples hold n/2+1 elements
  double norm=2.0/n;
  for(i=0;i<on;i++) a[i]=p[i]=0; // use a and p for re im
  for(i=0;i<on;i++)  // run t to complex
  {
    for(j=0;j<n;j++)
    {
      a[i] += in[j] * cos(2*Pi*i*j/n);
      p[i] -= in[j] * sin(2*Pi*i*j/n);
    }
    re=a[i]; // convert to polar
    im=p[i];
    m=sqrt(re*re + im*im);
    if(re==0) re=1E-20;
    ph=atan(im/re);
    if(re<0 && im<0) ph=ph-Pi;
    if(re<0 && im>= 0) ph=ph+Pi;
    a[i]=m*norm;
    p[i]=ph;
  }
  return on;
}


/*=== Function ===============================================*/

int InversePolarDFTOfSamples(int n, double *a, double *p, double *out)

/*--- Description --------------------------------------------//

Same as InverseDFTOfSamples but transformation from polar coordinates
(a for amplutude and p for phase).

n must be odd out contains (n-1)*2 values

//------------------------------------------------------------*/
{
  int i,k;
  int on;
  if(n%2==0) n--; // if n not odd make it odd
  on=(n-1)*2;    // output contains
  for(i=0;i<on;i++) out[i] = 0;
  for(k=0; k<n; k++)
  {
    double rp=(a[k]*cos(p[k]));
    double ip=-(a[k]*sin(p[k]));
    if(k==0) rp=rp/2;
    if(k==(n-1)) rp=rp/2;
    for(i=0;i<on; i++)
    {
      out[i] = out[i] + rp * cos(2*Pi*k*i/on);
      out[i] = out[i] + ip * sin(2*Pi*k*i/on);
    }
  }
  return on;
}


/*==> FFT FastFourier Transformation <========================*/

// compute an inband complex to complex fft of 2^m samples
// m>0 forward m<0 reverse

static int FFT_Samples(int m, double *x, double *y)
{
   int n,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;
   int dir=1;
   if(m<0) m = -m, dir=0;
   if(m==0) return 0;

   /* Calculate the number of points */
   n=1<<m;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++)
   {
      if (i < j)
      {
         tx = x[i];
   ty = y[i];
         x[i] = x[j];
   y[i] = y[j];
         x[j] = tx;
   y[j] = ty;
      }
      k = i2;
      while (k <= j) { j -= k; k >>= 1;}
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++)
   {
      l1 = l2; l2 <<= 1;
      u1 = 1.0; u2 = 0.0;
      for (j=0;j<l1;j++)
      {
         for (i=j;i<n;i+=l2)
   {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1) c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) for (i=0;i<n;i++) { x[i] /= n; y[i] /= n; }
   return 1;
}


/*=== Function ===============================================*/

int ComplexFFTOfSamples(int o, double *ire, double *iim,
    double *ore, double *oim)

/*--- Description --------------------------------------------//

Performs a complex to to complex FFT of 2^o samples.
Input is given by 2^o samples (real and imaginary part) in ire and iim
and output contains 2^o samples of the FFT transformed data.

If o is < 0 a reverse transformation is done with 2^(-o) samples.
Returns 0 on error and n (the number of valid samples) on success.

//------------------------------------------------------------*/
{
  int n;
  if(o>30 || o<-30 || o==0) return 0;
  if(o>0) n=1<<o; else n=1<<(-o);
  CopySamples(n,ire,ore);
  CopySamples(n,iim,oim);
  FFT_Samples(o,ore,oim);
  return n;
}


/*=== Function ===============================================*/

int WindowFunctionOfSamples(int n, double *in , double *out, int type)

/*--- Description --------------------------------------------//

Applies a window functions to n samples given by in and stores
them in out. type is the type of window function to be used

0 = default = none (copy)
1 = sinus(0-pi/2)
2 = sinus(0-pi/2)^2
3 = sinus(0-pi/2)^3
4 = Hamming window
5 = Blackman window

Other functions will follow.
Returns n.

If type is negative the reverse window function is applied.
if in is 0 out contains the window function (max set to 1),
which can be used for windowing of signals. This method is faster
than calculating the window function each time before using it.

//------------------------------------------------------------*/
{
  int i;
  double x;
  if(in==0) for(in=out,i=0;i<n;i++) out[i]=1;
  switch(type)
  {
    default: return CopySamples(n,in,out);
    case  1: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]*x; break;
    case -1: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]/x; break;
    case  2: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]*x*x; break;
    case -2: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]/x/x; break;
    case  3: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]*x*x*x; break;
    case -3: for(i=0;i<n;i++) x=sin(1.*(i+0.5)/(n)*Pi), out[i]=in[i]/x/x/x; break;
    case  4: for(i=0;i<n;i++) x=0.54-0.46*cos(2.*(i+0.5)/(n)*Pi), out[i]=in[i]*x; break;
    case -4: for(i=0;i<n;i++) x=0.54-0.46*cos(2.*(i+0.5)/(n)*Pi), out[i]=in[i]/x; break;
    case  5: for(i=0;i<n;i++) x=0.42-0.50*cos(2.*(i+0.5)/(n)*Pi)+0.08*cos(4.*(i+0.5)/(n)*Pi), out[i]=in[i]*x; break;
    case -5: for(i=0;i<n;i++) x=0.42-0.50*cos(2.*(i+0.5)/(n)*Pi)+0.08*cos(4.*(i+0.5)/(n)*Pi), out[i]=in[i]/x; break;
  }
  return n;
}


/*=== Function ===============================================*/

int RealFFTOfSamples(int o, double *in, double *out, int type)

/*--- Description --------------------------------------------//

Performs a real to amplitude FFT of 2^o samples.
Input is given by 2^o samples (real part) in (in)
and output contains 2^o/2 samples of the FFT transformed data
in polar amplitudes.

Returns 0 (o<1) on error and the number of valid samples in out
(2^o/2) on success.

//------------------------------------------------------------*/
{
  int i,n;
  if(o>30 || o<1) return 0;
  n=1<<o;
  double *im=(double*)calloc(n,sizeof(double));
  double *re=(double*)malloc(n*sizeof(double));
  WindowFunctionOfSamples(n,in,re,type);
  FFT_Samples(o,re,im);
  //out[0]=sqrt(re[0]*re[0]+im[0]*im[0]); //Inconsistent with DFT
  for(i=0;i<n/2;i++) out[i]=2.*sqrt(re[i]*re[i]+im[i]*im[i]);
  free(im);
  free(re);
  return n/2;
}

/*=== Function ===============================================*/

int RealNFFTOfSamples(int n, double *in, double *out, int type)

/*--- Description --------------------------------------------//

Performs a real to amplitude FFT of n samples. Input is given by
n samples (real part) in (in) and output contains also n samples
of the FFT transformed data in polar amplitudes. The n/2 samples
received by the FFT are spread to fill up the n samples in out.

Returns 0 (o<1) on error and the number of valid samples in out
(n) on success.

//------------------------------------------------------------*/
{
  int o=ceil(log10(n)/log10(2)); //log2(n) missing
  int nfft=1<<o;
  if(o>30 || o<1) return 0;
  double **fft=Allocate2DSamples(2,nfft);
  //ResizeSamples(n,in,nfft,fft[0],0,n);
  WindowFunctionOfSamples(n,in,&fft[0][(int)((nfft-n)/2)],type);
  RealFFTOfSamples(o,fft[0],fft[1],0);
  //MultiplyAddSamples(n,fft[1],fft[1],(double)nfft/(double)n,0);
  ResizeSamples(nfft/2,fft[1],n,out,0,nfft/2);
  Free2DSamples(fft);
  return n;
}



/*=== Function ===============================================*/

int PolarFFTOfSamples(int o, double *in, double *a, double *p, int type)

/*--- Description --------------------------------------------//

Same as RealFFTOfSamples but transformation to polar coordinates
(a for amplitude and p for phase).

returns n/2.
n must be even out holds n/2 samples
a is normalized to the amplitude of the input signal.

//------------------------------------------------------------*/
{
  int i,n;
  double ph;
  if(o>30 || o<1) return 0;
  n=1<<o;
  double *im=(double*)calloc(n,sizeof(double));
  double *re=(double*)malloc(n*sizeof(double));
  WindowFunctionOfSamples(n,in,re,type);
  FFT_Samples(o,re,im);
  //a[0]=sqrt(re[0]*re[0]+im[0]*im[0]); //inconsistent with DFT
  for(i=0;i<n/2;i++)
  {
    a[i]=2.*sqrt(re[i]*re[i] + im[i]*im[i]);
    if(re[i]==0) re[i]=1E-20;
    ph=atan(im[i]/re[i]);
    if(re[i]<0 && im[i]<0) ph=ph-Pi;
    if(re[i]<0 && im[i]>= 0) ph=ph+Pi;
    p[i]=ph;
  }
  free(im);
  free(re);
  return n/2;
}


/*==> Convolution <===========================================*/

/*=== Function ===============================================*/

int ConvolutionOfSamples(int n, double *in, int nh,
    double *h, double *out)

/*--- Description --------------------------------------------//

Convolve n elements of array in by a convolution kernel given by
h and size nh. out contains n+nh-1 elements.

The function returns n+nh-1.
output must hold n+nh-1 samples

//------------------------------------------------------------*/
{
  int ii,io,ih;
  for(io=0; io<n+nh-1; io++) out[io]=0.0;
  for(ii=0; ii<n; ii++)
    for(ih=0; ih<nh; ih++)
      out[ii+ih]+=in[ii]*h[ih];
  // fprintf(stderr,"convolution %d %d ==>  %d\n", n, nh, n+nh-1  );
  return n+nh-1;
}


/*=== Function ===============================================*/

int ConvolutionKernelOfSamples(int n, double *in, int delta,
    int nh, double *out)

/*--- Description --------------------------------------------//

This function computes a convolution kernel (stored in out)
with maximum size nh for a given response in array in (size n) which is
caused by a delta function at index position delta.

The kernel may be then used to convolve signals of type in
to a subset of delta signals.

Returns the length of the convolution kernel, which can be smaller
than nh.

//------------------------------------------------------------*/
{
  int i,i2,i3;
  double *deltatime=(double *)calloc(n,sizeof(double));
  double *deltare=(double *)calloc((n/2+2),sizeof(double));
  double *deltaim=(double *)calloc((n/2+2),sizeof(double));
  double *inre=(double *)calloc((n/2+2),sizeof(double));
  double *inim=(double *)calloc((n/2+2),sizeof(double));
  deltatime[delta]=1;

  DFTOfSamples(n,deltatime,deltare,deltaim);
  i2=DFTOfSamples(n,in,inre,inim);
  // fprintf(stderr,"DFT of input %d ==> samples %d\n", n, i2 );
  DivideComplexSamples(i2,deltare,deltaim,inre,inim,deltare,deltaim);
  i3=InverseDFTOfSamples(i2,deltare,deltaim,deltatime);
  // fprintf(stderr,"inverse DFT of deconvolution FT  %d ==> samples %d\n", i2, i3 );
  if(nh>i3) nh=i3;
  for(i=0; i<nh; i++) out[i]=deltatime[i];
  free(deltatime);
  free(deltare);
  free(deltaim);
  free(inre);
  free(inim);
  return nh;
}


/*==> Histograms <============================================*/

/*=== Function ===============================================*/

int IncrementSamples(int n, double xle, double xhe, double *hist,
    double x, double value, double width)

/*--- Description --------------------------------------------//

A position (0..n) for x is computed from xle xhe (low and high
edges of the sample) and used to increment the sample at this
position by value in a region of width around pos.
If width>0 then usually more than one sample will be filled,
if width==0, then it is the same as HistogramSamples.

If le==he, the sample is forced to be filled spread over all
samples.

The function returns 1.

//------------------------------------------------------------*/
{
  if(xhe==xle){ if(!width) width=1; xle=x-0.5*width; xhe=x+0.5*width; };
  double xi,res;
  res=n/(xhe-xle);
  xi=(x-xle)*res-0.5; //here we loss all the significant digits
  width*=res;

  if(width==0) Fill1Samples(n,hist,xi,value);
  else FillSamples(n,hist,xi,value,width);
  if(debug>3) fprintf(stderr,"IncrementSamples: n %d xi %.10g value %.10g width %.10g\n",n,xi,value,width);
  return 1;
}


/*=== Function ===============================================*/

int Increment2DSamples(int ny, double yle, double yhe, int nx,
    double xle, double xhe, double **hist, double y,
    double x, double value, double ywidth, double xwidth)
/*--- Description --------------------------------------------//

Same as IncrementSamples but for a two dimensional array hist.
if [x,y]le==[x,y]he the sample is forced to be spread equally
over that dimension.

//------------------------------------------------------------*/
{
  if(xhe==xle){ if(!xwidth) xwidth=1; xle=x-0.5*xwidth; xhe=x+0.5*xwidth; };
  if(yhe==yle){ if(!ywidth) ywidth=1; yle=y-0.5*ywidth; yhe=y+0.5*ywidth; };
  double xi,yi,xres,yres;
  xres=nx/(xhe-xle);
  yres=ny/(yhe-yle);
	if(debug>5) fprintf(stderr,"Increment2DSamples: ny %d yle %.10g yhe %.10g nx %d xle %.10g xhe %.10g y %.10g x %.10g yres %.10g xres %.10g\n",ny,yle,yhe,nx,xle,xhe,y,x,yres,xres);
  xi=(x-xle)*xres-0.5; //here we loss all the significant digits
  yi=(y-yle)*yres-0.5; //here we loss all the significant digits

  xwidth*=xres;
  ywidth*=yres;
  if(xwidth==0 && ywidth==0) Fill2D1Samples(ny,nx,hist,yi,xi,value);
  else Fill2DSamples(ny,nx,hist,yi,xi,value,ywidth,xwidth);
	if(debug>3) fprintf(stderr,"Increment2DSamples: ny %d nx %d yi %.10g xi %.10g value %.10g ywidth %.10g xwdith %.10g\n",ny,nx,yi,xi,value,ywidth,xwidth);
  return 1;
}


/*=== Function ===============================================*/

int HistogramSamples(int n, double *hist, double xle,
    double xhe, double x, double value)

/*--- Description --------------------------------------------//

An index ( 0..n) for the x is computed from the xle xhe
(low and high edges of the histogram) and is used to increase
the value of element hist[index] by value.

The function returns 1.

//------------------------------------------------------------*/
{
  if(xhe==xle) return 0;
  double xi,res;
  res=n/(xhe-xle);
  xi=(x-xle)*res;
  if(xi<0 || xi>=n) return 0;
  hist[(int)xi]+=value;
  return 1;
}


/*=== Function ===============================================*/

int Histogram2DSamples
(
   int ny, int nx, double *hist,
   double yle, double yhe,  double xle, double xhe,
   double y, double x, double value
)
/*--- Description --------------------------------------------//

Same as HistogramSamples but for a two dimensional array hist.

//------------------------------------------------------------*/

{
  if(xhe==xle || yhe==yle) return 0;
  double xi,yi,xres,yres;
  int index;
  xres=nx/(xhe-xle);
  xi=(x-xle)*xres;
  if(xi<0 || xi>=nx) return 0;
  yres=ny/(yhe-yle);
  yi=(y-yle)*yres;
  if(yi<0 || yi>=ny) return 0;
  index=(int)xi+(int)yi*nx;
  hist[index]+=value;
  return 1;
}


/*=== Function ===============================================*/

double ValueOfSamplesIndex(int n, double xle, double xhe, double xi)

/*--- Description --------------------------------------------//

Returns the value in the range xle xhe corresponding to a samples
index xi in (0..n)

//------------------------------------------------------------*/
{
double res=(xhe-xle)/n;
return (xi+0.5)*res+xle;
}


/*=== Function ===============================================*/

double IndexOfSamplesValue(int n, double xle, double xhe, double x)

/*--- Description --------------------------------------------//

Returns the samples index (0..n) corresponding to a value x in
the range xle,xhe.

//------------------------------------------------------------*/
{
double res=n/(xhe-xle);
return (x-xle)*res-0.5;
}


/*==> Samples I/O Fuctions <==================================*/

/*=== Function ===============================================*/

int Write2DSamples(const char *name, int mode, int ny, double yle,
    double yhe, int nx, double xle, double xhe, double **data,
    double norm, const char *desc)

/*--- Description --------------------------------------------//

Writes out a sample specified in (data) to file with name (name).

There are three different mode to safe the files:

's' = scattering plot compressed, no zeros will be saved
'g' = scattering plot uncompressed, can be used with gnuplot
'm' = matrix style output, can be used with python
'b' = binary data (caution: the input array must be in a single block
      in memory (Allocate2DSamples makes sure of that))

If name ends with .gz then the output is piped to gzip.

If successful the functions returns mode, otherwise zero.

//------------------------------------------------------------*/
{
const char *desct;
if(!name) { if(debug) fprintf(stderr,"WriteSamples/ERROR: No file name provided\n"); return 0; }
if(!data) { if(debug) fprintf(stderr,"WriteSamples/ERROR: input array not defined\n"); return 0;}

FILE *fp=0;
int flen=strlen(name);
if(flen>2 && !strncmp(&name[flen-3],".gz",3))
{
  char gzip[210]; sprintf(gzip,"gzip -c > %s",name); fp=popen(gzip,"w");
  if(!fp){ if(debug) fprintf(stderr,"WriteSamplesHeader/ERROR: gzip problem, can not open samples file %s\n",name); return 0; }
}
if(!fp && flen==1 && name[0]=='-'){ fp=stdout; flen=0; }
if(!fp){ fp=fopen(name,"w"); flen=-flen; }
if(!fp){ if(debug) fprintf(stderr,"WriteSamples/ERROR: can not write samples %s\n",name); return 0; }

const char *tag;
if(mode==0) mode='s';
if(desc) desct=desc; else desct="none";

switch(mode)
{
  case 's': tag="Sam""plesSctt"; break;
  case 'g': tag="Sam""plesGnpl"; break;
  case 'm': tag="Sam""plesMtrx"; break;
  case 'b': tag="Sam""plesBnry"; break;
  default : if(debug) fprintf(stderr,"WriteSamples/ERROR: unknown mode %c\n",mode); return 0;
}

fprintf(fp,"#!%s/size-x,low-edge-x,high-edge-x,size-y,low-edge-y,high-edge-y,norm,description: ",tag);
if(nx==1 && ny>1) fprintf(fp,"%d %.7g %.7g %d %.7g %.7g %.7g %s\n",ny,yle,yhe,nx,xle,xhe,norm,desct);
else fprintf(fp,"%d %.7g %.7g %d %.7g %.7g %.7g %s\n",nx,xle,xhe,ny,yle,yhe,norm,desct);

int i,j;
float *tmp;

switch(mode)
{
  case 's':
    if(nx==1 && ny==1){ fprintf(fp,"%.7g %.7g %.7g",(xhe-xle)*0.5,(yhe-yle)*0.5,data[0][0]); break; }
    for(j=0;j<ny;j++)
    {
      for(i=0;i<nx;i++)
      {
        if(data[j][i])
        {
          if(nx>1) fprintf(fp,"%.7g ",(i+0.5)*(xhe-xle)/nx+xle);
          if(ny>1) fprintf(fp,"%.7g ",(j+0.5)*(yhe-yle)/ny+yle);
          fprintf(fp,"%.7g\n",data[j][i]);
        }
      }
    }
    break;

  case 'g':
    if(nx==1 && ny==1) {fprintf(fp,"%.7g %.7g %.7g",(xhe-xle)*0.5,(yhe-yle)*0.5,data[0][0]); break;}
    for(j=0;j<ny;j++)
    {
      for(i=0;i<nx;i++)
      {
        if(nx>1) fprintf(fp,"%.7g ",(i+0.5)*(xhe-xle)/nx+xle);
        if(ny>1) fprintf(fp,"%.7g ",(j+0.5)*(yhe-yle)/ny+yle);
        fprintf(fp,"%.7g\n",data[j][i]);
      }
      if(ny>1 && nx>1) fprintf(fp,"\n");
    }
    break;

  case 'm':
    for(j=0;j<ny;j++)
    {
      for(i=0;i<nx;i++)
      {
        fprintf(fp,"%.7g ",data[j][i]);
        if(ny==1) fprintf(fp,"\n");
      }
      if(ny>1) fprintf(fp,"\n");
    }
    break;

  case 'b':
    tmp=(float*)malloc(nx*sizeof(float));
    for(j=0;j<ny;j++)
    {
      for(i=0;i<nx;i++) tmp[i]=data[j][i];
      fwrite(tmp,sizeof(float),nx,fp);
    }
    free(tmp);
    break;
}

if(debug>2) fprintf(stderr,"WriteSamples: samples saved %s\n",name);
if(flen>0) pclose(fp);
if(flen<0) fclose(fp);

return 1;
}


/*=== Function ===============================================*/

int WriteSamples(const char *name, int mode, int nx, double xle,
    double xhe, double *data, double norm, const char *desc)

/*--- Description --------------------------------------------//

Writes out a sample specified in (data) to file with name (name).

It uses Write2DSamples, so check that function for more information.

If successful the functions returns mode, otherwise zero.

//------------------------------------------------------------*/

{
double *data2D[1]; data2D[0]=data;
return Write2DSamples(name,mode,1,0,1,nx,xle,xhe,data2D,norm,desc);
}


/*=== Function ===============================================*/

int ReadSamplesHeaderString(char *line, int *ny, double *yle,
    double *yhe, int *nx, double *xle, double *xhe,
    double *norm, char *desc)

/*--- Description --------------------------------------------//

Reads the string (line) for header information. It fills the
respective pointers if provided. It works for all
types of samples data files.

If name ends with .gz then the input is piped from gzip.

If executed correctly it returns the mode of the data file (see
Write2DSamples), otherwise it returns 0 to indicate that something
went wrong.

The function is written to be backwards compatible with older samples
(histogram) formats.

//------------------------------------------------------------*/

{

if(!line || line[0]!='#'){ if(debug) fprintf(stderr,"ReadSamplesHeaderString/ERROR: String is not a proper header\n"); return 0; }
int mode=0,i;

//it is necessary to use atof to be compatible with fsamples (float rounding error)
char nyt[100],nxt[100],xtle[100],ytle[100],xthe[100],ythe[100],normt[100],desct[2000];
if(debug>4) fprintf(stderr,"ReadSamplesHeaderString: %s", line);

//to assure backwards compatibility
if(strncmp("#!Sam""ples",line,9))
{
  if(sscanf(line,"%*s %*s %s %s %s %s %s %s %s %[^\n]",nxt,nyt,xtle,xthe,ytle,ythe,normt,desct)!=8)
  {
    if(sscanf(line,"%*s %s %s %s %s %[^\n]",nxt,xtle,xthe,normt,desct)!=5)
    {
      if(debug) fprintf(stderr,"ReadSamplesHeader/ERROR: String is not a proper header\n");
      return 0;
    }
    sprintf(nyt,"1"); sprintf(ytle,"0"); sprintf(ythe,"1");
  }
  if(debug>1) fprintf(stderr,"ReadSamplesHeaderString/WARNING: String is an old file type\n");
  mode='s';
}
else
{
  if(!strncmp("Sctt",&line[9],4)) mode='s';
  if(!strncmp("Gnpl",&line[9],4)) mode='g';
  if(!strncmp("Mtrx",&line[9],4)) mode='m';
  if(!strncmp("Bnry",&line[9],4)) mode='b';
  if(!mode || sscanf(line,"%*s %s %s %s %s %s %s %s %[^\n]",nxt,xtle,xthe,nyt,ytle,ythe,normt,desct)!=8)
  {
    if(debug) fprintf(stderr,"ReadSamplesHeaderString/ERROR: String is not a proper header\n");
    return 0;
  }
}

for(i=0; i<100 && desct[i]!='\n' && desct[i]; i++) if(desc) desc[i]=desct[i],desc[i+1]=0;
if(norm) *norm=atof(normt);
if(nx)   *nx=atoi(nxt);
if(ny)   *ny=atoi(nyt);
if(xle) *xle=atof(xtle);
if(yle) *yle=atof(ytle);
if(xhe) *xhe=atof(xthe);
if(yhe) *yhe=atof(ythe);
LastHeaderMode__=mode;

if(debug>2) fprintf(stderr,"ReadSamplesHeaderString: Header read %c %s %s %s %s %s %s %s %s\n",mode,nyt,ytle,ythe,nxt,xtle,xthe,normt,desct);
return mode;
}


/*=== Function ===============================================*/

int ReadSamplesHeader(const char *name, int *ny, double *yle,
    double *yhe, int *nx, double *xle, double *xhe,
    double *norm, char *desc)

/*--- Description --------------------------------------------//

Reads the header information of a file name ('-' for stdin) of a data
file. It fills the respective pointers if provided. It works for all
types of samples data files.

If name ends with .gz then the input is piped from gzip.

If executed correctly it returns the mode of the data file (see
Write2DSamples), otherwise it returns 0 to indicate that something
went wrong.

If the line does not start with # and stdin is selected, it will
not consume the line and the line can be read with another call.

The function is written to be backwards compatible with older samples
(histogram) formats.

//------------------------------------------------------------*/
{
if(!name) { if(debug) fprintf(stderr,"ReadSamplesHeader/ERROR: No file name provided\n"); return 0;}
FILE *fp=0;
int flen=strlen(name);
if(flen>2 && !strncmp(&name[flen-3],".gz",3))
{
  char gzip[210]; sprintf(gzip,"gzip -dc < %s",name); fp=popen(gzip,"r");
  if(!fp){ if(debug) fprintf(stderr,"ReadSamplesHeader/ERROR: gzip problem, can not open samples file %s\n",name); return 0; }
}
if(!fp && flen==1 && name[0]=='-'){ fp=stdin; flen=0; }
if(!fp) {fp=fopen(name,"r"); flen=-flen;}
if(!fp) { if(debug) fprintf(stderr,"ReadSamplesHeader/ERROR: can not open samples file %s\n",name); return 0;}
if(debug>4) fprintf(stderr,"ReadSamplesHeader: file %s",name);

if(!flen) //if stdin lets to the pervert way and have a peak :D, this is allowed by the C/C++ standard
{
  int c=getc(fp); ungetc(c,fp); // get the first caracter for check if line can be a header
  if(c!='#') return 0;
}

char line[4096];
if(!fgets(line,4096,fp))
{
  if(flen>0) pclose(fp);
  if(flen<0) fclose(fp);
  return 0;
}
if(flen>0) pclose(fp);
if(flen<0) fclose(fp);
return ReadSamplesHeaderString(line, ny, yle, yhe, nx, xle, xhe, norm, desc);
}


/*=== Function ===============================================*/

int ReadAdd2DSamples(const char *name, int ny, double yle,
    double yhe, int nx, double xle, double xhe, double **data, double multi)

/*--- Description --------------------------------------------//

Reads the data of a file with name ('-' for stdin) and multiples
each value by multi and adds it to the sample saved in dataranging
from [x,y]le to [x,y]he. It uses Increment2DSample, that means the
sample size is adjusted to the size of data, and only the ranges
[x,y]le to [x,y]heare filled. This might lead to unwanted artefacts
if the dimensions are not a multiple of the input dimensions.

It is possible to add a 2D sample to a 1D sample and inverse. The
samples will be only filled in the overlapping area of data and the
input histogram.

The function performs differently if [x,y]le and [x,y]he are choosen
equal. In that case the following will happen (1D means n[x,y]=1):

1D -> 2D: The 1D sample is streched over the entire range of the 2D
          sample
2D -> 1D: The 2D sample is projected to a 1D sample.
1D -> 1D: The 1D sample is forced to match the input range
2D -> 2D: The 2D sample is forced to match the input range

If name ends with .gz then the data is piped from gzip.

It returns 0 if an error occured or the mode of the data file
if correctly executed.

//------------------------------------------------------------*/
{
if(!name){ if(debug) fprintf(stderr,"ReadAddSamples/ERROR: No file name provided\n"); return 0; };
FILE *fp=0;
int flen=strlen(name);
if(flen>2 && !strncmp(&name[flen-3],".gz",3))
{
  char gzip[210]; sprintf(gzip,"gzip -dc < %s",name); fp=popen(gzip,"r");
  if(!fp){ if(debug) fprintf(stderr,"ReadAddSamples/ERROR: gzip problem, can not open samples file %s\n",name); return 0; };
}
if(!fp && flen==1 && name[0]=='-'){ fp=stdin; flen=0; };
if(!fp){ fp=fopen(name,"r"); flen=-flen; };
if(!fp){ if(debug) fprintf(stderr,"ReadAddSamples/ERROR: can not open samples file %s\n",name); return 0; }

const int ll=1024;
char line[ll];
int mode=0;
int i=0,j=0,p=0,pp=0,nyt=ny,nxt=nx,nr=0, tmode=0;
double ytle=yle,ythe=yhe,xtle=xle,xthe=xhe;
double xwidth=0,ywidth=0,xtwidth=(xhe-xle)/nx,ytwidth=(yle-yhe)/ny,normt=0;
char cx[100],cy[100];
double x,y,l1,l2;
float z;
float *tmp;

if(debug>2) fprintf(stderr, "ReaddAddSamples: Initial mode %c\n",mode);
if(!flen)
{
  mode=LastHeaderMode__;
  if(mode=='b') goto dobinarynow; //if we read from stdin and a mode is set to binary we have to go at it right away
}

while(fgets(line,ll,fp))
{
  if(line[0]=='#')
  {
    if(nx==1 && ny>1) tmode=ReadSamplesHeaderString(line,&nxt,&xtle,&xthe,&nyt,&ytle,&ythe,&normt,0);
    else tmode=ReadSamplesHeaderString(line,&nyt,&ytle,&ythe,&nxt,&xtle,&xthe,&normt,0);
    if(mode && tmode && debug>1) fprintf(stderr,"ReadAddSamples/WARNING: Reading a second histogram. Norm information will be lost\n");
    if(tmode)
    {
      mode=tmode;
      i=j=0; //reset all the counters
      xtwidth=(xthe-xtle)/nxt;
      ytwidth=(ythe-ytle)/nyt;

      xwidth=(xhe-xle)/nx;
      ywidth=(yhe-yle)/ny;

      //only Fill by width if new samples is not subset of old samples (avoid rounding errors)
      l1=fabs((xtle-xle)/xwidth); l2=fabs(xwidth/xtwidth);
      if((fabs(floor(l1)-l1)<1e-7 || fabs(ceil(l1)-l1)<1e-7) && (fabs(floor(l2)-l2)<1e-7 || fabs(ceil(l2)-l2)<1e-7)) xwidth=0; else xwidth=xtwidth;
      l1=fabs((ytle-yle)/ywidth); l2=fabs(ywidth/ytwidth);
      if((fabs(floor(l1)-l1)<1e-7 || fabs(ceil(l1)-l1)<1e-7) && (fabs(floor(l2)-l2)<1e-7 || fabs(ceil(l2)-l2)<1e-7)) ywidth=0; else ywidth=ytwidth;

      if(debug>2) fprintf(stderr,"ReadAddSamples: ywidth %.10g xwidth %.10g\n",ywidth,xwidth);
    }
    if(mode=='b') goto dobinarynow; //read next line
    continue;
  }
  switch(mode)
  {
    case 's': case 'g':
      nr=sscanf(line,"%s %s %f",cx,cy,&z);
      //it is necessary to use atof to be compatible with fsamples (float rounding error)
      x=atof(cx),y=atof(cy);
      switch(nr)
      {
        case 2:
          z=y; if(nx==1 && ny>1){ y=x; x=xtwidth*0.5+xtle; } else y=ytwidth*0.5+ytle;
        case 3:
          if(z)
          {
            if(debug>3) fprintf(stderr,"ReadAddSamples: y %.10g x %.10g z %.10g\n",y,x,z*multi);
            Increment2DSamples(ny,yle,yhe,nx,xle,xhe,data,y,x,z*multi,ywidth,xwidth);
          }
        default:
          break;
      }
      //if(debug>2) fprintf(stderr,"ReadAddSamples: sample read %s, mode %c\n", name, mode);
      break;

    case 'm':
      p=0; pp=0;
      while(line[pp]!='\0' && line[pp]!='\n' && sscanf(&line[pp],"%f%n",&z, &p)==1)
      {
        pp+=p;
        if(z)
        {
          x=(i+0.5)*xtwidth+xtle;
          y=(j+0.5)*ytwidth+ytle;
          if(debug>3) fprintf(stderr,"ReadAddSamples: y %.10g x %.10g z %.10g\n",y,x,z*multi);
          Increment2DSamples(ny,yle,yhe,nx,xle,xhe,data,y,x,z*multi,ywidth,xwidth);
        }
        i++; if(i>=nxt){ j++; i=0; }; if(j>=nyt){ j=0; break; };
      }
      //if(debug>2) fprintf(stderr,"ReadAddSamples: sample read %s, mode %c\n", name, mode);
      break;

    case 'b':
    dobinarynow:
      tmp=(float*)malloc(nxt*sizeof(float));
      for(j=0; j<nyt; j++)
      {
        if((unsigned int)(nxt)!=fread(tmp,sizeof(float),nxt,fp)) break;
        for(i=0;i<nxt;i++)
        {
          z=tmp[i];
          if(z)
          {
            x=(i+0.5)*xtwidth+xtle;
            y=(j+0.5)*ytwidth+ytle;
            if(debug>3) fprintf(stderr,"ReadAddSamples: y %.10g x %.10g z %.10g\n",y,x,z*multi);
            Increment2DSamples(ny,yle,yhe,nx,xle,xhe,data,y,x,z*multi,ywidth,xwidth);
          }
        }
      }
      free(tmp);
    break;

    default:
      break;
      //if(debug) fprintf(stderr,"ReadAddSamples/ERROR: can't read header of file %s\n", name);
  }
}
LastHeaderMode__=0;
if(flen>0) pclose(fp);
if(flen<0) fclose(fp);
return mode;
}


/*=== Function ===============================================*/

int ReadAddSamples(const char *name, int nx, double xle, double xhe,
    double *data, double multi)

/*--- Description --------------------------------------------//

Same as ReadAdd2DSample but for a 1D sample. See the above
description for more information.

Stdin is supported.

If a 2D sample is read, it will be projected to a 1D sample.

It returns 0 if an error occured or the mode of the data file
if correctly executed.

//------------------------------------------------------------*/

{
double *data2D[1]; data2D[0]=data;
return ReadAdd2DSamples(name,1,0,0,nx,xle,xhe,data2D,multi);
}


/*=== Function ===============================================*/

double *ReadSamples(const char *name, int *nx, double *xle,
        double *xhe, double *norm, char *desc)

/*--- Description --------------------------------------------//

Reads the data of a 1D samples file . The samples header information
will be stored into the arguments if provided.

Stdin is not supported.

If a 2D sample is read, it will be projected to a 1D sample.

The function uses AllocateSamples() so the array must be freed
with FreeSamples().

If name ends with .gz then the data is piped from gzip.

It returns 0 if an error occured or a pointer to a
1D sample if correctly executed.

//------------------------------------------------------------*/
{
int nxt, mode;
double xtle,xthe;
mode=ReadSamplesHeader(name,0,0,0,&nxt,&xtle,&xthe,norm,desc);
if(mode==0) return 0;
double *data=AllocateSamples(nxt);
double *data2D[1]; data2D[0]=data;
if(!ReadAdd2DSamples(name,1,0,0,nxt,xtle,xthe,data2D,1)) {FreeSamples(data); return 0;}
if(nx) *nx=nxt;
if(xle) *xle=xtle;
if(xhe) *xhe=xthe;
return data;
}


/*=== Function ===============================================*/

double **Read2DSamples(const char *name, int *ny, double *yle,
       double *yhe, int *nx, double *xle, double *xhe,
       double *norm, char *desc)

/*--- Description --------------------------------------------//

Reads the data of a 2D sample file. The samples header information
will be stored into the arguments if provided.

Stdin is not supported.

If name ends with .gz then the data is piped from gzip.

The function uses Allocate2DSamples() so the array must be freed
with Free2DSamples().

It returns 0 if an error occured or a pointer to a 2D sample if
correctly executed.



//------------------------------------------------------------*/
{
int nxt,nyt,mode;
double xtle,xthe,ytle,ythe;
mode=ReadSamplesHeader(name,&nyt,&ytle,&ythe,&nxt,&xtle,&xthe,norm,desc);
if(mode==0) return 0;
double **data=Allocate2DSamples(nyt,nxt);
if(!ReadAdd2DSamples(name,nyt,ytle,ythe,nxt,xtle,xthe,data,1)) {Free2DSamples(data); return 0;}
if(ny) *ny=nyt;
if(yle) *yle=ytle;
if(yhe) *yhe=ythe;
if(nx) *nx=nxt;
if(xle) *xle=xtle;
if(xhe) *xhe=xthe;
return data;
}


//////////////////////////////////////////////////////////////////



#ifdef TEST
static double data[1000];
int main()
{

data[9]=0.1;
data[10]=1;
data[11]=0.1;

double x=FwhmOfSamples(20,data,10);
printf("%.10g \n",x);


}

#endif

//int main(){return 0;}
