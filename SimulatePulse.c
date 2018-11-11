# include "../../adl3/include/ADL.h"
# include "../../adl3/include/ADL_Traces_NUMRES.h"
# include "../../adl3/include/ADL_Fields_SIMION.h"
# include <stdlib.h>
# include <math.h>
# include <unistd.h>

/*=== Function ===============================================*/

int LRAverageSamplesFilter(int n, double *in, double *out,
    int m, int r)

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
{
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




int main ( int argc, char **argv)
{

	FILE *output;
  FILE *time_info_output;
	struct ADL_EVENT *evt;
	int i,j,l,k;
	double P[4]; //4
	/*vector x_positions;
	vector z_positions;
	vector segment_number;
	vector halfq_time;*/
  double x_positions[20000] ={0};
	double z_positions[20000]={0};
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
	double width = 0.05; //strtod(argv[2], NULL); //in CM TODO
	printf("the widht %lf \n", width);
	double gridsize=ADL_G_SIMION_GridSize;       //predefined variable for the gridsize used
	double center=ADL_G_SIMION_Center;           //predefined variable of the center of a symmetric array (2D and 3D)
	struct SIMION_HEADER *h=ADL_G_SIMION_Header; //to access nx, ny, nz
	double height=h->nz*gridsize;
	double activeheight = height - 0.22;
	double nz = h->nz;
  fprintf(stderr, "Variables: center %g height %g\n",center, height);

//for (k = 0; k < 11; k++){
//  for (i = 0; i < 300; i++){
<<<<<<< HEAD
for (k = 0; k < 300; k++){ // k from 0 to 10 should cover
  for (i = 0; i < 12; i++){ // i from 0 to 300 should cover
=======
for (k = 0; k < 10; k++){ // k from 0 to 10 should cover
  for (i = 0; i < 1; i++){ // i from 0 to 300 should cover
>>>>>>> 8e4fc31f062c017dc3c2e0335f07bdd8b411ae1f
  //k = 0; i = 0;
  //for (i = 0; i < (width/0.01 + 1); i++){
	printf("HEIGHT  %lf \n", height);
	printf("POINT IN Z %lf \n",  height - (height*k/nz)) - 0.1;
	printf("NUMBER OF STEPS TOTAL %lf \n", nz);
	printf("NUMBER OF STEPS LEFT %lf \n",  (nz - k)) ;
	//lets create a new event structure:
	evt = new_event();
	//Fill in the Hit Pattern (HP):
	evt->HP.Eint[0]=1.0;             //Energy of interaction 0 (we only simulate a single interaction here)
	evt->HP.T0= 0.0;               //Time the interaction occurs in the trace

<<<<<<< HEAD
  evt->HP.Pos[0][0]= 0.1 - 0.025 + i*0.005;//0.36;//(width*1 + 0.06) + i*0.01;//0.085; //(width*1.5 + 0.06);//(1.5 * width) + 0.06;// 0.135+0.015; //(1.5 * width) + 0.06;//0.15 + 0.05*i; //0.15 + 0.01*(i+1);//(width/2.0); //(1.0/10.0)*(i+1);           //Position where this interaction occures (in the plane) ????
	evt->HP.Pos[0][1]= 0.1;             //FOR ALL I KNOW THIS SHOULD REMAIN ZERO
	evt->HP.Pos[0][2]= height - 0.11+0.005- k*.005;    //in cm!
        
	double x_position = 0.1 - 0.025 + i * 0.005;// 0.36;//(width*1 + 0.06)+ i*0.01;//0.085; //(width*1.5 + 0.06); //(width + 0.06) + 0.01*i;//0.135 + 0.015;//(1.5 * width) + 0.06;// 0.15 + 0.05*i ;// 0.21 //0.15 + 0.01*(i+1);;//width/2.0; //(1.0/10.0)*(i+1);   //((random1/ 100.0) + 0.1);
	double z_position = height - 0.11 + 0.005 - k * .005; // ((random2 / 100.0) - 0.1);
        //std::cout<"YYYYYYYYYYYY "<<<z_position<<std::endl;
=======
  evt->HP.Pos[0][0]= 0.1 - 0.25 + i*0.005;//0.36;//(width*1 + 0.06) + i*0.01;//0.085; //(width*1.5 + 0.06);//(1.5 * width) + 0.06;// 0.135+0.015; //(1.5 * width) + 0.06;//0.15 + 0.05*i; //0.15 + 0.01*(i+1);//(width/2.0); //(1.0/10.0)*(i+1);           //Position where this interaction occures (in the plane) ????
	evt->HP.Pos[0][1]= 1.0;             //FOR ALL I KNOW THIS SHOULD REMAIN ZERO
	evt->HP.Pos[0][2]= height - 0.01 - k*.005;    //in cm!

	double x_position = 0.1 - 0.25 + i*0.005;// 0.36;//(width*1 + 0.06)+ i*0.01;//0.085; //(width*1.5 + 0.06); //(width + 0.06) + 0.01*i;//0.135 + 0.015;//(1.5 * width) + 0.06;// 0.15 + 0.05*i ;// 0.21 //0.15 + 0.01*(i+1);;//width/2.0; //(1.0/10.0)*(i+1);   //((random1/ 100.0) + 0.1);
	double z_position = height - 0.01 - k*.005; // ((random2 / 100.0) - 0.1);
>>>>>>> 8e4fc31f062c017dc3c2e0335f07bdd8b411ae1f
/*

	//RANDOM POSITIONS
	float random1 = float((rand() % 70) + 20); //in mm
	float random2 = float((rand() % 160) + 20) ;// in mm
  printf("here %f \n", random1);
	printf("there %f \n", random2);
//	random1 = 6.4;
//	random2 = 10.1;


	evt->HP.Pos[0][0]= (random1/ 100.0) + 0.1;         //Position where this interaction occures (in the plane) ????
	evt->HP.Pos[0][1]=0;               //FOR ALL I KNOW THIS SHOULD REMAIN ZERO
	evt->HP.Pos[0][2]= (random2 / 100.0) - 0.1;      //in cm!

	double x_position =(random1/ 100.0) + 0.1; //((random1/ 100.0) + 0.1);
	double z_position =(random2 / 100.0) - 0.1; // ((random2 / 100.0) - 0.1);

*/
	//check if the points are in the detector, only then calculate the trace and write the file
	double P1[4]={0,evt->HP.Pos[0][0],evt->HP.Pos[0][1],evt->HP.Pos[0][2]};
	if(!ADL_G_InDetector(P1)){ printf("Point X %g Y %g Z %g not in detector\n",evt->HP.Pos[0][0],evt->HP.Pos[0][1],evt->HP.Pos[0][2]); return 1; }

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
		for (l = 0; l < ADL_G_EVENT_NSEG + 1; l++){
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

						x_positions[arraycounter] = (double (x_position));
						z_positions[arraycounter] = (double(z_position));
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

	//printf("%i %i %f %f %lf \n", eventnumber, segment_number[m], x_positions[m], z_positions[m], halfq_time[m] );
	fprintf(time_info_output, "%i %i %lf %lf %lf \n", eventnumber, segment_number[m], x_positions[m], z_positions[m], halfq_time[m] );
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
		  printf("%i %i %lf %lf %lf \n", eventnumber, segment_number[m], x_positions[m], z_positions[m], halfq_time[m] );
		  fprintf(time_info_output, "%i %i %lf %lf %lf \n", eventnumber, segment_number[m], x_positions[m], z_positions[m], halfq_time[m] );}
			else{continue;}
	}
fclose(time_info_output);
}
eventnumber++;
}}
	return 0;
}
