#include<stdio.h>
#include<math.h>
#include<string.h> //strcpy()
#include<stdlib.h> //atoi()
#include "pgpg.h"
#include "pgpg_submodule.h"
#include "pgpg_lcelltable_log_unsigned_add.h"

#define LONG long long int
static struct {
  int nlog;
  int nman;
  int ncut;
  int npipe;
  int bit_exp;
  char lcromname[256];
  int rom_nstage;
  int is_pipe[20];
  int npmax;
} this;

// included from pgpg.c
void itobc(int idata,char* sdata,int nbit);


static void create_table_log_unsigned_add( nbit_man, nbit_tin,tablecnt)
     int nbit_man;
     int *nbit_tin;
     int tablecnt;
{
  int i,iy,nentry;
  double inscale,x;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"add%1d.mif",tablecnt);
  fp = fopen(filename,"w");
  inscale = (double)(1<<nbit_man);

  i = 0;
  do{
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    i++; 
  }while(iy>0);
  i--;          /* cut a address at iy=0 */

  *nbit_tin = (int) (log((double)i)/log(2.0)+1.0);
  nentry = (int) pow(2.0, (double)(*nbit_tin));

  fprintf(fp,"DEPTH = %d;\n",nentry);
  fprintf(fp,"WIDTH = %d;\n\n",nbit_man);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  /* output of entry==0x0, because WIDTH is nbit_man (not nbit_man+1). */
  fprintf(fp,"    0 : 0 ;\n");

  for(i=1;i<nentry;i++){
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    fprintf(fp,"    %X : %X ;\n",i,iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}


void generate_pg_log_unsigned_add(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int j;
  int nbit_log,nbit_man,nstage,nbit_tin;
  char sx[HSTRLEN],sy[HSTRLEN],sz[HSTRLEN];
  int ic,im,ie,is;
  int iflag=1;
  int is_esb;
  static int tablecnt=0;
  char func_name[HSTRLEN];

  is_esb = (*vd).alteraesbflag;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  strcpy(sz,sdata[3]);
  nbit_log = atoi(sdata[4]);
  nbit_man = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  printf("log unsigned add(ver2) : %s + %s = %s : ",sx,sy,sz);
  printf("nbit log %d man %d nstage %d\n",nbit_log,nbit_man,nstage);

  // --- PIPELINE MAP -----------------------------------------------
  this.npmax=4;
  for(j=0;j<=this.npmax;j++) this.is_pipe[j]=0;
  for(j=0;j<2;j++){
    if(is_esb==0){  //+++++++++++++++ LCELL ++++++++++++++++++
      if(nstage<1){
	nstage=1; printf("nstage: fixed to %d\n",nstage);
      }else if(nstage==1){
	// null
      }else if(nstage==2){
	this.is_pipe[2]=1;
      }else if(nstage==3){
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
      }else if(nstage==4){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
      }else if(nstage==5){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
	this.is_pipe[4]=1;
      }else if(nstage>5){
	nstage=5; printf("nstage: fixed to %d\n",nstage);
      }
    }else{          //+++++++++++++++ ESB ++++++++++++++++++
      if(nstage<1){
	nstage=1; printf("nstage: fixed to %d\n",nstage);
      }else if(nstage==1){
	// null
      }else if(nstage==2){
	this.is_pipe[2]=1;
      }else if(nstage==3){
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
      }else if(nstage==4){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
      }else if(nstage>4){
	nstage=4; printf("nstage: fixed to %d\n",nstage);
      }
    }
  }
  // ---------------------------------------------------------------


  // --- ROM GENERATION ---
  if((*vd).alteraesbflag){
    create_table_log_unsigned_add(nbit_man,&nbit_tin,tablecnt);
  }else{
    int np=0;
    char name[256];

    // change : 2004/04/23
    //    if(this.is_pipe[3]) np=1; else np=0;
    if(this.is_pipe[3]) np++;
    if(this.is_pipe[4]) np++;
    create_table_lcell_log_unsigned_add(nbit_man,np,&nbit_tin,name);
    strcpy(this.lcromname,name);
    this.rom_nstage = np;
  }

  ic = (*vd).ic;
  sprintf(func_name,"pg_log_unsigned_add_%d_%d_%d",nbit_log,nbit_man,nstage);
  sprintf((*vd).sdc[ic], "  component %s\n", func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(%d downto 0);      \n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0);       \n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          clock : in std_logic);                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: %s\n",(*vd).ucnt,func_name);im++;
  sprintf((*vd).sdm[im], "            port map(x=>%s,y=>%s,z=>%s,clock=>pclk);       \n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "                                                                   \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);           \n",sx,nbit_log-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);           \n",sy,nbit_log-1);is++;
  (*vd).is = is;

  /*** generate component *****************************************************************************/
  if(iflag != 0) return;

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
  sprintf((*vd).sde[ie], "-- * PGPG UNSIGNED LOGARITHMIC ADDER MODULE                      * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  VERSION: 3.00                                              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Jun 03 10:52:01 JST 2003              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;\n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "entity %s is\n",func_name);ie++;
  sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(%d downto 0);\n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0);\n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "        clock : in std_logic);\n");ie++;
  sprintf((*vd).sde[ie], "end %s;\n",func_name);ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  sprintf((*vd).sde[ie], "architecture rtl of %s is\n",func_name);ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;
  sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
  sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  if((*vd).alteraesbflag){
    // ESB ROM
    if(this.is_pipe[3]){
      sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_table                                            \n" );ie++;
    }
    sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer ;                                   \n" );ie++;
    sprintf((*vd).sde[ie], "	     OUT_WIDTH: integer ;                                      \n" );ie++;
    sprintf((*vd).sde[ie], "	     TABLE_FILE: string);                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);       \n" );ie++;
    sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  }else{
    // Lcell ROM
    sprintf((*vd).sde[ie], "  component %s                      \n",this.lcromname);ie++;
    sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",nbit_tin-1);ie++;
    sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
    sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",nbit_man-1);ie++;
    sprintf((*vd).sde[ie], "  end component;                                    \n\n" );ie++;
  }
    
  sprintf((*vd).sde[ie], "  signal x1,y1,xy,yx : std_logic_vector(%d downto 0);  \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "  signal xy0,yx0 : std_logic_vector(%d downto 0);  \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "  signal x0,y0 : std_logic_vector(%d downto 0);  \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "  signal x2,x3,x3r,x4 : std_logic_vector(%d downto 0);     \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d0,d1,d4 : std_logic_vector(%d downto 0);     \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal df0,df0r,df1 : std_logic;                                      \n" );ie++;
  sprintf((*vd).sde[ie], "  signal z0 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d2r_msb,d2r_msbr : std_logic;                         \n" );ie++;
  sprintf((*vd).sde[ie], "  signal d2 : std_logic_vector(%d downto 0);          \n", nbit_man );ie++;

  sprintf((*vd).sde[ie], "  signal sign0,sign1,sign1r,sign2 : std_logic;                            \n" );ie++;
  sprintf((*vd).sde[ie], "  signal signxy : std_logic_vector(1 downto 0);                    \n" );ie++;        
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  x1 <= '0' & x(%d downto 0);                          \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  y1 <= '0' & y(%d downto 0);                          \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log );ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy);                   \n" );ie++;
  sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log );ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>y1,datab=>x1,result=>yx);                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  if(this.is_pipe[1]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 1 -----------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
    sprintf((*vd).sde[ie], "      xy0 <= xy;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "      yx0 <= yx;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "      x0 <= x;                                                     \n" );ie++;     
    sprintf((*vd).sde[ie], "      y0 <= y;                                                     \n" );ie++;     
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 1 (OFF) -----------------\n" );ie++;
    sprintf((*vd).sde[ie], "  xy0 <= xy;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  yx0 <= yx;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  x0 <= x;                                                     \n" );ie++;     
    sprintf((*vd).sde[ie], "  y0 <= y;                                                     \n" );ie++;     
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }      
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  sprintf((*vd).sde[ie], "  x2 <= x0(%d downto 0) when yx0(%d)='1' else y0(%d downto 0);\n",nbit_log-2,nbit_log-1,nbit_log-2);ie++;
  sprintf((*vd).sde[ie], "  d0 <= xy0(%d downto 0) when yx0(%d)='1' else yx0(%d downto 0);\n",
	  nbit_log-2, nbit_log-1, nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  signxy <= x0(%d)&y0(%d);                    \n", nbit_log-1,nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "  with signxy select                                               \n" );ie++;    
  sprintf((*vd).sde[ie], "    sign0 <= y0(%d) when \"01\",                        \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "             x0(%d) when others;                        \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  if(this.is_pipe[2]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 2 -----------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
    sprintf((*vd).sde[ie], "      x3 <= x2;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "      d1 <= d0;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "      sign1 <= sign0;                                              \n" );ie++;     
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 2 (OFF) -----------------\n" );ie++;
    sprintf((*vd).sde[ie], "  x3 <= x2;                                                       \n" );ie++;
    sprintf((*vd).sde[ie], "  d1 <= d0;                                                       \n" );ie++;
    sprintf((*vd).sde[ie], "  sign1 <= sign0;                                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }      
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  {
    char sbit[STRLEN];
    itobc(0,sbit,nbit_log-1-nbit_tin);
    sprintf((*vd).sde[ie], "  df0 <= '1' when d1(%d downto %d)=\"%s\" else '0';\n", nbit_log-2, nbit_tin, sbit );ie++;
  }

  /* ALL OR -> NOT (PLUS LOWER BITS) */
  sprintf((*vd).sde[ie], "  \n");ie++;
  sprintf((*vd).sde[ie], "  -- ALL OR -> NOT (PLUS) --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_tin);
    sprintf((*vd).sde[ie], "  d2r_msb <= '1' when d1(%d downto 0)=\"%s\" else '0';\n",nbit_tin-1, _zero );ie++;
  }

  /***********************/
  /* TABLE OBJECT        */
  /***********************/
  sprintf((*vd).sde[ie], "\n");ie++;
  if((*vd).alteraesbflag){
    if(this.is_pipe[3]){ 
      sprintf((*vd).sde[ie],"  u3: table generic map");ie++;
      sprintf((*vd).sde[ie],"(IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"add%1d.mif\")\n",nbit_tin,nbit_man,tablecnt);ie++;
    }else{
      sprintf((*vd).sde[ie],"  u3: unreg_table generic map");ie++;
      sprintf((*vd).sde[ie],"(IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"add%1d.mif\")\n",nbit_tin,nbit_man,tablecnt);ie++;
    }
    tablecnt++;
  }else{
    sprintf((*vd).sde[ie],"  u3: %s\n",this.lcromname);ie++;
  }
  sprintf((*vd).sde[ie], "            port map(indata=>d1(%d downto 0),outdata=>d2(%d downto 0),clk=>clock);\n",nbit_tin-1, nbit_man-1 );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;


  if(this.is_pipe[3]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 3 -----------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
    sprintf((*vd).sde[ie], "      df0r <= df0;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "      x3r <= x3;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "      d2r_msbr <= d2r_msb;                                           \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "      sign1r <= sign1;                                              \n" );ie++;      
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 3 (OFF) -----------------\n" );ie++;
    sprintf((*vd).sde[ie], "  df0r <= df0;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  x3r <= x3;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  d2r_msbr <= d2r_msb;                                             \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "  sign1r <= sign1;                                                \n" );ie++;      
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(this.is_pipe[4]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 4 -----------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                                           \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                           \n" );ie++;
    sprintf((*vd).sde[ie], "      df1 <= df0r;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "      x4 <= x3r;                                                 \n" );ie++;
    sprintf((*vd).sde[ie], "      d2(%d) <= d2r_msbr;                                        \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "      sign2 <= sign1r;                                           \n" );ie++;      
    sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 4 (OFF) -----------------\n" );ie++;
    sprintf((*vd).sde[ie], "  df1 <= df0r;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  x4 <= x3r;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "  d2(%d) <= d2r_msbr;                                            \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "  sign2 <= sign1r;                                               \n" );ie++;      
    sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  {
    char sbit[STRLEN];
    itobc(0,sbit,nbit_man+1);
    sprintf((*vd).sde[ie], "  d4(%d downto 0) <= d2 when (df1 = '1') else \"%s\";\n", nbit_man, sbit );ie++;
    itobc(0,sbit,nbit_log-nbit_man-2);
    sprintf((*vd).sde[ie], "  d4(%d downto %d) <= \"%s\";           \n", nbit_log-2, nbit_man+1, sbit );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  sprintf((*vd).sde[ie], "  u4: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>x4,datab=>d4,result=>z0);                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  sprintf((*vd).sde[ie], "  -- PIPELINE 4 -----------------------\n");ie++;
  sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
  sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
  sprintf((*vd).sde[ie], "      z(%d downto 0) <= z0;                            \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "      z(%d) <= sign2;                                  \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
  sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
  sprintf((*vd).sde[ie], "  ------------------------------------.\n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
  sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;   
  (*vd).ie = ie;

  // -- GENERATE LCELL ROM 
  if((*vd).alteraesbflag){
    if(this.is_pipe[3]) generate_pg_table(vd); else generate_pg_unreg_table(vd);
  }else{
    generate_pg_table_lcell_of_log_unsigned_add(vd);
  }
  // -- END GENERATE LCELL ROM


}  




