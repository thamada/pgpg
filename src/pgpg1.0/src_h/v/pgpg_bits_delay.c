// This is same as pg_pdelay module except for entity name.
#include<stdio.h>
#include<stdlib.h> //atoi()
#include<string.h> //strcpy(),strcmp()
#include "pgpg.h"

void generate_pg_bits_delay(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int i;
  int nbit,ndelay;
  char sx[STRLEN],sy[STRLEN];
  char func_name[STRLEN];
  int ic,im,ie,is,iflag,j;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit = atoi(sdata[3]);
  ndelay = atoi(sdata[4]);

  printf("pipeline delay: %s = %s ",sx,sy);
  printf("ndelay %d: nbit %d\n",ndelay,nbit);

  ic = (*vd).ic;
  sprintf(func_name,"pg_bits_delay_%d",ndelay);
  sprintf((*vd).sdc[ic], "  component %s\n",func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    generic (PG_WIDTH: integer);\n" );ic++;
    sprintf((*vd).sdc[ic], "    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);\n" );ic++;
    sprintf((*vd).sdc[ic], "	     y : out std_logic_vector(PG_WIDTH-1 downto 0);\n" );ic++;
    sprintf((*vd).sdc[ic], "	     clk: in std_logic);\n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;\n" );ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s generic map(PG_WIDTH=>%d) ",(*vd).ucnt,func_name,nbit);im++;
  sprintf((*vd).sdm[im], "port map(x=>%s,y=>%s,clk=>pclk);\n",sx,sy);im++;
  sprintf((*vd).sdm[im], "\n" );im++;
  (*vd).im = im; 
  (*vd).ucnt++;

  is = (*vd).is;
  if(strcmp(sx,"p_run")!=0){
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  }
  (*vd).is = is;

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "entity %s is\n" ,func_name);ie++;
    sprintf((*vd).sde[ie], "  generic (PG_WIDTH: integer);\n" );ie++;
    sprintf((*vd).sde[ie], "  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "	   y : out std_logic_vector(PG_WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "	   clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "end %s;\n" ,func_name);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of %s is\n" ,func_name);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    for(i=0;i<ndelay+1;i++){
      sprintf((*vd).sde[ie], "  signal x%d : std_logic_vector(PG_WIDTH-1 downto 0);   \n",i);ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  x0 <= x;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clk) begin\n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
    for(i=ndelay;i>0;i--){
      sprintf((*vd).sde[ie], "      x%d <= x%d;\n",i,i-1);ie++;
    }
    sprintf((*vd).sde[ie], "    end if;\n" );ie++;
    sprintf((*vd).sde[ie], "  end process;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  y <= x%d;\n",ndelay);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
    (*vd).ie = ie;
  }

}  







