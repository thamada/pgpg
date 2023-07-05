#include<stdio.h>
#include<stdlib.h> // atoi(),exit()
#include<string.h> // strcpy(),strcmp()
#include "pgpg.h"


void generate_pg_log_shift(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int j;
  int nbit,nshift;
  char sx[STRLEN],sy[STRLEN];
  char snshift[STRLEN];
  int ic,im,ie,is,iflag;	

  nshift = atoi(sdata[1]);
  if(nshift<0){
    sprintf(snshift,"m%d",-nshift);
  }else{
    sprintf(snshift,"%d",nshift);
  }	  
  if((nshift>2)||(nshift<-2)){
    fprintf(stderr,"pg_log_shift: nshift is out of range: %d\n",nshift); 
    exit(1);
  }
  strcpy(sx,sdata[2]);
  strcpy(sy,sdata[3]);
  nbit = atoi(sdata[4]);

  printf("log shift : %s = %s nshift %d : ",sx,sy,nshift);
  printf("nbit %d snshift %s\n",nbit,snshift);

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_log_shift_%s                        \n" ,snshift);ic++;
  iflag=0;
  for(j=0;j<ic-1;j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    generic (PG_WIDTH: integer);                           \n" );ic++;
    sprintf((*vd).sdc[ic], "    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);    \n" );ic++;
    sprintf((*vd).sdc[ic], "	     y : out std_logic_vector(PG_WIDTH-1 downto 0);  \n" );ic++;
    sprintf((*vd).sdc[ic], "	     clk: in std_logic);                             \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                           \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                           \n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_log_shift_%s generic map(PG_WIDTH=>%d)       \n",(*vd).ucnt,snshift,nbit);im++;
  sprintf((*vd).sdm[im], "                    port map(x=>%s,y=>%s,clk=>pclk);         \n",sx,sy);im++;
  sprintf((*vd).sdm[im], "                                                                  \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  (*vd).is = is;

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;                                              \n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    sprintf((*vd).sde[ie], "entity pg_log_shift_%s is                          \n" ,snshift);ie++;
    sprintf((*vd).sde[ie], "  generic (PG_WIDTH: integer);                             \n" );ie++;
    sprintf((*vd).sde[ie], "  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);      \n" );ie++;
    sprintf((*vd).sde[ie], "	   y : out std_logic_vector(PG_WIDTH-1 downto 0);    \n" );ie++;
    sprintf((*vd).sde[ie], "	   clk : in std_logic);                              \n" );ie++;
    sprintf((*vd).sde[ie], "end pg_log_shift_%s;                               \n" ,snshift);ie++;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of pg_log_shift_%s is             \n" ,snshift);ie++;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    if(nshift == 2) sprintf((*vd).sde[ie], "  y <= '0' & x(PG_WIDTH-2) & x(PG_WIDTH-5 downto 0) & \"00\"; \n" );ie++;
    if(nshift == 1) sprintf((*vd).sde[ie], "  y <= '0' & x(PG_WIDTH-2) & x(PG_WIDTH-4 downto 0) & '0';     \n" );ie++;
    if(nshift == 0) sprintf((*vd).sde[ie], "  y <= x(PG_WIDTH-1 downto 0);                          \n" );ie++;
    if(nshift == -1) sprintf((*vd).sde[ie], "  y <= '0' & x(PG_WIDTH-2) & '0' & x(PG_WIDTH-3 downto 1);   \n" );ie++;
    if(nshift == -2) sprintf((*vd).sde[ie], "  y <= '0' & x(PG_WIDTH-2) & \"00\" & x(PG_WIDTH-3 downto 2);\n" );ie++;
    sprintf((*vd).sde[ie], "                                                           \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                   \n" );ie++;
    (*vd).ie = ie;
  }
}  
