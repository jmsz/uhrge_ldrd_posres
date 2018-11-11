//The ADL includes:
#include "../../adl3/include/ADL.h"

extern int STRIP_G_TopNumber;
extern int STRIP_G_BotNumber;

#include <string.h>
#include <stdio.h>
//*****************************************************************************************

int main(int argc, char **argv)
{
  char filename_setupfile[100];
  int  i,len;
  struct  ADL_KEYWORD **Kwords;
  double ReadVersion=0.0;

  //initialize the required functions
  if(argc==1){printf("No SETUP file indicated.\n"); return 1;};
  sprintf(filename_setupfile, "%s", argv[1]);

  //get list of keywords from file:
  if (strlen(filename_setupfile)>1) {// if a filename is supplied, parse it
    Kwords = ADL_parse_file (filename_setupfile);

    //overwrite keywords with parsed values:
    len = Kwords[1]->linenumber;
    for (i=0;i<len;i++) {
      if (strcmp(Kwords[2+i]->keyword,"ADL_G_DEBUG")==0)
        sscanf(Kwords[2+i]->svalue,"%d",&ADL_G_DEBUG);
      if (strcmp(Kwords[2+i]->keyword,"ADL_G_VERSION")==0)
        sscanf(Kwords[2+i]->svalue,"%lf",&ReadVersion);
      //GEOMETRY:
      if ( (strcmp(Kwords[2+i]->keyword,"SIMION_GEOMETRY_PLANAR")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_GEOMETRY_BEGE")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_GEOMETRY_COAX")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_GEOMETRY_INVC")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_GEOMETRY_STRIP")==0) )
         printf("%-20.20s < %-35.35s %d\n",Kwords[2+i]->keyword,Kwords[2+i]->svalue,SIMION_Setup_GEOMETRY(Kwords[2+i]->keyword,Kwords[2+i]->svalue));
      if ( (strcmp(Kwords[2+i]->keyword,"SIMION_SOLVER_POISSON")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_SOLVER_LAPLACE")==0)
         ||(strcmp(Kwords[2+i]->keyword,"SIMION_SOLVER_INHOMOG")==0))
         printf("%-20.20s < %-35.35s %d\n",Kwords[2+i]->keyword,Kwords[2+i]->svalue,SIMION_Setup_SOLVER(Kwords[2+i]->keyword,Kwords[2+i]->svalue));
    }
  } //end parsing file

  if (ReadVersion != ADL_G_VERSION) printf("\nWARNING ADL (%.2lf) setup: version (%.2lf) outdated ?\n",ADL_G_VERSION,ReadVersion);
  double STRIP_G_Height = 1.5;
  double STRIP_G_ImpTop = 1;
  double Vdepletion;
  Vdepletion = ((STRIP_G_Height*STRIP_G_Height*STRIP_G_ImpTop*1e10*1.602e-19*100)/(2*SIMION_G_EpsScale*8.85e-12)); // Charge density over 2xdielectric constant times height sqr
  SIMION_G_Voltage = Vdepletion + 500;
  ADL_Status();
  SIMION_Status_GEOMETRY();
  SIMION_Status_SOLVER();

  char buf[100];
  struct SIMION_PA *field=0, *field3D=0;

  //Back Segments in 3d
  SIMION_G_Dimension=3;
  field=SIMION_G_newPA();

//  for(i = 0; i<108; i++) 
  for(i=0; i<STRIP_G_TopNumber+STRIP_G_BotNumber; i++) //had top + bot + 1
  {
    SIMION_G_Structure(field, i);
    sprintf(buf, "Strip_Stru_%02d.pa",i);
    SIMION_WRITE_PA(field, 0, buf);
    SIMION_G_WField(field, i);
    SIMION_G_Solve(field);
    sprintf(buf, "Strip_Wpot_%02d.pa",i);
    SIMION_WRITE_PA(field, 0, buf);
  }
  SIMION_G_EField(field, 0);
  SIMION_G_Solve(field);
  SIMION_WRITE_PA(field, 0, 0);

  //free_SIMION_PA(field);

  //Remaining Segments in 2d
/*  SIMION_G_Dimension=2;
  field = SIMION_G_newPA();
  for(i=9;i<20;i++)
  {
    SIMION_G_Structure(field, i);
    sprintf(buf,"InvC_Stru_%02d.pa", i);
    field3D = SIMION_2Dto3D_FIELD(field, 0);
    SIMION_WRITE_PA(field3D, 0, buf);
    SIMION_G_WField(field, i);
    SIMION_G_Solve(field);
    sprintf(buf,"InvC_Wpot_%02d.pa", i);
    field3D = SIMION_2Dto3D_FIELD(field, 0);
    SIMION_WRITE_PA(field3D, 0, buf);
  } */

  //point contact
/*  SIMION_G_Structure(field, 0);
  sprintf(buf,"InvC_Stru_%02d.pa", 0);
  field3D = SIMION_2Dto3D_FIELD(field, 0);
  SIMION_WRITE_PA(field3D, 0, buf);

  SIMION_G_WField(field, 0);
  SIMION_G_Solve(field);
  sprintf(buf,"InvC_Wpot_%02d.pa",0);
  field3D = SIMION_2Dto3D_FIELD(field, 0);
  SIMION_WRITE_PA(field3D, 0, buf);*/

  return 0;
}
