#include<stdio.h>
#include<stdlib.h> // atoi(),exit()
#include<string.h> // strcpy(),strcmp()
#include<math.h>   // abs()
#include "pgpg.h"

// included from pgpg.c
void itobc(int idata,char* sdata,int nbit);

void generate_pg_bits_shift(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int j;
  int nbit,nshift;
  char sx[STRLEN],sz[STRLEN];
  char snshift[STRLEN];
  int ic,im,ie,is,iflag;

  nshift = atoi(sdata[1]);
  strcpy(sx,sdata[2]);
  strcpy(sz,sdata[3]);
  nbit   = atoi(sdata[4]);

  if(nshift<0) sprintf(snshift,"m%d",-nshift);
  else         sprintf(snshift,"%d",nshift);

  { // error check
    int nsabs = abs(nshift);
    if(nsabs>=nbit){
      fprintf(stderr,"pg_bits_shift: nshift(=%d) must be less than nbit(=%d)\n",nsabs,nbit);
      exit(1);
    }
    if(nsabs==0){
      fprintf(stderr,"pg_bits_shift: nshift is ZERO.\n");
      exit(1);
    }
  }
  printf("bits shift : %s = %s nshift %d : ",sx,sz,nshift);
  printf("nbit %d snshift %s\n",nbit,snshift);

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_bits_shift_%d_%s\n",nbit,snshift);ic++;
  iflag=0;
  for(j=0;j<ic-1;j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x : in std_logic_vector(%d downto 0);\n", nbit-1);ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0));\n",nbit-1);ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_bits_shift_%d_%s ",(*vd).ucnt,nbit,snshift);im++;
  sprintf((*vd).sdm[im], "port map(x=>%s,z=>%s);\n",sx,sz);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  (*vd).is = is;

  if(iflag != 0) return;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "entity pg_bits_shift_%d_%s is\n",nbit,snshift);ie++;
  sprintf((*vd).sde[ie], "  port( x : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0));\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "end pg_bits_shift_%d_%s;\n",nbit,snshift);ie++;
  sprintf((*vd).sde[ie], "architecture rtl of pg_bits_shift_%d_%s is\n",nbit,snshift);ie++;
  sprintf((*vd).sde[ie], "begin\n");ie++;

  if(nshift==1){
    sprintf((*vd).sde[ie], " z <= x(%d downto 0) & '0';\n",nbit-2);ie++;
  }else if(nshift==-1){
    sprintf((*vd).sde[ie], " z <= '0' & x(%d downto 1);\n",nbit-1);ie++;
  }else if(nshift>1){
    char _zero[STRLEN];
    itobc(0,_zero,nshift);
    if(nshift==(nbit-1)){
      sprintf((*vd).sde[ie], " z <= x(0) & \"%s\";\n",_zero);ie++;
    }else{
      sprintf((*vd).sde[ie], " z <= x(%d downto 0) & \"%s\";\n",nbit-1-nshift,_zero);ie++;
    }
  }else{ // if(nshift<0)
    char _zero[STRLEN];
    int _nabs;
    _nabs = -nshift;
    itobc(0,_zero,_nabs);
    if(_nabs==(nbit-1)){
      sprintf((*vd).sde[ie], " z <= \"%s\" & x(%d);\n",_zero,nbit-1);ie++;
    }else{
      sprintf((*vd).sde[ie], " z <= \"%s\" & x(%d downto %d);\n",_zero,nbit-1,_nabs);ie++;
    }
  }
  sprintf((*vd).sde[ie], "end rtl;\n");ie++;
  (*vd).ie = ie;
}  
