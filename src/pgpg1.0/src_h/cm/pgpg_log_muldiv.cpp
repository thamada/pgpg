/* --- function list -----------------------------
       void std::generate_pg_log_muldiv(...);
   -----------------------------------------------*/ 
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include "pgpg.h"
namespace std{

  void generate_pg_log_muldiv(char sdata[][STRLEN],vhdl_description *vd)
  {
    int nbit,nstage;
    char sx[STRLEN],sy[STRLEN],sz[STRLEN];
    char muldiv_flag[STRLEN];
    int ic,im,ie,is;
    static int flag=1;

    strcpy(muldiv_flag,sdata[1]);
    strcpy(sx,sdata[2]);
    strcpy(sy,sdata[3]);
    strcpy(sz,sdata[4]);
    nbit = atoi(sdata[5]);
    nstage = atoi(sdata[6]);

    printf("log muldiv: %s %s %s = %s : ",muldiv_flag,sx,sy,sz);
    printf("nbit %d nstage %d\n",nbit,nstage); 

    if(flag==1){
      ic = (*vd).ic;
      sprintf((*vd).sdc[ic], "  component pg_log_muldiv                                        \n" );ic++;
      sprintf((*vd).sdc[ic], "    generic (PG_MULDIV : string;                                 \n" );ic++;
      sprintf((*vd).sdc[ic], "             PG_PIPELINE : integer;                              \n" );ic++;
      sprintf((*vd).sdc[ic], "             PG_WIDTH : integer);                                \n" );ic++;
      sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        \n" );ic++;
      sprintf((*vd).sdc[ic], "	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        \n" );ic++;
      sprintf((*vd).sdc[ic], "	     clk: in std_logic);                                   \n" );ic++;
      sprintf((*vd).sdc[ic], "  end component;                                                 \n" );ic++;
      sprintf((*vd).sdc[ic], "                                                                 \n" );ic++;
      (*vd).ic = ic;
    }

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  u%d: pg_log_muldiv generic map(PG_MULDIV=>\"%s\",\n",(*vd).ucnt,muldiv_flag);im++;
    sprintf((*vd).sdm[im], "                                PG_PIPELINE=>%d,\n",nstage)            ;im++;
    sprintf((*vd).sdm[im], "                                PG_WIDTH=>%d)\n",nbit)                 ;im++;
    sprintf((*vd).sdm[im], "                    port map(x=>%s,y=>%s,clk=>pclk,z=>%s);\n",sx,sy,sz);im++;
    sprintf((*vd).sdm[im], "                                                                  \n" );im++;
    (*vd).im = im;
    (*vd).ucnt++;

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit-1);is++;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sy,nbit-1);is++;
    (*vd).is = is;

    if(flag==1){
      ie= (*vd).ie;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "entity pg_log_muldiv is                                            \n" );ie++;
      sprintf((*vd).sde[ie], "  generic (PG_MULDIV: string;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "           PG_PIPELINE: integer;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "           PG_WIDTH: integer);                                     \n" );ie++;
      sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);            \n" );ie++;
      sprintf((*vd).sde[ie], "	   z : out std_logic_vector(PG_WIDTH-1 downto 0);            \n" );ie++;
      sprintf((*vd).sde[ie], "	   clk : in std_logic);                                      \n" );ie++;
      sprintf((*vd).sde[ie], "end pg_log_muldiv;                                                 \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "architecture rtl of pg_log_muldiv is                               \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "             LPM_PIPELINE: integer;                                \n" );ie++;
      sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          clock: in std_logic;                                     \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);                \n" );ie++;
      sprintf((*vd).sde[ie], "  z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);                \n" );ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-2,LPM_DIRECTION=>PG_MULDIV,\n" );ie++;
      sprintf((*vd).sde[ie], "                              LPM_PIPELINE=>PG_PIPELINE)           \n" );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x(PG_WIDTH-3 downto 0),datab=>y(PG_WIDTH-3 downto 0),\n" );ie++;
      sprintf((*vd).sde[ie], "                clock=>clk,result=>z(PG_WIDTH-3 downto 0));        \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;
      (*vd).ie = ie;
      flag=0;
    }
  }







}
