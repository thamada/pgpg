#include<stdio.h>
#include<stdlib.h> //exit(),atoi()
#include<math.h>
#include<string.h> // strcpy(),strcmp()
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc()

static struct {
  char lcrom_name_p[256]; 
  int is_pipe[20];
  int npmax;
} this;

/***************************************************
                 TABLE OF PLUS
 **************************************************/
static void create_table_log_add_plus( nbit_man, nbit_tin,tablecnt)
     int nbit_man;
     int *nbit_tin;
     int tablecnt;
{
  int i,iy,nentry;
  double inscale,x;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"lap%1d.mif",tablecnt); /* lap : log adder plus */
  fp = fopen(filename,"w");
  inscale = (double)(1<<nbit_man);
  if(nbit_man > 31){printf("Error: pgpg_log_add.c\n nbit_man is out of range(%d).\n",nbit_man);exit(1);}

  /* CALCURATE TABLE INPUT WIDTH */
  i = 0;
  do{
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));
    i++;
  }while(iy>0);
  i--;

  *nbit_tin = (int) (log((double)i)/log(2.0)+1.0); /* log((double)i)/log(2.0) <==> (priority encoder of i) */
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

/***************************************************
                 TABLE OF MINUS
 **************************************************/
static void create_table_log_add_minus( nbit_man, nbit_tin,nbit_tout,tablecnt)
     int nbit_man;
     int *nbit_tin;
     int *nbit_tout;
     int tablecnt;
{
  int i,iy,nentry;
  double inscale,x;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"lam%1d.mif",tablecnt); /* lam : log adder plus */
  fp = fopen(filename,"w");
  inscale = (double)(1<<nbit_man);
  if(nbit_man > 31){printf("Error: pgpg_log_add.c\n nbit_man is out of range(%d).\n",nbit_man);exit(1);}


  /* CALCURATE TABLE INPUT WIDTH */
  i = 0;
  do{
    x = 1.0 - pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 - inscale*log(x)/log(2.0));
    i++;
  }while(iy>0);
  i--;
  *nbit_tin = (int) (log((double)i)/log(2.0)+1.0); /* log((double)i)/log(2.0) <==> (priority encoder of i) */
  nentry = (int) pow(2.0, (double)(*nbit_tin));

  /* CALCURATE TABLE OUTPUT WIDTH */
  {
    double _x;
    int _iy;
    _x = 1.0 - pow(2.0, - (1.0+0.25)/inscale);
    _iy = (int) (0.5 - inscale*log(_x)/log(2.0));
    *nbit_tout = (int) (log((double)_iy)/log(2.0)+1.0); /* log((double)_iy)/log(2.0) <==> (priority encoder of _iy) */
    if((0x10000000&_iy) != 0 || _iy == 0){
      printf("Error: pgpg_log_add.c\n output width of minus table is out of range(%d).\n",*nbit_tout);
      exit(1);
    }
  }

  fprintf(fp,"DEPTH = %d;\n",nentry);
  fprintf(fp,"WIDTH = %d;\n\n",*nbit_tout);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  /* output of entry==0x0 */
  fprintf(fp,"    0 : 0 ;\n");

  for(i=1;i<nentry;i++){
    x = 1.0 - pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 - inscale*log(x)/log(2.0));
    fprintf(fp,"    %X : %X ;\n",i,iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}




void generate_pg_log_add(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int j;
  int nbit_log,nbit_man,nstage;
  int nbit_tin_plus,nbit_tout_plus;
  int nbit_tin_minus,nbit_tout_minus;
  char sx[STRLEN],sy[STRLEN],sz[STRLEN];
  int ic,im,ie,is;
  int iflag=1;
  int is_esb=0;
  static int tablecnt=0;
  static int tablecnt_minus=0;

  is_esb = (*vd).alteraesbflag;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  strcpy(sz,sdata[3]);
  nbit_log = atoi(sdata[4]);
  nbit_man = atoi(sdata[5]);
  nstage = atoi(sdata[6]);

  printf("log add : %s + %s = %s : ",sx,sy,sz);
  printf("nbit log %d man %d nstage %d\n",nbit_log,nbit_man,nstage);

  // --- PIPELINE MAP -----------------------------------------------
  this.npmax=4;
  for(j=0;j<=this.npmax;j++) this.is_pipe[j]=0;
  for(j=0;j<2;j++){
    if(is_esb==0){  //+++++++++++++++ ESB ++++++++++++++++++
      if(nstage<1){
	nstage=1; printf("nstage: fixed to %d\n",nstage);
      }else if(nstage==1){
	this.is_pipe[4]=1;
      }else if(nstage==2){
	this.is_pipe[1]=1;
	this.is_pipe[4]=1;
      }else if(nstage==3){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[4]=1;
      }else if(nstage==4){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
	this.is_pipe[4]=1;
      }else if(nstage>4){
	nstage=4; printf("nstage: fixed to %d\n",nstage);
      }
    }else{          //+++++++++++++++ LCELL ++++++++++++++++++
      if(nstage<1){
	nstage=1; printf("nstage: fixed to %d\n",nstage);
      }else if(nstage==1){
	this.is_pipe[4]=1;
      }else if(nstage==2){
	this.is_pipe[1]=1;
	this.is_pipe[4]=1;
      }else if(nstage==3){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[4]=1;
      }else if(nstage==4){
	this.is_pipe[1]=1;
	this.is_pipe[2]=1;
	this.is_pipe[3]=1;
	this.is_pipe[4]=1;
      }else if(nstage>4){
	nstage=4; printf("nstage: fixed to %d\n",nstage);
      }
    }
  }
  // ---------------------------------------------------------------

  /* CALC TABLE INPUT/OUTPUT WIDTH AND GENERATE TABLE MIF-FILE */
  create_table_log_add_plus(nbit_man,&nbit_tin_plus,tablecnt);
  create_table_log_add_minus(nbit_man,&nbit_tin_minus,&nbit_tout_minus,tablecnt_minus);
  nbit_tout_plus = nbit_man;

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_log_add_%d_%d                    \n", nbit_log, nbit_man );ic++;
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
  sprintf((*vd).sdm[im], "  u%d: pg_log_add_%d_%d               \n",(*vd).ucnt,nbit_log,nbit_man );im++;
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
  sprintf((*vd).sde[ie], "-- * PGPG SIGNED LOGARITHMIC ADDER MODULE                        * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  VERSION: 1.03                                              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Sat Apr 22 09:00:01 JST 2004              * \n" );ie++;
  sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "entity pg_log_add_%d_%d is                     \n", nbit_log, nbit_man );ie++;
  sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(%d downto 0);        \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0);         \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "        clock : in std_logic);                                     \n" );ie++;
  sprintf((*vd).sde[ie], "end pg_log_add_%d_%d;                          \n", nbit_log, nbit_man );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

  sprintf((*vd).sde[ie], "architecture rtl of pg_log_add_%d_%d is        \n", nbit_log, nbit_man );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;
  sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
  sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(is_esb){ // -------- ESB -------
    if(this.is_pipe[2]){
      sprintf((*vd).sde[ie], "  component lpm_rom                                           \n");ie++;
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                             \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                           \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_ADDRESS_CONTROL: STRING;                     \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_FILE: STRING);                               \n");ie++;
      sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);\n");ie++;
      sprintf((*vd).sde[ie], "         outclock: in std_logic;                              \n");ie++;
      sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n");ie++;
      sprintf((*vd).sde[ie], "  end component;                                              \n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  -- UNREGISTERED TABLE --                                    \n");ie++;
      sprintf((*vd).sde[ie], "  component lpm_rom                                           \n");ie++;
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                             \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                           \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_ADDRESS_CONTROL: STRING;                     \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_OUTDATA: STRING;                             \n");ie++;
      sprintf((*vd).sde[ie], "             LPM_FILE: STRING);                               \n");ie++;
      sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);\n");ie++;
      sprintf((*vd).sde[ie], "  --     outclock: in std_logic;                              \n");ie++;
      sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n");ie++;
      sprintf((*vd).sde[ie], "  end component;                                              \n");ie++;
    }
  }else{ // -------- Lcell ROM ------- //hoge
    sprintf((*vd).sde[ie], "  component %s \n",this.lcrom_name_p);ie++;
    sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(LPM_WIDTHAD-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "         clk: in std_logic;                              \n");ie++;
    sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n");ie++;
    sprintf((*vd).sde[ie], "  end component;                                              \n");ie++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "  signal sign_x,sign_y : std_logic;                    \n" );ie++;
  sprintf((*vd).sde[ie], "  signal nonz_x,nonz_y : std_logic;                    \n" );ie++;
  sprintf((*vd).sde[ie], "  signal x0,y0 : std_logic_vector(%d downto 0);        \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal x1,y1,xy : std_logic_vector(%d downto 0);     \n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "  signal yx : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal sign_xy : std_logic;                          \n" );ie++;
  sprintf((*vd).sde[ie], "  signal x2 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal x3 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal x4 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal x5 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal d1 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
  sprintf((*vd).sde[ie], "  signal df0_plus,df1_plus : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal df0_minus,df1_minus : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal d2r_msb : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal d2_plus : std_logic_vector(%d downto 0);\n", nbit_man );ie++; /* (nbit_man+1)bit! */
  sprintf((*vd).sde[ie], "  signal d2_minus : std_logic_vector(%d downto 0);\n", nbit_tout_minus-1 );ie++;
  sprintf((*vd).sde[ie], "  signal d4_plus : std_logic_vector(%d downto 0); \n", nbit_man );ie++; /* (nbit_man+1)bit */
  sprintf((*vd).sde[ie], "  signal d4_minus,not_d4_minus,one_for_minusadd,add_at_minus : std_logic_vector(%d downto 0);\n",nbit_tout_minus-1 );ie++; /* (nbit_tout)bit */
  sprintf((*vd).sde[ie], "  signal allor_of_add_at_minus : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal d5_plus : std_logic_vector(%d downto 0); \n", nbit_log-2);ie++; /* (nbit_log-1)bit */
  sprintf((*vd).sde[ie], "  signal d5_minus : std_logic_vector(%d downto 0);\n", nbit_log-2);ie++; /* (nbit_log-1)bit */
  sprintf((*vd).sde[ie], "  signal d6,d7 : std_logic_vector(%d downto 0);\n", nbit_log-2);ie++; /* (nbit_log-1)bit */
  sprintf((*vd).sde[ie], "  signal last_add : std_logic_vector(%d downto 0);\n", nbit_log-2);ie++; /* (nbit_log-1)bit */

  sprintf((*vd).sde[ie], "  signal signx_eq_signy0 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal signx_eq_signy1 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal signx_eq_signy2 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal signx_ne_signxy : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal sign_z0 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal sign_z1 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal sign_z2 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal sign_z3 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal allor_of_diff : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal must_not_be_zero0 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal must_not_be_zero1 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal must_not_be_zero2 : std_logic;\n");ie++;

  sprintf((*vd).sde[ie], "  signal z_logpart : std_logic_vector(%d downto 0);\n",nbit_log-3 );ie++; /* (nbit_log-2)bit */
  sprintf((*vd).sde[ie], "  signal z_nonzero : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal z0 : std_logic_vector(%d downto 0);\n",nbit_log-1);ie++;
  sprintf((*vd).sde[ie], "  signal z1 : std_logic_vector(%d downto 0);\n",nbit_log-1);ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "begin\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  sign_x <= x(%d);\n",nbit_log-1);ie++;
  sprintf((*vd).sde[ie], "  sign_y <= y(%d);\n",nbit_log-1);ie++;
  sprintf((*vd).sde[ie], "  nonz_x <= x(%d);\n",nbit_log-2);ie++;
  sprintf((*vd).sde[ie], "  nonz_y <= y(%d);\n",nbit_log-2);ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  /****************************************
   *           R E S E T                  *
   ****************************************/
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_log-1);
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "  -- RESET --\n");ie++;
    sprintf((*vd).sde[ie], "  x0 <= x(%d downto 0) when nonz_x='1' else \"%s\";\n",nbit_log-2,_zero);ie++;
    sprintf((*vd).sde[ie], "  y0 <= y(%d downto 0) when nonz_y='1' else \"%s\";\n",nbit_log-2,_zero);ie++;
  }

  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  x1 <= '0' & x0;\n");ie++;
  sprintf((*vd).sde[ie], "  y1 <= '0' & y0;\n");ie++;

  /****************************************
   *        Asymmetric ADDERs             *
   ****************************************/
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- (Asymmetric) ADDERs --\n");ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n",nbit_log);ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy);\n");ie++;
  sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n",nbit_log-1);ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>y0,datab=>x0,result=>yx);\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "  sign_xy <= xy(%d);                \n",nbit_log-1 );ie++;

  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- MULTIPLEXORT (SELECT LOGPART OF THE LAST ADD OPERAND AND DIFF) --\n");ie++;
  sprintf((*vd).sde[ie], "  x2 <= x0 when sign_xy='0' else y0;\n");ie++;
  sprintf((*vd).sde[ie], "  d0 <= xy(%d downto 0) when sign_xy='0' else yx;\n",nbit_log-2);ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  /****************************************
   *     SIGN/NONZERO EVALUATION          *
   ****************************************/
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- SIGN/NONZERO EVALUATION --\n");ie++;
  sprintf((*vd).sde[ie], "  -- XNOR --\n");ie++;
  sprintf((*vd).sde[ie], "  signx_eq_signy0 <= not (sign_x xor sign_y);\n" );ie++;
  sprintf((*vd).sde[ie], "  -- XOR --\n");ie++;
  sprintf((*vd).sde[ie], "  signx_ne_signxy <= sign_x xor sign_xy;\n" );ie++;
  sprintf((*vd).sde[ie], "  -- MULTIPLEXOR --\n");ie++;
  sprintf((*vd).sde[ie], "  sign_z0 <= signx_ne_signxy when signx_eq_signy0='0' else sign_x;\n");ie++;

  sprintf((*vd).sde[ie], "\n\n");ie++;
  if(this.is_pipe[1]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 1 -------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                         \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then         \n" );ie++;
    sprintf((*vd).sde[ie], "      x3 <= x2;                                \n" );ie++;
    sprintf((*vd).sde[ie], "      d1 <= d0;                                \n" );ie++;
    sprintf((*vd).sde[ie], "      sign_z1 <= sign_z0;                      \n" );ie++;
    sprintf((*vd).sde[ie], "      signx_eq_signy1 <= signx_eq_signy0;      \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 1 (OFF) -------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  x3 <= x2;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  d1 <= d0;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  sign_z1 <= sign_z0;                          \n" );ie++;
    sprintf((*vd).sde[ie], "  signx_eq_signy1 <= signx_eq_signy0;          \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }      
  sprintf((*vd).sde[ie], "  \n");ie++;

  /* ALL OR (PLUS)*/
  sprintf((*vd).sde[ie], "  -- ALL OR (PLUS) --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_log-1-nbit_tin_plus);
    sprintf((*vd).sde[ie], "  df0_plus <= '0' when d1(%d downto %d)=\"%s\" else '1';\n",
	    nbit_log-2, nbit_tin_plus, _zero );ie++;
  }
  /* ALL OR (MINUS)*/
  sprintf((*vd).sde[ie], "  -- ALL OR (MINUS) --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_log-1-nbit_tin_minus);
    sprintf((*vd).sde[ie], "  df0_minus <= '0' when d1(%d downto %d)=\"%s\" else '1';\n",
	    nbit_log-2, nbit_tin_minus, _zero );ie++;
  }

  /* ALL OR -> NOT (PLUS LOWER BITS)*/
  sprintf((*vd).sde[ie], "  \n");ie++;
  sprintf((*vd).sde[ie], "  -- ALL OR -> NOT (PLUS) --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_tin_plus);
    sprintf((*vd).sde[ie], "  d2r_msb <= '1' when d1(%d downto 0)=\"%s\" else '0';\n",nbit_tin_plus-1, _zero );ie++;
  }

  /***********************/
  /* TABLE OBJECT (PLUS) */
  /***********************/
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- TABLE PLUS (INPUT: %d-bit, OUTPUT: %d-bit) --\n",nbit_tin_plus,nbit_man);ie++;
  if(this.is_pipe[2]){
    sprintf((*vd).sde[ie], "  u3: lpm_rom generic map (LPM_WIDTH=>%d,          \n",nbit_man);ie++;
    sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,        \n",nbit_tin_plus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_FILE=>\"lap%1d.mif\")     \n",tablecnt);ie++;
    sprintf((*vd).sde[ie], "  port map(address=>d1(%d downto 0),q=>d2_plus(%d downto 0),outclock=>clock);\n"
	    ,nbit_tin_plus-1,nbit_man-1 );ie++;
    tablecnt++;
  }else{
    sprintf((*vd).sde[ie], "  -- UNREGISTERED TABLE --\n");ie++;
    sprintf((*vd).sde[ie], "  u3: lpm_rom generic map (LPM_WIDTH=>%d,\n",nbit_man);ie++;
    sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,\n",nbit_tin_plus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\",\n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_OUTDATA=>\"UNREGISTERED\",\n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_FILE=>\"lap%1d.mif\")\n",tablecnt);ie++;
    sprintf((*vd).sde[ie], "  port map(address=>d1(%d downto 0),q=>d2_plus(%d downto 0));\n"
	    ,nbit_tin_plus-1,nbit_man-1);ie++;
    tablecnt++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;


  /************************/
  /* TABLE OBJECT (MINUS) */
  /************************/
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- TABLE MINUS (INPUT: %d-bit, OUTPUT: %d-bit) --\n",nbit_tin_minus,nbit_tout_minus);ie++;
  if(this.is_pipe[2]){ 
    sprintf((*vd).sde[ie], "  u4: lpm_rom generic map (LPM_WIDTH=>%d,\n",nbit_tout_minus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,\n",nbit_tin_minus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_FILE=>\"lam%1d.mif\")\n",tablecnt_minus);ie++;
    sprintf((*vd).sde[ie], "    port map(address=>d1(%d downto 0),q=>d2_minus(%d downto 0),outclock=>clock);\n"
	    ,nbit_tin_minus-1,nbit_tout_minus-1 );ie++;
    tablecnt_minus++;
  }else{
    sprintf((*vd).sde[ie], "  -- UNREGISTERED TABLE --\n" );ie++;
    sprintf((*vd).sde[ie], "  u4: lpm_rom generic map (LPM_WIDTH=>%d,\n",nbit_tout_minus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,\n",nbit_tin_minus);ie++;
    sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\",\n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_OUTDATA=>\"UNREGISTERED\",\n");ie++;
    sprintf((*vd).sde[ie], "                           LPM_FILE=>\"lam%1d.mif\")\n",tablecnt_minus);ie++;
    sprintf((*vd).sde[ie], "    port map(address=>d1(%d downto 0),q=>d2_minus(%d downto 0));\n"
	    ,nbit_tin_minus-1,nbit_tout_minus-1 );ie++;
    tablecnt_minus++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;


  /****************************************
   *     SIGN/NONZERO EVALUATION          *
   ****************************************/
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "  -- SIGN/NONZERO EVALUATION --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_log-1);
    sprintf((*vd).sde[ie], "  -- ALLOR OF DIFF --\n");ie++;
    sprintf((*vd).sde[ie], "  allor_of_diff <= '0' when d1=\"%s\" else '1';\n",_zero);ie++;
  }
  sprintf((*vd).sde[ie], "  -- OR --\n");ie++;
  sprintf((*vd).sde[ie], "  must_not_be_zero0 <= allor_of_diff or signx_eq_signy1;\n" );ie++;

  sprintf((*vd).sde[ie], "\n\n");ie++;
  if(this.is_pipe[2]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 2 -------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                         \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then         \n" );ie++;
    sprintf((*vd).sde[ie], "      df1_plus <= df0_plus;                    \n" );ie++;
    sprintf((*vd).sde[ie], "      df1_minus <= df0_minus;                  \n" );ie++;
    sprintf((*vd).sde[ie], "      d2_plus(%d) <= d2r_msb;                  \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "      x4 <= x3;                                \n" );ie++;
    sprintf((*vd).sde[ie], "      sign_z2 <= sign_z1;                      \n" );ie++;
    sprintf((*vd).sde[ie], "      signx_eq_signy2 <= signx_eq_signy1;      \n" );ie++;
    sprintf((*vd).sde[ie], "      must_not_be_zero1 <= must_not_be_zero0;  \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 2 (OFF) -------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  df1_plus <= df0_plus;                        \n" );ie++;
    sprintf((*vd).sde[ie], "  df1_minus <= df0_minus;                      \n" );ie++;
    sprintf((*vd).sde[ie], "  d2_plus(%d) <= d2r_msb;                      \n",nbit_man );ie++;
    sprintf((*vd).sde[ie], "  x4 <= x3;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  sign_z2 <= sign_z1;                          \n" );ie++;
    sprintf((*vd).sde[ie], "  signx_eq_signy2 <= signx_eq_signy1;          \n" );ie++;
    sprintf((*vd).sde[ie], "  must_not_be_zero1 <= must_not_be_zero0;      \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }


  /* MULTIPLEXORT (PLUS) */
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- MULTIPLEXOR (PLUS)  --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_man+1);
    sprintf((*vd).sde[ie], "  d4_plus <= d2_plus when (df1_plus = '0') else \"%s\";\n",_zero);ie++;
  }
  sprintf((*vd).sde[ie], "  -- SET ZEROS AT HIGHER-BIT OF D5_PLUS  --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_log-nbit_man-2);
    sprintf((*vd).sde[ie], "  d5_plus <= \"%s\" & d4_plus;\n",_zero);ie++;
  }

  /* MULTIPLEXORT (MINUS) */
  sprintf((*vd).sde[ie], "  -- MULTIPLEXOR (MINUS) --\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_tout_minus);
    sprintf((*vd).sde[ie], "  d4_minus <= d2_minus when (df1_minus = '0') else \"%s\";\n",_zero);ie++;
  }

  /****************************************
   *      ADDER FOR 2'S COMPLEMENT        *
   ****************************************/
  sprintf((*vd).sde[ie], "\n");ie++;
  {
    char _one[STRLEN];
    itobc(0,_one,nbit_tout_minus-1);
    sprintf((*vd).sde[ie], "  one_for_minusadd <= \"%s1\";\n",_one);ie++;
  }
  sprintf((*vd).sde[ie], "  not_d4_minus <= not d4_minus;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- ADDER FOR 2'S COMPLEMENT --\n");ie++;
  sprintf((*vd).sde[ie], "  u5: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_tout_minus);ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>one_for_minusadd,datab=>not_d4_minus,result=>add_at_minus);\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  {
    char _zero[STRLEN];
    itobc(0,_zero,nbit_tout_minus);
    sprintf((*vd).sde[ie], "  -- ALLOR OF ADD_AT_MINUS --\n");ie++;
    sprintf((*vd).sde[ie], "  allor_of_add_at_minus <= '0' when add_at_minus=\"%s\" else '1';\n",_zero);ie++;
  }
  {
    char _allhigh[STRLEN];
    char _alllow[STRLEN];
    int _ilen;
    _ilen = (int)(pow(2.0,(double)(nbit_log-nbit_tout_minus-1)));
    itobc(_ilen-1,_allhigh,nbit_log-nbit_tout_minus-1);
    itobc(0,_alllow,nbit_log-nbit_tout_minus-1);
    sprintf((*vd).sde[ie], "  -- ALLOR OF ADD_AT_MINUS --\n");ie++;
    sprintf((*vd).sde[ie], "  d5_minus(%d downto %d) <= \"%s\" when allor_of_add_at_minus='1' else \"%s\";\n",
	    nbit_log-2,nbit_tout_minus,_allhigh,_alllow);ie++;
  }
  sprintf((*vd).sde[ie], "  d5_minus(%d downto 0) <= add_at_minus;\n",nbit_tout_minus-1);ie++;

  /* MULTIPLEXORT (MINUS) */
  sprintf((*vd).sde[ie], "\n\n");ie++;
  sprintf((*vd).sde[ie], "  -- MULTIPLEXOR (SELECT PLUS_DIFF OR MINUS_DIFF) --\n");ie++;
  sprintf((*vd).sde[ie], "  d6 <= d5_plus when (signx_eq_signy2 = '1') else d5_minus;\n");ie++;
  sprintf((*vd).sde[ie], "\n\n");ie++;

  if(this.is_pipe[3]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 3 -------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                         \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then         \n" );ie++;
    sprintf((*vd).sde[ie], "      x5 <= x4;                                \n" );ie++;
    sprintf((*vd).sde[ie], "      d7 <= d6;                                \n" );ie++;
    sprintf((*vd).sde[ie], "      must_not_be_zero2 <= must_not_be_zero1;  \n" );ie++;
    sprintf((*vd).sde[ie], "      sign_z3 <= sign_z2;                      \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 3 (OFF) -------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  x5 <= x4;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  d7 <= d6;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  must_not_be_zero2 <= must_not_be_zero1;      \n" );ie++;
    sprintf((*vd).sde[ie], "  sign_z3 <= sign_z2;                          \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }

  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- LAST ADDER --\n");ie++;
  sprintf((*vd).sde[ie], "  u6: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-1 );ie++;
  sprintf((*vd).sde[ie], "       port map(dataa=>x5,datab=>d7,result=>last_add);\n" );ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "  -- NONZERO EVALUATION --\n");ie++;
  sprintf((*vd).sde[ie], "  z_nonzero <= must_not_be_zero2 and last_add(%d);\n",nbit_log-2);ie++;

  sprintf((*vd).sde[ie], "  -- Z_LOGPART --\n");ie++;
  sprintf((*vd).sde[ie], "  z_logpart <= last_add(%d downto 0);\n",nbit_log-3);ie++; 

  sprintf((*vd).sde[ie], "  -- JOINT Z --\n");ie++;
  sprintf((*vd).sde[ie], "  z0 <= sign_z3 & z_nonzero & z_logpart;\n");ie++; 

  sprintf((*vd).sde[ie], "\n\n");ie++;
  if(this.is_pipe[4]){
    sprintf((*vd).sde[ie], "  -- PIPELINE 4 -------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  process(clock) begin                         \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then         \n" );ie++;
    sprintf((*vd).sde[ie], "      z <= z0;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  -- PIPELINE 4 (OFF) -------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "  z <= z0;                                     \n" );ie++;
    sprintf((*vd).sde[ie], "  --------------------------------------------.\n" );ie++;
  }

  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- END --\n");ie++;
  sprintf((*vd).sde[ie], "end rtl;\n");ie++;
  (*vd).ie = ie;
}


