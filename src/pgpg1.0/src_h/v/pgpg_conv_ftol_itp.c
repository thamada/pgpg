/* 
   Author: Tsuyoshi Hamada
   Version 2.01 : release

   Dec 15, 2003 : add generate_fixed_pipelinemap(int nstage)
   Jul 01, 2003 : first revision
*/ 
#include<stdio.h>
#include<math.h>
#include<string.h> // strcmp(), strcpy(), strcat()
#include "pgpg.h"
#include "pgpg_submodule.h"
           // generate_pg_table(), generate_pg_unreg_table()
           // generate_pg_penc(), generate_pg_reged_penc()
           // generate_pg_reged_shift_ftol(), generate_pg_shift_ftol()
           // generate_pg_unreg_mult(), generate_pg_reged_mult()
           // generate_reged_add_sub(), generate_reged_add_sub_cout()
           // generate_unreg_add_sub_cout(), generate_pg_lcell_rom_old()
#include "pgpg_lcelltable_core.h" // generate_pg_lcell_rom(vd,ilen,olen,depth,np,name,rom);
#define LONG long long int
#define REAL long double
#define __PIPELINEMAP_MANUAL__ 1

static struct {
  int nfix;
  int nlog;
  int nman;
  int ncut;
  int npipe;
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
    if((nfix==32)&&(nlog==17)&&(nman==8)&&(ncut==4)){
      (this.pipe_delay)[1] = 0.542;   // (ns) ireg to 1-th reg
      (this.pipe_delay)[2] = 1.163;   // (ns) 1-th reg to 2-th reg
      (this.pipe_delay)[3] = 0.598;   // (ns) 2-th reg to 3-th reg
      (this.pipe_delay)[4] = 0.615;   // (ns) 3-th reg to 4-th reg
      (this.pipe_delay)[5] = 1.879;   // (ns) 4-th reg to 5-th reg
      (this.pipe_delay)[6] = 0.0001;  // (ns) 5-th reg to 6-th reg
      (this.pipe_delay)[7] = 1.940;   // (ns) 6-th reg to 7-th reg
      (this.pipe_delay)[8] = 2.009;   // (ns) 7-th reg to 8-th reg
      (this.pipe_delay)[9] = 0.904;   // (ns) 8-th reg to 9-th reg
      (this.pipe_delay)[10] = 1.042;  // (ns) 9-th reg to 10-th reg
      (this.pipe_delay)[11] = 0.0001; // (ns) 10-th reg to 11-th reg
      (this.pipe_delay)[12] = 0.0001; // (ns) 11-th reg to 12-th reg
      (this.pipe_delay)[13] = 0.0001; // (ns) 12-th reg to 13-th reg
      (this.pipe_delay)[14] = 0.0001; // (ns) 13-th reg to 14-th reg
    }else{
      //////////////////////////////////////////////////////////////
      // Not Release ///////////////////////////////////////////////
      //////////////////////////////////////////////////////////////
      (this.pipe_delay)[1] = 0.542;   // (ns) ireg to 1-th reg
      (this.pipe_delay)[2] = 1.163;   // (ns) 1-th reg to 2-th reg
      (this.pipe_delay)[3] = 0.598;   // (ns) 2-th reg to 3-th reg
      (this.pipe_delay)[4] = 0.615;   // (ns) 3-th reg to 4-th reg
      (this.pipe_delay)[5] = 1.879;   // (ns) 4-th reg to 5-th reg
      (this.pipe_delay)[6] = 0.0001;  // (ns) 5-th reg to 6-th reg
      (this.pipe_delay)[7] = 1.940;   // (ns) 6-th reg to 7-th reg
      (this.pipe_delay)[8] = 2.009;   // (ns) 7-th reg to 8-th reg
      (this.pipe_delay)[9] =  0.0 + 0.1*pow((double)npipe,3.1); // (ns) 8-th reg to 9-th reg
      (this.pipe_delay)[10] = 1.042;  // (ns) 9-th reg to 10-th reg
      (this.pipe_delay)[11] = 0.0 + 0.1*pow((double)npipe,3.0); // (ns) 10-th reg to 11-th reg
      (this.pipe_delay)[12] = 0.0 + 0.1*pow((double)npipe,2.9); // (ns) 11-th reg to 12-th reg
      (this.pipe_delay)[13] = 0.0 + 0.1*pow((double)npipe,2.8); // (ns) 12-th reg to 13-th reg
      (this.pipe_delay)[14] = 0.0 + 0.1*pow((double)npipe,2.9); // (ns) 12-th reg to 13-th reg
    }
  }else{
    fprintf(stderr,"pg_conv_ftol_itp is only support EP20K400EFC672-2X\n");exit(1);
  }
}

/******* functions for interporated table generation ***********/
static REAL func_convftol(REAL x)
{
  REAL f;
  f = logl(1.0+x)/logl(2.0);
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
  _func = &func_convftol;
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
  /*  pg_fix_to_log_itp(c0)          ,   0110   */
  /*  pg_fix_to_log_itp(c1)          ,   0111   */
  int fname_vec;

  fname_vec =  (0x1<<15)| (0x6<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
  sprintf(this.filename_c0,"%04x.mif",fname_vec);
  sprintf(this.lcromname_c0,"%04x",fname_vec);

  fname_vec =  (0x1<<15)| (0x7<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
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

void generate_pg_conv_ftol_itp(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int i;
  int nbit_fix,nbit_log,nbit_man,nstage,nbit_penc=0;
  int ncut;
  int is_esb; 
  char sx[STRLEN],sy[STRLEN];
  int ic,im,ie,is,iflag,j;

  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  nbit_fix = atoi(sdata[3]);
  nbit_log = atoi(sdata[4]);
  nbit_man = atoi(sdata[5]);
  nstage = atoi(sdata[6]);
  ncut = atoi(sdata[7]);

  is_esb = (*vd).alteraesbflag;
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
  if(nstage>14){
    nstage = 14;
    printf("nstage: fixed to %d\n",nstage);
  }

  printf("conv ftol (itp): %s = %s : ",sx,sy);
  printf("nbit fix %d log %d man %d ncut %d nstage %d\n",nbit_fix,nbit_log,nbit_man,ncut,nstage);

  /* SET STATIC PARAMETER */
  this.nfix = nbit_fix;
  this.nlog = nbit_log;
  this.nman = nbit_man;
  this.ntin = nbit_man + 2;
  this.ncut = ncut;
  this.npipe = nstage;
  this.bit_ext = 2; /* Fixed */
  this.npmax = 14;

  generate_critical_path_model(nbit_fix,nbit_log,nbit_man,ncut,nstage,"EP20K400EFC672-2X");
  if(__PIPELINEMAP_MANUAL__ == 1){
    generate_fixed_pipelinemap(nstage);
    //    generate_pipelinemap_manual(nstage);
  }else{
    generate_pipelinemap(nstage);
  }

  //--debug pipelining--
  //  for(i=1;i<=this.npmax;i++)if(this.is_pipe[i]) printf("%02d\n",i);
  //--debug pipelining--

  if(nbit_fix>32) nbit_penc = 6;
  if((nbit_fix<33)&&(nbit_fix>16)) nbit_penc = 5;
  if((nbit_fix<17)&&(nbit_fix>8)) nbit_penc = 4;
  if(nbit_fix<9) nbit_penc = 3;
  if((nbit_penc+nbit_man+2)>nbit_log) printf("nbit_log is too small !!!\n");
  this.npenc = nbit_penc;

  /* table instantiation  (5 steps) */
  do_malloc_for_rom();                                                     /* Step 1 */
  gen_rom((LONG)(this.ntin),(LONG)nbit_man,(LONG)ncut,(LONG)this.bit_ext); /* Step 2 */
  calc_rom_iowidth();                                                      /* Step 3 */
  calc_rom_size();                                                         /* Step 4 */
  set_romfilenames(nbit_man,ncut);                                         /* Step 5 */
  /*** generate component decleration ****/

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_conv_ftol_itp_%d_%d_%d_%d_%d\n",nbit_fix,nbit_log,nbit_man,ncut,nstage);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(fixdata : in std_logic_vector(%d downto 0);     \n",nbit_fix-1);ic++;
    sprintf((*vd).sdc[ic], "         logdata : out std_logic_vector(%d downto 0);    \n",nbit_log-1);ic++;
    sprintf((*vd).sdc[ic], "         clk : in std_logic);                                      \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;   
    (*vd).ic = ic;
  }

  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_conv_ftol_itp_%d_%d_%d_%d_%d ",(*vd).ucnt,nbit_fix,nbit_log,nbit_man,ncut,nstage);im++;
  sprintf((*vd).sdm[im], "  port map (fixdata=>%s,logdata=>%s,clk=>pclk);                 \n",sx,sy);im++;
  sprintf((*vd).sdm[im], "                                                                     \n" );im++;   
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_fix-1);is++;
  (*vd).is = is;

  /*** generate component ****/

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;   
    sprintf((*vd).sde[ie], "-- ****************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG LOGARITHMIC TO FIX-POINT CONVERTER USING INTERPOLATED ROM * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                       * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  VERSION: 2.01                                                 * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Jul 01 11:41:30 JST 2003                 * \n" );ie++;
    sprintf((*vd).sde[ie], "-- ****************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;   
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;   
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "entity pg_conv_ftol_itp_%d_%d_%d_%d_%d is\n",nbit_fix,nbit_log,nbit_man,ncut,nstage);ie++;
    sprintf((*vd).sde[ie], "  port(fixdata : in std_logic_vector(%d downto 0);      \n", nbit_fix-1);ie++;     
    sprintf((*vd).sde[ie], "       logdata : out std_logic_vector(%d downto 0);     \n", nbit_log-1);ie++;     
    sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;     
    sprintf((*vd).sde[ie], "end pg_conv_ftol_itp_%d_%d_%d_%d_%d;\n",nbit_fix,nbit_log,nbit_man,ncut,nstage);ie++;     
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "architecture rtl of pg_conv_ftol_itp_%d_%d_%d_%d_%d is \n",nbit_fix,nbit_log,nbit_man,ncut,nstage);ie++;   
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
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  component unreg_table                                            \n" );ie++;     
      sprintf((*vd).sde[ie], "  generic (IN_WIDTH: integer ;                                     \n" );ie++;     
      sprintf((*vd).sde[ie], "           OUT_WIDTH: integer ;                                    \n" );ie++;     
      sprintf((*vd).sde[ie], "           TABLE_FILE: string);                                    \n" );ie++;     
      sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;     
      sprintf((*vd).sde[ie], "      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        \n" );ie++;     
      sprintf((*vd).sde[ie], "      clk : in std_logic);                                         \n" );ie++;     
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
    }else{
      // LCELL ROM
      int c0i =(int)(this.rom_c0_ilen);
      int c0o =(int)(this.rom_c0_olen);
      int c1i =(int)(this.rom_c1_ilen);
      int c1o =(int)(this.rom_c1_olen);
      int nst; // n-satge of lcell_rom module
      if((this.is_pipe)[7]) nst=1; else nst=0;
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
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;   
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;   
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;   
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;   
    sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "  component unreg_add_sub_cout                               \n" );ie++;   
    sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                                 \n" );ie++;   
    sprintf((*vd).sde[ie], "             DIRECTION: string);                             \n" );ie++;   
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;   
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);    \n" );ie++;   
    sprintf((*vd).sde[ie], "          cout: out std_logic);                              \n" );ie++;   
    sprintf((*vd).sde[ie], "  end component;                                             \n" );ie++;   
    sprintf((*vd).sde[ie], "                                                             \n" );ie++;   
    sprintf((*vd).sde[ie], "  component reged_add_sub_cout                               \n" );ie++;   
    sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                                 \n" );ie++;   
    sprintf((*vd).sde[ie], "             DIRECTION: string;                              \n" );ie++;   
    sprintf((*vd).sde[ie], "             PIPELINE: integer);                             \n" );ie++;   
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;   
    sprintf((*vd).sde[ie], "          clock: in std_logic;                               \n" );ie++;   
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);    \n" );ie++;   
    sprintf((*vd).sde[ie], "          cout: out std_logic);                              \n" );ie++;   
    sprintf((*vd).sde[ie], "  end component;                                             \n" );ie++;   
    sprintf((*vd).sde[ie], "                                                             \n" );ie++;
    sprintf((*vd).sde[ie], "  component reged_add_sub                                    \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (WIDTH: integer;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "             DIRECTION: string;                              \n" );ie++;
    sprintf((*vd).sde[ie], "             PIPELINE: integer);                             \n" );ie++;
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));   \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                             \n" );ie++;
    sprintf((*vd).sde[ie], "                                                             \n" );ie++;
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

    if((this.is_pipe)[3]==1){
      sprintf((*vd).sde[ie], "  component reged_penc_%d_%d_1                   \n",nbit_fix-1,nbit_penc );ie++;
      sprintf((*vd).sde[ie], "    port( a : in std_logic_vector(%d downto 0);  \n",nbit_fix-2);ie++;   
      sprintf((*vd).sde[ie], "          clock : in std_logic;                  \n");ie++;
      sprintf((*vd).sde[ie], "          c : out std_logic_vector(%d downto 0));\n",nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "  end component;                                 \n");ie++;   
    }else{
      sprintf((*vd).sde[ie], "  component penc_%d_%d                           \n",nbit_fix-1,nbit_penc );ie++;
      sprintf((*vd).sde[ie], "    port( a : in std_logic_vector(%d downto 0);  \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "          c : out std_logic_vector(%d downto 0));\n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "  end component;                                 \n" );ie++;   
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    /* ---- 5-TH PIPELINE ---- */
    if((this.is_pipe)[5]==1){
      sprintf((*vd).sde[ie], "  component reged_shift_ftol_%d_%d_1\n",nbit_fix-2,this.ntin);ie++;
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);\n", nbit_fix-3 );ie++;
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n", nbit_penc-1);ie++;
      sprintf((*vd).sde[ie], "          clock : in std_logic;\n");ie++;
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0));\n",this.ntin-1);ie++;
      sprintf((*vd).sde[ie], "  end component;\n");ie++;
    }else{
      // 2003/07/01 cut next line.
      //      sprintf((*vd).sde[ie], "  component shift_ftol_round_%d_%d\n",nbit_fix-2,this.ntin );ie++;   
      sprintf((*vd).sde[ie], "  component shift_ftol_%d_%d\n",nbit_fix-2,this.ntin );ie++;   
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);\n", nbit_fix-3 );ie++;   
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0));\n",this.ntin-1);ie++;   
      sprintf((*vd).sde[ie], "  end component;\n");ie++;   
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "  signal finv,d0,d1,d2,d3,d4: std_logic_vector(%d downto 0);\n",nbit_fix-2);ie++;
    sprintf((*vd).sde[ie], "  signal d5,d6: std_logic_vector(%d downto 0);\n", nbit_fix-3 );ie++;
    sprintf((*vd).sde[ie], "  signal d7,d8: std_logic_vector(%d downto 0);\n", this.ntin-1 );ie++;
    sprintf((*vd).sde[ie], "  signal one: std_logic_vector(%d downto 0);\n", nbit_fix-2 );ie++;   

    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(i=0;i<13;i++){sprintf((*vd).sde[ie], "sign%d,",i);ie++;}
    sprintf((*vd).sde[ie], "sign13: std_logic;\n");ie++;

    sprintf((*vd).sde[ie], "  signal c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10: std_logic_vector(%d downto 0);\n",nbit_penc-1);ie++;
    sprintf((*vd).sde[ie], "  signal nz0,nz1,nz2,nz3,nz4,nz5,nz6,nz7,nz8,nz9,nz10,nz11: std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "  signal tbl_int0,tbl_int1: std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "  signal tbl_man0,tbl_man1: std_logic_vector(%d downto 0);\n",this.nman-1);ie++;
    sprintf((*vd).sde[ie], "  signal log_exp: std_logic_vector(%d downto 0);\n",this.npenc-1);ie++;
    sprintf((*vd).sde[ie], "  -- FOR INTERPOLATED TABLE\n" );ie++;
    sprintf((*vd).sde[ie], "  signal itp_x: std_logic_vector(%d downto 0);\n",this.ntin-this.ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_dx0,itp_dx1: std_logic_vector(%d downto 0);\n",this.ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c0,itp_c0d0,itp_c0d1,itp_c0d2,itp_addx: std_logic_vector(%d downto 0);\n",this.nman+1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1: std_logic_vector(%d downto 0);\n",this.nman+2);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx: std_logic_vector(%d downto 0);\n",this.nman+this.ncut+2);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx_shft0,itp_c1dx_shft1: std_logic_vector(%d downto 0);\n",this.ncut);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx2,itp_c1dx3: std_logic_vector(%d downto 0);\n",this.ncut+1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_addy,itp_addz: std_logic_vector(%d downto 0);\n",this.nman+1);ie++;

    //--- BEGIN ----
    sprintf((*vd).sde[ie], "\n" );ie++;   
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;   
    sprintf((*vd).sde[ie], "  finv <= NOT fixdata(%d downto 0);\n", nbit_fix-2 );ie++;
    {
      char _one[256];
      strcpy(_one,"");
      for(i=0;i<(nbit_fix-2);i++) strcat(_one,"0");
      strcat(_one,"1");
      sprintf((*vd).sde[ie], "  one <= \"%s\";\n", _one);ie++;
    }
    sprintf((*vd).sde[ie], "  d0 <= fixdata(%d downto 0);\n", nbit_fix-2 );ie++;
    sprintf((*vd).sde[ie], "  sign0 <= fixdata(%d);\n", nbit_fix-1 );ie++;

    /* ---- 1-TH PIPELINE ---- */
    if((this.is_pipe)[1]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 1 ---\n");ie++;
      sprintf((*vd).sde[ie], "  u1: reged_add_sub generic map (WIDTH=>%d,DIRECTION=>\"ADD\",PIPELINE=>1)\n",nbit_fix-1);ie++;
      sprintf((*vd).sde[ie], "                  port map(result=>d2,dataa=>finv,datab=>one,clock=>clk);\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign1 <= sign0;    \n");ie++;
      sprintf((*vd).sde[ie], "      d1 <= d0;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 1(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n",nbit_fix-1);ie++;
      sprintf((*vd).sde[ie], "                  port map(result=>d2,dataa=>finv,datab=>one);\n");ie++;
      sprintf((*vd).sde[ie], "  sign1 <= sign0;        \n");ie++;   
      sprintf((*vd).sde[ie], "  d1 <= d0;              \n");ie++;   
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }

    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  with sign1 select      \n");ie++;
    sprintf((*vd).sde[ie], "    d3 <= d1 when '0',   \n");ie++;
    sprintf((*vd).sde[ie], "          d2 when others;\n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 2-TH PIPELINE ---- */
    if((this.is_pipe)[2]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 2 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      d4 <= d3;          \n");ie++;
      sprintf((*vd).sde[ie], "      sign2 <= sign1;    \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 2(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  d4 <= d3;              \n");ie++;
      sprintf((*vd).sde[ie], "  sign2 <= sign1;        \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;   

    sprintf((*vd).sde[ie], "  with d4 select\n");ie++;
    {
      char _zero[256];
      strcpy(_zero,"");
      for(i=0;i<(nbit_fix-1);i++) strcat(_zero,"0");
      sprintf((*vd).sde[ie], "    nz0 <= '0' when \"%s\",\n",_zero);ie++;   
      sprintf((*vd).sde[ie], "           '1' when others;\n");ie++;   
    }
    sprintf((*vd).sde[ie], "\n");ie++;

    /* ---- 3-TH PIPELINE ---- */
    if((this.is_pipe)[3]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 3 ---\n");ie++;
      sprintf((*vd).sde[ie], "  u2: reged_penc_%d_%d_1 port map (a=>d4,clock=>clk,c=>c0);\n",nbit_fix-1,nbit_penc);ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      sign3 <= sign2;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz1 <= nz0;        \n");ie++;
      sprintf((*vd).sde[ie], "      d5 <= d4(%d downto 0);\n",nbit_fix-3);ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 3(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  u2: penc_%d_%d port map (a=>d4,c=>c0);\n",nbit_fix-1,nbit_penc);ie++;
      sprintf((*vd).sde[ie], "  sign3 <= sign2;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz1 <= nz0;            \n");ie++;
      sprintf((*vd).sde[ie], "  d5 <= d4(%d downto 0); \n",nbit_fix-3);ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++;

 

    /* ---- 4-TH PIPELINE ---- */
    if((this.is_pipe)[4]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 4 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign4 <= sign3;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz2 <= nz1;        \n");ie++;
      sprintf((*vd).sde[ie], "      c1 <= c0;          \n");ie++;
      sprintf((*vd).sde[ie], "      d6 <= d5;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 4(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign4 <= sign3;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz2 <= nz1;            \n");ie++;
      sprintf((*vd).sde[ie], "  c1 <= c0;              \n");ie++;
      sprintf((*vd).sde[ie], "  d6 <= d5;              \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }    
    sprintf((*vd).sde[ie], "\n" );ie++;


    /* ---- 5-TH PIPELINE ---- */
    if((this.is_pipe)[5]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 5 ---\n");ie++;
      sprintf((*vd).sde[ie], "  u3: reged_shift_ftol_%d_%d_1\n",nbit_fix-2,this.ntin);ie++;
      sprintf((*vd).sde[ie], "            port map (indata=>d6,control=>c1,clock=>clk,outdata=>d7);\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign5 <= sign4;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz3 <= nz2;        \n");ie++;
      sprintf((*vd).sde[ie], "      c2 <= c1;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 5(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  u3: shift_ftol_%d_%d\n",nbit_fix-2,this.ntin );ie++;
      sprintf((*vd).sde[ie], "            port map (indata=>d6,control=>c1,outdata=>d7);\n");ie++;
      sprintf((*vd).sde[ie], "      sign5 <= sign4;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz3 <= nz2;        \n");ie++;
      sprintf((*vd).sde[ie], "      c2 <= c1;          \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 6-TH PIPELINE ---- */
    if((this.is_pipe)[6]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 6 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign6 <= sign5;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz4 <= nz3;        \n");ie++;
      sprintf((*vd).sde[ie], "      c3 <= c2;          \n");ie++;
      sprintf((*vd).sde[ie], "      d8 <= d7;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 6(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign6 <= sign5;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz4 <= nz3;            \n");ie++;
      sprintf((*vd).sde[ie], "  c3 <= c2;              \n");ie++;
      sprintf((*vd).sde[ie], "  d8 <= d7;              \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n");ie++; 

    /* -----------------------------------------------------------
          PIPELINE DELAY (START)
       ----------------------------------------------------------- */
    sprintf((*vd).sde[ie], "-- PIPELINE DELAY\n");ie++;
    /* ---- 7-TH PIPELINE ---- */
    if((this.is_pipe)[7]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 7 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign7 <= sign6;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz5 <= nz4;        \n");ie++;
      sprintf((*vd).sde[ie], "      c4 <= c3;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 7(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign7 <= sign6;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz5 <= nz4;            \n");ie++;
      sprintf((*vd).sde[ie], "  c4 <= c3;              \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    /* ---- 8-TH PIPELINE ---- */
    if((this.is_pipe)[8]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 8 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign8 <= sign7;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz6 <= nz5;        \n");ie++;
      sprintf((*vd).sde[ie], "      c5 <= c4;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 8(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign8 <= sign7;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz6 <= nz5;            \n");ie++;
      sprintf((*vd).sde[ie], "  c5 <= c4;              \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    /* ---- 9-TH PIPELINE ---- */
    if((this.is_pipe)[9]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 9 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign9 <= sign8;    \n");ie++;
      sprintf((*vd).sde[ie], "      nz7 <= nz6;        \n");ie++;
      sprintf((*vd).sde[ie], "      c6 <= c5;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 9(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign9 <= sign8;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz7 <= nz6;            \n");ie++;
      sprintf((*vd).sde[ie], "  c6 <= c5;              \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    /* ---- 10-TH PIPELINE ---- */
    if((this.is_pipe)[10]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 10 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign10 <= sign9;   \n");ie++;
      sprintf((*vd).sde[ie], "      nz8 <= nz7;        \n");ie++;
      sprintf((*vd).sde[ie], "      c7 <= c6;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 10(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign10 <= sign9;        \n");ie++;
      sprintf((*vd).sde[ie], "  nz8 <= nz7;             \n");ie++;
      sprintf((*vd).sde[ie], "  c7 <= c6;               \n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    /* ---- 11-TH PIPELINE ---- */
    if((this.is_pipe)[11]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 11 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign11 <= sign10;  \n");ie++;
      sprintf((*vd).sde[ie], "      nz9 <= nz8;        \n");ie++;
      sprintf((*vd).sde[ie], "      c8 <= c7;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 11(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign11 <= sign10;       \n");ie++;
      sprintf((*vd).sde[ie], "  nz9 <= nz8;             \n");ie++;
      sprintf((*vd).sde[ie], "  c8 <= c7;               \n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    /* ---- 12-TH PIPELINE ---- */
    if((this.is_pipe)[12]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 12 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign12 <= sign11;  \n");ie++;
      sprintf((*vd).sde[ie], "      nz10 <= nz9;       \n");ie++;
      sprintf((*vd).sde[ie], "      c9 <= c8;          \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 12(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign12 <= sign11;       \n");ie++;
      sprintf((*vd).sde[ie], "  nz10 <= nz9;            \n");ie++;
      sprintf((*vd).sde[ie], "  c9 <= c8;               \n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    /* ---- 13-TH PIPELINE ---- */
    if((this.is_pipe)[13]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 13 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin     \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      sign13 <= sign12;  \n");ie++;
      sprintf((*vd).sde[ie], "      nz11 <= nz10;      \n");ie++;
      sprintf((*vd).sde[ie], "      c10 <= c9;         \n");ie++;
      sprintf((*vd).sde[ie], "    end if;              \n");ie++;
      sprintf((*vd).sde[ie], "  end process;           \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 13(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  sign13 <= sign12;       \n");ie++;
      sprintf((*vd).sde[ie], "  nz11 <= nz10;           \n");ie++;
      sprintf((*vd).sde[ie], "  c10 <= c9;              \n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    /* -----------------------------------------------------------
          PIPELINE DELAY (END)
       ----------------------------------------------------------- */


    // FOR INTERPORATION TABLE
    /* Instansiate the MIF Files */
    if((*vd).alteraesbflag){
      create_mif_c0();
      create_mif_c1();
    }
    sprintf((*vd).sde[ie], "-- TABLE FUNCTION PART (START) \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG CONV FIXED-POINT TO LOGARITHMIC MODULE OF              * \n" );ie++;
    sprintf((*vd).sde[ie], "-- * INTERPORATED TABLE LOGIC : f(x+dx) ~= c0(x) + c1(x)dx       * \n" );ie++;
    sprintf((*vd).sde[ie], "-- * The c0(x) and c1(x) are chebyshev coefficients.             * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "  itp_x   <= d8(%d downto %d);\n",this.ntin-1,ncut);ie++;
    sprintf((*vd).sde[ie], "  itp_dx0 <= d8(%d downto 0);\n",ncut-1);ie++;

    /* ---- 7-TH PIPELINE ---- */
    if((this.is_pipe)[7]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 7 ---\n");ie++;
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
      sprintf((*vd).sde[ie], "  --- PIPELINE 7(OFF) ---\n");ie++;
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


    /* ---- 8,9-TH PIPELINE ---- */
    if(((this.is_pipe)[8]==1)||((this.is_pipe)[9]==1)){
      sprintf((*vd).sde[ie], "  --- PIPELINE 8 and 9 ---\n");ie++;
      sprintf((*vd).sde[ie], "  -- ITP MULT --                   \n");ie++;
      sprintf((*vd).sde[ie], "  itp_mult: reged_mult             \n");ie++;
      sprintf((*vd).sde[ie], "    GENERIC MAP (                  \n");ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHA => %d,               \n",(int)this.rom_c1_olen);ie++;
      sprintf((*vd).sde[ie], "    IN_WIDTHB => %d,               \n",this.ncut);ie++;
      sprintf((*vd).sde[ie], "    OUT_WIDTH => %d,               \n",(int)this.rom_c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    REPRESENTATION => \"UNSIGNED\",\n");ie++;
      if(((this.is_pipe)[8]==1)&&((this.is_pipe)[9]==1)){
	sprintf((*vd).sde[ie], "    PIPELINE => 2\n");ie++;
      }else{
	sprintf((*vd).sde[ie], "    PIPELINE => 1\n");ie++;
      }
      sprintf((*vd).sde[ie], "    )                    \n");ie++;
      sprintf((*vd).sde[ie], "    PORT MAP (           \n");ie++;
      sprintf((*vd).sde[ie], "    clock  => clk,       \n");ie++;
      sprintf((*vd).sde[ie], "    dataa  => itp_c1,    \n");ie++;
      sprintf((*vd).sde[ie], "    datab  => itp_dx1,   \n");ie++;
      sprintf((*vd).sde[ie], "    result => itp_c1dx   \n");ie++;
      sprintf((*vd).sde[ie], "    );                   \n");ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      if((this.is_pipe)[8]==1){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
	sprintf((*vd).sde[ie], "      itp_c0d0 <= itp_c0;         \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      }else{
	sprintf((*vd).sde[ie], "  itp_c0d0 <= itp_c0;             \n" );ie++;
      }
      if((this.is_pipe)[9]==1){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
	sprintf((*vd).sde[ie], "      itp_c0d1 <= itp_c0d0;       \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      }else{
	sprintf((*vd).sde[ie], "  itp_c0d1 <= itp_c0d0;           \n" );ie++;
      }
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 8 and 9 (OFF)---\n");ie++;
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
      sprintf((*vd).sde[ie], "    result => itp_c1dx                 \n");ie++;
      sprintf((*vd).sde[ie], "    );                                 \n");ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c0d0 <= itp_c0;  \n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d1 <= itp_c0d0;\n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "  -- SHIFT %d-bit(DX's MANTISSA) TO LSB\n",this.ntin);ie++;
    sprintf((*vd).sde[ie], "  itp_c1dx_shft0 <= itp_c1dx(%d downto %d);\n",this.nman+this.ncut+2,this.ntin);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    /* ---- 10-TH PIPELINE ---- */
    if((this.is_pipe)[10]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 10 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                   \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then     \n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx_shft1 <= itp_c1dx_shft0;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c0d2 <= itp_c0d1;            \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                            \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                         \n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 10(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx_shft1 <= itp_c1dx_shft0;\n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d2 <= itp_c0d1;   \n" );ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    {
      char _half[256];
      strcpy(_half,"");
      if(this.ncut<this.bit_ext){
	fprintf(stderr,"Error at pg_conv_ftol_itp.\n");
	fprintf(stderr,"\t bit_ext is larger than ncut.\n");
	fprintf(stderr,"\t bit_ext %d\n",this.bit_ext);
	fprintf(stderr,"\t ncut    %d\n",this.ncut);
	exit(1);
      }
      for(i=0;i<(this.ncut+1-this.bit_ext);i++) strcat(_half,"0");
      strcat(_half,"1");
      for(i=0;i<(this.bit_ext-1);i++) strcat(_half,"0");
      sprintf((*vd).sde[ie], "  itp_round_add: unreg_add_sub_cout             \n");ie++;
      sprintf((*vd).sde[ie], "        generic map (WIDTH=>%d,DIRECTION=>\"ADD\")\n",this.ncut+1 );ie++;
      sprintf((*vd).sde[ie], "        port map(result=>itp_c1dx2(%d downto 0),\n",this.ncut );ie++;
      sprintf((*vd).sde[ie], "                 dataa=>\"%s\",                 \n",_half );ie++;
      sprintf((*vd).sde[ie], "                 datab=>itp_c1dx_shft1,         \n" );ie++;
      sprintf((*vd).sde[ie], "                 cout=>itp_c1dx2(%d));          \n",this.ncut+1);ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    /* ---- 11-TH PIPELINE ---- */
    if((this.is_pipe)[11]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 11 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx3 <= itp_c1dx2;     \n" );ie++;
      sprintf((*vd).sde[ie], "      itp_addx <= itp_c0d2;       \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 11(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx3 <= itp_c1dx2; \n" );ie++;
      sprintf((*vd).sde[ie], "  itp_addx <= itp_c0d2;   \n" );ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    {
      int nbit_zero;
      char _zero[256];
      nbit_zero = this.nman-this.ncut;
      strcpy(_zero,"");
      for(i=0;i<nbit_zero;i++) strcat(_zero,"0");
      if(nbit_zero>1){sprintf((*vd).sde[ie], "  itp_addy <= \"%s\" & itp_c1dx3;\n",_zero);ie++;}
      else if(nbit_zero==1){sprintf((*vd).sde[ie], "  itp_addy <= '0' & itp_c1dx3;\n");ie++;}
      else if(nbit_zero==0){sprintf((*vd).sde[ie], "  itp_addy <= itp_c1dx3;\n");ie++;}
      else { fprintf(stderr,"Err: Ncut = %d\n",this.ncut);exit(1);}
    }

    /* ---- 12-TH PIPELINE ---- */
    if((this.is_pipe)[12]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 12 ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_add: reged_add_sub_cout\n");ie++;
      sprintf((*vd).sde[ie], "      generic map (WIDTH=>%d,DIRECTION=>\"ADD\",PIPELINE=>1)\n", this.nman+2);ie++;
      sprintf((*vd).sde[ie], "      port map(result=>itp_addz,dataa=>itp_addx,datab=>itp_addy,clock=>clk,cout=>tbl_int0);\n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 12(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  itp_add: unreg_add_sub_cout\n");ie++;
      sprintf((*vd).sde[ie], "      generic map (WIDTH=>%d,DIRECTION=>\"ADD\")\n", this.nman+2);ie++;
      sprintf((*vd).sde[ie], "      port map(result=>itp_addz,dataa=>itp_addx,datab=>itp_addy,cout=>tbl_int0);\n");ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }

    sprintf((*vd).sde[ie], "  tbl_man0 <= itp_addz(%d downto %d);\n",this.nman+1,this.bit_ext);ie++;

    /* ---- 13-TH PIPELINE ---- */
    if((this.is_pipe)[13]==1){
      sprintf((*vd).sde[ie], "  --- PIPELINE 13 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      tbl_int1 <= tbl_int0;       \n" );ie++;
      sprintf((*vd).sde[ie], "      tbl_man1 <= tbl_man0;       \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                    \n" );ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 13(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  tbl_int1 <= tbl_int0;\n" );ie++;
      sprintf((*vd).sde[ie], "  tbl_man1 <= tbl_man0;\n" );ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "-- TABLE FUNCTION PART (END) \n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    {
      char _zero[256];
      strcpy(_zero,"");
      for(i=0;i<(this.npenc-1);i++) strcat(_zero,"0");
      sprintf((*vd).sde[ie], "  exp_add: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n",this.npenc);ie++;
      sprintf((*vd).sde[ie], "                  port map(result=>log_exp,dataa=>c10,datab=>\"%s\"&tbl_int1);\n",_zero);ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 14-TH PIPELINE ---- */
    if((this.is_pipe)[14]==1){
      char _zero[256];
      int _nzero;
      _nzero = this.nlog-this.npenc-this.nman-2;
      strcpy(_zero,"");
      for(i=0;i<_nzero;i++) strcat(_zero,"0");
      sprintf((*vd).sde[ie], "  --- PIPELINE 14 ---\n");ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                   \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then     \n");ie++;
      sprintf((*vd).sde[ie], "      logdata(%d downto 0) <= tbl_man1;\n",this.nman-1);ie++;
      sprintf((*vd).sde[ie], "      logdata(%d downto %d) <= log_exp;\n",this.npenc+this.nman-1,this.nman);ie++;
      sprintf((*vd).sde[ie], "      logdata(%d downto %d) <= \"%s\"; \n",this.nlog-3,this.npenc+this.nman,_zero);ie++;
      sprintf((*vd).sde[ie], "      logdata(%d) <= nz11;             \n",this.nlog-2);ie++;
      sprintf((*vd).sde[ie], "      logdata(%d) <= sign13;           \n",this.nlog-1);ie++;
      sprintf((*vd).sde[ie], "    end if;                            \n");ie++;
      sprintf((*vd).sde[ie], "  end process;                         \n");ie++;
      sprintf((*vd).sde[ie], "  ----------------------.\n");ie++;
    }else{
      char _zero[256];
      int _nzero;
      _nzero = this.nlog-this.npenc-this.nman-2;
      strcpy(_zero,"");
      for(i=0;i<_nzero;i++) strcat(_zero,"0");
      sprintf((*vd).sde[ie], "  --- PIPELINE 14(OFF) ---\n");ie++;
      sprintf((*vd).sde[ie], "  logdata(%d downto 0) <= tbl_man1;\n",this.nman-1);ie++;
      sprintf((*vd).sde[ie], "  logdata(%d downto %d) <= log_exp;\n",this.npenc+this.nman-1,this.nman);ie++;
      sprintf((*vd).sde[ie], "  logdata(%d downto %d) <= \"%s\"; \n",this.nlog-3,this.npenc+this.nman,_zero);ie++;
      sprintf((*vd).sde[ie], "  logdata(%d) <= nz11;             \n",this.nlog-2);ie++;
      sprintf((*vd).sde[ie], "  logdata(%d) <= sign13;           \n",this.nlog-1);ie++;
      sprintf((*vd).sde[ie], "  -----------------------.\n");ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++; 
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
    (*vd).ie = ie;

    /* --- GENERATE SUB MODULE --*/
    if((*vd).alteraesbflag){
      generate_pg_table(vd);
      generate_pg_unreg_table(vd);
    }
    generate_pg_penc(vd,nbit_fix-1,nbit_penc);
    generate_pg_reged_penc(vd,nbit_fix-1,nbit_penc,1);
    generate_pg_reged_shift_ftol(vd,nbit_fix-2,this.ntin,nbit_penc,1);
    generate_pg_shift_ftol(vd,nbit_fix-2,this.ntin,nbit_penc);
    generate_pg_unreg_mult(vd);
    generate_pg_reged_mult(vd);
    generate_reged_add_sub(vd);
    generate_reged_add_sub_cout(vd);
    generate_unreg_add_sub_cout(vd);

    // -- GENERATE LCELL ROM 
    if(((*vd).alteraesbflag)==0){  // c0 rom
      int ilen,olen,depth,np;
      char name[256];
      LONG* rom;
      ilen = this.rom_c0_ilen;
      olen = this.rom_c0_olen;
      depth = this.rom_depth; // entries of array (not rom)
      if((this.is_pipe)[7]) np = 1; else np = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c0;
      strcpy(name,this.lcromname_c0);
      generate_pg_lcell_rom(vd,ilen,olen,depth,np,name,rom);
    }
    if(((*vd).alteraesbflag)==0){  // c1 rom
      int ilen,olen,depth,np;
      char name[256];
      LONG* rom;
      ilen = this.rom_c1_ilen;
      olen = this.rom_c1_olen;
      depth = this.rom_depth;
      if((this.is_pipe)[7]) np = 1; else np = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c1;
      strcpy(name,this.lcromname_c1);
      generate_pg_lcell_rom(vd,ilen,olen,depth,np,name,rom);
    }
    // -- END GENERATE LCELL ROM
    free_rom_memory();
  }
  (*vd).alteraesbflag = is_esb;
}  



// pipelining map fixed for pg_conv_ftol_itp(fix32,log17,man8,cut5)
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
    (this.is_pipe[8]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 4){                       //---- NP : 4
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[10]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 5){                       //---- NP : 5 ,2003/12/16
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 6){                       //---- NP : 6 ,2003/12/16
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 7){                       //---- NP : 7 ,2003/12/16
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 8){                       //---- NP : 8
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 9){                       //---- NP : 9
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 10){                      //---- NP : 10
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 11){                      //---- NP : 11
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 12){                      //---- NP : 12
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
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
    (this.is_pipe[11]) = 1;
    (this.is_pipe[12]) = 1;
    (this.is_pipe[13]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == pmax){                    //---- NP : MAX(=14, Apr/30/2003)
    //    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
  }else{                                      //---- NP : 0
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  }
  return;
}
