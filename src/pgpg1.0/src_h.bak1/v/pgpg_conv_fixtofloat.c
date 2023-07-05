/* change by T.hamada 2003/11/20 */
/* 2003/11/20 : coding start */
/* Under Constructing @hoge */
#include<stdio.h>
#include<stdlib.h> // malloc(),free(),atoi()
#include<string.h> // strcpy(),strcmp(),strcat()
#include<math.h>   // log()
#include "pgpg.h"
#include "pgpg_submodule.h" // i2bit(),generate_pg_penc(),generate_pg_shift_ftol(),generate_pg_table()
#include "pgpg_lcelltable_conv_ftol.h" // create_table_lcell_conv_ftol(),generate_pg_table_lcell_of_conv_ftol()
#define LONG long long int
static struct {
  char func_name[STRLEN];
  char name_shift[STRLEN];
  int  nstage_shift;
  char name_add0[STRLEN];
  int  nstage_add0;
  int is_pipe[100];
  char lcromname[256];
  int rom_ilen;
  int rom_olen;
} this;


void generate_pg_conv_fixtofloat(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int i;
  int nbit_fix,nbit_man,nstage,nbit_penc=0,nbit_entry,nadd;
  int nbit_float;
  int round_mode;
  char sx[STRLEN],sy[STRLEN];
  char sd[STRLEN];
  char sd2[STRLEN];
  int ic,im,ie,is,iflag,j;
  static int tablecnt=0;
  int nbit_log=0; //hoge

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit_fix   = atoi(sdata[3]);
  nbit_float = atoi(sdata[4]);
  nbit_man   = atoi(sdata[5]);
  nstage     = atoi(sdata[6]);
  round_mode = atoi(sdata[7]);

  printf("conv fix to float: %s = %s : ",sx,sy);
  printf("nbit fix %d float %d man %d nstage %d\n",nbit_fix,nbit_float,nbit_man,nstage);
  if(nstage<1){ 
    nstage = 1;
    printf("nstage: fixed to %d\n",nstage);
  }
  if(nstage>16){
    nstage = 16;
    printf("nstage: fixed to %d\n",nstage);
  }

  /*** SET FUNCTION NAME ***/
  sprintf(this.func_name,"pg_conv_fixtofloat_%d_%d_%d_%d_%d",nbit_fix,nbit_float,nbit_man,nstage,round_mode);

  /*** generate component decleration ****/
  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component %s\n",this.func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(fixdata   : in std_logic_vector(%d downto 0);\n",nbit_fix-1);ic++;
    sprintf((*vd).sdc[ic], "         floatdata : out std_logic_vector(%d downto 0);\n",nbit_float-1);ic++;
    sprintf((*vd).sdc[ic], "         clk : in std_logic);\n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;\n" );ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;   
    (*vd).ic = ic;
  }

  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s ",(*vd).ucnt,this.func_name);im++;
  sprintf((*vd).sdm[im], "  port map (fixdata=>%s,floatdata=>%s,clk=>pclk);\n",sx,sy);im++;
  sprintf((*vd).sdm[im], "\n");im++;   
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/
  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit_fix-1);is++;
  (*vd).is = is;


  /*** generate component *****************************************************************************/
  if(iflag != 0) return;

  for(i=0;i<20;i++) this.is_pipe[i]=0;

  //hoge
  for(i=0;i<20;i++) this.is_pipe[i]=1; 
  this.nstage_shift = 0;
  this.nstage_add0  = 0;

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "-- ***************************************************************\n");ie++;
  sprintf((*vd).sde[ie], "-- * PGPG FIXED-POINT TO FLOATING-POINT FORMAT CONVERTER         *\n");ie++;
  sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    *\n");ie++;
  sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Nov 20 22:52:01 JST 2003              *\n");ie++;
  sprintf((*vd).sde[ie], "-- ***************************************************************\n");ie++;
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "entity %s is\n",this.func_name);ie++;
  sprintf((*vd).sde[ie], "  port(fixdata   : in std_logic_vector(%d downto 0);\n",nbit_fix-1);ie++;
  sprintf((*vd).sde[ie], "       floatdata : out std_logic_vector(%d downto 0);\n",nbit_float-1);ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);\n");ie++;
  sprintf((*vd).sde[ie], "end %s;\n",this.func_name);ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "architecture rtl of %s is \n",this.func_name);ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf(this.name_add0,"pg_adder_RCA_ADD_%d_%d",nbit_fix-1,this.nstage_add0);
  sprintf((*vd).sde[ie], "  component %s\n",this.name_add0);ie++;
  sprintf((*vd).sde[ie], "  port (x,y: in std_logic_vector(%d downto 0);\n",nbit_fix-1);ie++;
  sprintf((*vd).sde[ie], "        z: out std_logic_vector(%d downto 0);\n",nbit_fix-1);ie++;
  sprintf((*vd).sde[ie], "        clk: in std_logic);\n");ie++;
  sprintf((*vd).sde[ie], "  end component;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  if(nbit_fix>32) nbit_penc = 6;
  if((nbit_fix<33)&&(nbit_fix>16)) nbit_penc = 5;
  if((nbit_fix<17)&&(nbit_fix>8)) nbit_penc = 4;
  if(nbit_fix<9) nbit_penc = 3;
  if((nbit_penc+nbit_man+1)>nbit_float) printf("nbit_float is too small !!!\n");

  sprintf((*vd).sde[ie], "  component penc_%d_%d\n",nbit_fix-1,nbit_penc );ie++;
  sprintf((*vd).sde[ie], "    port( a : in std_logic_vector(%d downto 0);\n",nbit_fix-2);ie++;
  sprintf((*vd).sde[ie], "          c : out std_logic_vector(%d downto 0));\n",nbit_penc-1);ie++;
  sprintf((*vd).sde[ie], "  end component;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf(this.name_shift,"shift_fixtofloat_%d_%d_%d_%d",nbit_fix-1,nbit_man+2,nbit_penc,this.nstage_shift);
  sprintf((*vd).sde[ie], "  component %s\n",this.name_shift);ie++;
  sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);\n",nbit_fix-3 );ie++;
  sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n",nbit_penc-1);ie++;
  sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0));\n",nbit_man+1);ie++;
  sprintf((*vd).sde[ie], "  end component;\n");ie++;

  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  signal d1,d0: std_logic_vector(%d downto 0);         \n", nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d2: std_logic_vector(%d downto 0);            \n", nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d3,d3r: std_logic_vector(%d downto 0);        \n", nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "  signal one: std_logic_vector(%d downto 0);           \n", nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "  signal sign: std_logic;                                          \n" );ie++;
  sprintf((*vd).sde[ie], "  signal c1: std_logic_vector(%d downto 0);            \n", nbit_penc-1);ie++;
  sprintf((*vd).sde[ie], "  signal d4: std_logic_vector(%d downto 0);            \n", nbit_fix-3 );ie++;
  sprintf((*vd).sde[ie], "  signal c2,c3,c4,add: std_logic_vector(%d downto 0);  \n", nbit_penc-1);ie++;
  sprintf((*vd).sde[ie], "  signal d5,d6: std_logic_vector(%d downto 0);         \n",nbit_man+1);ie++;
  sprintf((*vd).sde[ie], "  signal sign0,sign0r,sign1,sign2,sign3: std_logic;    \n" );ie++;
  sprintf((*vd).sde[ie], "  signal nz0,nz1,nz2: std_logic;                       \n" );ie++;
  sprintf((*vd).sde[ie], "\n"      );ie++;
  sprintf((*vd).sde[ie], "begin\n" );ie++;
  sprintf((*vd).sde[ie], "\n"      );ie++;
  sprintf((*vd).sde[ie], "  d1 <=  NOT fixdata(%d downto 0);\n",nbit_fix-2);ie++;
  strcpy(sd,"");
  for(i=0;i<(nbit_fix-2);i++) strcat(sd,"0");
  strcat(sd,"1");
  sprintf((*vd).sde[ie], "  one <= \"%s\";\n",sd);ie++;
  sprintf((*vd).sde[ie], "  u1: %s port map(z=>d2,x=>d1,y=>one);\n",this.name_add0);ie++;
  sprintf((*vd).sde[ie], "  d0 <= fixdata(%d downto 0);\n"         ,nbit_fix-2    );ie++;
  sprintf((*vd).sde[ie], "  sign0 <= fixdata(%d);\n"               ,nbit_fix-1    );ie++;
  sprintf((*vd).sde[ie], "\n"                       );ie++;
  sprintf((*vd).sde[ie], "  with sign0 select\n"    );ie++;
  sprintf((*vd).sde[ie], "    d3 <= d0 when '0',\n" );ie++;
  sprintf((*vd).sde[ie], "    d2 when others;\n"    );ie++;
  sprintf((*vd).sde[ie], "\n"                       );ie++;

  /* SHIFTER */
  sprintf((*vd).sde[ie], "  u3: %s port map(indata=>d4,control=>c2,outdata=>d5);\n",this.name_shift);ie++;
  sprintf((*vd).sde[ie], "  man0  <= d5(%d downto 2);\n",nbit_man+1);ie++;
  sprintf((*vd).sde[ie], "  sbit0 <= d5(1); -- Stiky Bit\n");ie++;
  sprintf((*vd).sde[ie], "  gbit0 <= d5(0); -- Guard Bit\n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  (*vd).ie = ie;
  //  generate_pg_shift_fixtofloat(vd,nbit_fix-2,nbit_entry,nbit_penc);
  generate_pg_shift_fixtofloat_withSbitGbit(vd,nbit_fix-1,nbit_man+2,5,this.nstage_shift);
  generate_pg_adder_RCA(vd,nbit_fix-1,this.nstage_add0,0);
  return; //debug-break
  /////////////////////////////////////////////////////////// 2003/12/10 hoge


  if((nstage==3)||(nstage==4)){
    sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
    sprintf((*vd).sde[ie], "      d3r <= d3;                                                 \n" );ie++;
    sprintf((*vd).sde[ie], "      sign1 <= sign0;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  d3r <= d3;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  sign1 <= sign0;                                              \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  sprintf((*vd).sde[ie], "  u2: penc_%d_%d port map (a=>d3r,c=>c1);   \n",nbit_fix-1,nbit_penc );ie++;  
  sprintf((*vd).sde[ie], "  with d3r select                                                \n" );ie++;
  strcpy(sd,"");
  for(i=0;i<(nbit_fix-1);i++) strcat(sd,"0");
  sprintf((*vd).sde[ie], "    nz0 <= '0' when \"%s\",                                  \n", sd );ie++;
  sprintf((*vd).sde[ie], "           '1' when others;                                      \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;
 
  if(nstage>1){
    sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
    sprintf((*vd).sde[ie], "      d4 <= d3r(%d downto 0);                        \n", nbit_fix-3 );ie++;
    sprintf((*vd).sde[ie], "      c2 <= c1;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "      sign2 <= sign1;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "      nz1 <= nz0;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  d4 <= d3r(%d downto 0);                        \n", nbit_fix-3 );ie++;
    sprintf((*vd).sde[ie], "  c2 <= c1;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "  sign2 <= sign1;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "  nz1 <= nz0;                                                \n" );ie++;
  }    
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  sprintf((*vd).sde[ie], "  u3: shift_ftol_%d_%d                     \n",nbit_fix-2,nbit_entry );ie++;
  sprintf((*vd).sde[ie], "            port map (indata=>d4,control=>c2,outdata=>d5);       \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  if(nstage==4){
    sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
    sprintf((*vd).sde[ie], "      d6 <= d5;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "      sign3 <= sign2;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "      nz2 <= nz1;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "      c3 <= c2;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  d6 <= d5;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "  sign3 <= sign2;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "  nz2 <= nz1;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "  c3 <= c2;                                                  \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  if((*vd).alteraesbflag){
    sprintf((*vd).sde[ie], "  u4: table generic map");ie++;
    sprintf((*vd).sde[ie], "(IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"ftl%1d.mif\") \n",nbit_entry,nbit_man,tablecnt);ie++;
  }else{
    sprintf((*vd).sde[ie], "  u4: %s\n",this.lcromname);ie++;
  }
  tablecnt++;
  sprintf((*vd).sde[ie], "            port map(indata=>d6,outdata=>logdata(%d downto 0),clk=>clk);\n", nbit_man-1 );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  if(nadd>0){ 
    int itmp,ii;
    sprintf((*vd).sde[ie], "  with d6 select                                                 \n" );ie++;
    strcpy(sd,""); for(i=0;i<nbit_penc-1;i++) strcat(sd,"0"); strcat(sd,"1");
    itmp = (1<<nbit_entry)-1;
    i2bit(itmp,sd2,nbit_entry);
    sprintf((*vd).sde[ie], "    add <= \"%s\" when \"%s\",                          \n", sd, sd2 );ie++;
    for(ii=1;ii<nadd;ii++){
      itmp--; 
      i2bit(itmp,sd2,nbit_entry);	
      sprintf((*vd).sde[ie], "           \"%s\" when \"%s\",                        \n", sd, sd2 );ie++;
    }
    strcpy(sd,""); for(i=0;i<nbit_penc;i++) strcat(sd,"0");
    sprintf((*vd).sde[ie], "           \"%s\" when others;                               \n", sd );ie++;
  }else{
    strcpy(sd,""); for(i=0;i<nbit_penc-1;i++) strcat(sd,"0"); strcat(sd,"0");
    sprintf((*vd).sde[ie], "  add <= \"%s\";                                             \n", sd );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  sprintf((*vd).sde[ie], "  u5: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_penc );ie++;
  sprintf((*vd).sde[ie], "                  port map(result=>c4,dataa=>c3,datab=>add);     \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  strcpy(sd,"");
  for(i=0;i<(nbit_log-nbit_man-nbit_penc-2);i++) strcat(sd,"0");
  sprintf((*vd).sde[ie], "  logdata(%d downto %d) <= \"%s\";                    \n", nbit_log-3,nbit_man+nbit_penc,sd );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;
  sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
  sprintf((*vd).sde[ie], "      logdata(%d) <= sign3 ;                          \n",nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "      logdata(%d) <= nz2;                             \n",nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "      logdata(%d downto %d) <= c4;                    \n",nbit_man+nbit_penc-1,nbit_man);ie++;
  sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
  sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;

  sprintf((*vd).sde[ie], "end rtl;                                                         \n" );ie++;
  (*vd).ie = ie;

  generate_pg_penc(vd,nbit_fix-1,nbit_penc);
  generate_pg_shift_ftol(vd,nbit_fix-2,nbit_entry,nbit_penc);

  if((*vd).alteraesbflag){
    generate_pg_table(vd);
  }else{
    generate_pg_table_lcell_of_conv_ftol(vd);
  }
}  









