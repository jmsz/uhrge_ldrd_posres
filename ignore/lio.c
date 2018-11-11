/*==> Module Lio  <===============================================//

Functions for reading list files line by line and parsing tags

//--- Version ----------------------------------------------------//

Version: 1.0
Date:    2015
Updated by: Marco Salathe (marco.salathe@mpi-hd.mpg.de)

//----------------------------------------------------------------*/

/*--- Include ----------------------------------------------------*/

//#include "lio.h"

/*----------------------------------------------------------------*/

/*================================================================*/

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>

#define lio_buffer_size__ 5000
#define lio_field_size__    20

static char lio_buffer__[lio_buffer_size__*lio_field_size__]={'\0'}, lio_bsplit__[lio_buffer_size__*lio_field_size__], lio_delim__[256];
static char *lio_fields__[lio_buffer_size__];
static int lio_fields_len__=0, lio_debug__=1;// lio_delim_len__=0;
static FILE *lio_file__=0;

/*==> Basic functions  <==========================================*/

/*=== Function ===================================================*/

int LioLogging(int level)

/*--- Description ------------------------------------------------//

set logging level for Lio modules.

//----------------------------------------------------------------*/

{
  lio_debug__=level;
  return 0;
}


/*=== Function ===================================================*/

int LioSetDelim(const char* delim)

/*--- Description ------------------------------------------------//

Set Delimiter for input.

//----------------------------------------------------------------*/

{
  if(delim) strcpy(lio_delim__,delim);
  else strcpy(lio_delim__," \t");
  if(lio_debug__>2) fprintf(stderr,"LioSetDelim: delim set to \"%s\"\n", lio_delim__);
  return 1;
}


/*=== Function ===================================================*/

int LioCloseFile()

/*--- Description ------------------------------------------------//

Closes a previously loaded file.

//----------------------------------------------------------------*/

{
  if(lio_file__) fclose(lio_file__);
  lio_file__=0;
  if(lio_debug__>1) fprintf(stderr,"LioCloseFile: File closed\n");
  return 1;
}


/*=== Function ===================================================*/

int LioOpenFile(const char* fname, const char* delim)

/*--- Description ------------------------------------------------//

Loads a file for processing. This function must be called before
entries can be read.

If the filename character pointer is set to zero the stdin will be
used.

If the filename is loaded correctly one is returned if there was an
error, zero.

Each character in delim is considered as a separator symbol.

//----------------------------------------------------------------*/

{
  if(lio_file__) LioCloseFile();
  if(!fname || fname[0]=='-')
  {
    if(lio_debug__>0) fprintf(stderr,"LioOpenFile: Input set to stdin\n");
    lio_file__=stdin;
  }
  else
  {
    if(lio_debug__>1) fprintf(stderr,"LioOpenFile: File %s opened for input\n",fname);
    lio_file__=fopen(fname,"r");
  }

  if(!lio_file__) { if (lio_debug__>0) fprintf(stderr,"LioOpenFile: ERROR: can't open file\n"); return 0; }

  LioSetDelim(delim);
  return 1;
}


/*=== Function ===================================================*/

char* LioGetRecord()

/*--- Description ------------------------------------------------//

Get the next record (next line in an ascii file).

If the end of file is reached or another error occurs the function
returns zero, if the record is valid and loaded it returns one.

//----------------------------------------------------------------*/

{
  if(!lio_file__) LioOpenFile(0," ");
  if(!fgets(lio_buffer__, lio_buffer_size__*lio_field_size__, lio_file__))
  {
    if(lio_debug__>1) fprintf(stderr,"LioGetRecord: WARNING: line can't be read'\n");
    return 0;
  }
  lio_fields_len__=0;
  if(lio_debug__>3) fprintf(stderr,"LioGetRecord: Line read: %s\n", lio_buffer__);
  return lio_buffer__;
}


/*=== Function ===================================================*/

int LioSplitRecord()

/*--- Description ------------------------------------------------//

Function to fill lio_bsplit__, and break it up into \0 terminated
blocks, each being pointed to by a pointer in lio_fields__.
It returns the number of pointers(columns) within the line.

//----------------------------------------------------------------*/

{
  if(!lio_buffer__[0]) { if (lio_debug__>0) fprintf(stderr,"LioSplitRecord: ERROR: Buffer not read use LioGetRecord()\n"); return 0; }
  strcpy(lio_bsplit__,lio_buffer__);
  char *runp=lio_bsplit__;
  int i=0;
  while(runp && runp[0]!='\0' && runp[0]!='\n')
  {
    if(strchr(lio_delim__,runp[0])) { runp[0]='\0'; runp++; }
    else
    {
      lio_fields__[i]=runp;
      std::cout<<runp<<std::endl;
      i++;
      runp=strpbrk(runp, lio_delim__);
      if(i>=lio_buffer_size__) { if (lio_debug__>2) fprintf(stderr,"LioSplitRecord: Buffer not long enough\n"); return 0; }
    }
  }
  if ((runp=strchr(lio_fields__[i-1],'\n'))) runp[0]='\0'; //to assure that searching for tag=";" also works.
  lio_fields__[i]=0;
  lio_fields_len__=i;
  if(lio_debug__>3) fprintf(stderr,"LioSplitRecord: Record split, len %d\n", i);
  return i;
}


/*=== Function ===================================================*/

int LioFindTag(const char *tag)

/*--- Description ------------------------------------------------//

Function to find tag in lio_fields__. Returns the index of the
tag, 0 if the tag is not set or lio_fields_len__ if the tag is
not found.

//----------------------------------------------------------------*/
{
  int i;
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetField: Splitting error\n"); return 0; }
  if(!tag){ if(lio_debug__>2) fprintf(stderr,"LioFindTag: Tag not set\n"); return 0; }
  for (i=0;i<lio_fields_len__;i++)
    if(strcmp(lio_fields__[i],tag)==0) return i;
  if (lio_debug__>2) fprintf(stderr,"LioFindTag: Tag not found\n");
  return lio_fields_len__;
}


/*=== Function ===================================================*/

char *LioGetChar(int n)

/*--- Description ------------------------------------------------//

Function to get the nth field as a string.

//----------------------------------------------------------------*/

{
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetField: Splitting error\n"); return 0; }
  if(n>=lio_fields_len__) { if(lio_debug__>2) fprintf(stderr,"LioGetField: Index out of range\n"); return 0; }
  return lio_fields__[n];
}


/*=== Function ===================================================*/

char **LioGetChars(int start)

/*--- Description ------------------------------------------------//

Function to get a array of pointers to the strings, starting at
field start.

//----------------------------------------------------------------*/

{
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetChars: Splitting error\n"); return 0; }
  if(start>=lio_fields_len__) { if(lio_debug__>2) fprintf(stderr,"LioGetChars: Index out of range\n"); return 0;}
  return &lio_fields__[start];
}


/*=== Function ===================================================*/

double LioGetDouble(int n)

/*--- Description ------------------------------------------------//

Function to get the nth field as a double.

//----------------------------------------------------------------*/

{
  double tmp;
  char *endp;
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetDouble: Splitting error\n"); return 0; }
  if(n>=lio_fields_len__) { if(lio_debug__>2) fprintf(stderr,"LioGetDouble: Index out of range\n"); return 0; }
  tmp=strtod(lio_fields__[n], &endp);
  if (lio_fields__[n] == endp) {if(lio_debug__>2) fprintf(stderr,"LioGetDouble: Not an number, \"%s\"\n",endp); return 0; }
  if (*endp != '\0') { if(lio_debug__>3) fprintf(stderr,"LioGetDouble: Number only partially converted, \"%s\"\n", endp); return tmp; }
  return tmp;
}


/*=== Function ===================================================*/

int LioGetDoubles(int start, int dsize, double *data)

/*--- Description ------------------------------------------------//

Function to get all the fields between start and start+dsize as
doubles that will be stored into data. Returns the number of
fields read.

//----------------------------------------------------------------*/

{
  double tmp;
  char *endp;
  int stop=dsize,i;
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetAllFieldsDouble: Splitting error\n"); return 0; }
  if(start>=lio_fields_len__) { if(lio_debug__>2) fprintf(stderr,"LioGetDoubles: Index out of range\n"); return 0;}
  if(!data) { if(lio_debug__>2) fprintf(stderr,"LioGetAllFieldsDouble: Array not defined\n"); return 0;}
  if(stop>lio_fields_len__-start) stop=lio_fields_len__-start;
  for(i=0;i<stop;i++)
  {
    tmp=strtod(lio_fields__[i+start], &endp);
    if (lio_fields__[i+start] == endp){ if(lio_debug__>2) fprintf(stderr,"LioAtof: Not an number, \"%s\"\n",endp); return i; }
    if (*endp != '\0') { if(lio_debug__>3) fprintf(stderr,"LioAtof: Number only partially converted, \"%s\"\n", endp); data[i]=tmp; }
    data[i]=tmp;
  }
  return i;
}


/*=== Function ===================================================*/

int LioGetNFields()

/*--- Description ------------------------------------------------//

Just returns the numbers of fields available

//----------------------------------------------------------------*/

{
  if(!lio_fields_len__) if(!LioSplitRecord()) { if(lio_debug__>2) fprintf(stderr,"LioGetNFields: Splitting error\n"); return 0; }
  return lio_fields_len__;
}

// entry point function
int nomain();

void _start(){
    // calling entry point
    nomain();
    exit(0);
}
int nomain()
{
    return 0;
}
