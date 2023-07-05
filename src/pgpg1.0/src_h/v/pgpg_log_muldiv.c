#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy(),strcmp()
#include "pgpg.h"
#include "pgpg_submodule.h"

void generate_pg_log_muldiv(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int nbit,nstage,istage;
  char sx[HSTRLEN],sy[HSTRLEN],sz[HSTRLEN];
  char muldiv_flag[HSTRLEN];
  int ic,im,ie,is;
  int iflag,j;
  int sflag=0;
  char func_name[HSTRLEN];
  
  strcpy(muldiv_flag,sdata[1]);
  strcpy(sx,sdata[2]);
  strcpy(sy,sdata[3]);
  strcpy(sz,sdata[4]);
  nbit = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  printf("log muldiv: %s %s %s = %s : ",muldiv_flag,sx,sy,sz);
  printf("nbit %d nstage %d\n",nbit,nstage); 

  if(strcmp(muldiv_flag,"MUL")==0){
    sprintf(func_name,"pg_log_mul_%d_%d",nbit,nstage);
    strcpy(muldiv_flag,"ADD");
  }else if(strcmp(muldiv_flag,"DIV")==0){
    sprintf(func_name,"pg_log_div_%d_%d",nbit,nstage);
    strcpy(muldiv_flag,"SUB");
  }else if(strcmp(muldiv_flag,"SDIV")==0){
    sprintf(func_name,"pg_log_sdiv_%d_%d",nbit,nstage);
    strcpy(muldiv_flag,"SUB");
    sflag = 1;
  }else{
    sprintf(func_name,"pg_log_muldiv_ERROR");
  }

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component %s\n",func_name);ic++;
  iflag=0;
  for(j=0;j<ic-1;j++) if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;

  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(%d downto 0);\n",nbit-1);ic++;
    sprintf((*vd).sdc[ic], "            z : out std_logic_vector(%d downto 0);  \n",nbit-1);ic++;
    sprintf((*vd).sdc[ic], "          clk : in std_logic);\n");ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s port map(x=>%s,y=>%s,z=>%s,clk=>pclk);\n",
          (*vd).ucnt,func_name,sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sy,nbit-1);is++;
  (*vd).is = is;

  if(iflag==0){
    ie= (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "entity %s is\n",func_name);ie++;
    sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
    sprintf((*vd).sde[ie], "          z : out std_logic_vector(%d downto 0);\n",nbit-1);ie++;
    sprintf((*vd).sde[ie], "        clk : in std_logic);\n");ie++;
    sprintf((*vd).sde[ie], "end %s;\n",func_name);ie++;
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "architecture rtl of %s is\n",func_name);ie++;
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "  component lpm_add_sub\n");ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;\n");ie++;
    sprintf((*vd).sde[ie], "             LPM_PIPELINE: integer;\n");ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);\n");ie++;  
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));\n");ie++;
    sprintf((*vd).sde[ie], "  end component;\n");ie++;
    sprintf((*vd).sde[ie], "\n");ie++;
    if(nstage>=2){
      for(istage=0;istage<(nstage-1);istage++){
	sprintf((*vd).sde[ie], " signal sign%d,nz%d : std_logic;\n", istage, istage);ie++;
      }
    }
    if(sflag==1){
      sprintf((*vd).sde[ie], " signal addx,addy,addz: std_logic_vector(%d downto 0);\n",nbit-2);ie++;
    }else{
      sprintf((*vd).sde[ie], " signal addx,addy,addz: std_logic_vector(%d downto 0); \n",nbit-3);ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "begin\n");ie++;
    sprintf((*vd).sde[ie], "\n");ie++;
    if(nstage==0){
      sprintf((*vd).sde[ie], "  z(%d) <= x(%d) xor y(%d);\n",nbit-1,nbit-1,nbit-1);ie++;
      sprintf((*vd).sde[ie], "  z(%d) <= x(%d) and y(%d);\n",nbit-2,nbit-2,nbit-2);ie++;
    }
    if(nstage==1){
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      z(%d) <= x(%d) xor y(%d);\n",nbit-1,nbit-1,nbit-1);ie++;
      sprintf((*vd).sde[ie], "      z(%d) <= x(%d) and y(%d);\n",nbit-2,nbit-2,nbit-2);ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
    }
    if(nstage>=2){
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign0 <= x(%d) xor y(%d);\n",nbit-1,nbit-1);ie++;
      sprintf((*vd).sde[ie], "      nz0 <= x(%d) and y(%d);\n",nbit-2,nbit-2);ie++;
      for(istage=1;istage<(nstage-1);istage++){
	sprintf((*vd).sde[ie], "      sign%d <= sign%d;\n", istage, istage-1);ie++;
	sprintf((*vd).sde[ie], "      nz%d <= nz%d;\n", istage, istage-1);ie++;
      }
      sprintf((*vd).sde[ie], "      z(%d) <= sign%d;\n",nbit-1,nstage-2);ie++;
      sprintf((*vd).sde[ie], "      z(%d) <= nz%d;\n",nbit-2,nstage-2);ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
    }

    if(sflag==1){
      char zero[HSTRLEN];
      sprintf((*vd).sde[ie], "  addx <= '0' & x(%d downto 0);\n",nbit-3);ie++;
      sprintf((*vd).sde[ie], "  addy <= '0' & y(%d downto 0);\n",nbit-3);ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(");ie++;
      sprintf((*vd).sde[ie], "LPM_WIDTH=>%d,LPM_PIPELINE=>%d,LPM_DIRECTION=>\"SUB\")\n",nbit-1,nstage);ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>addx,datab=>addy,clock=>clk,result=>addz);\n");ie++;
      i2bit(0,zero,nbit-2);
      sprintf((*vd).sde[ie], "  with addz(%d) select\n",nbit-2);ie++;
      sprintf((*vd).sde[ie], "    z(%d downto 0) <= \"%s\" when '1',\n",nbit-3,zero);ie++;
      sprintf((*vd).sde[ie], "    addz(%d downto 0) when others;\n",nbit-3);ie++;
    }else{
      sprintf((*vd).sde[ie], "  addx <= x(%d downto 0);\n",nbit-3);ie++;
      sprintf((*vd).sde[ie], "  addy <= y(%d downto 0);\n",nbit-3);ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,",nbit-2);ie++;
      sprintf((*vd).sde[ie], "LPM_PIPELINE=>%d,LPM_DIRECTION=>\"%s\")\n",nstage,muldiv_flag);ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>addx,datab=>addy,clock=>clk,result=>addz);\n");ie++;
      sprintf((*vd).sde[ie], "  z(%d downto 0) <= addz;\n",nbit-3);ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "end rtl;\n");ie++;
    (*vd).ie = ie;
  }
}  
