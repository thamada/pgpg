#include<stdio.h>
#include<stdlib.h> // atoi(),exit()
#include<string.h> // strcpy(),strcmp()
#include "pgpg.h"

void generate_pg_bits_and(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int j;
  int nbit;
  char sx[STRLEN],sy[STRLEN],sz[STRLEN];
  int ic,im,ie,is,iflag;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  strcpy(sz,sdata[3]);
  nbit = atoi(sdata[4]);

  printf("bits AND : %s = %s & %s : ",sx,sy,sz);
  printf("nbit %d\n",nbit);

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_bits_and_%d\n",nbit);ic++;
  iflag=0;
  for(j=0;j<ic-1;j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x : in std_logic_vector(%d downto 0);\n", nbit-1);ic++;
    sprintf((*vd).sdc[ic], "          y : in std_logic_vector(%d downto 0);\n", nbit-1);ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0));\n",nbit-1);ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_bits_and_%d ",(*vd).ucnt,nbit);im++;
  sprintf((*vd).sdm[im], "port map(x=>%s,y=>%s,z=>%s);\n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sy,nbit-1);is++;
  (*vd).is = is;

  if(iflag != 0) return;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "entity pg_bits_and_%d is\n",nbit);ie++;
  sprintf((*vd).sde[ie], "  port( x : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "        y : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0));\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "end pg_bits_and_%d;\n",nbit);ie++;
  sprintf((*vd).sde[ie], "architecture rtl of pg_bits_and_%d is\n",nbit);ie++;
  sprintf((*vd).sde[ie], "begin\n");ie++;
  sprintf((*vd).sde[ie], " z <= x and y;\n");ie++;
  sprintf((*vd).sde[ie], "end rtl;\n");ie++;
  (*vd).ie = ie;
}  
