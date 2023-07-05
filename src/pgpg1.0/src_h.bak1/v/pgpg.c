/* 
   last updated at 2003/12/12 bugfixed at signal definition part of (*vd).sds[i]
   last updated at 2003/10/31 lcell RCA support
   last updated at 2003/09/23 bugfixed
   last updated at 2003/07/01
*/
#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy(),strcat(),strcmp(),strtok()
#include "pgpg.h"
#include "pgpg_submodule.h"

#ifdef PGPGVT
static int is_pgpgvt=1;
#else
static int is_pgpgvt=0;
#endif

#ifdef LCROM
static int is_lcrom=1;
#else
static int is_lcrom=0;
#endif

#ifdef JDATA_WIDTH
static int fixed_jdata_width=JDATA_WIDTH;
#else
static int fixed_jdata_width=0;
#endif

/* device selection */
#ifdef STRATIX
#define DEVICE "stratix"
#endif
#ifdef CYCLONE
#define DEVICE "cyclone"
#endif
#ifdef APEX20KE
#define DEVICE "apex20ke"
#endif
#ifdef APEX20KC
#define DEVICE "apex20kc"
#endif
#ifndef DEVICE
#define DEVICE "apex20k"
#endif



void itobc(idata,sdata,nbit)
int idata;
char sdata[];
int nbit;
{
  int i,itmp;
  strcpy(sdata,"");
  for(i=0;i<nbit;i++){
    itmp =0x1&(idata>>(nbit-1-i));
    if(itmp == 1) strcat(sdata,"1");
    if(itmp == 0) strcat(sdata,"0");
  }
}


void generate_pg_fix_addsub          (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_log_muldiv          (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_log_shift           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_pdelay              (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_conv_ftol           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_conv_ftol_itp       (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_conv_ltof           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_conv_ltof_itp       (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_log_add             (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_log_unsigned_add    (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_log_unsigned_add_itp(char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_fix_accum           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_rundelay            (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_wtable              (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_dwtable             (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_and            (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_or             (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_xor            (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_join           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_part           (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_inv            (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_shift          (char sdata[][STRLEN],struct vhdl_description *vd);
void generate_pg_bits_delay          (char sdata[][STRLEN],struct vhdl_description *vd);
//void generate_pg_conv_fixtofloat     (char sdata[][STRLEN],struct vhdl_description *vd);


void generate_pipe_com(FILE *fp,struct vhdl_description *vd); //@pgpg_pipe_com.c
void generate_pg_set_fodata(char sdata[][STRLEN],struct vhdl_description *vd,int *i); //@pgpg_set_io.c
void generate_pg_set_idata (char sdata[][STRLEN],struct vhdl_description *vd,int *i); //@pgpg_set_io.c
void generate_pg_set_jdata (char sdata[][STRLEN],struct vhdl_description *vd,int *i); //@pgpg_set_io.c
void generate_pg_set_data  (char sdata[][STRLEN],struct vhdl_description *vd,int *i); //@pgpg_set_io.c



void generate_pg_pipe(vd)
struct vhdl_description *vd;
{
  int ii,i,ivmp,i0;
  static char sd[20000][STRLEN];
  FILE *fp,*fopen();

  ii = 0;
  sprintf(sd[ii], "library ieee;                                                       \n" );ii++;
  sprintf(sd[ii], "use ieee.std_logic_1164.all;                                        \n" );ii++;
  if((*vd).nvmp>1){
    sprintf(sd[ii], "use ieee.std_logic_unsigned.all;                                    \n" );ii++;  
  }
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "entity pipe is                                                      \n" );ii++;
  sprintf(sd[ii], "  generic(JDATA_WIDTH : integer :=72) ;                             \n" );ii++;
  sprintf(sd[ii], "port(p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);         \n" );ii++;
  sprintf(sd[ii], "     p_run : in std_logic;                                          \n" );ii++;
  sprintf(sd[ii], "     p_we :  in std_logic;                                          \n" );ii++;
  sprintf(sd[ii], "     p_adri : in std_logic_vector(3 downto 0);                      \n" );ii++;
  sprintf(sd[ii], "     p_adrivp : in std_logic_vector(3 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_datai : in std_logic_vector(31 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_adro : in std_logic_vector(3 downto 0);                      \n" );ii++;
  sprintf(sd[ii], "     p_adrovp : in std_logic_vector(3 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_datao : out std_logic_vector(31 downto 0);                   \n" );ii++;
  sprintf(sd[ii], "     p_runret : out std_logic;                                      \n" );ii++;
  sprintf(sd[ii], "     rst,pclk : in std_logic);                                      \n" );ii++;
  sprintf(sd[ii], "end pipe;                                                           \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  sprintf(sd[ii], "architecture std of pipe is                                         \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  for(i=0;i<(*vd).ic;i++) sprintf(sd[i+ii],"%s",(*vd).sdc[i]);
  ii += (*vd).ic;

  for(i=0;i<(*vd).is;i++){
    int iflag,j;
    char* p;                                                /*** HAMADA ***/
    char pi[HSTRLEN];                                       /*** HAMADA ***/
    char stmp[HSTRLEN];                                     /*** HAMADA ***/
    sprintf(stmp,"%s",(*vd).sds[i]);                        /*** HAMADA ***/
    p = (char *)strstr(stmp,"signal");                      /*** HAMADA ***/
    p = (char *)strtok(p,":");                              /*** HAMADA ***/
    sprintf(pi,"%s",p);
    iflag = 0;
    for(j=0;j<i;j++){
      //      if(strcmp((*vd).sds[i],(*vd).sds[j])==0) iflag=1; /* HAMADA */
      char* pj;                                             /*** HAMADA ***/
      strcpy(stmp,(*vd).sds[j]);                            /*** HAMADA ***/
      pj = (char *)strstr(stmp,"signal");                   /*** HAMADA ***/
      pj = (char *)strtok(pj,":");                          /*** HAMADA ***/
      if(strcmp(pi,pj)==0) iflag=1;                         /*** HAMADA ***/
    }
    if(iflag==0){
      sprintf(sd[i+ii],"%s",(*vd).sds[i]);
      ii += (*vd).is;
    }
  }
  if((*vd).nvmp>1){
    for(i=0;i<(*vd).iip;i++){
      sscanf((*vd).sdip[i][1],"%d",&i0);
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        sprintf(sd[ii], "  signal %s%d: std_logic_vector(%d downto 0);\n",(*vd).sdip[i][0],ivmp,i0-1);ii++;
      }
    }
    sprintf(sd[ii], "  signal vmp_phase: std_logic_vector(3 downto 0);                 \n" );ii++;
    sprintf(sd[ii], "  signal irun: std_logic_vector(%d downto 0);      \n",(*vd).nvmp-1 );ii++;    
    for(i=1;i<(*vd).nvmp;i++){
      sprintf(sd[ii], "  signal jdata%d: std_logic_vector(JDATA_WIDTH-1 downto 0);      \n",i );ii++;
    }
  }
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "begin                                                               \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  generate_pg_set_data(sd,vd,&ii);   //--- bugfixed 2003/09/23 ---

  generate_pg_set_jdata(sd,vd,&ii);  //--- bugfixed 2003/09/23 ---

  generate_pg_set_idata(sd,vd,&ii);  //--- bugfixed 2003/09/23 ---
  
  for(i=0;i<(*vd).im;i++) sprintf(sd[i+ii],"%s",(*vd).sdm[i]);
  ii += (*vd).im;

  generate_pg_set_fodata(sd,vd,&ii); //--- bugfixed 2003/09/23 ---

  sprintf(sd[ii], "end std;                                                            \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;


  if(is_pgpgvt){
    fp = fopen("pg_pipe.vhd","w");
    generate_pipe_com(fp,vd);
    for(i=0;i<ii;i++) fprintf(fp,"%s",sd[i]);
    fclose(fp);
  }else{
    fp = fopen("pg_module.vhd","w");
    for(i=0;i<(*vd).ie;i++) fprintf(fp,"%s",(*vd).sde[i]);
    //    ii += (*vd).ie;
    fclose(fp);
  }

}


void sdtest(sd)
struct vhdl_description *sd;
{
  printf("%s\n",(*sd).sdc[0]);
  sprintf((*sd).sdc[0],"test test 2\n");
  (*sd).ic = 10;
}

int main(argc,argv)
int argc;
char *argv[];
{
  char device[STRLEN];
  char isdata[STRLEN];
  static char sdata[100][STRLEN];
  char *p;
  int i,ii;
  FILE *fp,*fopen();
  static struct vhdl_description sd;
  int jdata_index_min=72;
  int jdata_index_max=0;
  sd.ic = 0;
  sd.im = 0;
  sd.ie = 0;
  sd.is = 0;
  sd.ucnt = 0;
  sd.nvmp = 1;
  sd.npipe = 1;
  sd.ijp = sd.iip = sd.ifo = sd.ida = 0;
  sd.jdata_width = 0;
  sd.nboost_fixaccum = 0; // now fixed
  sd.alteraesbflag = (1-is_lcrom);

  if(argc!=2){
    printf("pgpg filename\n");
    exit(0);
  }else{
    fp = fopen(argv[1],"r");
  }
	
  /*	while(fscanf(fp,"%s",isdata)!=EOF){*/
  while(fgets(isdata,100,fp)!=NULL){	  
    i = 0;
    p = (char* )strtok(isdata,"( ");
    strcpy(sdata[i],p);
    i++;
    while((p = (char* )strtok(NULL, ",(); \n"))!=NULL){
      strcpy(sdata[i],p);
      i++;
    }
    /*	  for(j=0;j<i;j++) printf("%d %s\n",j,sdata[j]);*/
    if(strcmp(sdata[0],"/NVMP")==0){
      sscanf(sdata[1],"%d",&(sd.nvmp));
      printf("nvmp %d\n",sd.nvmp);
    }
    if(strcmp(sdata[0],"/NPIPE")==0){
      sscanf(sdata[1],"%d",&(sd.npipe));
      printf("npipe %d\n",sd.npipe);
    }
    if(strcmp(sdata[0],"/JPSET")==0){
      ii = sd.ijp;
      strcpy(sd.sdjp[ii][0],sdata[1]);
      strcpy(sd.sdjp[ii][1],sdata[2]);
      strcpy(sd.sdjp[ii][2],sdata[3]);
      strcpy(sd.sdjp[ii][3],sdata[4]);
      (sd.ijp)++;
      if(jdata_index_min > atoi(sdata[3])) jdata_index_min = atoi(sdata[3]);
      if(jdata_index_max < atoi(sdata[4])) jdata_index_max = atoi(sdata[4]);
      if(fixed_jdata_width>0){
	sd.jdata_width = fixed_jdata_width;
      }else{
	sd.jdata_width = jdata_index_max - jdata_index_min + 1;
      }
    }
    if(strcmp(sdata[0],"/IPSET")==0){
      ii = sd.iip;
      strcpy(sd.sdip[ii][0],sdata[1]);
      //      strcpy(sd.sdip[ii][1],sdata[2]);
      strcpy(sd.sdip[ii][1],sdata[4]);
      (sd.iip)++;
    }
    if(strcmp(sdata[0],"/FOSET")==0){
      ii = sd.ifo;
      strcpy(sd.sdfo[ii][0],sdata[1]);
      //      strcpy(sd.sdfo[ii][1],sdata[2]);
      strcpy(sd.sdfo[ii][1],sdata[4]);
      (sd.ifo)++;
    }
    if(strcmp(sdata[0],"/DATASET")==0){
      ii = sd.ida;
      strcpy(sd.sdda[ii][0],sdata[1]);
      strcpy(sd.sdda[ii][1],sdata[2]);
      strcpy(sd.sdda[ii][2],sdata[3]);
      strcpy(sd.sdda[ii][3],sdata[4]);	    	    
      (sd.ida)++;
    }
    if(strcmp(sdata[0],"pg_fix_addsub")==0){
      generate_pg_fix_addsub(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_log_muldiv")==0){
      generate_pg_log_muldiv(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_log_shift")==0){
      generate_pg_log_shift(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_pdelay")==0){
      generate_pg_pdelay(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_conv_ftol")==0){
      generate_pg_conv_ftol(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_conv_ftol_itp")==0){
      generate_pg_conv_ftol_itp(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_conv_ltof")==0){
      generate_pg_conv_ltof(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_conv_ltof_itp")==0){
      generate_pg_conv_ltof_itp(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_log_add")==0){
      generate_pg_log_add(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_log_unsigned_add")==0){
      generate_pg_log_unsigned_add(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_log_unsigned_add_itp")==0){
      generate_pg_log_unsigned_add_itp(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_fix_accum")==0){
      generate_pg_fix_accum(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_rundelay")==0){
      generate_pg_rundelay(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_wtable")==0){
      generate_pg_wtable(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_dwtable")==0){
      generate_pg_dwtable(sdata,&sd);
    }
    if(strcmp(sdata[0],"pg_bits_and")==0)  generate_pg_bits_and(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_or")==0)   generate_pg_bits_or(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_xor")==0)  generate_pg_bits_xor(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_join")==0) generate_pg_bits_join(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_part")==0) generate_pg_bits_part(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_inv")==0)  generate_pg_bits_inv(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_shift")==0) generate_pg_bits_shift(sdata,&sd);
    if(strcmp(sdata[0],"pg_bits_delay")==0) generate_pg_bits_delay(sdata,&sd);

    //    if(strcmp(sdata[0],"pg_conv_fixtofloat")==0) generate_pg_conv_fixtofloat(sdata,&sd);

  }
  fclose(fp);


  { /* logic-cell generate */
    strcpy(device,DEVICE);
    generate_pg_lcell(&sd,device);
  }

  generate_pg_pipe(&sd);
  return 1;
}
