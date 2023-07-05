/* 2003/09/22 : separate lcell table generation */
#include<stdio.h>
#include<stdlib.h> // malloc(),free(),atoi()
#include<string.h> // strcpy(),strcmp(),strcat()
#include<math.h>   // pow(),log()
#include "pgpg.h"
#include "pgpg_submodule.h"
         // itobc(),generate_pg_reged_shift_ltof(),
         // generate_pg_table(),generate_pg_unreg_table()
#include "pgpg_lcelltable_conv_ltof.h" // create_table_lcell_conv_ltof(),generate_pg_table_lcell_of_conv_ltof()
#define LONG long long int
static struct {
  char lcromname[256];
  int rom_ilen;
  int rom_olen;
  int rom_nstage;
} this;


static void create_table_conv_ltof(nbit_in,nbit_out,tablecnt)
int nbit_in,nbit_out;
{
  int i,iy,nentry,outmask;
  double inscale,outscale,x;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"ltf%1d.mif",tablecnt);
  fp = fopen(filename,"w");
  nentry = 1<<nbit_in;
  inscale = (double)nentry;
  outscale = (double)(1<<nbit_out);

  fprintf(fp,"DEPTH = %d;\n",nentry);
  fprintf(fp,"WIDTH = %d;\n\n",nbit_out);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  for(i=0;i<nentry;i++){
    x = pow(2.0,((double) i)/inscale);
    iy = (int) (outscale*(x-1.0)+0.5);
    outmask = ((int)outscale)-1;
    fprintf(fp,"    %X : %X ;\n",i,outmask&iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}

static void generate_pg_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl;
{
  int ie,j,nbit_sin,nbit_shift,nbit_smax;
  char sd[STRLEN];
  int iflag;

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity shift_ltof_%d_%d is                        \n", nbit_in,nbit_out );ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    nbit_sin = nbit_out + 2*(nbit_in-1);
    nbit_smax =  nbit_in + nbit_out - 1;
    nbit_shift = (int)(log((double)(nbit_smax))/log(2.0) + 1.0);

    sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);       \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);     \n", nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "        clk : in std_logic;                                         \n" );ie++;
    sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));   \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end shift_ltof_%d_%d;                             \n", nbit_in,nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of shift_ltof_%d_%d is\n",            nbit_in,nbit_out );ie++; 
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  component lpm_clshift                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH : POSITIVE;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_WIDTHDIST : POSITIVE);                             \n" );ie++;
    sprintf((*vd).sde[ie], "    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  signal d0,o0 : std_logic_vector(%d downto 0);          \n",nbit_sin-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    itobc(0,sd,nbit_sin-nbit_in);
    sprintf((*vd).sde[ie], "  d0 <= \"%s\" & indata;                                        \n", sd );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>%d,LPM_WIDTHDIST=>%d)\n", nbit_sin,nbit_shift );ie++;
    sprintf((*vd).sde[ie], "          port map (data=>d0,distance=>control(%d downto 0),result=>o0);\n",nbit_shift-1 );ie++;
    sprintf((*vd).sde[ie], "  outdata <= o0(%d downto %d);                \n",nbit_smax-1,nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;  
  }
}

void generate_pg_conv_ltof(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int nbit_fix,nbit_log,nbit_man,nstage,nbit_frac,nbit_ctl;
  char sx[HSTRLEN],sy[HSTRLEN];
  char func_name[HSTRLEN];
  char sd[HSTRLEN];
  int ic,im,ie,is;
  int iflag, j;
  static int tablecnt=0;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit_log = atoi(sdata[3]);
  nbit_man = atoi(sdata[4]);
  nbit_fix = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  printf("conv ltof: %s = %s : ",sx,sy);
  printf("nbit log %d man %d fix %d nstage %d\n",nbit_log,nbit_man,nbit_fix,nstage);
  if(nstage<1){
    nstage = 1;
    printf("nstage: fixed to %d\n",nstage);
  }
  if(nstage>5){
    nstage = 5;
    printf("nstage: fixed to %d\n",nstage);
  }

  /*** generate component decleration ****/

  ic = (*vd).ic;
  sprintf(func_name,"pg_conv_ltof_%d_%d_%d_%d",nbit_log,nbit_man,nbit_fix,nstage);
  sprintf((*vd).sdc[ic], "  component %s      \n", func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(logdata : in std_logic_vector(%d downto 0);\n",nbit_log-1);ic++;
    sprintf((*vd).sdc[ic], "	     fixdata : out std_logic_vector(%d downto 0);\n",nbit_fix-1);ic++;
    sprintf((*vd).sdc[ic], "	     clk : in std_logic);\n");ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "                                                                  \n" );ic++;
    (*vd).ic = ic; 
  }
  
  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s port map (logdata=>%s,fixdata=>%s,clk=>pclk);\n",(*vd).ucnt,func_name,sx,sy);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_log-1);is++;
  (*vd).is = is;

  /*** generate component ********************************************************/
  if(iflag !=0) return;
  // --- ROM GENERATION ---
  nbit_frac = nbit_man + 1;
  if((*vd).alteraesbflag){
    create_table_conv_ltof(nbit_man,nbit_frac-1,tablecnt);
  }else{
    char name[256];
    if(nstage>=2) this.rom_nstage = 1; else  this.rom_nstage = 0;
    create_table_lcell_conv_ltof(nbit_fix,nbit_man,this.rom_nstage,name);
    strcpy(this.lcromname,name);
    this.rom_ilen = nbit_man;
    this.rom_olen = nbit_man;
  }

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "-- ***************************************************************\n" );ie++;
  sprintf((*vd).sde[ie], "-- * PGPG LOGARITHMIC TO FIXED-POINT FORMAT CONVERTER            *\n" );ie++;
  sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    *\n" );ie++;
  sprintf((*vd).sde[ie], "-- *  VERSION: 2.00                                              *\n" );ie++;
  sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Jun 03 22:52:01 JST 2003              *\n" );ie++;
  sprintf((*vd).sde[ie], "-- ***************************************************************\n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "entity %s is\n", func_name);ie++;
  sprintf((*vd).sde[ie], "  port(logdata : in std_logic_vector(%d downto 0);\n",nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "       fixdata : out std_logic_vector(%d downto 0);\n",nbit_fix-1 );ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);\n" );ie++;
  sprintf((*vd).sde[ie], "end %s;\n", func_name);ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  sprintf((*vd).sde[ie], "architecture rtl of %s is\n", func_name);ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  if((*vd).alteraesbflag){
    // ESB ROM
    if(nstage>=2){
      sprintf((*vd).sde[ie], "  component table\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_table\n" );ie++;
    }
    sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer;\n" );ie++;
    sprintf((*vd).sde[ie], "             OUT_WIDTH: integer;\n" );ie++;
    sprintf((*vd).sde[ie], "             TABLE_FILE: string);\n" );ie++;
    sprintf((*vd).sde[ie], "    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);   \n" );ie++;
    sprintf((*vd).sde[ie], "         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "         clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
  }else{
    // Lcell ROM
    sprintf((*vd).sde[ie], "  component %s\n",this.lcromname);ie++;
    sprintf((*vd).sde[ie], "    port(indata : in std_logic_vector(%d downto 0);\n",this.rom_ilen - 1 );ie++;
    sprintf((*vd).sde[ie], "         outdata : out std_logic_vector(%d downto 0);\n",this.rom_olen - 1 );ie++;
    sprintf((*vd).sde[ie], "         clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
  }
  sprintf((*vd).sde[ie], "\n" );ie++;


  nbit_ctl = nbit_log - nbit_man - 2;
  if(nstage>=3){
    int nbit_exp = nbit_log-nbit_man-2;
    sprintf((*vd).sde[ie], "  component reged_shift_ltof_%d_%d_%d_%d\n", nbit_man+1,nbit_exp,nbit_fix-1,nstage-2);ie++;
    sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);\n",nbit_frac-1 );ie++;
    sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n",   nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "          clk : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0)); \n",nbit_fix-2 );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  component shift_ltof_%d_%d\n", nbit_frac,nbit_fix-1 );ie++;
    sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);\n",nbit_frac-1 );ie++;
    sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n",   nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "          clk : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0)); \n",nbit_fix-2 );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
  }

  sprintf((*vd).sde[ie], "  signal frac1 : std_logic_vector(%d downto 0);\n",nbit_frac-2 );ie++;
  sprintf((*vd).sde[ie], "  signal frac2 : std_logic_vector(%d downto 0);\n",nbit_frac-1 );ie++;
  sprintf((*vd).sde[ie], "  signal sign1 : std_logic;\n" );ie++;
  sprintf((*vd).sde[ie], "  signal sign2 : std_logic;\n" );ie++;
  sprintf((*vd).sde[ie], "  signal sign3 : std_logic;\n" );ie++;
  sprintf((*vd).sde[ie], "  signal sign4 : std_logic;\n" );ie++;
  sprintf((*vd).sde[ie], "  signal nz1 : std_logic;\n" );ie++;
  sprintf((*vd).sde[ie], "  signal exp1 : std_logic_vector(%d downto 0);\n",nbit_ctl-1 );ie++;
  sprintf((*vd).sde[ie], "  signal fix1 : std_logic_vector(%d downto 0);\n",nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  sprintf((*vd).sde[ie], "begin\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  if(nstage>=2){
    sprintf((*vd).sde[ie], "  process(clk) begin\n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
    sprintf((*vd).sde[ie], "      sign1 <= logdata(%d);\n",nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "      nz1 <= logdata(%d);\n",nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "      exp1 <= logdata(%d downto %d);\n",nbit_log-3,nbit_man);ie++;
    sprintf((*vd).sde[ie], "    end if;\n" );ie++;
    sprintf((*vd).sde[ie], "  end process;\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  sign1 <= logdata(%d);\n",nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "  nz1 <= logdata(%d);\n",nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  exp1 <= logdata(%d downto %d);\n",nbit_log-3,nbit_man);ie++;
  }
  sprintf((*vd).sde[ie], "\n" );ie++;
  if((*vd).alteraesbflag){
    if(nstage>=2){
      sprintf((*vd).sde[ie], "  u1: table generic map (IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"ltf%1d.mif\")\n",
	      nbit_man,nbit_frac-1,tablecnt );ie++;
    }else{
      sprintf((*vd).sde[ie], "  u1: unreg_table generic map (IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"ltf%1d.mif\")\n",
	      nbit_man,nbit_frac-1,tablecnt );ie++;
    }
  }else{
    sprintf((*vd).sde[ie], "  u1: %s\n",this.lcromname);ie++;
  }
  tablecnt++;
  sprintf((*vd).sde[ie], "          port map(indata=>logdata(%d downto 0),outdata=>frac1,clk=>clk);\n",nbit_man-1 );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  itobc(0,sd,nbit_frac);
  sprintf((*vd).sde[ie], "  with nz1 select\n" );ie++;
  sprintf((*vd).sde[ie], "    frac2 <= '1' & frac1 when '1',\n" );ie++;
  sprintf((*vd).sde[ie], "             \"%s\" when others;\n",sd );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  sprintf((*vd).sde[ie], "  -------------------------------------------------------------------\n" );ie++;
  sprintf((*vd).sde[ie], "  -- PIPELINE 3,4,5 STAGES\n" );ie++;
  if(nstage>=3){
    int nbit_exp = nbit_log-nbit_man-2;
    sprintf((*vd).sde[ie], "  u3: reged_shift_ltof_%d_%d_%d_%d\n",nbit_man+1,nbit_exp,nbit_fix-1,nstage-2);ie++;
    sprintf((*vd).sde[ie], "      port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);\n");ie++;
    sprintf((*vd).sde[ie], "  process(clk) begin                   \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then     \n" );ie++;
    sprintf((*vd).sde[ie], "      sign2 <= sign1;                  \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                            \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                         \n" );ie++;
    if(nstage>=4){
      sprintf((*vd).sde[ie], "  process(clk) begin                 \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then   \n" );ie++;
      sprintf((*vd).sde[ie], "      sign3 <= sign2;                \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                          \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                       \n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "--  process(clk) begin               \n" );ie++;
      sprintf((*vd).sde[ie], "--    if(clk'event and clk='1') then \n" );ie++;
      sprintf((*vd).sde[ie], "      sign3 <= sign2;                \n" );ie++;
      sprintf((*vd).sde[ie], "--    end if;                        \n" );ie++;
      sprintf((*vd).sde[ie], "--  end process;                     \n" );ie++;
    }
    if(nstage>=5){
      sprintf((*vd).sde[ie], "  process(clk) begin                 \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then   \n" );ie++;
      sprintf((*vd).sde[ie], "      sign4 <= sign3;                \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                          \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                       \n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "--  process(clk) begin               \n" );ie++;
      sprintf((*vd).sde[ie], "--    if(clk'event and clk='1') then \n" );ie++;
      sprintf((*vd).sde[ie], "      sign4 <= sign3;                \n" );ie++;
      sprintf((*vd).sde[ie], "--    end if;                        \n" );ie++;
      sprintf((*vd).sde[ie], "--  end process;                     \n" );ie++;
    }
  }else{
    sprintf((*vd).sde[ie], "  u3: shift_ltof_%d_%d\n", nbit_frac,nbit_fix-1 );ie++;
    sprintf((*vd).sde[ie], "            port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);\n" );ie++;
    sprintf((*vd).sde[ie], "  sign4 <= sign3;\n" );ie++;
    sprintf((*vd).sde[ie], "  sign3 <= sign2;\n" );ie++;
    sprintf((*vd).sde[ie], "  sign2 <= sign1;\n" );ie++;
  }
  sprintf((*vd).sde[ie], "  -------------------------------------------------------------------\n" );ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  if(nstage>=1){
    sprintf((*vd).sde[ie], "  process(clk) begin                \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then  \n" );ie++;
    sprintf((*vd).sde[ie], "      fixdata(%d) <= sign4;         \n",nbit_fix-1 );ie++;
    sprintf((*vd).sde[ie], "      fixdata(%d downto 0) <= fix1; \n",nbit_fix-2 );ie++;
    sprintf((*vd).sde[ie], "    end if;                         \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                      \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "--  process(clk) begin              \n" );ie++;
    sprintf((*vd).sde[ie], "--    if(clk'event and clk='1') then\n" );ie++;
    sprintf((*vd).sde[ie], "      fixdata(%d) <= sign4;         \n",nbit_fix-1 );ie++;
    sprintf((*vd).sde[ie], "      fixdata(%d downto 0) <= fix1; \n",nbit_fix-2 );ie++;
    sprintf((*vd).sde[ie], "--    end if;                       \n" );ie++;
    sprintf((*vd).sde[ie], "--  end process;                    \n" );ie++;
  }
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
  (*vd).ie = ie;

  if(nstage>=3){
    int shifter_np = nstage - 2;
    generate_pg_reged_shift_ltof(vd,nbit_frac,nbit_fix-1,nbit_ctl,shifter_np);
  }else{
    generate_pg_shift_ltof(vd,nbit_frac,nbit_fix-1,nbit_ctl);
  }


  if((*vd).alteraesbflag){
    if(nstage>=2) generate_pg_table(vd); else generate_pg_unreg_table(vd);
  }else{
    generate_pg_table_lcell_of_conv_ltof(vd);
  }
}  




