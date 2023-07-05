/* 
   Author: Tsuyoshi Hamada
   Version 2.02 : release

   Jan 30, 2004 : bugfix on itp_addy, printf -> sprintf 
   Dec 15, 2003 : add generate_fixed_pipelinemap(int nstage)
   Apr 30, 2003 : first revision
*/ 
#include<stdio.h>
#include<string.h> // strcmp(),strcpy(),strcat(),
#include<math.h>
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc(),
          // generate_pg_table(),generate_pg_unreg_table(),
          // generate_pg_reged_mult(),generate_pg_unreg_mult()
          // generate_reged_add_sub_cout(),generate_unreg_add_sub_cout()
          // generate_reged_add_sub(),generate_unreg_add_sub()
          // generate_pg_reged_shift_ltof(),generate_pg_unreg_shift_ltof()
#include "pgpg_lcelltable_core.h" // generate_pg_lcell_rom()
#define LONG long long int
#define REAL long double
#define __PIPELINEMAP_MANUAL__ 1

static struct {
  int nfix;
  int nlog;
  int nman;
  int ncut;
  int npipe;
  int nexp;   // exponent bit width : (int)(log(nfix+nman)/log(2.0) + 1.0)
  int bit_ext;
  int ntin;
  int npenc;
  char filename_c0[100];
  char filename_c1[100];
  char lcromname_c0[256];
  char lcromname_c1[256];
  LONG* rom_c0;
  LONG* rom_c1;
  REAL* rom_real_c0;
  REAL* rom_real_c1;
  LONG rom_depth;
  LONG rom_c0_ilen;
  LONG rom_c1_ilen;
  LONG rom_c0_olen;
  LONG rom_c1_olen;
  LONG rom_c0_size;
  LONG rom_c1_size;
  LONG rom_total_size;
  int is_pipe[20];
  int npmax;
  double pipe_delay[20]; // nano second
  double reg_reg_delay;  // nano second
} this;

#include "generate_pipelinemap.h"

// This subroutine must be specified by each module,
//  and set (this.pipe_delay)[] and this.reg_reg_delay.
static void generate_critical_path_model(nfix,nlog,nman,ncut,npipe,device)
     int nfix;
     int nlog;
     int nman;
     int ncut;
     int npipe;
     char device[200];
{
  int i,pmax;
  pmax = this.npmax;
  for(i=0;i<=pmax;i++) (this.pipe_delay)[i] = 0.0;
  if(strcmp(device,"EP20K400EFC672-2X") == 0){  //--- DEVICE EP20K400EFC672-2X
    this.reg_reg_delay = 4.280;  // (ns)
    if((nfix==57)&&(nlog==17)&&(nman==8)&&(ncut==4)){
      (this.pipe_delay)[1] = 2.6;   // (ns) ireg to 1-th reg
      (this.pipe_delay)[2] = 2.0;   // (ns) 1-th reg to 2-th reg
      (this.pipe_delay)[3] = 2.0;   // (ns) 2-th reg to 3-th reg
      (this.pipe_delay)[4] = 2.0;   // (ns) 3-th reg to 4-th reg
      (this.pipe_delay)[5] = 2.0;    // (ns) 4-th reg to 5-th reg
      (this.pipe_delay)[6] = 0.00001;   // (ns) 5-th reg to 6-th reg
      (this.pipe_delay)[7] = 2.5;   // (ns) 6-th reg to 7-th reg
      (this.pipe_delay)[8] = 2.5;   // (ns) 7-th reg to 8-th reg
      (this.pipe_delay)[9] = 2.0;   // (ns) 8-th reg to 9-th reg
      (this.pipe_delay)[10] = 2.0;  // (ns) 9-th reg to 10-th reg
      (this.pipe_delay)[11] = 0.00001;  // (ns) 10-th reg to 11-th reg
      (this.pipe_delay)[12] = 0.00001;    // (ns) 11-th reg to 12-th reg
      (this.pipe_delay)[13] = 1.1;  // (ns) 12-th reg to 13-th reg
      (this.pipe_delay)[14] = 1.1;  // (ns) 13-th reg to 14-th reg
      (this.pipe_delay)[15] = 1.1;  // (ns) 14-th reg to 15-th reg
      (this.pipe_delay)[16] = 1.1;  // (ns) 15-th reg to 16-th reg
    }else{
      //////////////////////////////////////////////////////////////
      // Not Release ///////////////////////////////////////////////
      //////////////////////////////////////////////////////////////
      (this.pipe_delay)[1] = 2.6;   // (ns) ireg to 1-th reg
      (this.pipe_delay)[2] = 2.0;   // (ns) 1-th reg to 2-th reg
      (this.pipe_delay)[3] = 2.0;   // (ns) 2-th reg to 3-th reg
      (this.pipe_delay)[4] = 2.0;   // (ns) 3-th reg to 4-th reg
      (this.pipe_delay)[5] = 2.0;    // (ns) 4-th reg to 5-th reg
      (this.pipe_delay)[6] = 0.00001;   // (ns) 5-th reg to 6-th reg
      (this.pipe_delay)[7] = 2.5;   // (ns) 6-th reg to 7-th reg
      (this.pipe_delay)[8] = 2.5;   // (ns) 7-th reg to 8-th reg
      (this.pipe_delay)[9] = 2.0;   // (ns) 8-th reg to 9-th reg
      (this.pipe_delay)[10] = 2.0;  // (ns) 9-th reg to 10-th reg
      (this.pipe_delay)[11] = 0.00001;  // (ns) 10-th reg to 11-th reg
      (this.pipe_delay)[12] = 0.00001;    // (ns) 11-th reg to 12-th reg
      (this.pipe_delay)[13] = 1.1;  // (ns) 12-th reg to 13-th reg
      (this.pipe_delay)[14] = 1.1;  // (ns) 13-th reg to 14-th reg
      (this.pipe_delay)[15] = 1.1;  // (ns) 14-th reg to 15-th reg
      (this.pipe_delay)[16] = 1.1;  // (ns) 15-th reg to 16-th reg
    }
  }else{
    fprintf(stderr,"pg_conv_ftol_itp is only support EP20K400EFC672-2X\n");exit(1);
  }
}

/******* functions for interporated table generation ***********/
static REAL func_convltof(REAL x)
{
  REAL f;
  f = powl(2.0,x)-1.0;
  return f;
}

static void do_malloc_for_rom()
{
  LONG imax;
  imax = (LONG)pow(2.0,(double)(this.ntin - this.ncut));
  this.rom_c0 = (LONG*)malloc(sizeof(LONG) * imax);      /* static menber */
  this.rom_c1 = (LONG*)malloc(sizeof(LONG) * imax);      /* static menber */
  this.rom_real_c0 = (REAL*)malloc(sizeof(REAL) * imax); /* static menber */
  this.rom_real_c1 = (REAL*)malloc(sizeof(REAL) * imax); /* static menber */
  this.rom_depth = imax;                                 /* static menber */
  return ;
}

static void free_rom_memory()
{
  free(this.rom_c0);
  free(this.rom_c1);
  free(this.rom_real_c0);
  free(this.rom_real_c1);
}

static void calc_coefficient_chebyshev(xk_min,xk_max,coef0,coef1)
     REAL xk_min;
     REAL xk_max;
     REAL* coef0;
     REAL* coef1;
{
  REAL x0,x1,bma,bpa,cc0,cc1;
  REAL f[2];
  REAL (*_func)(REAL);
  _func = &func_convltof;
  x0 = -0.5*sqrtl(2.0);
  x1 =  0.5*sqrtl(2.0);
  bma=0.5*(xk_max-xk_min);
  bpa=0.5*(xk_max+xk_min);
  f[0] = (*_func)(x0*bma+bpa);
  f[1] = (*_func)(x1*bma+bpa);
  cc0 = 0.5*(f[0]+f[1]);
  cc1 = 0.5*sqrtl(2.0)*(f[1]-f[0]);
  (*coef0) = cc0 - cc1;
  (*coef1) = cc1/bma;
}

static void gen_rom(_ntin,_nman,_ncut,_bit_ext)
     LONG _ntin;
     LONG _nman;
     LONG _ncut;
     LONG _bit_ext;
{
  LONG _bit_ext_c0,_bit_ext_c1,i,imax;
  _bit_ext_c0 = _bit_ext;
  _bit_ext_c1 = _bit_ext;

  imax = this.rom_depth; /* using static member */

  for(i=0;i<imax;i++){
    REAL xmin = (REAL)(i<<_ncut);
    REAL xmax = (REAL)(((i+0x1LL)<<_ncut)-0x1LL);

    xmin = (xmin+0.00) / (REAL)(0x1LL<<_ntin);
    xmax = (xmax+0.00) / (REAL)(0x1LL<<_ntin);
    {
      REAL real_c0,real_c1;
      calc_coefficient_chebyshev(xmin,xmax,&real_c0,&real_c1);
      (this.rom_real_c0)[i] = real_c0;                                   /* static member */ 
      (this.rom_real_c1)[i] = real_c1;                                   /* static member */ 
      (this.rom_c0)[i] = (LONG)(real_c0*powl(2.0,_nman+_bit_ext_c0)+0.5); /* static member */ 
      (this.rom_c1)[i] = (LONG)(real_c1*powl(2.0,_nman+_bit_ext_c1)+0.5); /* static member */ 
    }
  }
  return;
}


static LONG calc_bitlength(input)
     LONG input;
{
  LONG i,out;
  out=0LL;
  if(input <0){input=0;}
  for(i=61LL;i >=0LL;i--){
    LONG buf;
    buf = 0x1LL & (input >>i);
    if(buf == 0x1LL){ out = (i+0x1LL); break;}
    out = (i+0x1LL);
  }
  if(out == 0LL){fprintf(stderr,"error at calc_bitlength.\n");exit(0);}
  return out;
}


static void calc_rom_iowidth()
{
  int is_c0,is_c1;
  LONG last_adr,c0_ilen,c1_ilen,c0_olen,c1_olen;
  LONG i;
  last_adr = this.rom_depth-1;
  c0_ilen = calc_bitlength(last_adr);
  c1_ilen = calc_bitlength(last_adr);
  c0_olen = 0x0LL;
  c1_olen = 0x0LL;
  is_c0=0;
  is_c1=0;
  for(i=0;i<=last_adr;i++){
    LONG tmp_c0_olen,tmp_c1_olen;
    tmp_c0_olen = calc_bitlength((this.rom_c0)[i]);
    tmp_c1_olen = calc_bitlength((this.rom_c1)[i]);
    if(tmp_c0_olen > c0_olen) c0_olen = tmp_c0_olen;
    if(tmp_c1_olen > c1_olen) c1_olen = tmp_c1_olen;
  }
  this.rom_c0_ilen = c0_ilen; // static member
  this.rom_c1_ilen = c1_ilen; // static member
  this.rom_c0_olen = c0_olen; // static member
  this.rom_c1_olen = c1_olen; // static member
  return;
}


static void calc_rom_size()
{
  LONG c0_depth,c1_depth,c0_size,c1_size;
  c0_depth = (LONG)powl(2.0,(REAL)this.rom_c0_ilen);
  c1_depth = (LONG)powl(2.0,(REAL)this.rom_c1_ilen);
  c0_size = (this.rom_c0_olen)*c0_depth;
  c1_size = (this.rom_c1_olen)*c1_depth;
  this.rom_c0_size = c0_size;              // static member
  this.rom_c1_size = c1_size;              // static member
  this.rom_total_size = c0_size + c1_size; // static member
  return;
}

static void set_romfilenames(nman,ncut)
     int nman;
     int ncut;
{
  /* File name must be 4 words(16-bit) as follows.
     fixed(1-bit) | Table Type id(4-bit) | Object information(11-bit)|   
     | Nman(6-bit) | Ncut(5-bit) | 
  */
  /*  Table Type                     ,   Table Type id */
  /*  pg_log_to_fix_itp(c0)          ,   1000   */
  /*  pg_log_to_fix_itp(c1)          ,   1001   */
  int fname_vec;

  fname_vec =  (0x1<<15)| (0x8<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
  sprintf(this.filename_c0,"%04x.mif",fname_vec);
  sprintf(this.lcromname_c0,"%04x",fname_vec);

  fname_vec =  (0x1<<15)| (0x9<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
  sprintf(this.filename_c1,"%04x.mif",fname_vec);
  sprintf(this.lcromname_c1,"%04x",fname_vec);

  return;
}


/*           CCCC    CC
 *          C    C  C  C
 *          C       C  C
 *          C    C  C  C
 *           CCCC    CC
 */
static void create_mif_c0()
{
  LONG i,nentry;
  FILE *fopen(),*fp;
  nentry = 0x1<<this.rom_c0_ilen;
  fp = fopen(this.filename_c0,"w");
  fprintf(fp,"DEPTH = %lld;\n",nentry);
  fprintf(fp,"WIDTH = %lld;\n\n",this.rom_c0_olen);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  for(i=0;i<this.rom_depth;i++){ /* start with i=0 not 1 !! */
    fprintf(fp,"    %llX : %llX ;\n",i,(this.rom_c0)[(int)i]);
  }
  for(i=this.rom_depth;i<nentry;i++){
    fprintf(fp,"    %llX : 0 ;\n",i);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}


/*           CCCC    C
 *          C    C  CC
 *          C        C
 *          C    C   C
 *           CCCC    C
 */
static void create_mif_c1()
{
  LONG i,nentry;
  FILE *fopen(),*fp;
  nentry = 0x1<<this.rom_c1_ilen;
  fp = fopen(this.filename_c1,"w");
  fprintf(fp,"DEPTH = %lld;\n",nentry);
  fprintf(fp,"WIDTH = %lld;\n\n",this.rom_c1_olen);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  for(i=0;i<this.rom_depth;i++){ /* start with i=0 not 1!! */
    fprintf(fp,"    %llX : %llX ;\n",i,(this.rom_c1)[(int)i]);
  }
  for(i=this.rom_depth;i<nentry;i++){
    fprintf(fp,"    %llX : 0 ;\n",i);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}

/******* functions for interporated table generation (END)***********/


static void generate_fixed_pipelinemap(int nstage);

void generate_pg_conv_ltof_itp(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int i;
  int nbit_fix,nbit_log,nbit_man,nstage,nbit_frac,nbit_ctl;
  int ncut;
  char sx[STRLEN],sy[STRLEN];
  int ic,im,ie,is;
  int iflag, j;
  int is_esb;
 
  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit_log = atoi(sdata[3]);
  nbit_man = atoi(sdata[4]);
  nbit_fix = atoi(sdata[5]);
  nstage = atoi(sdata[6]);
  ncut = atoi(sdata[7]);
  is_esb=(*vd).alteraesbflag;
  (*vd).alteraesbflag = 1;


  /* NEED ERROR CODE like "if(ncut<3){..}" */
  if((ncut<2)||(ncut>=nbit_man)){
    fprintf(stderr,"Error at pg_conv_ftol_itp.\n");
    fprintf(stderr,"\t ncut must be [2,%d).\n",nbit_man);
    fprintf(stderr,"\t But ncut is %d\n",ncut);
    exit(0);
  }

  if(nstage<1){ 
    nstage = 1;
    printf("nstage: fixed to %d\n",nstage);
  }
  if(nstage>16){
    nstage = 16;
    printf("nstage: fixed to %d\n",nstage);
  }

  printf("conv ltof (itp): %s = %s : ",sx,sy);
  printf("nbit log %d man %d fix %d nstage %d cut %d\n",nbit_log,nbit_man,nbit_fix,nstage,ncut);

  /* SET STATIC PARAMETER */
  this.nfix = nbit_fix;
  this.nlog = nbit_log;
  this.nman = nbit_man;
  this.ntin = nbit_man;
  this.ncut = ncut;
  this.npipe = nstage;
  this.bit_ext = 2; /* Fixed */
  this.npmax = 16;
  this.nexp = (int)(log((double)(nbit_fix+nbit_man))/log(2.0) + 1.0);

  generate_critical_path_model(nbit_fix,nbit_log,nbit_man,ncut,nstage,"EP20K400EFC672-2X");
  if(__PIPELINEMAP_MANUAL__ == 1){
    //    generate_pipelinemap_manual(nstage);
    generate_fixed_pipelinemap(nstage); // change at 2003/12/15
  }else{
    generate_pipelinemap(nstage);
  }

  //--debug pipelining--
  //    for(i=1;i<=this.npmax;i++)if(this.is_pipe[i]) printf("%02d\n",i);
  //--debug pipelining--

  /* table instantiation  (5 steps) */
  do_malloc_for_rom();                                                     /* Step 1 */
  gen_rom((LONG)(this.ntin),(LONG)nbit_man,(LONG)ncut,(LONG)this.bit_ext); /* Step 2 */
  calc_rom_iowidth();                                                      /* Step 3 */
  calc_rom_size();                                                         /* Step 4 */
  set_romfilenames(nbit_man,ncut);                                         /* Step 5 */

  /*** generate component decleration ****/
  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_conv_ltof_itp_%d_%d_%d_%d_%d\n", nbit_log,nbit_man,nbit_fix,ncut,nstage);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(logdata : in std_logic_vector(%d downto 0);   \n",nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "	     fixdata : out std_logic_vector(%d downto 0);  \n",nbit_fix-1 );ic++;
    sprintf((*vd).sdc[ic], "	     clk : in std_logic);                                     \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                  \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                  \n" );ic++;
    (*vd).ic = ic; 
  }
  
  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_conv_ltof_itp_%d_%d_%d_%d_%d\n",(*vd).ucnt,nbit_log,nbit_man,nbit_fix,ncut,nstage);im++;
  sprintf((*vd).sdm[im], "                  port map (logdata=>%s,fixdata=>%s,clk=>pclk);\n",sx,sy);im++;   
  sprintf((*vd).sdm[im], "                                                                   \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_log-1);is++;
  (*vd).is = is;

  /*** generate component ****/

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "-- ****************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG FIX-POINT CONVERTER TO LOGARITHMIC USING INTERPOLATED ROM * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                       * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  VERSION: 2.00                                                 * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Wed Jun 04 21:01:00 JST 2003                 * \n" );ie++;
    sprintf((*vd).sde[ie], "-- ****************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "entity pg_conv_ltof_itp_%d_%d_%d_%d_%d is\n", nbit_log,nbit_man,nbit_fix,ncut,nstage);ie++;
    sprintf((*vd).sde[ie], "  port(logdata : in std_logic_vector(%d downto 0);\n",nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "       fixdata : out std_logic_vector(%d downto 0);\n",nbit_fix-1 );ie++;
    sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;
    sprintf((*vd).sde[ie], "end pg_conv_ltof_itp_%d_%d_%d_%d_%d;\n", nbit_log,nbit_man,nbit_fix,ncut,nstage);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "architecture rtl of pg_conv_ltof_itp_%d_%d_%d_%d_%d is \n", nbit_log,nbit_man,nbit_fix,ncut,nstage);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    if((*vd).alteraesbflag){
      // ESB ROM
      if((this.is_pipe)[1]){
	sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;
      }else{    
	sprintf((*vd).sde[ie], "  component unreg_table                                            \n" );ie++;
      }
      sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer ;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "             OUT_WIDTH: integer ;                                  \n" );ie++;
      sprintf((*vd).sde[ie], "             TABLE_FILE: string);                                  \n" );ie++;
      sprintf((*vd).sde[ie], "    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);        \n" );ie++;
      sprintf((*vd).sde[ie], "         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);     \n" );ie++;
      sprintf((*vd).sde[ie], "         clk : in std_logic);                                      \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    }else{
      // LCELL ROM
      int c0i =(int)(this.rom_c0_ilen);
      int c0o =(int)(this.rom_c0_olen);
      int c1i =(int)(this.rom_c1_ilen);
      int c1o =(int)(this.rom_c1_olen);
      int nst; // n-satge of lcell_rom module
      if((this.is_pipe)[1]) nst=1; else nst=0;
      sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_%d                      \n",this.lcromname_c0,c0i,c0o,nst);ie++;
      sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c0i-1);ie++;
      sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
      sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c0o-1);ie++;
      sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
      sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_%d                      \n",this.lcromname_c1,c1i,c1o,nst);ie++;
      sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c1i-1);ie++;
      sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
      sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c1o-1);ie++;
      sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    nbit_frac = nbit_man + 1;
    //    nbit_ctl = nbit_log - nbit_man - 2;
    nbit_ctl = this.nexp;

    if(((this.is_pipe)[13])||((this.is_pipe)[14])||((this.is_pipe)[15])){
      int _np=0;
      if((this.is_pipe)[13]) _np++;
      if((this.is_pipe)[14]) _np++;
      if((this.is_pipe)[15]) _np++;
      sprintf((*vd).sde[ie], "  component reged_shift_ltof_%d_%d_%d_%d\n", this.nman+1,this.nexp,this.nfix-1,_np );ie++;
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);   \n",nbit_frac-1 );ie++;
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n",   nbit_ctl-1 );ie++;
      sprintf((*vd).sde[ie], "          clk : in std_logic;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0)); \n",nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_shift_ltof_%d_%d_%d              \n", this.nman+1,this.nexp,this.nfix-1);ie++;
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);   \n",nbit_frac-1 );ie++;
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n",   nbit_ctl-1 );ie++;
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0)); \n",nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    if(((this.is_pipe)[2])||((this.is_pipe)[3])||((this.is_pipe)[4])){
      sprintf((*vd).sde[ie], "  component reged_mult                                       \n" );ie++;   
      sprintf((*vd).sde[ie], "    generic (IN_WIDTHA: integer;                             \n" );ie++;   
      sprintf((*vd).sde[ie], "             IN_WIDTHB: integer;                             \n" );ie++;   
      sprintf((*vd).sde[ie], "             OUT_WIDTH: integer;                             \n" );ie++;   
      sprintf((*vd).sde[ie], "             REPRESENTATION: string :=\"UNSIGNED\";          \n" );ie++;   
      sprintf((*vd).sde[ie], "             PIPELINE: integer);                             \n" );ie++;   
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(IN_WIDTHA-1 downto 0);  \n" );ie++;   
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(IN_WIDTHB-1 downto 0);  \n" );ie++;   
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(OUT_WIDTH-1 downto 0);\n" );ie++;   
      sprintf((*vd).sde[ie], "          clock: in std_logic);                               \n" );ie++;   
      sprintf((*vd).sde[ie], "  end component;                                             \n" );ie++;   
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_mult                                       \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (IN_WIDTHA: integer;                             \n" );ie++;
      sprintf((*vd).sde[ie], "             IN_WIDTHB: integer;                             \n" );ie++;
      sprintf((*vd).sde[ie], "             OUT_WIDTH: integer;                             \n" );ie++;
      sprintf((*vd).sde[ie], "             REPRESENTATION: string :=\"UNSIGNED\");         \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(IN_WIDTHA-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(IN_WIDTHB-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(OUT_WIDTH-1 downto 0) \n" );ie++;
      sprintf((*vd).sde[ie], "          );                                                 \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                             \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    if((this.is_pipe)[7]){
      sprintf((*vd).sde[ie], "  component reged_add_sub_cout                           \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                             \n" );ie++;
      sprintf((*vd).sde[ie], "             DIRECTION: string;                          \n" );ie++;
      sprintf((*vd).sde[ie], "             PIPELINE: integer);                         \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          clock: in std_logic;   \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
      sprintf((*vd).sde[ie], "          cout : out std_logic);                         \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                         \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_add_sub_cout                           \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                             \n" );ie++;
      sprintf((*vd).sde[ie], "             DIRECTION: string);                         \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
      sprintf((*vd).sde[ie], "          cout : out std_logic);                         \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                         \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    if((this.is_pipe)[9]){
      sprintf((*vd).sde[ie], "  component reged_add_sub                                 \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                              \n" );ie++;
      sprintf((*vd).sde[ie], "             DIRECTION: string;                           \n" );ie++;
      sprintf((*vd).sde[ie], "             PIPELINE: integer);                          \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(WIDTH-1 downto 0);   \n" );ie++;
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(WIDTH-1 downto 0);   \n" );ie++;
      sprintf((*vd).sde[ie], "          clock: in std_logic;   \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));\n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                          \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  component unreg_add_sub                                 \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                              \n" );ie++;
      sprintf((*vd).sde[ie], "             DIRECTION: string);                          \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa: in std_logic_vector(WIDTH-1 downto 0);   \n" );ie++;
      sprintf((*vd).sde[ie], "          datab: in std_logic_vector(WIDTH-1 downto 0);   \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));\n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                          \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

      // INTERNAL WIRES
    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(i=0;i<15;i++){sprintf((*vd).sde[ie], "sign%d,",i);ie++;}
    sprintf((*vd).sde[ie], "sign15: std_logic;\n");ie++;

    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(i=0;i<11;i++){sprintf((*vd).sde[ie], "nz%d,",i);ie++;}
    sprintf((*vd).sde[ie], "nz11: std_logic;\n");ie++;

    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(i=0;i<12;i++){sprintf((*vd).sde[ie], "exp%d,",i);ie++;}
    sprintf((*vd).sde[ie], "exp12: std_logic_vector(%d downto 0);\n",this.nexp-1);ie++;
    sprintf((*vd).sde[ie], "  signal frac0,frac1 : std_logic_vector(%d downto 0);\n",this.nman);ie++;
    sprintf((*vd).sde[ie], "  signal absol : std_logic_vector(%d downto 0);\n",this.nfix-2);ie++;
    sprintf((*vd).sde[ie], "  -- FOR INTERPOLATED TABLE\n" );ie++;
    sprintf((*vd).sde[ie], "  signal man0 : std_logic_vector(%d downto 0);\n",this.nman-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_x : std_logic_vector(%d downto 0);\n",this.nman-this.ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_dx0,itp_dx1 : std_logic_vector(%d downto 0);\n",this.ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c0,itp_c0d0,itp_c0d1,itp_c0d2 : std_logic_vector(%d downto 0);\n",(int)this.rom_c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c0d3,itp_c0d4,itp_c0d5,itp_c0d6 : std_logic_vector(%d downto 0);\n",(int)this.rom_c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1 : std_logic_vector(%d downto 0);\n",(int)this.rom_c1_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx0,itp_c1dx1 : std_logic_vector(%d downto 0);\n",(int)this.rom_c1_olen+this.ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx_shft0,itp_c1dx_shft1 : std_logic_vector(%d downto 0);\n"
	    ,(int)this.rom_c1_olen+this.ncut-this.nman-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_rcoll_add,itp_c1dx2 : std_logic_vector(%d downto 0);\n"
	    ,(int)this.rom_c1_olen+this.ncut-this.nman);ie++;
    sprintf((*vd).sde[ie], "  signal itp_addy : std_logic_vector(%d downto 0);\n",(int)this.rom_c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_addz0,itp_addz1 : std_logic_vector(%d downto 0);\n",(int)this.rom_c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_out0,itp_out1 : std_logic_vector(%d downto 0);\n",this.nman-1);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "  sign0 <= logdata(%d);\n",this.nlog-1);ie++;
    sprintf((*vd).sde[ie], "  nz0 <= logdata(%d);\n",this.nlog-2);ie++;
    sprintf((*vd).sde[ie], "  exp0 <= logdata(%d downto %d);\n",this.nman+this.nexp-1,nbit_man);ie++;
    sprintf((*vd).sde[ie], "  man0 <= logdata(%d downto 0);\n",nbit_man-1);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 1-TH to 11-TH PIPELINE ---- */
    for(i=1;i<=11;i++){
      if((this.is_pipe)[i]){
	sprintf((*vd).sde[ie], "  --- PIPELINE %d ---\n",i);ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin\n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
	sprintf((*vd).sde[ie], "      sign%d <= sign%d;\n",i,i-1);ie++;
	sprintf((*vd).sde[ie], "      nz%d   <= nz%d;\n"  ,i,i-1);ie++;
	sprintf((*vd).sde[ie], "      exp%d  <= exp%d;\n" ,i,i-1);ie++;
	sprintf((*vd).sde[ie], "    end if;\n");ie++;
	sprintf((*vd).sde[ie], "  end process;\n");ie++;
	sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
      }else{
	sprintf((*vd).sde[ie], "  --- PIPELINE %d(OFF) ---\n",i);ie++;
	sprintf((*vd).sde[ie], "  sign%d <= sign%d;\n",i,i-1);ie++;
	sprintf((*vd).sde[ie], "  nz%d   <= nz%d;\n"  ,i,i-1);ie++;
	sprintf((*vd).sde[ie], "  exp%d  <= exp%d;\n" ,i,i-1);ie++;
	sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
      }
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    /* ---- 12-TH PIPELINE ---- */
    if((this.is_pipe)[12]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 12 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign12 <= sign11;\n");ie++;
      sprintf((*vd).sde[ie], "      exp12  <= exp11;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 12(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign12 <= sign11;\n");ie++;
      sprintf((*vd).sde[ie], "  exp12  <= exp11;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 13-TH to 15-TH PIPELINE ---- */
    for(i=13;i<=15;i++){
      if((this.is_pipe)[i]){
	sprintf((*vd).sde[ie], "  --- PIPELINE %d ---\n",i);ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin\n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
	sprintf((*vd).sde[ie], "      sign%d <= sign%d;\n",i,i-1);ie++;
	sprintf((*vd).sde[ie], "    end if;\n");ie++;
	sprintf((*vd).sde[ie], "  end process;\n");ie++;
	sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
      }else{
	sprintf((*vd).sde[ie], "  --- PIPELINE %d(OFF) ---\n",i);ie++;
	sprintf((*vd).sde[ie], "  sign%d <= sign%d;\n",i,i-1);ie++;
	sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
      }
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    // FOR INTERPORATION TABLE
    /* Instansiate the MIF Files */
    if((*vd).alteraesbflag){
      create_mif_c0();
      create_mif_c1();
    }
    sprintf((*vd).sde[ie], "-- TABLE FUNCTION PART (START) \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG CONV LOGARITHMIC TO FIXED-POINT MODULE OF              * \n" );ie++;
    sprintf((*vd).sde[ie], "-- * INTERPORATED TABLE LOGIC : f(x+dx) ~= c0(x) + c1(x)dx       * \n" );ie++;
    sprintf((*vd).sde[ie], "-- * The c0(x) and c1(x) are chebyshev coefficients.             * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "  itp_x   <= man0(%d downto %d);\n",this.nman-1,ncut);ie++;
    sprintf((*vd).sde[ie], "  itp_dx0 <= man0(%d downto 0);\n",ncut-1);ie++;
    sprintf((*vd).sde[ie], "\n");ie++;

    /* ---- 1-TH PIPELINE ---- */
    if((this.is_pipe)[1]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 1 ---\n");ie++;
      if((*vd).alteraesbflag){
	// -- ESB ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) -- output reged                    \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: table generic map (IN_WIDTH=>%d,\n",(int)this.rom_c0_ilen);ie++;
	sprintf((*vd).sde[ie], "                           OUT_WIDTH=>%d,     \n",(int)this.rom_c0_olen);ie++;
	sprintf((*vd).sde[ie], "                           TABLE_FILE=>\"%s\")\n",this.filename_c0);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clk);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) -- output reged                    \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: table generic map (IN_WIDTH=>%d,\n",(int)this.rom_c1_ilen);ie++;
	sprintf((*vd).sde[ie], "                           OUT_WIDTH=>%d,     \n",(int)this.rom_c1_olen);ie++;
	sprintf((*vd).sde[ie], "                           TABLE_FILE=>\"%s\")\n",this.filename_c1);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clk);\n");ie++;
	sprintf((*vd).sde[ie], "\n");ie++;
      }else{
	// -- LCELL ROM ---
	int c0i =(int)(this.rom_c0_ilen);
	int c0o =(int)(this.rom_c0_olen);
	int c1i =(int)(this.rom_c1_ilen);
	int c1o =(int)(this.rom_c1_olen);
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lcell_rom_%s_%d_%d_1\n",this.lcromname_c0,c0i,c0o);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clk);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lcell_rom_%s_%d_%d_1\n",this.lcromname_c1,c1i,c1o);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clk);\n\n");ie++;
      }
      sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;   
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;   
      sprintf((*vd).sde[ie], "      itp_dx1 <= itp_dx0;         \n" );ie++;   
      sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;   
      sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;   
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 1(OFF) ---\n");ie++;
      if((*vd).alteraesbflag){
	// -- ESB ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) --                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: unreg_table generic map (IN_WIDTH=>%d,\n",(int)this.rom_c0_ilen);ie++;
	sprintf((*vd).sde[ie], "                           OUT_WIDTH=>%d,           \n",(int)this.rom_c0_olen);ie++;
	sprintf((*vd).sde[ie], "                           TABLE_FILE=>\"%s\")      \n",this.filename_c0);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clk); \n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: unreg_table generic map (IN_WIDTH=>%d,\n",(int)this.rom_c1_ilen);ie++;
	sprintf((*vd).sde[ie], "                           OUT_WIDTH=>%d,           \n",(int)this.rom_c1_olen);ie++;
	sprintf((*vd).sde[ie], "                           TABLE_FILE=>\"%s\")      \n",this.filename_c1);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clk); \n\n");ie++;
	sprintf((*vd).sde[ie], "\n");ie++;
      }else{
	// -- LCELL ROM ---
	int c0i =(int)(this.rom_c0_ilen);
	int c0o =(int)(this.rom_c0_olen);
	int c1i =(int)(this.rom_c1_ilen);
	int c1o =(int)(this.rom_c1_olen);
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lcell_rom_%s_%d_%d_0\n",this.lcromname_c0,c0i,c0o);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clk);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lcell_rom_%s_%d_%d_0\n",this.lcromname_c1,c1i,c1o);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clk);\n\n");ie++;
      }
      sprintf((*vd).sde[ie], "  itp_dx1 <= itp_dx0;    \n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n\n");ie++;

    /* ---- 2,3,4-TH PIPELINE ---- */
    if(((this.is_pipe)[2])||((this.is_pipe)[3])||((this.is_pipe)[4])){
      sprintf((*vd).sde[ie], "  --- PIPELINE 2,3,4 ---\n");ie++;
      sprintf((*vd).sde[ie], "  -- ITP MULT --                   \n");ie++;
      sprintf((*vd).sde[ie], "  itp_mult: reged_mult             \n");ie++;
      sprintf((*vd).sde[ie], "    GENERIC MAP (                  \n");ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHA => %d,               \n",(int)this.rom_c1_olen);ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHB => %d,               \n",this.ncut);ie++;
      sprintf((*vd).sde[ie], "    OUT_WIDTH => %d,               \n",(int)this.rom_c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    REPRESENTATION => \"UNSIGNED\",\n");ie++;
      {
	int _np=0;
	if((this.is_pipe)[2]) _np++;
	if((this.is_pipe)[3]) _np++;
	if((this.is_pipe)[4]) _np++;
	sprintf((*vd).sde[ie], "    PIPELINE => %d\n",_np);ie++;
      }
      sprintf((*vd).sde[ie], "    )                    \n");ie++;
      sprintf((*vd).sde[ie], "    PORT MAP (           \n");ie++;
      sprintf((*vd).sde[ie], "    clock  => clk,       \n");ie++;
      sprintf((*vd).sde[ie], "    dataa  => itp_c1,    \n");ie++;
      sprintf((*vd).sde[ie], "    datab  => itp_dx1,   \n");ie++;
      sprintf((*vd).sde[ie], "    result => itp_c1dx0  \n");ie++;
      sprintf((*vd).sde[ie], "    );                   \n");ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      if((this.is_pipe)[2]){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
	sprintf((*vd).sde[ie], "      itp_c0d0 <= itp_c0;         \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      }else{
	sprintf((*vd).sde[ie], "  itp_c0d0 <= itp_c0;             \n" );ie++;
      }
      if((this.is_pipe)[3]){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
	sprintf((*vd).sde[ie], "      itp_c0d1 <= itp_c0d0;       \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      }else{
	sprintf((*vd).sde[ie], "  itp_c0d1 <= itp_c0d0;           \n" );ie++;
      }
      if((this.is_pipe)[4]){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
	sprintf((*vd).sde[ie], "      itp_c0d2 <= itp_c0d1;       \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      }else{
	sprintf((*vd).sde[ie], "  itp_c0d2 <= itp_c0d1;           \n" );ie++;
      }
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 2,3,4 (OFF)---\n");ie++;
      sprintf((*vd).sde[ie], "  -- ITP MULT --                       \n");ie++;
      sprintf((*vd).sde[ie], "  itp_mult: unreg_mult                 \n");ie++;
      sprintf((*vd).sde[ie], "    GENERIC MAP (                      \n");ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHA => %d,                  \n",(int)this.rom_c1_olen);ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHB => %d,                  \n",this.ncut);ie++;
      sprintf((*vd).sde[ie], "    OUT_WIDTH => %d,                  \n",(int)this.rom_c1_olen+this.ncut);ie++;
      sprintf((*vd).sde[ie], "    REPRESENTATION => \"UNSIGNED\")\n");ie++;
      sprintf((*vd).sde[ie], "    PORT MAP (                         \n");ie++;
      sprintf((*vd).sde[ie], "    dataa  => itp_c1,                  \n");ie++;
      sprintf((*vd).sde[ie], "    datab  => itp_dx1,                 \n");ie++;
      sprintf((*vd).sde[ie], "    result => itp_c1dx0                \n");ie++;
      sprintf((*vd).sde[ie], "    );                                 \n");ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d0 <= itp_c0;  \n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d1 <= itp_c0d0;\n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d2 <= itp_c0d1;\n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 5-TH PIPELINE ---- */
    if((this.is_pipe)[5]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 5 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c0d3 <= itp_c0d2;\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx1 <= itp_c1dx0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 5(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d3 <= itp_c0d2;\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx1 <= itp_c1dx0;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "  -- SHIFT %d-bit(DX's MANTISSA) TO LSB\n",this.nman);ie++;
    sprintf((*vd).sde[ie], "  itp_c1dx_shft0 <= itp_c1dx1(%d downto %d);\n",((int)this.rom_c1_olen+this.ncut-1),this.nman);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 6-TH PIPELINE ---- */
    if((this.is_pipe)[6]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 6 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c0d4 <= itp_c0d3;\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx_shft1 <= itp_c1dx_shft0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 6(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d4 <= itp_c0d3;\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx_shft1 <= itp_c1dx_shft0;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 7-TH PIPELINE ---- */
    if((this.is_pipe)[7]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 7 ---\n");ie++;
      {
	char _half[256];
	int _addwidth;
	strcpy(_half,"");
	if(this.ncut<this.bit_ext){
	  fprintf(stderr,"Error at pg_conv_ltof_itp.\n");
	  fprintf(stderr,"\t bit_ext is larger than ncut.\n");
	  fprintf(stderr,"\t bit_ext %d\n",this.bit_ext);
	  fprintf(stderr,"\t ncut    %d\n",this.ncut);
	  exit(1);
	}
	_addwidth = (int)this.rom_c1_olen+this.ncut-this.nman;
	for(i=0;i<(_addwidth-this.bit_ext);i++) strcat(_half,"0");
	strcat(_half,"1");
	for(i=0;i<(this.bit_ext-1);i++) strcat(_half,"0");
	sprintf((*vd).sde[ie], "  itp_round_add: reged_add_sub_cout                 \n");ie++;
	sprintf((*vd).sde[ie], "        generic map (WIDTH=>%d,DIRECTION=>\"ADD\",PIPELINE=>1)\n",_addwidth);ie++;
	sprintf((*vd).sde[ie], "        port map(result=>itp_rcoll_add(%d downto 0),\n",_addwidth-1);ie++;
	sprintf((*vd).sde[ie], "                 clock=>clk,                        \n");ie++;
	sprintf((*vd).sde[ie], "                 dataa=>\"%s\",                     \n",_half);ie++;
	sprintf((*vd).sde[ie], "                 datab=>itp_c1dx_shft1,             \n" );ie++;
	sprintf((*vd).sde[ie], "                 cout=>itp_rcoll_add(%d));          \n",_addwidth);ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c0d5 <= itp_c0d4;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      {
	char _half[256];
	int _addwidth;
	strcpy(_half,"");
	if(this.ncut<this.bit_ext){
	  fprintf(stderr,"Error at pg_conv_ltof_itp.\n");
	  fprintf(stderr,"\t bit_ext is larger than ncut.\n");
	  fprintf(stderr,"\t bit_ext %d\n",this.bit_ext);
	  fprintf(stderr,"\t ncut    %d\n",this.ncut);
	  exit(1);
	}
	_addwidth = (int)this.rom_c1_olen+this.ncut-this.nman;
	for(i=0;i<(_addwidth-this.bit_ext);i++) strcat(_half,"0");
	strcat(_half,"1");
	for(i=0;i<(this.bit_ext-1);i++) strcat(_half,"0");
	sprintf((*vd).sde[ie], "  itp_round_add: unreg_add_sub_cout                 \n");ie++;
	sprintf((*vd).sde[ie], "        generic map (WIDTH=>%d,DIRECTION=>\"ADD\")  \n",_addwidth );ie++;
	sprintf((*vd).sde[ie], "        port map(result=>itp_rcoll_add(%d downto 0),\n",_addwidth-1 );ie++;
	sprintf((*vd).sde[ie], "                 dataa=>\"%s\",                     \n",_half );ie++;
	sprintf((*vd).sde[ie], "                 datab=>itp_c1dx_shft1,             \n" );ie++;
	sprintf((*vd).sde[ie], "                 cout=>itp_rcoll_add(%d));          \n",_addwidth);ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }
      sprintf((*vd).sde[ie], "  --- PIPELINE 7(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d5 <= itp_c0d4;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 8-TH PIPELINE ---- */
    if((this.is_pipe)[8]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 8 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c0d6 <= itp_c0d5;\n");ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx2 <= itp_rcoll_add;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 8(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d6 <= itp_c0d5;\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx2 <= itp_rcoll_add;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;

    {
      int nbit_zero;
      char _zero[256];
      nbit_zero = (int)this.rom_c0_olen - (int)this.rom_c1_olen - this.ncut + this.nman - 1; // It is nothing to 'this.bit_ext' length. 
      strcpy(_zero,"");
      for(i=0;i<nbit_zero;i++) strcat(_zero,"0");
      //      if(nbit_zero >1){sprintf((*vd).sde[ie], "  itp_addy <= \"%s\" & itp_c1dx2;\n",_zero);ie++;} //bugfixed 2003/12/15
      if(nbit_zero >0){sprintf((*vd).sde[ie], "  itp_addy <= \"%s\" & itp_c1dx2;\n",_zero);ie++;} 
      else if(nbit_zero == 1){sprintf((*vd).sde[ie], "  itp_addy <= '0' & itp_c1dx2;\n");ie++;} //bugfixed 2004/01/30
      else if(nbit_zero == 0){sprintf((*vd).sde[ie], "  itp_addy <= itp_c1dx2;\n");ie++;}       //bugfixed 2004/01/30
      else { fprintf(stderr,"Err: Ncut = %d\n",this.ncut);exit(1);}
    }

    /* ---- 9-TH PIPELINE ---- */
    if((this.is_pipe)[9]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 9 ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_add: reged_add_sub\n");ie++;
      sprintf((*vd).sde[ie], "      generic map (WIDTH=>%d,DIRECTION=>\"ADD\",PIPELINE=>1)\n",(int)this.rom_c0_olen);ie++;
      sprintf((*vd).sde[ie], "      port map(result=>itp_addz0,dataa=>itp_c0d6,datab=>itp_addy,clock=>clk);\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 9(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_add: unreg_add_sub\n");ie++;
      sprintf((*vd).sde[ie], "      generic map (WIDTH=>%d,DIRECTION=>\"ADD\")\n",(int)this.rom_c0_olen);ie++;
      sprintf((*vd).sde[ie], "      port map(result=>itp_addz0,dataa=>itp_c0d6,datab=>itp_addy);\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;

    /* ---- 10-TH PIPELINE ---- */
    if((this.is_pipe)[10]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 10 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_addz1 <= itp_addz0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 10(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_addz1 <= itp_addz0;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;


    // -- (>> EXT_BIT)
    sprintf((*vd).sde[ie], "  itp_out0 <= itp_addz1(%d downto %d);\n",(int)this.rom_c0_olen-1,this.bit_ext);ie++;

    /* ---- 11-TH PIPELINE ---- */
    if((this.is_pipe)[11]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 11 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      itp_out1 <= itp_out0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 11(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_out1 <= itp_out0;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "-- TABLE FUNCTION PART (END) \n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    {
      char _zero[256];
      itobc(0,_zero,this.nman+1);
      sprintf((*vd).sde[ie], "  with nz11 select                   \n" );ie++;
      sprintf((*vd).sde[ie], "    frac0 <= '1' & itp_out1 when '1',\n" );ie++;
      sprintf((*vd).sde[ie], "             \"%s\" when others;     \n",_zero );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    /* ---- 12-TH PIPELINE ---- */
    if((this.is_pipe)[12]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 12 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      frac1 <= frac0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 12(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  frac1 <= frac0;\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;


    /* ---- 13,14,15-TH PIPELINE ---- */
    if(((this.is_pipe)[13])||((this.is_pipe)[14])||((this.is_pipe)[15])){
      int _np=0;
      sprintf((*vd).sde[ie], "  --- PIPELINE 13,14,15 ---\n");ie++;
      if((this.is_pipe)[13]) _np++;
      if((this.is_pipe)[14]) _np++;
      if((this.is_pipe)[15]) _np++;
      sprintf((*vd).sde[ie], "  u3: reged_shift_ltof_%d_%d_%d_%d\n",this.nman+1,this.nexp,this.nfix-1,_np);ie++;
      sprintf((*vd).sde[ie], "            port map (indata=>frac1,control=>exp12,outdata=>absol,clk=>clk);\n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 13,14,15(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  u3: unreg_shift_ltof_%d_%d_%d \n",this.nman+1,this.nexp,this.nfix-1);ie++;
      sprintf((*vd).sde[ie], "            port map (indata=>frac1,control=>exp12,outdata=>absol);\n" );ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;


    /* ---- 16-TH PIPELINE ---- */
    if((this.is_pipe)[16]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 16 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      fixdata(%d downto 0) <= absol;\n",this.nfix-2);ie++;
      sprintf((*vd).sde[ie], "      fixdata(%d) <= sign15;\n",this.nfix-1);ie++;
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 16(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  fixdata(%d downto 0) <= absol;\n",this.nfix-2);ie++;
      sprintf((*vd).sde[ie], "  fixdata(%d) <= sign15;\n",this.nfix-1);ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;   
    (*vd).ie = ie;


    // TABLE SUBMODULE GENERATION
    if((*vd).alteraesbflag){
      if((this.is_pipe)[1]) generate_pg_table(vd); else generate_pg_unreg_table(vd);
    }
    // MULTIPLIER SUBMODULE GENERATION
    if(((this.is_pipe)[2])||((this.is_pipe)[3])||((this.is_pipe)[4]))
      generate_pg_reged_mult(vd); else  generate_pg_unreg_mult(vd);
    // ADDER(WITH COUT) SUBMODULE GENERATION
    if((this.is_pipe)[7]) generate_reged_add_sub_cout(vd); else generate_unreg_add_sub_cout(vd);
    // ADDER SUBMODULE GENERATION
    if((this.is_pipe)[9]) generate_reged_add_sub(vd); else generate_unreg_add_sub(vd);
    // SHIFTER SUBMODULE GENERATION
    if(((this.is_pipe)[13])||((this.is_pipe)[14])||((this.is_pipe)[15])){
      int shifter_np = 0;
      if((this.is_pipe)[13]) shifter_np++;
      if((this.is_pipe)[14]) shifter_np++;
      if((this.is_pipe)[15]) shifter_np++;
      generate_pg_reged_shift_ltof(vd,nbit_frac,nbit_fix-1,nbit_ctl,shifter_np);
    }else{
      generate_pg_unreg_shift_ltof(vd,this.nman+1,nbit_fix-1,nbit_ctl);
    }
    // -- GENERATE LCELL ROM 
    if(((*vd).alteraesbflag)==0){// c0 rom
      int ilen,olen,depth,np;
      char name[256];
      LONG* rom;
      ilen = this.rom_c0_ilen;
      olen = this.rom_c0_olen;
      depth = this.rom_depth; // entries of array (not rom)
      if((this.is_pipe)[1]) np = 1; else np = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c0;
      strcpy(name,this.lcromname_c0);
      generate_pg_lcell_rom(vd,ilen,olen,depth,np,name,rom);
    }
    if(((*vd).alteraesbflag)==0){// c1 rom
      int ilen,olen,depth,np;
      char name[256];
      LONG* rom;
      ilen = this.rom_c1_ilen;
      olen = this.rom_c1_olen;
      depth = this.rom_depth;
      if((this.is_pipe)[1]) np = 1; else np = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c1;
      strcpy(name,this.lcromname_c1);
      generate_pg_lcell_rom(vd,ilen,olen,depth,np,name,rom);
    }
    // -- END GENERATE LCELL ROM

    free_rom_memory();
  }
  (*vd).alteraesbflag = is_esb;
}  




// pipelining map fixed for pg_conv_ltof_itp(log17,man8,fix57,cut5)
static void generate_fixed_pipelinemap(npipe)
     int npipe;
{
  int i,pmax;
  pmax = this.npmax;
  (this.is_pipe)[0] = 0; // this is not used.
  if(npipe == 1){                             //---- NP : 1
    for(i=1;i<=(pmax-1);i++) (this.is_pipe)[i] = 0;
    (this.is_pipe)[pmax] = 1;
  }else if(npipe == 2){                       //---- NP : 2
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 3){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 4){                       //---- NP : 4
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 5){                       //---- NP : 5
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 6){                       //---- NP : 6
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 7){                       //---- NP : 7
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 8){                       //---- NP : 8
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 9){                       //---- NP : 9
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 10){                      //---- NP : 10
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 11){                      //---- NP : 11
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[14]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 12){                      //---- NP : 12
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[14]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 13){                      //---- NP : 13
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[14]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 14){                      //---- NP : 14
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[14]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 15){                      //---- NP : 15
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[14]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == pmax){                    //---- NP : MAX(=16, Apr/30/2003)
    //    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
  }else{                                      //---- NP : 0
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  }


  //  for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  //  (this.is_pipe[2]) = 1;
  //  (this.is_pipe[6]) = 1;
  //  (this.is_pipe[9]) = 1;

  return;
}
