/* --- function list -----------------------------
       void std::generate_pg_fix_accum(...);
   -----------------------------------------------*/ 

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include "pgpg.h"
namespace std{

  void generate_pg_fix_accum(char sdata[][STRLEN],vhdl_description *vd)
  {
    int i;
    int nbit,nstage,nbits;
    char fx[STRLEN],sx[STRLEN];
    char sd[STRLEN];
    int ic,im,ie,is,nzero,iflag,j;
    static int flag=1;
    int nbitl,nbith;

    strcpy(fx,sdata[1]);
    strcpy(sx,sdata[2]);
    nbit = atoi(sdata[3]);
    nbits = atoi(sdata[4]);
    nstage = atoi(sdata[5]);

    printf("fix accum: %s += %s : ",fx,sx);
    printf("nbit %d nbit_accum %d nstage %d\n",nbit,nbits,nstage); 

    ic = (*vd).ic;
    sprintf((*vd).sdc[ic], "  component pg_fix_accum_%d_%d_%d                          \n", nbit,nbits,nstage);ic++;
    iflag=0;
    for(j=0;j<(ic-1);j++){
      if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
    }
    if(iflag==0){
      sprintf((*vd).sdc[ic], "    port (fdata: in std_logic_vector(%d downto 0);    \n", nbit-1 );ic++;
      sprintf((*vd).sdc[ic], "          sdata: out std_logic_vector(%d downto 0);   \n" , nbits-1 );ic++;
      sprintf((*vd).sdc[ic], "          run: in std_logic;                                        \n" );ic++;
      sprintf((*vd).sdc[ic], "          clk: in std_logic);                                       \n" );ic++;
      sprintf((*vd).sdc[ic], "  end component;                                                    \n" );ic++;
      sprintf((*vd).sdc[ic], "                                                                    \n" );ic++;
      (*vd).ic = ic;
    }

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  u%d: pg_fix_accum_%d_%d_%d\n",(*vd).ucnt,nbit,nbits,nstage);im++;
    sprintf((*vd).sdm[im], "                   port map(fdata=>%s,sdata=>%s,run=>runr,clk=>pclk);\n",fx,sx);im++;
    sprintf((*vd).sdm[im], "                                                                    \n" );im++;
    (*vd).im = im;
    (*vd).ucnt++;

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",fx,nbit-1);is++;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbits-1);is++;
    (*vd).is = is;

    if(iflag==0){
      ie = (*vd).ie;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "entity pg_fix_accum_%d_%d_%d is                     \n",nbit,nbits,nstage );ie++;
      sprintf((*vd).sde[ie], "  port (fdata: in std_logic_vector(%d downto 0);      \n", nbit-1 );ie++;
      sprintf((*vd).sde[ie], "        sdata: out std_logic_vector(%d downto 0);     \n", nbits-1 );ie++;
      sprintf((*vd).sde[ie], "        run : in std_logic;                                         \n" );ie++;
      sprintf((*vd).sde[ie], "        clk : in std_logic);                                        \n" );ie++;
      sprintf((*vd).sde[ie], "end pg_fix_accum_%d_%d_%d;                           \n",nbit,nbits,nstage );ie++;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "architecture rtl of pg_fix_accum_%d_%d_%d is        \n",nbit,nbits,nstage );ie++;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "  component lpm_add_sub                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer);                                   \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   \n" );ie++;
      sprintf((*vd).sde[ie], "          cin : in std_logic;                                       \n" );ie++;
      sprintf((*vd).sde[ie], "          add_sub : in std_logic;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "          cout : out std_logic;                                     \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      if(nstage==1){
	sprintf((*vd).sde[ie], "  signal fx: std_logic_vector(%d downto 0);           \n", nbits-1 );ie++;
	sprintf((*vd).sde[ie], "  signal run1 : std_logic;                                          \n" );ie++;
	sprintf((*vd).sde[ie], "  signal signx,cx1: std_logic;                                      \n" );ie++;
	sprintf((*vd).sde[ie], "  signal fxl,sxl1,sxl2: std_logic_vector(%d downto 0);\n", nbits-1 );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                                \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                  \n" );ie++;
	sprintf((*vd).sde[ie], "      run1 <= run;                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	nzero = nbits - nbit + 1;
	strcpy(sd,"");
	for(i=0;i<nzero;i++) strcat(sd,"0");
	sprintf((*vd).sde[ie], "  fx <= \"%s\" & fdata(%d downto 0);                      \n", sd,nbit-2);ie++;
	sprintf((*vd).sde[ie], "  signx <= not fdata(%d);                                   \n", nbit-1 );ie++;
	sprintf((*vd).sde[ie], "  fxl <= fx(%d downto 0);                                   \n", nbits-1);ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d)                  \n", nbits);ie++;
	sprintf((*vd).sde[ie], "      port map(dataa=>sxl2,                                         \n" );ie++;
	sprintf((*vd).sde[ie], "               datab=>fxl,add_sub=>signx,                           \n" );ie++;
	sprintf((*vd).sde[ie], "               cin=>fdata(%d),                               \n", nbit-1);ie++;
	sprintf((*vd).sde[ie], "               cout=>cx1,result=>sxl1);                             \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                                \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                  \n" );ie++;
	sprintf((*vd).sde[ie], "      if(run1 = '1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "        sxl2 <= sxl1;                                               \n" );ie++;
	sprintf((*vd).sde[ie], "      else                                                          \n" );ie++;
	sprintf((*vd).sde[ie], "        if(run='1') then                                            \n" );ie++;
	strcpy(sd,"");
	for(i=0;i<nbits;i++) strcat(sd,"0");
	sprintf((*vd).sde[ie], "          sxl2 <= \"%s\";                                        \n", sd);ie++;
	sprintf((*vd).sde[ie], "        end if;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "      end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  sdata(%d downto 0) <= sxl2;                               \n", nbits-1);ie++;
      }
      if(nstage==2){
	sprintf((*vd).sde[ie], "  signal fx: std_logic_vector(%d downto 0);               \n", nbits-1);ie++;
	sprintf((*vd).sde[ie], "  signal run2,run1 : std_logic;                                   \n" );ie++;
	sprintf((*vd).sde[ie], "  signal signx,signx1,cx1,cx2: std_logic;                         \n" );ie++;
	nbitl = nbits/2;
	nbith = nbits - nbitl;
	sprintf((*vd).sde[ie], "  signal fxl,sxl1,sxl2: std_logic_vector(%d downto 0);            \n",nbitl-1);ie++;
	sprintf((*vd).sde[ie], "  signal fxh,fxh1,sxh1,sxh2 : std_logic_vector(%d downto 0);      \n",nbith-1);ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "begin                                                             \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
	sprintf((*vd).sde[ie], "      run2 <= run1;                                               \n" );ie++;
	sprintf((*vd).sde[ie], "      run1 <= run;                                                \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	nzero = nbits - nbit + 1;
	strcpy(sd,"");
	for(i=0;i<nzero;i++) strcat(sd,"0");
	sprintf((*vd).sde[ie], "  fx <= \"%s\" & fdata(%d downto 0);                              \n",sd,nbit-2);ie++;
	sprintf((*vd).sde[ie], "  signx <= not fdata(%d);                                         \n",nbit-1);ie++;
	sprintf((*vd).sde[ie], "  fxl <= fx(%d downto 0);                                         \n",nbitl-1);ie++;
	sprintf((*vd).sde[ie], "  fxh <= fx(%d downto %d);                                        \n",nbits-1,nbitl);ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d)                      \n",nbitl);ie++;
	sprintf((*vd).sde[ie], "      port map(dataa=>sxl2,                                       \n" );ie++;
	sprintf((*vd).sde[ie], "               datab=>fxl,add_sub=>signx,                         \n" );ie++;
	sprintf((*vd).sde[ie], "               cin=>fdata(%d),                                    \n",nbit-1);ie++;
	sprintf((*vd).sde[ie], "               cout=>cx1,result=>sxl1);                           \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
	sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
	sprintf((*vd).sde[ie], "        sxl2 <= sxl1;                                             \n" );ie++;
	sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
	sprintf((*vd).sde[ie], "        if(run='1') then                                          \n" );ie++;
	strcpy(sd,"");
	for(i=0;i<nbitl;i++) strcat(sd,"0");
	sprintf((*vd).sde[ie], "          sxl2 <= \"%s\";                                         \n",sd);ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "      cx2 <= cx1;                                                 \n" );ie++;
	sprintf((*vd).sde[ie], "      fxh1 <= fxh;                                                \n" );ie++;
	sprintf((*vd).sde[ie], "      signx1 <= signx;                                            \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d)                      \n",nbith);ie++;
	sprintf((*vd).sde[ie], "      port map(dataa=>sxh2,                                       \n" );ie++;
	sprintf((*vd).sde[ie], "               datab=>fxh1,add_sub=>signx1,                       \n" );ie++;
	sprintf((*vd).sde[ie], "               cin=>cx2,result=>sxh1);                            \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
	sprintf((*vd).sde[ie], "      if(run2 = '1') then                                         \n" );ie++;
	sprintf((*vd).sde[ie], "        sxh2 <= sxh1;                                             \n" );ie++;
	sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
	sprintf((*vd).sde[ie], "        if(run1='1') then                                         \n" );ie++;
	strcpy(sd,"");
	for(i=0;i<nbith;i++) strcat(sd,"0");
	sprintf((*vd).sde[ie], "          sxh2 <= \"%s\";                                         \n",sd);ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  sdata(%d downto 0) <= sxl2;                                     \n", nbitl-1);ie++;
	sprintf((*vd).sde[ie], "  sdata(%d downto %d) <= sxh2;                                    \n", nbits-1,nbitl);ie++;
      }
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
      (*vd).ie = ie;
      flag=0;
    }

  }  





}
