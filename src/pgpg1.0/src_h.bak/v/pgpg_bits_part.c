#include<stdio.h>
#include<stdlib.h> // atoi(),exit()
#include<string.h> // strcpy(),strcmp()
#include "pgpg.h"

void generate_pg_bits_part(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int j;
  int nbitx,nbitz,nmsb,nlsb;
  char sx[STRLEN],sz[STRLEN];
  int ic,im,ie,is,iflag;

  strcpy(sx,sdata[1]);
  strcpy(sz,sdata[2]);
  nbitx  = atoi(sdata[3]);
  nmsb   = atoi(sdata[4]);
  nlsb   = atoi(sdata[5]);
  nbitz  = nmsb-nlsb+1;

  printf("bits part : %s = %s(%d downto %d) : ",sz,sx,nmsb,nlsb);
  printf("nbitx : %d, nbitz : %d\n",nbitx,nbitz);

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_bits_part_%d_%d_%d\n",nbitx,nmsb,nlsb);ic++;
  iflag=0;
  for(j=0;j<ic-1;j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x : in std_logic_vector(%d downto 0);\n", nbitx-1);ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0));\n",nbitz-1);ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_bits_part_%d_%d_%d ",(*vd).ucnt,nbitx,nmsb,nlsb);im++;
  sprintf((*vd).sdm[im], "port map(x=>%s,z=>%s);\n",sx,sz);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbitx-1);is++;
  (*vd).is = is;

  if(iflag != 0) return;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "entity pg_bits_part_%d_%d_%d is\n",nbitx,nmsb,nlsb);ie++;
  sprintf((*vd).sde[ie], "  port( x : in std_logic_vector(%d downto 0);\n",nbitx-1);ie++;
  sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0));\n",nbitz-1);ie++;
  sprintf((*vd).sde[ie], "end pg_bits_part_%d_%d_%d;\n",nbitx,nmsb,nlsb);ie++;
  sprintf((*vd).sde[ie], "architecture rtl of pg_bits_part_%d_%d_%d is\n",nbitx,nmsb,nlsb);ie++;
  sprintf((*vd).sde[ie], "begin\n" );ie++;
  sprintf((*vd).sde[ie], " z <= x(%d downto %d);\n",nmsb,nlsb);ie++;
  sprintf((*vd).sde[ie], "end rtl;\n");ie++;
  (*vd).ie = ie;
}  
