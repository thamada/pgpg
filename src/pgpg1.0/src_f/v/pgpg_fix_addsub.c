#include<stdio.h>
#include "pgpg.h"

void generate_pg_fix_addsub(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
	int i,ii;
	int nbit,nstage;
	char sx[STRLEN],sy[STRLEN],sz[STRLEN];
        char addsub_flag[STRLEN];
        static char sd[1000][STRLEN];
  int ic,im,ie,is;
  static int flag=1;

	strcpy(addsub_flag,sdata[1]);
	strcpy(sx,sdata[2]);
	strcpy(sy,sdata[3]);
	strcpy(sz,sdata[4]);
	nbit = atoi(sdata[5]);
	nstage = atoi(sdata[6]);

        printf("fix addsub: %s %s %s = %s : ",addsub_flag,sx,sy,sz);
	printf("nbit %d nstage %d\n",nbit,nstage); 

  if(flag==1){
  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_fix_addsub                                          \n" );ic++;
  sprintf((*vd).sdc[ic], "    generic(PG_WIDTH: integer;                                     \n" );ic++;
  sprintf((*vd).sdc[ic], "            PG_PIPELINE: integer;                                 \n" );ic++;
  sprintf((*vd).sdc[ic], "            PG_DIRECTION: string);                                 \n" );ic++;
  sprintf((*vd).sdc[ic], "    port(x,y : in std_logic_vector(PG_WIDTH-1 downto 0);           \n" );ic++;
  sprintf((*vd).sdc[ic], "	 z : out std_logic_vector(PG_WIDTH-1 downto 0);              \n" );ic++;
  sprintf((*vd).sdc[ic], "	 clk : in std_logic);                                        \n" );ic++;
  sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
  sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;
  (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_fix_addsub generic map (PG_WIDTH=>%d,PG_PIPELINE=>%d,     \n",(*vd).ucnt,nbit,nstage);im++;
  sprintf((*vd).sdm[im], "                              PG_DIRECTION=>\"%s\")                \n",addsub_flag);im++;
  sprintf((*vd).sdm[im], "               port map (x=>%s,y=>%s,z=>%s,clk=>pclk);             \n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "                                                                   \n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sy,nbit-1);is++;
  (*vd).is = is;

  if(flag==1){
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "entity pg_fix_addsub is                                            \n" );ie++;
  sprintf((*vd).sde[ie], "  generic(PG_WIDTH: integer;                                       \n" );ie++;
  sprintf((*vd).sde[ie], "          PG_PIPELINE: integer;                                    \n" );ie++;
  sprintf((*vd).sde[ie], "          PG_DIRECTION: string);                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  port(                                                            \n" );ie++;
  sprintf((*vd).sde[ie], "       x,y : in std_logic_vector(PG_WIDTH-1 downto 0);             \n" );ie++;
  sprintf((*vd).sde[ie], "       z : out std_logic_vector(PG_WIDTH-1 downto 0);              \n" );ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic                                          \n" );ie++;
  sprintf((*vd).sde[ie], "  );                                                               \n" );ie++;
  sprintf((*vd).sde[ie], "end pg_fix_addsub;                                                 \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "architecture rtl of pg_fix_addsub is                               \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], " component lpm_add_sub                                             \n" );ie++;
  sprintf((*vd).sde[ie], "  generic (LPM_WIDTH: integer;                                     \n" );ie++;
  sprintf((*vd).sde[ie], "	   LPM_PIPELINE: integer;                                    \n" );ie++;
  sprintf((*vd).sde[ie], "	   LPM_DIRECTION: string);                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);    \n" );ie++;
  sprintf((*vd).sde[ie], "      clock: in std_logic;                                         \n" );ie++;
  sprintf((*vd).sde[ie], "      result: out std_logic_vector(LPM_WIDTH-1 downto 0));         \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map (LPM_WIDTH=>PG_WIDTH,                \n" );ie++;
  sprintf((*vd).sde[ie], "                LPM_PIPELINE=>PG_PIPELINE,LPM_DIRECTION=>PG_DIRECTION)\n" );ie++;
  sprintf((*vd).sde[ie], "         port map(dataa=>x,datab=>y,clock=>clk,result=>z);         \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;
  (*vd).ie = ie;
  flag = 0;
  }
}
  


