# include "../../adl3/include/ADL.h"
# include "../../adl3/include/lio.h"
# include "../../adl3/include/ADL_Traces_NUMRES.h"
# include "../../adl3/include/ADL_Fields_SIMION.h"
# include <stdlib.h>
# include <math.h>
# include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
//#include "lio.h"
//#include "random.c"
//#include "samples.c"
//#include "y2dplot.c"
/*=== Function ===============================================*/

/**int LRAverageSamplesFilter(int n, double *in, double *out,
    int m, int r)**/

/*--- Description --------------------------------------------//

Multi path left right smooth array in (size n) m times by the width r.
Smoothing is applied first from left to right and then from right to
left.
This retains the center of gravity of signals.

r must be an integer. No subchannel smoothing allowed.
Outside elements of the array, which needs to be known for computation,
are replaced by the edges of the in array and are linear interpolated
to the first an last computable element.

The function returns n.

//------------------------------------------------------------*/
/**{
  int i,ie;
  int w=r;
  double sum=0;
  double newv=0;
  if(w<1) return 0;
  //if(debug>3) fprintf(stderr,"LRAverageSamples: loops %d width %d \n",m,w);
  double le,re;
  while(m-->0)
  {
  for(le=i=0;i<w;i++) le+=in[i]/w;
  for(re=i=0;i<w;i++) re+=in[n-i-1]/w;
  for(sum=0,i=0; i<w; i++) sum+=in[i];
  for(i=0; i<n-w; i++) { newv=sum/w; sum+=in[i+w]-in[i]; out[i]=newv; }
  ie=i-1;
  for(sum=0,i=ie; i>ie-w; i--) sum+=out[i];
  for(i=ie; i>w-1; i--) { newv=sum/w; sum+=out[i-w]-out[i];
out[i]=newv; }
  for(i=0;i<w;i++) out[i]=(le*(w-i)+out[w]*i)/w; // leftedge
  for(i=0;i<w;i++) out[n-1-i]=(re*(w-i)+out[n-1-w]*i)/w; // rightedge
  in=out;
  }
  return n;
}
**/
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
// LIO FUNCTIONS END HERE CUT OFF START SIMULATE


int main ( int argc, char **argv)
{

	FILE *output;
  FILE *time_info_output;
	struct ADL_EVENT *evt;
	int i,j,l,k;
	double P[4]; //4
  int segment_number[20000]={0};
  float halfq_time[20000]={0};
	char buffer[100];
	char time_info_file[100];
	int eventnumber = 0;

	//initialize the required functions
	if(argc==1){printf("No path to SETUP file indicated.\n"); return 1;};
	sprintf(buffer, "%s", argv[1]);

	ADL_Setup(buffer);
	ADL_Status();
	ADL_Status_FIELDS();

	//get some variables that define the used arrays (such as height)
	//double width = 0.1;//strtod(argv[2], NULL); //in CM TODO
	//printf("the width %lf \n", width);
	double gridsize=ADL_G_SIMION_GridSize;       //predefined variable for the gridsize used
	double center=ADL_G_SIMION_Center;           //predefined variable of the center of a symmetric array (2D and 3D)
	struct SIMION_HEADER *h=ADL_G_SIMION_Header; //to access nx, ny, nz
	double height=h->nz*gridsize;
	double activeheight = height - 0.22;
	double nz = h->nz;
  fprintf(stderr, "Variables: center %g height %g\n",center, height);

double interaction_x_positions[5];
double interaction_y_positions[5];
double interaction_z_positions[5];
double interaction_energis[5];
double interaction_pid[5];//double buffer [100]={0};

//char * delimiter = '2';
//egad = putchar(032);

//const char* charVal=" ";
//char charVal = char(' ');
//std::cout<< charVal<< charVal << "a" << charVal << charVal<<std::endl;
//delim[] = static const char* " "
//LioSetDelim(charVal);
char* line;
LioOpenFile("test-500-1det.dat"," ,\n");
for (int i = 0; i < 5; i = i + 1){
    line = LioGetRecord();
  //  std::cout<<line<<std::endl;
    int num_fields = LioGetNFields();
//    std::cout<<num_fields<<std::endl;
    if(num_fields == 3){std::cout<<"HEADER"<< num_fields << std::endl;}
    if(num_fields == 6){
    char* test = LioGetChar(3);
    std::cout<<test<<std::endl;
    interaction_x_positions[i]=strtod(LioGetChar(3),NULL);
    interaction_y_positions[i]=strtod(LioGetChar(4),NULL);
    interaction_z_positions[i]=strtod(LioGetChar(5),NULL);
    interaction_energies[i]=strtod(LioGetChar(2),NULL);
    interaction_pid[i]=strtod(LioGetChar(0),NULL);
    std::cout<<" HERE: "<<interaction_x_positions[i]<< " "<<interaction_y_positions[i]
    << " "<<interaction_z_positions[i]<< " "<< interaction_energies[i]<<
    interaction_pid[i]<<std::endl;
  }
}
LioCloseFile();

std::cout<< "END: "<< interaction_x_positions[0]<<std::endl;
std::cout<< "END: "<< interaction_y_positions[0]<<std::endl;
std::cout<< "END: "<< interaction_z_positions[0]<<std::endl;
std::cout<< "END: "<< interaction_energies[0]<<std::endl;
std::cout<< "END: "<< interaction_pid[0]<<std::endl;

std::cout<<interaction_pid <<std::endl;
std::cout<<interaction_energies <<std::endl;

for (k = 0; k < 1; k++){ // i is in x dimension, k is in z dimension
  evt = new_event();
  for (i = 0; i < 5; i++){
	printf("HEIGHT  %lf \n", height);
	printf("POINT IN Z %lf \n",  height - (height*k/nz)) - 0.1;
	printf("NUMBER OF STEPS TOTAL %lf \n", nz);
	printf("NUMBER OF STEPS LEFT %lf \n",  (nz - k)) ;
	//lets create a new event structure:
	//Fill in the Hit Pattern (HP):
	evt->HP.Eint[i]=interaction_energies[k];             //Energy of interaction 0 (we only simulate a single interaction here)
	//evt->HP.Eint[1]=0.0;             //Energy of interaction 0 (we only simulate a single interaction here)
	evt->HP.T0= 0.000;               //Time the interaction occurs in the trace
	evt->HP.Pos[i][0]=interaction_x_positions[i];             //Position where this interaction occures (in the plane) ????
  evt->HP.Pos[i][1]=interaction_x_positions[i];           //FOR ALL I KNOW THIS SHOULD REMAIN ZERO
	evt->HP.Pos[i][2]=interaction_z_positions[i];
  int x_position = interaction_x_positions[i];
  int z_position = interaction_z_positions[i];
	//check if the points are in the detector, only then calculate the trace and write the file
	double P1[4]={0,evt->HP.Pos[i][0],evt->HP.Pos[i][1],evt->HP.Pos[i][2]};
//  double P2[4]={0,evt->HP.Pos[1][0],evt->HP.Pos[1][1],evt->HP.Pos[1][2]};
	if(!ADL_G_InDetector(P1)){ printf("Point X %g Y %g Z %g not in detector\n",evt->HP.Pos[i][0],evt->HP.Pos[i][1],evt->HP.Pos[i][2]); return 1; }
//  if(!ADL_G_InDetector(P2)){ printf("Point X %g Y %g Z %g not in detector\n",evt->HP.Pos[1][0],evt->HP.Pos[1][1],evt->HP.Pos[1][2]); return 1; }
  interaction_x_positions[k] = (double (x_position));
  interaction_z_positions[k] = (double(z_position));
  }
	//On basis of the HP, here the traces are generated
	//Traces are stored in the Trace Data (TD) part of the event:
	ADL_G_CalculateTraces(evt);


//  for (l = 0; l < ADL_G_EVENT_DIMT + 1; l++){LRAverageSamplesFilter(25, (evt->TD).Tr[l], (evt->TD).Tr[l], 1, 3);}

	//print traces
	sprintf(buffer, "%s_SS_%d.txt", SIMION_G_Description, eventnumber );
	output = fopen(buffer,"w");
	fprintf(output, "# TeX TeY TeZ ThX ThY THZ P1 P2\n");
	for (j = 0; j < ADL_G_EVENT_DIMT + 1; j++) {
		fprintf(output, "%le %le %le ", ADL_G_NUMRES_XYZe[j][1], ADL_G_NUMRES_XYZe[j][2], ADL_G_NUMRES_XYZe[j][3]);
		fprintf(output, "%le %le %le ", ADL_G_NUMRES_XYZh[j][1], ADL_G_NUMRES_XYZh[j][2], ADL_G_NUMRES_XYZh[j][3]);
		for (l = 0; l < ADL_G_EVENT_NSEG; l++){
			 // LRAverageSamplesFilter(ADL_G_EVENT_DIMT, (evt->TD).Tr[l], (evt->TD).Tr[l], 1, 3); //ATTENTION TODO PREAMP
		    fprintf(output, "%lf ", (evt->TD).Tr[l][j]);}
		fprintf(output, "\n");
  }

	double maxq;
	double minq;
	double halfq;
	int p;
	int q;
	double halftime;
	double tenthtime;
	int halfindex;
	int tenthindex;
	double tenthq;
	double charge;
	double charge1;
	int arraycounter = 0;

				for (l = 0; l < ADL_G_EVENT_NSEG; l++){
				  if (fabs((evt->TD).Tr[l][ADL_G_EVENT_DIMT])> 0.8){
						maxq = fabs((evt->TD).Tr[l][ADL_G_EVENT_DIMT]);
						minq =fabs((evt->TD).Tr[l][0]);
						halfq = fabs((maxq - minq)/2.0);
						tenthq = fabs((maxq - minq)/10.0);
						charge1 = minq;
						charge = minq;
            p = 0;
						q = 0;

						while (charge1 < tenthq){
							charge1 = fabs((evt->TD).Tr[l][q]);
							//printf("iter: %lf \n", charge1);
							q++;
						}
						tenthindex = q;
						tenthtime =  double(q*ADL_G_EVENT_DeltaT);
						tenthtime = double(tenthtime);
						printf("segment %d, tenth index %i, tenth time %lf \n", l, tenthindex, tenthtime);

            while (charge < halfq){
							charge = fabs((evt->TD).Tr[l][p]);
							//printf("iter: %lf \n", charge);
							p++;
						}
            halfindex = p;
						halftime =  double(p*ADL_G_EVENT_DeltaT);
						printf("segment %d, half index %i, half time %lf \n", l, halfindex,  halftime);

						//printf("threshold value %lf \n", charge);
						//printf("x %f , z %f , segment %i , halftime %f , arraycounter %i \n ", x_position, z_position, l, halftime, arraycounter);

						segment_number[arraycounter] = (int (l));
						halfq_time[arraycounter] = (double(halftime) - double(tenthtime));
						printf("recorded t50 %lf \n" ,halfq_time[arraycounter] );
						arraycounter++;

}


}
	fclose(output); //this is where the for loop over events closes
  sleep(1);
  sprintf(time_info_file,  "TIMING_full.txt");
//time_info_file = "TIMING.txt";
time_info_output = fopen(time_info_file,"a");
fprintf(time_info_output , "#Event  L(segment)  X(mm)  Z(mm)  T(us) \n");
for (int m = 0; m < arraycounter; m++){

	//printf("%i %i %f %f %lf \n", eventnumber, segment_number[m], interaction_x_positions[m], interaction_z_positions[m], halfq_time[m] );
	fprintf(time_info_output, "%i %i %lf %lf %lf \n", eventnumber, segment_number[m], interaction_x_positions[m], interaction_z_positions[m], halfq_time[m] );
}
	fclose(time_info_output);

		for (l = 0; l < ADL_G_EVENT_NSEG; l++){
	  sprintf(time_info_file,  "TIMING_full_%i.txt", l);
	//time_info_file = "TIMING.txt";
	  time_info_output = fopen(time_info_file, "a");
		printf("WE MADE IT");
	  fprintf(time_info_output , "#Event  L(segment)  X(mm)  Z(mm)  T(us) \n");
	  for (int m = 0; m < arraycounter; m++){
			if (segment_number[m] == l){
				printf("IN THE LOOP");
		  printf("%i %i %lf %lf %lf \n", eventnumber, segment_number[m], interaction_x_positions[m], interaction_z_positions[m], halfq_time[m] );
		  fprintf(time_info_output, "%i %i %lf %lf %lf \n", eventnumber, segment_number[m], interaction_x_positions[m], interaction_z_positions[m], halfq_time[m] );}
			else{continue;}
	}
fclose(time_info_output);
}
eventnumber++;
//}
}
	return 0;
}
