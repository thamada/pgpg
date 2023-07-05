#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy()
#include<math.h> //ceil()
#include "pgpg.h"
#include "pgpg_submodule.h" // generate_pg_adder_RCA()

void generate_pg_fix_addsub(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int nbit,nstage;
  char sx[STRLEN],sy[STRLEN],sz[STRLEN];
  char addsub_flag[STRLEN];
  int ic,im,ie,is;
  int iflag;
  int j;

  strcpy(addsub_flag,sdata[1]);
  strcpy(sx,sdata[2]);
  strcpy(sy,sdata[3]);
  strcpy(sz,sdata[4]);
  nbit = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  /*****************************************************
   The nstage>2 cases are under construction.
  ******************************************************/
  if(nstage>2){
    nstage=2;
    printf("WARNING!!: fix_addsub: nstage fixed to %d.\n",nstage);
  }

  printf("fix addsub: %s %s %s = %s : ",addsub_flag,sx,sy,sz);
  printf("nbit %d nstage %d\n",nbit,nstage); 

  ic = (*vd).ic;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sdc[ic], "  component pg_fix_add_%d_%d\n",nbit,nstage);ic++;
  }else{
    sprintf((*vd).sdc[ic], "  component pg_fix_sub_%d_%d\n",nbit,nstage);ic++;
  }
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(x,y : in std_logic_vector(%d downto 0);\n",nbit-1);ic++;
    sprintf((*vd).sdc[ic], "         z : out std_logic_vector(%d downto 0);\n",nbit-1 );ic++;
    sprintf((*vd).sdc[ic], "         clk : in std_logic);\n");ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n");ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sdm[im], "  u%d: pg_fix_add_%d_%d\n",(*vd).ucnt,nbit,nstage);im++;
  }else{
    sprintf((*vd).sdm[im], "  u%d: pg_fix_sub_%d_%d\n",(*vd).ucnt,nbit,nstage);im++;
  }
  sprintf((*vd).sdm[im], "       port map (x=>%s,y=>%s,z=>%s,clk=>pclk);\n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sy,nbit-1);is++;
  (*vd).is = is;


  if(iflag !=0) return;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n" );ie++;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "--+-------------------------+\n");ie++;
    sprintf((*vd).sde[ie], "--| PGPG Fixed-Point Add    |\n");ie++;
    sprintf((*vd).sde[ie], "--|      by Tsuyoshi Hamada |\n");ie++;
    sprintf((*vd).sde[ie], "--+-------------------------+\n");ie++;
  }else{
    sprintf((*vd).sde[ie], "--+-------------------------+\n");ie++;
    sprintf((*vd).sde[ie], "--| PGPG Fixed-Point Sub    |\n");ie++;
    sprintf((*vd).sde[ie], "--|      by Tsuyoshi Hamada |\n");ie++;
    sprintf((*vd).sde[ie], "--+-------------------------+\n");ie++;
  }
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "entity pg_fix_add_%d_%d is\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "entity pg_fix_sub_%d_%d is\n",nbit,nstage);ie++;
  }
  sprintf((*vd).sde[ie], "  port(x,y : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "       z : out std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);\n" );ie++;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "end pg_fix_add_%d_%d;\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "end pg_fix_sub_%d_%d;\n",nbit,nstage);ie++;
  }
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "architecture rtl of pg_fix_add_%d_%d is\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "architecture rtl of pg_fix_sub_%d_%d is\n",nbit,nstage);ie++;
  }
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "  component pg_adder_RCA_ADD_%d_%d\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "  component pg_adder_RCA_SUB_%d_%d\n",nbit,nstage);ie++;
  }
  sprintf((*vd).sde[ie], "    port (x,y: in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "          z:  out std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "          clk: in std_logic);\n");ie++;
  sprintf((*vd).sde[ie], "  end component;\n");ie++;
  sprintf((*vd).sde[ie], "begin\n");ie++;
  if(strcmp(addsub_flag,"ADD")==0){
    sprintf((*vd).sde[ie], "  u0: pg_adder_RCA_ADD_%d_%d\n",nbit,nstage );ie++;
  }else{
    sprintf((*vd).sde[ie], "  u0: pg_adder_RCA_SUB_%d_%d\n",nbit,nstage );ie++;
  }
  sprintf((*vd).sde[ie], "      port map(x=>x,y=>y,z=>z,clk=>clk);\n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
  (*vd).ie = ie;

  // -- RCA generate --
  if(strcmp(addsub_flag,"ADD")==0){
    generate_pg_adder_RCA(vd,nbit,nstage,0); // ADD
  }else{
    generate_pg_adder_RCA(vd,nbit,nstage,1); // SUB
  }
}
