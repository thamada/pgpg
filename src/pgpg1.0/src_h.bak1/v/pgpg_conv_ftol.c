/* change by T.hamada 2003/09/22 */
/* 2003/09/22 : separate lcell table generation */
/* 2003/03/31 : generate_pg_penc is moved to pgpg.c */
#include<stdio.h>
#include<stdlib.h> // malloc(),free(),atoi()
#include<string.h> // strcpy(),strcmp(),strcat()
#include<math.h>   // log()
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc(),generate_pg_penc(),generate_pg_shift_ftol(),generate_pg_table()
#include "pgpg_lcelltable_conv_ftol.h" // create_table_lcell_conv_ftol(),generate_pg_table_lcell_of_conv_ftol()
#define LONG long long int
static struct {
  char lcromname[256];
  LONG* rom;
  int rom_ilen;
  int rom_olen;
  int rom_nstage;
} this;



static void create_table_conv_ftol(nbit_in,nbit_out,nadd,tablecnt)
int nbit_in,nbit_out,*nadd,tablecnt;
{
  int i,iy,nentry,outmask;
  double inscale,outscale;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"ftl%1d.mif",tablecnt);
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

  *nadd = 0;
  for(i=0;i<nentry;i++){
    iy = outscale*log(1.0+(((double)i)+0.5)/inscale)/log(2.0)+0.5;
    if(iy==((int)outscale)) (*nadd)++;
    outmask = ((int)outscale)-1;
    fprintf(fp,"    %X : %X ;\n",i,outmask&iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}


void generate_pg_conv_ftol(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int i;
  int nbit_fix,nbit_log,nbit_man,nstage,nbit_penc=0,nbit_entry,nadd;
  char sx[HSTRLEN],sy[HSTRLEN];
  char func_name[HSTRLEN];
  char sd[HSTRLEN];
  char sd2[HSTRLEN];
  int ic,im,ie,is,iflag,j;
  static int tablecnt=0;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit_fix = atoi(sdata[3]);
  nbit_log = atoi(sdata[4]);
  nbit_man = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  printf("conv ftol: %s = %s : ",sx,sy);
  printf("nbit fix %d log %d man %d nstage %d\n",nbit_fix,nbit_log,nbit_man,nstage);
  if(nstage<1){ 
    nstage = 1;
    printf("nstage: fixed to %d\n",nstage);
  }
  if(nstage>4){ 
    nstage = 4;
    printf("nstage: fixed to %d\n",nstage);
  }
       
  /*** generate component decleration ****/

  ic = (*vd).ic;
  sprintf(func_name,"pg_conv_ftol_%d_%d_%d_%d",nbit_fix,nbit_log,nbit_man,nstage);
  sprintf((*vd).sdc[ic], "  component %s\n",func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(fixdata : in std_logic_vector(%d downto 0);\n",nbit_fix-1);ic++;
    sprintf((*vd).sdc[ic], "         logdata : out std_logic_vector(%d downto 0);\n",nbit_log-1);ic++;
    sprintf((*vd).sdc[ic], "         clk : in std_logic);\n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;\n" );ic++;
    sprintf((*vd).sdc[ic], "\n" );ic++;   
    (*vd).ic = ic;
  }

  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s port map (fixdata=>%s,logdata=>%s,clk=>pclk);\n",(*vd).ucnt,func_name,sx,sy);im++;
  sprintf((*vd).sdm[im], "\n" );im++;   
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit_fix-1);is++;
  (*vd).is = is;

  /*** generate component *****************************************************************************/
  if(iflag != 0) return;

  // --- ROM GENERATION ---
  nbit_entry = nbit_man + 2;
  if((*vd).alteraesbflag){
    create_table_conv_ftol(nbit_entry,nbit_man,&nadd,tablecnt);
  }else{
    char name[256];
    this.rom_nstage = 1; // currently fixed
    create_table_lcell_conv_ftol(nbit_fix,nbit_man,this.rom_nstage,&nadd,name);
    strcpy(this.lcromname,name);
    this.rom_ilen = nbit_man+2;
    this.rom_olen = nbit_man;
  }

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
  sprintf((*vd).sde[ie], "-- * PGPG FIXED-POINT TO LOGARITHMIC FORMAT CONVERTER            * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  VERSION: 2.00                                              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Jun 03 22:52:01 JST 2003              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;   
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "entity %s is         \n",func_name);ie++;   
  sprintf((*vd).sde[ie], "  port(fixdata : in std_logic_vector(%d downto 0);      \n", nbit_fix-1);ie++;     
  sprintf((*vd).sde[ie], "       logdata : out std_logic_vector(%d downto 0);     \n", nbit_log-1);ie++;     
  sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;     
  sprintf((*vd).sde[ie], "end %s;              \n",func_name);ie++;     
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "architecture rtl of %s is \n", func_name);ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   

  if((*vd).alteraesbflag){
    // ESB ROM
    sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;     
    sprintf((*vd).sde[ie], "  generic (IN_WIDTH: integer ;                                     \n" );ie++;     
    sprintf((*vd).sde[ie], "           OUT_WIDTH: integer ;                                    \n" );ie++;     
    sprintf((*vd).sde[ie], "           TABLE_FILE: string);                                    \n" );ie++;     
    sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;     
    sprintf((*vd).sde[ie], "      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        \n" );ie++;     
    sprintf((*vd).sde[ie], "      clk : in std_logic);                                         \n" );ie++;     
    sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
  }else{
    // Lcell ROM
    sprintf((*vd).sde[ie], "  component %s                      \n",this.lcromname);ie++;
    sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",this.rom_ilen-1);ie++;
    sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
    sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",this.rom_olen-1);ie++;
    sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
  }

  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;   
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;   
  sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;   
  sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;   
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   

  if(nbit_fix>32) nbit_penc = 6;
  if((nbit_fix<33)&&(nbit_fix>16)) nbit_penc = 5;
  if((nbit_fix<17)&&(nbit_fix>8)) nbit_penc = 4;
  if(nbit_fix<9) nbit_penc = 3;
  if((nbit_penc+nbit_man+2)>nbit_log) printf("nbit_log is too small !!!\n");

  sprintf((*vd).sde[ie], "  component penc_%d_%d                        \n",nbit_fix-1,nbit_penc );ie++;   
  sprintf((*vd).sde[ie], "    port( a : in std_logic_vector(%d downto 0);        \n", nbit_fix-2 );ie++;   
  sprintf((*vd).sde[ie], "          c : out std_logic_vector(%d downto 0));      \n", nbit_penc-1);ie++;   
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "  component shift_ftol_%d_%d                 \n",nbit_fix-2,nbit_entry );ie++;   
  sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);   \n", nbit_fix-3 );ie++;   
  sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);  \n", nbit_penc-1);ie++;   
  sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0));\n",nbit_entry-1);ie++;   
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "  signal d1,d0: std_logic_vector(%d downto 0);         \n", nbit_fix-2 );ie++;   
  sprintf((*vd).sde[ie], "  signal d2: std_logic_vector(%d downto 0);            \n", nbit_fix-2 );ie++;   
  sprintf((*vd).sde[ie], "  signal d3,d3r: std_logic_vector(%d downto 0);        \n", nbit_fix-2 );ie++;   
  sprintf((*vd).sde[ie], "  signal one: std_logic_vector(%d downto 0);           \n", nbit_fix-2 );ie++;   
  sprintf((*vd).sde[ie], "  signal sign: std_logic;                                          \n" );ie++;   
  sprintf((*vd).sde[ie], "  signal c1: std_logic_vector(%d downto 0);            \n", nbit_penc-1);ie++;   
  sprintf((*vd).sde[ie], "  signal d4: std_logic_vector(%d downto 0);            \n", nbit_fix-3 );ie++;   
  sprintf((*vd).sde[ie], "  signal c2,c3,c4,add: std_logic_vector(%d downto 0);  \n", nbit_penc-1);ie++;   
  sprintf((*vd).sde[ie], "  signal d5,d6: std_logic_vector(%d downto 0);         \n",nbit_entry-1);ie++;   
  sprintf((*vd).sde[ie], "  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                \n" );ie++;   
  sprintf((*vd).sde[ie], "  signal nz0,nz1,nz2: std_logic;                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "  d1 <=  NOT fixdata(%d downto 0);                     \n", nbit_fix-2 );ie++;   
  strcpy(sd,"");
  for(i=0;i<(nbit_fix-2);i++) strcat(sd,"0");
  strcat(sd,"1");
  sprintf((*vd).sde[ie], "  one <= \"%s\";                                              \n", sd);ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_fix-1);ie++;
  sprintf((*vd).sde[ie], "                  port map(result=>d2,dataa=>d1,datab=>one);     \n" );ie++;
  sprintf((*vd).sde[ie], "  d0 <= fixdata(%d downto 0);                        \n", nbit_fix-2 );ie++;
  sprintf((*vd).sde[ie], "  sign0 <= fixdata(%d);                              \n", nbit_fix-1 );ie++;
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
  sprintf((*vd).sde[ie], "  with sign0 select                                              \n" );ie++;   
  sprintf((*vd).sde[ie], "    d3 <= d0 when '0',                                           \n" );ie++;   
  sprintf((*vd).sde[ie], "    d2 when others;                                              \n" );ie++;   
  sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   

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
    itobc(itmp,sd2,nbit_entry);
    sprintf((*vd).sde[ie], "    add <= \"%s\" when \"%s\",                          \n", sd, sd2 );ie++;   
    for(ii=1;ii<nadd;ii++){
      itmp--; 
      itobc(itmp,sd2,nbit_entry);	
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









