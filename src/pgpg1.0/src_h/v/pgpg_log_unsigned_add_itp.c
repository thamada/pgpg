#include<stdio.h>
#include<math.h>
#include<stdlib.h> // malloc(),exit()
#include<string.h> // strcmp(),strcpy()
#include "pgpg.h"
#include "pgpg_submodule.h"
#include "pgpg_lcelltable_core.h"

#define LONG long long int
#define REAL long double
#define __PIPELINEMAP_AUTO__ 0

// included from pgpg.c
void itobc(int idata,char* sdata,int nbit);

static struct {
  int nlog;
  int nman;
  int ncut;
  int npipe;
  int bit_exp;
  char filename_c0[100];
  char filename_c1[100];
  char lcromname_c0[100];
  char lcromname_c1[100];
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
#include "pgpg_log_unsigned_add_itp_pipemap.h"

// This subroutine must be specified by each module,
//  and set (this.pipe_delay)[] and this.reg_reg_delay.
static void generate_critical_path_model(nlog,nman,ncut,npipe,device)
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
    (this.pipe_delay)[1] =  8.5*pow(1.3,nlog-23);   // (ns) ireg to 1-th reg
    (this.pipe_delay)[2] =  9.5*pow(1.1122,nlog-23);   // (ns) 1-th reg to 2-th reg
    (this.pipe_delay)[3] =  3.0*pow(1.135,ncut-7);   // (ns) 2-th reg to 3-th reg
    (this.pipe_delay)[4] =  6.0*pow(1.501,ncut-7); // (ns) 3-th reg to 4-th reg
    (this.pipe_delay)[5] = 11.0*pow(1.30,ncut-7); // (ns) 4-th reg to 5-th reg
    (this.pipe_delay)[6] =  4.0*pow(1.3,ncut-7); // (ns) 5-th reg to 6-th reg
    (this.pipe_delay)[7] = 10.0*pow(1.265,ncut-7);   // (ns) 6-th reg to 7-th reg
    (this.pipe_delay)[8] =  6.6*pow(1.13,nman-14);   // (ns) 7-th reg to 8-th reg
    (this.pipe_delay)[9] =  6.5*pow(1.13,nman-14);   // (ns) 8-th reg to 9-th reg
  }else{
    fprintf(stderr,"pg_log_unsigned_add_itp supports only EP20K400EFC672-2X\n");exit(1);
  }
}




/* properties for ROM table */

/******* functions for interporated table generation ***********/
static REAL func_logadd(REAL x)
{
  REAL f;
  f = logl(1.0+powl(2.0,-1.0*x))/logl(2.0);
  return f;
}

static void do_malloc_for_rom(_nlog,_nman,_ncut)
     LONG _nlog;
     LONG _nman;
     LONG _ncut;
{
  LONG i,imax;
  REAL (*_func)(REAL);
  _func = &func_logadd;
  imax = (LONG)pow(2.0,(double)(_nlog-1-_ncut));
  // calc maximum rom-size
  for(i=0;i<imax;i++){
    REAL x = ((REAL)i)/(REAL)(1<<(_nman-_ncut));
    if((LONG)((*_func)(x)*powl(2.0,(REAL)_nman)+0.5) == 0){ // Tout = 0x0
      imax = i;
      break;
    }
  }
  this.rom_c0 = (LONG*)malloc(sizeof(LONG) * imax);      /* static menber */
  this.rom_c1 = (LONG*)malloc(sizeof(LONG) * imax);      /* static menber */
  this.rom_real_c0 = (REAL*)malloc(sizeof(REAL) * imax); /* static menber */
  this.rom_real_c1 = (REAL*)malloc(sizeof(REAL) * imax); /* static menber */
  this.rom_depth = imax;                                 /* static menber */
  return ;
}

void free_rom_memory()
{
  free(this.rom_c0);
  free(this.rom_c1);
  free(this.rom_real_c0);
  free(this.rom_real_c1);
}

void calc_coefficient_chebyshev(xk_min,xk_max,coef0,coef1)
     REAL xk_min;
     REAL xk_max;
     REAL* coef0;
     REAL* coef1;
{
  REAL x0,x1,bma,bpa,cc0,cc1;
  REAL f[2];
  REAL (*_func)(REAL);
  _func = &func_logadd;
  x0 = -0.5*sqrtl(2.0);
  x1 =  0.5*sqrtl(2.0);
  bma=0.5*(xk_max-xk_min);
  bpa=0.5*(xk_max+xk_min);
  f[0] = (*_func)(x0*bma+bpa);
  f[1] = (*_func)(x1*bma+bpa);
  cc0 = 0.5*(f[0]+f[1]);
  cc1 = 0.5*sqrtl(2.0)*(f[1]-f[0]);
  (*coef0) = cc0 - cc1;
  (*coef1) = (-1.0)*cc1/bma;
}

void gen_rom(_nman,_ncut,_bit_exp)
     LONG _nman;
     LONG _ncut;
     LONG _bit_exp;
{
  LONG _bit_exp_c0,_bit_exp_c1,i,imax;
  _bit_exp_c0 = _bit_exp;
  _bit_exp_c1 = _bit_exp;

  imax = this.rom_depth; /* using static member */

  for(i=0;i<imax;i++){
    REAL xmin = (REAL)(i<<_ncut);
    REAL xmax = (REAL)(((i+0x1LL)<<_ncut)-0x1LL);

    xmin = (xmin+0.25) / (REAL)(0x1LL<<_nman);  // bug-fixed 2003/03/11
    xmax = (xmax+0.25) / (REAL)(0x1LL<<_nman);  // bug-fixed 2003/03/11
    {
      REAL real_c0,real_c1;
      calc_coefficient_chebyshev(xmin,xmax,&real_c0,&real_c1);
      (this.rom_real_c0)[i] = real_c0;                                   /* static member */ 
      (this.rom_real_c1)[i] = real_c1;                                   /* static member */ 
      (this.rom_c0)[i] = (LONG)(real_c0*powl(2.0,_nman+_bit_exp_c0)+0.5); /* static member */ 
      (this.rom_c1)[i] = (LONG)(real_c1*powl(2.0,_nman+_bit_exp_c1)+0.5); /* static member */ 
    }
  }
  return;
}


LONG calc_bitlength(input)
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


void calc_rom_iowidth()
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
    // --- this is future suppot. ---
    //      if(this.rom_c0[i] == 0x0LL)
    //        if(is_c0==0){c0_ilen = this->calc_bitlength(i-1); is_c0=0x1;}
    //      if(this.rom_c1[i] == 0x0LL)
    //        if(is_c1==0){c1_ilen = this->calc_bitlength(i-1); is_c1=0x1;}
  }
  this.rom_c0_ilen = c0_ilen; // static member
  this.rom_c1_ilen = c1_ilen; // static member
  this.rom_c0_olen = c0_olen; // static member
  this.rom_c1_olen = c1_olen; // static member
  return;
}


void calc_rom_size()
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

void set_romfilenames(nman,ncut)
     int nman;
     int ncut;
{
  /* File name must be 4 words(16-bit) as follows.
     fixed(1-bit) | Table Type id(4-bit) | Object information(11-bit)|   
     | Nman(6-bit) | Ncut(5-bit) | 
  */
  /*  Table Type                     ,   Table Type id */
  /*  pg_conv_ftol                   ,   0000   */
  /*  pg_conv_ltof                   ,   0001   */
  /*  --- reserved  ---              ,   0010   */
  /*  pg_log_unsigned_add            ,   0011   */
  /*  pg_log_unsigned_add_itp(c0)    ,   0100   */
  /*  pg_log_unsigned_add_itp(c1)    ,   0101   */
  /*  pg_fix_to_log_itp(c0)          ,   0110   */
  /*  pg_fix_to_log_itp(c1)          ,   0111   */
  /*  pg_log_to_fix_itp(c0)          ,   1000   */
  /*  pg_log_to_fix_itp(c1)          ,   1001   */
  /*  pg_log_add(+)                  ,   1010   */
  /*  pg_log_add(-)                  ,   1011   */
  /*  pg_log_add_itp(+,c0)           ,   1100   */
  /*  pg_log_add_itp(+,c1)           ,   1101   */
  /*  pg_log_add_itp(-,c0)           ,   1110   */
  /*  pg_log_add_itp(-,c1)           ,   1111   */
  int fname_vec;

  fname_vec =  (0x1<<15)| (0x4<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
  sprintf(this.filename_c0,"%04x.mif",fname_vec);
  sprintf(this.lcromname_c0,"%04x",fname_vec);

  fname_vec =  (0x1<<15)| (0x5<<11) | ((0x3f&this.nman)<<5) | (0x1f&this.ncut);
  sprintf(this.filename_c1,"%04x.mif",fname_vec);
  sprintf(this.lcromname_c1,"%04x",fname_vec);

  return;
}


/******* functions for interporated table generation (END)***********/

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
 *  MINUS   C        C
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


/**************************************************************************** this code is moved to pgpg.c  
static void generate_reged_add_sub(vd)
     struct vhdl_description *vd;
{
  int i,ie,j;
  int iflag;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "ENTITY reged_add_sub IS\n");ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  GENERIC(WIDTH : INTEGER;DIRECTION: STRING;PIPELINE: INTEGER);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));\n" );ie++;
    sprintf((*vd).sde[ie], "END reged_add_sub;\n");ie++;
    sprintf((*vd).sde[ie], "ARCHITECTURE rtl OF reged_add_sub IS\n");ie++;
    sprintf((*vd).sde[ie], "  COMPONENT lpm_add_sub\n" );ie++;
    sprintf((*vd).sde[ie], "    GENERIC (LPM_WIDTH: INTEGER;    \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_PIPELINE: INTEGER; \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));\n");ie++;
    sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
    sprintf((*vd).sde[ie], "BEGIN\n");ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub GENERIC MAP(");ie++;
    sprintf((*vd).sde[ie], "LPM_WIDTH=>WIDTH,LPM_DIRECTION=>DIRECTION,LPM_PIPELINE=>PIPELINE) ");ie++;
    sprintf((*vd).sde[ie], "PORT MAP(dataa=>dataa,datab=>datab,result=>result,clock=>clock);\n" );ie++;
    sprintf((*vd).sde[ie], "END rtl;\n\n");ie++;
    (*vd).ie = ie;
  }
}
********************************************************************************************************/

void generate_pg_log_unsigned_add_itp(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
  int j;
  int nbit_log,nbit_man,nstage,nbit_tin;
  int c0_ilen,c1_ilen,c0_olen,c1_olen,ncut;
  char sx[STRLEN],sy[STRLEN],sz[STRLEN];
  //  static char sd[1000][STRLEN];
  int ic,im,ie,is;
  int is_esb;
  int iflag=1;
  strcpy(sx,sdata[1]);
  strcpy(sy,sdata[2]);
  strcpy(sz,sdata[3]);
  nbit_log = atoi(sdata[4]);
  nbit_man = atoi(sdata[5]);
  nstage = atoi(sdata[6]);
  ncut   = atoi(sdata[7]);

  is_esb = (*vd).alteraesbflag;

  /* NEED ERROR CODE like "if(ncut<3){..}" */
  if((ncut<2)||(ncut>=nbit_man)){
    fprintf(stderr,"Error at pg_log_unsigned_add_itp.\n");
    fprintf(stderr,"\t ncut must be [2,%d).\n",nbit_man);
    fprintf(stderr,"\t But ncut is %d\n",ncut);
    exit(0);
  }

  if(nstage<1){
    nstage = 1;
    printf("nstage: fixed to %d\n",nstage);
  }
  if(nstage>10){
    nstage = 9;
    printf("nstage: fixed to %d\n",nstage);
  }

  printf("log unsigned add(itp) : %s + %s = %s : ",sx,sy,sz);
  printf("nbit log %d man %d nstage %d ncut %d\n",nbit_log,nbit_man,nstage,ncut);

  /* SET STATIC PARAMETER */
  this.nlog = nbit_log;
  this.nman = nbit_man;
  this.ncut = ncut;
  this.npipe = nstage;
  this.bit_exp = 2; /* Fixed */
  this.npmax = 9;

  

  // --- pipeline map ---
  {
    int is_stratix=0;
    char device[256];
    strcpy(device,(*vd).device);
    if(strcmp(device,"stratix")==0)       is_stratix=1;
    else if(strcmp(device,"cyclone")==0)  is_stratix=1;
    else if(strcmp(device,"apex20k")==0)  is_stratix=0;
    else if(strcmp(device,"apex20ke")==0) is_stratix=0;
    generate_critical_path_model(nbit_log,nbit_man,ncut,nstage,"EP20K400EFC672-2X");
    if(__PIPELINEMAP_AUTO__ == 0){
      if(!(is_esb)){  // --- LCELL  ---
	if((nbit_log==17)&&(nbit_man==8)&&(ncut==6)){  generate_fixed_pipelinemap_stratix_lc_log17_man_8_cut6(&nstage);}
	else{ generate_fixed_pipelinemap_stratix_lc_log17_man_8_cut6(&nstage); }   // Dangarous !!
      }else{          // --- ESB ---
	if(is_stratix){
	  if((nbit_log==17)&&(nbit_man==8)&&(ncut==6)){ generate_fixed_pipelinemap_stratix_nolc_log17_man_8_cut6(&nstage);}
	  else{ generate_fixed_pipelinemap_stratix_nolc_log17_man_8_cut6(&nstage); }
	}else{
	  generate_fixed_pipelinemap_stratix_lc_log17_man_8_cut6(&nstage);         // Dangarous !!
	}
      }
    }else if(__PIPELINEMAP_AUTO__ == 1){
      generate_pipelinemap(nstage);        // Dangarous !!
    }else{
      generate_fixed_pipelinemap(nstage);  // Dangarous !!
    }
  }



  //--debug pipelining--
  //  {int i;for(i=1;i<=this.npmax;i++)if(this.is_pipe[i]) printf("%02d\n",i);}
  //--debug pipelining--

  /* table initialization  (5 steps) */
  do_malloc_for_rom((LONG)nbit_log,(LONG)nbit_man,(LONG)ncut); /* Step 1 */
  gen_rom((LONG)nbit_man,(LONG)ncut,(LONG)this.bit_exp);       /* Step 2 */
  calc_rom_iowidth();                                          /* Step 3 */
  calc_rom_size();                                             /* Step 4 */
  set_romfilenames(nbit_man,ncut);                             /* Step 5 */



  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_log_unsigned_add_itp_%d_%d_%d_%d\n", nbit_log, nbit_man,ncut,nstage );ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(%d downto 0);\n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0);\n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          clock : in std_logic);\n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;\n" );ic++;
    sprintf((*vd).sdc[ic], "\n");ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_log_unsigned_add_itp_%d_%d_%d_%d\n",(*vd).ucnt,nbit_log,nbit_man,ncut,nstage);im++;
  sprintf((*vd).sdm[im], "            port map(x=>%s,y=>%s,z=>%s,clock=>pclk);\n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "\n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbit_log-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sy,nbit_log-1);is++;
  (*vd).is = is;

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG UNSIGNED LOGARITHMIC ADDER(INTERPOLATED) MODULE        * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  AUTHOR: Tsuyoshi Hamada                                    * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  VERSION: 1.03                                              * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  LAST MODIFIED AT Tue Mar 25 10:52:09 JST 2003              * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- Nman\t%d-bit\n",this.nman );ie++;
    sprintf((*vd).sde[ie], "-- Ncut\t%d-bit\n",this.ncut );ie++;
    sprintf((*vd).sde[ie], "-- Npipe\t%d\n",this.npipe );ie++;
    sprintf((*vd).sde[ie], "-- rom_c0(address)\t%d-bit\n",(int)this.rom_c0_ilen);ie++;
    sprintf((*vd).sde[ie], "-- rom_c1(address)\t%d-bit\n",(int)this.rom_c1_ilen);ie++;
    sprintf((*vd).sde[ie], "-- rom_c0(data)\t%d-bit\n",(int)this.rom_c0_olen);ie++;
    sprintf((*vd).sde[ie], "-- rom_c1(data)\t%d-bit\n\n",(int)this.rom_c1_olen);ie++;

    sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "entity pg_log_unsigned_add_itp_%d_%d_%d_%d is\n", nbit_log, nbit_man,ncut,nstage );ie++;
    sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(%d downto 0);\n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0);\n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "        clock : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "end pg_log_unsigned_add_itp_%d_%d_%d_%d;\n", nbit_log, nbit_man,ncut,nstage );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    sprintf((*vd).sde[ie], "architecture rtl of pg_log_unsigned_add_itp_%d_%d_%d_%d is\n", nbit_log, nbit_man,ncut,nstage );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    if((this.is_pipe)[4]||(this.is_pipe)[5]){
      sprintf((*vd).sde[ie], "  COMPONENT lpm_mult\n" );ie++;
      sprintf((*vd).sde[ie], "    GENERIC (\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widtha            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widthb            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widthp            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widths            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_type              : STRING;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_representation    : STRING;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_pipeline          : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_hint              : STRING\n" );ie++;
      sprintf((*vd).sde[ie], "    );\n" );ie++;
      sprintf((*vd).sde[ie], "    PORT (\n" );ie++;
      sprintf((*vd).sde[ie], "      clock : IN STD_LOGIC; \n" );ie++;
      sprintf((*vd).sde[ie], "      dataa : IN STD_LOGIC_VECTOR (lpm_widtha-1 DOWNTO 0); \n" );ie++;
      sprintf((*vd).sde[ie], "      datab : IN STD_LOGIC_VECTOR (lpm_widthb-1 DOWNTO 0); \n" );ie++;
      sprintf((*vd).sde[ie], "      result : OUT STD_LOGIC_VECTOR (lpm_widthp-1 DOWNTO 0)\n" );ie++;
      sprintf((*vd).sde[ie], "    );\n" );ie++;
      sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  COMPONENT lpm_mult\n" );ie++;
      sprintf((*vd).sde[ie], "    GENERIC (\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widtha            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widthb            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widthp            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_widths            : INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_type              : STRING;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_representation    : STRING;\n" );ie++;
      sprintf((*vd).sde[ie], "      lpm_hint              : STRING\n" );ie++;
      sprintf((*vd).sde[ie], "    );\n" );ie++;
      sprintf((*vd).sde[ie], "    PORT (\n" );ie++;
      sprintf((*vd).sde[ie], "      dataa : IN STD_LOGIC_VECTOR (lpm_widtha-1 DOWNTO 0); \n" );ie++;
      sprintf((*vd).sde[ie], "      datab : IN STD_LOGIC_VECTOR (lpm_widthb-1 DOWNTO 0); \n" );ie++;
      sprintf((*vd).sde[ie], "      result : OUT STD_LOGIC_VECTOR (lpm_widthp-1 DOWNTO 0)\n" );ie++;
      sprintf((*vd).sde[ie], "    );\n" );ie++;
      sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }
    if((this.is_pipe)[1]||(this.is_pipe)[8]){
      sprintf((*vd).sde[ie], "  COMPONENT reged_add_sub\n" );ie++;
      sprintf((*vd).sde[ie], "    GENERIC (WIDTH: INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "             PIPELINE: INTEGER;\n" );ie++;
      sprintf((*vd).sde[ie], "             DIRECTION: STRING);\n" );ie++;
      sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
      sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));\n" );ie++;
      sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    sprintf((*vd).sde[ie], "  COMPONENT lpm_add_sub\n" );ie++;
    sprintf((*vd).sde[ie], "    GENERIC (LPM_WIDTH: INTEGER;\n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: STRING);\n" );ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));\n" );ie++;
    sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;


    if((this.is_pipe)[3]){
      if(is_esb){
	// ESB ROM
	sprintf((*vd).sde[ie], "  component lpm_rom                                                \n" );ie++;
	sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                                  \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                                \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_ADDRESS_CONTROL: STRING;                          \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_FILE: STRING);                                    \n" );ie++;
	sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);     \n" );ie++;
	sprintf((*vd).sde[ie], "         outclock: in std_logic;                                   \n" );ie++;
	sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));           \n" );ie++;
	sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      }else{
	// LCELL ROM
	int c0i =(int)(this.rom_c0_ilen);
	int c0o =(int)(this.rom_c0_olen);
	int c1i =(int)(this.rom_c1_ilen);
	int c1o =(int)(this.rom_c1_olen);
	sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_1                      \n",this.lcromname_c0,c0i,c0o);ie++;
	sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c0i-1);ie++;
	sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
	sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c0o-1);ie++;
	sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
	sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_1                      \n",this.lcromname_c1,c1i,c1o);ie++;
	sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c1i-1);ie++;
	sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
	sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c1o-1);ie++;
	sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }
    }else{
      if(is_esb){
	// ESB ROM
	sprintf((*vd).sde[ie], "  -- UNREGISTERED TABLE --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  component lpm_rom                                                \n" );ie++;
	sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                                  \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                                \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_ADDRESS_CONTROL: STRING;                          \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_OUTDATA: STRING;                                  \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_FILE: STRING);                                    \n" );ie++;
	sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);     \n" );ie++;
	sprintf((*vd).sde[ie], "  --     outclock: in std_logic;                                   \n" );ie++;
	sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));           \n" );ie++;
	sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;    
      }else{
	// LCELL ROM
	int c0i =(int)(this.rom_c0_ilen);
	int c0o =(int)(this.rom_c0_olen);
	int c1i =(int)(this.rom_c1_ilen);
	int c1o =(int)(this.rom_c1_olen);
	sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_0                      \n",this.lcromname_c0,c0i,c0o);ie++;
	sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c0i-1);ie++;
	sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
	sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c0o-1);ie++;
	sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
	sprintf((*vd).sde[ie], "  component lcell_rom_%s_%d_%d_0                      \n",this.lcromname_c1,c1i,c1o);ie++;
	sprintf((*vd).sde[ie], "   port (indata: in std_logic_vector(%d downto 0);    \n",c1i-1);ie++;
	sprintf((*vd).sde[ie], "         clk: in std_logic;                           \n" );ie++;
	sprintf((*vd).sde[ie], "         outdata: out std_logic_vector(%d downto 0)); \n",c1o-1);ie++;
	sprintf((*vd).sde[ie], "  end component;                                      \n" );ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }
    }
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "  signal x1,y1,xy : std_logic_vector(%d downto 0);     \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "  signal yx : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal xd,yd : std_logic_vector(%d downto 0);        \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal x2,x3,x4,x5,x6,x7,x8 : std_logic_vector(%d downto 0);\n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal d0,d1,d4 : std_logic_vector(%d downto 0);     \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal z0 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal sign0,sign1,sign2,sign3,sign4,sign5,sign6,sign7,sign8 : std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "  signal signxy : std_logic_vector(1 downto 0);        \n" );ie++;

    // FOR INTERPORATION
    /* Instansiate the Memory Initialize Files */
    if(is_esb){
      create_mif_c0();
      create_mif_c1();
    }
    nbit_tin     = (int)(this.rom_c0_ilen)+ncut; //(=this.rom_c1_ilen+ncut)
    c0_ilen   = (int)(this.rom_c0_ilen);
    c1_ilen  = (int)(this.rom_c1_ilen);
    c0_olen  = (int)(this.rom_c0_olen); //(=nbit_man+2[bit_exp])
    c1_olen = (int)(this.rom_c1_olen); //(=nbit_man+2[bit_exp])


    sprintf((*vd).sde[ie], "  -- FOR TABLE SUB LOGIC\n" );ie++;
    sprintf((*vd).sde[ie], "  signal df0,df1,df2,df3,df4,df5 : std_logic;                  \n" );ie++;
    sprintf((*vd).sde[ie], "  signal d_isz0,d_isz1,d_isz2,d_isz3,d_isz4,d_isz5 : std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "  signal d2 : std_logic_vector(%d downto 0);           \n", nbit_man );ie++;
    sprintf((*vd).sde[ie], "  signal itp_x  : std_logic_vector(%d downto 0);\n",nbit_tin -ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_dx0,itp_dx1 : std_logic_vector(%d downto 0);\n",ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c0,itp_c0d0,itp_c0d1,itp_c0d2 : std_logic_vector(%d downto 0);\n",c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1 : std_logic_vector(%d downto 0);\n",c1_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx : std_logic_vector(%d downto 0);\n",c1_olen+ncut-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx_shift : std_logic_vector(%d downto 0);\n",c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c1dx2: std_logic_vector(%d downto 0);\n",c0_olen-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_subx,itp_suby,itp_subz: std_logic_vector(%d downto 0);\n"
	    ,c0_olen);ie++;
    sprintf((*vd).sde[ie], "  signal itp_c0_c1dx: std_logic_vector(%d downto 0);\n",nbit_man-1);ie++;
    sprintf((*vd).sde[ie], "  signal itp_out0,itp_out1: std_logic_vector(%d downto 0);\n",nbit_man-1);ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;


    sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  x1 <= '0' & x(%d downto 0);                          \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  y1 <= '0' & y(%d downto 0);                          \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    /* ---- 1-TH PIPELINE ---- */
    if((this.is_pipe)[1]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 1 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  u1: reged_add_sub \n");ie++;
      sprintf((*vd).sde[ie], "       generic map(WIDTH=>%d,DIRECTION=>\"SUB\",PIPELINE=>1)\n",nbit_log);ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy,clock=>clock);\n" );ie++;
      sprintf((*vd).sde[ie], "  u2: reged_add_sub \n");ie++;
      sprintf((*vd).sde[ie], "       generic map(WIDTH=>%d,DIRECTION=>\"SUB\",PIPELINE=>1)\n",nbit_log-1);ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>y(%d downto 0),datab=>x(%d downto 0),result=>yx,clock=>clock);\n",
	      nbit_log-2,nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin                \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      xd <= x(%d downto 0);           \n",nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "      yd <= y(%d downto 0);           \n",nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "      sign1 <= sign0;                \n" );ie++;     
      sprintf((*vd).sde[ie], "    end if;                           \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                        \n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 1(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy);                   \n" );ie++;
      sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>y(%d downto 0),datab=>x(%d downto 0),result=>yx);\n",
	      nbit_log-2,nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "  xd <= x(%d downto 0);\n",nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "  yd <= y(%d downto 0);\n",nbit_log-2);ie++;
      sprintf((*vd).sde[ie], "  sign1 <= sign0;\n" );ie++;    
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }      
    sprintf((*vd).sde[ie], "\n\n" );ie++;


    sprintf((*vd).sde[ie], "  x2 <= xd when xy(%d)='0' else yd;\n",nbit_log-1);ie++;
    sprintf((*vd).sde[ie], "  d0 <= xy(%d downto 0) when xy(%d)='0' else yx;\n",nbit_log-2, nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  signxy <= x(%d)&y(%d);                    \n", nbit_log-1,nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "  with signxy select                                               \n" );ie++;    
    sprintf((*vd).sde[ie], "    sign0 <= y(%d) when \"01\",                        \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "             x(%d) when others;                        \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;


    /* ---- 2-TH PIPELINE ---- */
    if((this.is_pipe)[2]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 2 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
      sprintf((*vd).sde[ie], "      x3 <= x2;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      d1 <= d0;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      sign2 <= sign1;                                             \n" );ie++;     
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 2(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x3 <= x2;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  d1 <= d0;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  sign2 <= sign1;                                                \n" );ie++;      
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }      
    sprintf((*vd).sde[ie], "\n\n" );ie++;


    sprintf((*vd).sde[ie], "-- TABLE PART (START) ---------------------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "-- INPUT  d1 : %d-bit\n",nbit_log-1);ie++;
    sprintf((*vd).sde[ie], "-- OUTPUT d4 : %d-bit\n",nbit_log-1);ie++;
    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_log-1-nbit_tin);
      sprintf((*vd).sde[ie], "  df0 <= '1' when d1(%d downto %d)=\"%s\" else '0';\n", nbit_log-2, nbit_tin,_zero);ie++;
    }
    /* ALL OR -> NOT (PLUS LOWER BITS) */
    sprintf((*vd).sde[ie], "  \n");ie++;
    sprintf((*vd).sde[ie], "  -- ALL OR -> NOT (PLUS) --\n");ie++;
    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_tin);
      sprintf((*vd).sde[ie], "  d_isz0 <= '1' when d1(%d downto 0)=\"%s\" else '0';\n",nbit_tin-1, _zero );ie++;
    }

    /***********************/
    /* TABLE OBJECT        */
    /***********************/
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "-- TABLE (INTERPOLATION) --\n");ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "-- * PGPG UNSIGNED LOGARITHMIC ADDER MODULE OF                   * \n" );ie++;
    sprintf((*vd).sde[ie], "-- * INTERPORATED TABLE LOGIC : f(x+dx) ~= c0(x) + c1(x)dx       * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *  c0(x) and c1(x) are chebyshev coefficients.                * \n" );ie++;
    sprintf((*vd).sde[ie], "-- *************************************************************** \n" );ie++;
    sprintf((*vd).sde[ie], "  itp_x   <= d1(%d downto %d);\n",nbit_tin-1,ncut);ie++;
    sprintf((*vd).sde[ie], "  itp_dx0 <= d1(%d downto 0);\n",ncut-1);ie++;
    sprintf((*vd).sde[ie], "\n");ie++;

    /* ---- 3-TH PIPELINE ---- */
    if((this.is_pipe)[3]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 3 ---\n" );ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  -- OUT REGISTERED TABLE --                          \n" );ie++;
      if(is_esb){
	// -- ESB ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  -- MEMORY SIZE : (2^%d)*%d bits\n",c0_ilen,c0_olen );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lpm_rom generic map (LPM_WIDTH=>%d,      \n",c0_olen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,           \n",c0_ilen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_FILE=>\"%s\")     \n",this.filename_c0);ie++;
	sprintf((*vd).sde[ie], "  port map(address=>itp_x,q=>itp_c0,outclock=>clock);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  -- MEMORY SIZE : (2^%d)*%d bits\n",c1_ilen,c1_olen );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lpm_rom generic map (LPM_WIDTH=>%d,     \n",c1_olen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,           \n",c1_ilen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_FILE=>\"%s\")     \n",this.filename_c1);ie++;
	sprintf((*vd).sde[ie], "  port map(address=>itp_x,q=>itp_c1,outclock=>clock);\n");ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }else{
	// -- LCELL ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lcell_rom_%s_%d_%d_1\n",this.lcromname_c0,c0_ilen,c0_olen);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clock);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lcell_rom_%s_%d_%d_1\n",this.lcromname_c1,c1_ilen,c1_olen);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clock);\n\n");ie++;
      }
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      df1 <= df0;\n" );ie++;
      sprintf((*vd).sde[ie], "      d_isz1 <= d_isz0;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_dx1 <= itp_dx0;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 3(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
      sprintf((*vd).sde[ie], "  -- UNREGISTERED TABLE --                            \n" );ie++;
      if(is_esb){
	// -- ESB ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  -- MEMORY SIZE : (2^%d)*%d bits\n",c0_ilen,c0_olen );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lpm_rom generic map (LPM_WIDTH=>%d,      \n",c0_olen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,           \n",c0_ilen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_OUTDATA=>\"UNREGISTERED\",         \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_FILE=>\"%s\")     \n",this.filename_c0);ie++;
	sprintf((*vd).sde[ie], "  port map(address=>itp_x,q=>itp_c0);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  -- MEMORY SIZE : (2^%d)*%d bits\n",c1_ilen,c1_olen );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lpm_rom generic map (LPM_WIDTH=>%d,     \n",c1_olen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>%d,           \n",c1_ilen);ie++;
	sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\", \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_OUTDATA=>\"UNREGISTERED\",         \n");ie++;
	sprintf((*vd).sde[ie], "                           LPM_FILE=>\"%s\")     \n",this.filename_c1);ie++;
	sprintf((*vd).sde[ie], "  port map(address=>itp_x,q=>itp_c1);\n");ie++;
      }else{
	// -- LCELL ROM ---
	sprintf((*vd).sde[ie], "  -- c0(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c0_rom: lcell_rom_%s_%d_%d_0\n",this.lcromname_c0,c0_ilen,c0_olen);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c0,clk=>clock);\n\n");ie++;
	sprintf((*vd).sde[ie], "  -- c1(x) --                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  itp_c1_rom: lcell_rom_%s_%d_%d_0\n",this.lcromname_c1,c1_ilen,c1_olen);ie++;
	sprintf((*vd).sde[ie], "  port map(indata=>itp_x,outdata=>itp_c1,clk=>clock);\n\n");ie++;
      }
      sprintf((*vd).sde[ie], "\n" );ie++;
      sprintf((*vd).sde[ie], "  df1 <= df0;\n" );ie++;
      sprintf((*vd).sde[ie], "  d_isz1 <= d_isz0;\n" );ie++;
      sprintf((*vd).sde[ie], "  itp_dx1 <= itp_dx0;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n\n");ie++;


    /* ---- 4,5-TH PIPELINE ---- */
    if((this.is_pipe)[4]||(this.is_pipe)[5]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 4,5 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  -- ITP MULT --  %d-bit * %d-bit -> %d-bit\n",c1_olen,ncut,c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "  itp_mult: lpm_mult                                      \n");ie++;
      sprintf((*vd).sde[ie], "    GENERIC MAP (                                         \n");ie++;
      sprintf((*vd).sde[ie], "    lpm_widtha => %d,                        \n",c1_olen);ie++;
      sprintf((*vd).sde[ie], "    lpm_widthb => %d,                        \n",ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_widthp => %d,                        \n",c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_widths => %d,                        \n",c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_type => \"LPM_MULT\",                \n");ie++;
      sprintf((*vd).sde[ie], "    lpm_representation => \"UNSIGNED\",      \n");ie++;
      {
	int _np=0;
	if((this.is_pipe)[4]) _np++;
	if((this.is_pipe)[5]) _np++;
	sprintf((*vd).sde[ie], "    lpm_pipeline => %d,\n",_np);ie++;
      }
      sprintf((*vd).sde[ie], "    lpm_hint => \"MAXIMIZE_SPEED=1\"         \n");ie++;
      sprintf((*vd).sde[ie], "    )                                        \n");ie++;
      sprintf((*vd).sde[ie], "    PORT MAP (                               \n");ie++;
      sprintf((*vd).sde[ie], "    clock  => clock,                        \n");ie++;
      sprintf((*vd).sde[ie], "    dataa  => itp_c1,                        \n");ie++;
      sprintf((*vd).sde[ie], "    datab  => itp_dx1,                       \n");ie++;
      sprintf((*vd).sde[ie], "    result => itp_c1dx                       \n");ie++;
      sprintf((*vd).sde[ie], "    );                                       \n");ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 4,5 (OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  -- ITP MULT --  %d-bit * %d-bit -> %d-bit\n",c1_olen,ncut,c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "  itp_mult: lpm_mult                                      \n");ie++;
      sprintf((*vd).sde[ie], "    GENERIC MAP (                                         \n");ie++;
      sprintf((*vd).sde[ie], "    lpm_widtha => %d,                        \n",c1_olen);ie++;
      sprintf((*vd).sde[ie], "    lpm_widthb => %d,                        \n",ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_widthp => %d,                        \n",c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_widths => %d,                        \n",c1_olen+ncut);ie++;
      sprintf((*vd).sde[ie], "    lpm_type => \"LPM_MULT\",                \n");ie++;
      sprintf((*vd).sde[ie], "    lpm_representation => \"UNSIGNED\",      \n");ie++;
      sprintf((*vd).sde[ie], "    lpm_hint => \"MAXIMIZE_SPEED=1\"         \n");ie++;
      sprintf((*vd).sde[ie], "    )                                        \n");ie++;
      sprintf((*vd).sde[ie], "    PORT MAP (                               \n");ie++;
      sprintf((*vd).sde[ie], "    dataa  => itp_c1,                        \n");ie++;
      sprintf((*vd).sde[ie], "    datab  => itp_dx1,                       \n");ie++;
      sprintf((*vd).sde[ie], "    result => itp_c1dx                       \n");ie++;
      sprintf((*vd).sde[ie], "    );                                       \n");ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }

    /* ---- 4-TH PIPELINE ---- */
    if((this.is_pipe)[4]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 4 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      df2 <= df1;\n" );ie++;
      sprintf((*vd).sde[ie], "      d_isz2 <= d_isz1;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c0d0 <= itp_c0;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 4(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  df2 <= df1;\n" );ie++;
      sprintf((*vd).sde[ie], "  d_isz2 <= d_isz1;\n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d0 <= itp_c0;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    /* ---- 5-TH PIPELINE ---- */
    if((this.is_pipe)[5]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 5 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      df3 <= df2;\n" );ie++;
      sprintf((*vd).sde[ie], "      d_isz3 <= d_isz2;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c0d1 <= itp_c0d0;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 5(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  df3 <= df2;\n" );ie++;
      sprintf((*vd).sde[ie], "  d_isz3 <= d_isz2;\n" );ie++;
      sprintf((*vd).sde[ie], "  itp_c0d1 <= itp_c0d0;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n\n" );ie++;


    {
      char _zero[STRLEN];
      itobc(0,_zero,c0_olen-(c1_olen+ncut-nbit_man));
      sprintf((*vd).sde[ie], "  -- SHIFT >> %d-bit , JOINE ZERO-VECTORS TO THE UPPER-BIT\n",nbit_man);ie++;
      sprintf((*vd).sde[ie], "  itp_c1dx_shift <= \"%s\" & itp_c1dx(%d downto %d);\n",_zero,
	      c1_olen+ncut-1,nbit_man);ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 6-TH PIPELINE ---- */
    if((this.is_pipe)[6]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 6 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      df4 <= df3;\n" );ie++;
      sprintf((*vd).sde[ie], "      d_isz4 <= d_isz3;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c0d2 <= itp_c0d1;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx2 <= itp_c1dx_shift;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 6(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "      df4 <= df3;\n" );ie++;
      sprintf((*vd).sde[ie], "      d_isz4 <= d_isz3;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c0d2 <= itp_c0d1;\n" );ie++;
      sprintf((*vd).sde[ie], "      itp_c1dx2 <= itp_c1dx_shift;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n\n" );ie++;

    sprintf((*vd).sde[ie], "  itp_subx <= '0' & itp_c0d2;\n");ie++;
    sprintf((*vd).sde[ie], "  itp_suby <= '0' & itp_c1dx2;\n");ie++;
    sprintf((*vd).sde[ie], "  itp_sub: lpm_add_sub \n");ie++;
    sprintf((*vd).sde[ie], "    generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", c0_olen+1 );ie++;
    sprintf((*vd).sde[ie], "    port map(dataa=>itp_subx,datab=>itp_suby,result=>itp_subz);\n\n");ie++;
    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_man);
      sprintf((*vd).sde[ie], "  -- IF [f(x+dx)=c0(x)-c1(x)dx<0] THEN [f(x+dx) := 0] ELSE SHIFT >> %d-bit\n"
	      ,this.bit_exp);ie++;
      sprintf((*vd).sde[ie], "  itp_c0_c1dx <= \"%s\" when (itp_subz(%d)='1') else itp_subz(%d downto %d);\n"
	      ,_zero, c0_olen, c0_olen-1,this.bit_exp);ie++;
    }
    sprintf((*vd).sde[ie], "\n\n");ie++;


    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_man);
      sprintf((*vd).sde[ie], "  itp_out0 <= itp_c0_c1dx when (d_isz4='0') else \"%s\";\n",_zero );ie++;
    }
    sprintf((*vd).sde[ie], "\n\n");ie++;


    /* ---- 7-TH PIPELINE ---- */
    if((this.is_pipe)[7]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 7 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      df5 <= df4;\n");ie++;
      sprintf((*vd).sde[ie], "      d_isz5 <= d_isz4;\n");ie++;
      sprintf((*vd).sde[ie], "      itp_out1 <= itp_out0;\n");ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 7(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  df5 <= df4;\n");ie++;
      sprintf((*vd).sde[ie], "  d_isz5 <= d_isz4;\n");ie++;
      sprintf((*vd).sde[ie], "  itp_out1 <= itp_out0;\n");ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }

    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "  d2(%d) <= d_isz5;\n",nbit_man);ie++;
    sprintf((*vd).sde[ie], "  d2(%d downto 0) <= itp_out1;\n",nbit_man-1 );ie++;

    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_man+1);
      sprintf((*vd).sde[ie], "  d4(%d downto 0) <= d2 when (df5 = '1') else \"%s\";\n", nbit_man,_zero);ie++;
    }
    {
      char _zero[STRLEN];
      itobc(0,_zero,nbit_log-nbit_man-2);
      sprintf((*vd).sde[ie], "  d4(%d downto %d) <= \"%s\";\n", nbit_log-2, nbit_man+1, _zero);ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "-- TABLE PART (END) ---------------------------------------------\n" );ie++;
    sprintf((*vd).sde[ie], "\n\n" );ie++;


    /* ---- 3-TH PIPELINE ---- */
    if((this.is_pipe)[3]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 3 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      x4 <= x3;\n" );ie++;
      sprintf((*vd).sde[ie], "      sign3 <= sign2;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 3(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x4 <= x3;\n" );ie++;
      sprintf((*vd).sde[ie], "  sign3 <= sign2;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 4-TH PIPELINE ---- */
    if((this.is_pipe)[4]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 4 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      x5 <= x4;\n" );ie++;
      sprintf((*vd).sde[ie], "      sign4 <= sign3;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 4(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x5 <= x4;\n" );ie++;
      sprintf((*vd).sde[ie], "  sign4 <= sign3;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 5-TH PIPELINE ---- */
    if((this.is_pipe)[5]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 5 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      x6 <= x5;\n" );ie++;
      sprintf((*vd).sde[ie], "      sign5 <= sign4;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 5(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x6 <= x5;\n" );ie++;
      sprintf((*vd).sde[ie], "  sign5 <= sign4;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 6-TH PIPELINE ---- */
    if((this.is_pipe)[6]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 6 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      x7 <= x6;\n" );ie++;
      sprintf((*vd).sde[ie], "      sign6 <= sign5;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 6(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x7 <= x6;\n" );ie++;
      sprintf((*vd).sde[ie], "  sign6 <= sign5;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 7-TH PIPELINE ---- */
    if((this.is_pipe)[7]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 7 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      x8 <= x7;\n" );ie++;
      sprintf((*vd).sde[ie], "      sign7 <= sign6;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 7(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  x8 <= x7;\n" );ie++;
      sprintf((*vd).sde[ie], "  sign7 <= sign6;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 8-TH PIPELINE ---- */
    if((this.is_pipe)[8]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 8 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  u4: reged_add_sub generic map(WIDTH=>%d,DIRECTION=>\"ADD\",PIPELINE=>1)\n", nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x8,datab=>d4,result=>z0,clock=>clock);\n\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      sign8 <= sign7;\n" );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 8(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  u4: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x8,datab=>d4,result=>z0);\n\n" );ie++;
      sprintf((*vd).sde[ie], "  sign8 <= sign7;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* ---- 9-TH PIPELINE ---- */
    if((this.is_pipe)[9]){
      sprintf((*vd).sde[ie], "  --- PIPELINE 9 ---\n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin\n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n" );ie++;
      sprintf((*vd).sde[ie], "      z(%d downto 0) <= z0;\n", nbit_log-2 );ie++;
      sprintf((*vd).sde[ie], "      z(%d) <= sign8;\n", nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "    end if;\n" );ie++;
      sprintf((*vd).sde[ie], "  end process;\n" );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  --- PIPELINE 9(OFF) ---\n" );ie++;
      sprintf((*vd).sde[ie], "  z(%d downto 0) <= z0;\n", nbit_log-2 );ie++;
      sprintf((*vd).sde[ie], "  z(%d) <= sign8;\n", nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "  ------------------.\n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;   
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;   
    sprintf((*vd).sde[ie], "-- ============= END  pg_log_unsigned_add interporation version    \n" );ie++;   
    (*vd).ie = ie;

    // -- START SUB LOGIC (REGED_ADD_SUB)
    if((this.is_pipe)[1]||(this.is_pipe)[8]) generate_reged_add_sub(vd);
    // -- END SUB LOGIC (REGED_ADD_SUB)


    // -- GENERATE LCELL ROM 
    if(is_esb==0){       // c0 rom
      int ilen,olen,depth,nstage;
      char name[256];
      LONG* rom;
      ilen = this.rom_c0_ilen;
      olen = this.rom_c0_olen;
      depth = this.rom_depth; // entries of array (not rom)
      if((this.is_pipe)[3]) nstage = 1; else nstage = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c0;
      strcpy(name,this.lcromname_c0);
      generate_pg_lcell_rom(vd,ilen,olen,depth,nstage,name,rom);
    }
    if(is_esb==0){       // c1 rom
      int ilen,olen,depth,nstage;
      char name[256];
      LONG* rom;
      ilen = this.rom_c1_ilen;
      olen = this.rom_c1_olen;
      depth = this.rom_depth;
      if((this.is_pipe)[3]) nstage = 1; else nstage = 0;       /* ---- 3-TH PIPELINE ---- */
      rom = this.rom_c1;
      strcpy(name,this.lcromname_c1);
      generate_pg_lcell_rom(vd,ilen,olen,depth,nstage,name,rom);
    }
    // -- END GENERATE LCELL ROM

    free_rom_memory();
  } // -- END iflag==0

}  
