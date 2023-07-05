#include<stdio.h>
#include<stdlib.h> // exit(),atoi()
#include<string.h> // strcmp(),strcpy(),strcat()
#include<math.h>
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc()

/*
    2003/05/14 bug-fixed at the nbit_count calc.
    2003/05/09 add boost option
    2003/05/08 available for nstage>2
*/

void generate_pg_fix_accum_reg(vd,noncout,boost)
     struct vhdl_description *vd;
     int noncout;
     int boost; /* 0,1,....,10 : this option is Altera-specific */
{
  int i,ie,ivmp;
  char mname [256];
  int nbit_count;
  int nvmp  = (*vd).nvmp;

  /* --- calc bitlength of "count" vector --- */
  if(nvmp<1){
    fprintf(stderr,"Err at pgpg_fix_accum\n");
    fprintf(stderr,"illegal NVMP value %d\n",nvmp);
    exit(1);
  }
  nbit_count = (int)ceil(log((double)nvmp)/log(2.0)); /* bug-fixed 2003/05/14 */ 

  /* --- set module name --- */
  if(noncout != 1){
    sprintf(mname, "fix_accum_reg_%d",nvmp);ie++;
  }else{
    sprintf(mname, "fix_accum_reg_last_%d",nvmp);ie++;
  } 

  /* --- check the duplication --- */
  {
    int j,ie;
    char check_entity[1000];
    int iflag=0;
    sprintf(check_entity,"entity %s is\n",mname);
    ie = (*vd).ie;
    for(j=0;j<(ie-1);j++) if(strcmp(check_entity,(*vd).sde[j])==0) iflag=1;
    if(iflag !=0 ) return;
  }

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_unsigned.all;                                    \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity %s is\n",mname);ie++;
  sprintf((*vd).sde[ie], "  generic(WIDTH: integer := 28);                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(WIDTH-1 downto 0);              \n" );ie++;
  if(nbit_count>1){
    sprintf((*vd).sde[ie], "       count : in std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
  }else if(nbit_count==1){ /* NVMP 2 */
    sprintf((*vd).sde[ie], "       count : in std_logic;\n");ie++;
  }

  sprintf((*vd).sde[ie], "       cin : in std_logic;     \n");ie++;
  sprintf((*vd).sde[ie], "       run : in std_logic;     \n");ie++;
  sprintf((*vd).sde[ie], "       addsub : in std_logic;  \n");ie++;
  if(noncout != 1){
    if(nbit_count>1){
      sprintf((*vd).sde[ie], "       countd : out std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
    }else if(nbit_count==1){ /* NVMP 2 */
      sprintf((*vd).sde[ie], "       countd : out std_logic;\n");ie++;
    }
    sprintf((*vd).sde[ie], "       addsubd : out std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "       rund : out std_logic;   \n");ie++;
    sprintf((*vd).sde[ie], "       cout : out std_logic;   \n");ie++;
  }else{
    sprintf((*vd).sde[ie], "--     countd : out std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
    sprintf((*vd).sde[ie], "--     addsubd : out std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "--     rund : out std_logic;   \n");ie++;
    sprintf((*vd).sde[ie], "--     cout : out std_logic;   \n");ie++;
  }
  for(i=0;i<nvmp;i++){sprintf((*vd).sde[ie], "       reg%d : out std_logic_vector(WIDTH-1 downto 0);\n",i);ie++;}
  sprintf((*vd).sde[ie], "       clk : in std_logic);    \n");ie++;
  sprintf((*vd).sde[ie], "end %s;\n",mname);ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "architecture rtl of %s is                                           \n",mname);ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  if(boost==0){
    sprintf((*vd).sde[ie], "  component lpm_add_sub                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer);                                   \n" );ie++;
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   \n" );ie++;
    sprintf((*vd).sde[ie], "          cin : in std_logic;                                       \n" );ie++;
    sprintf((*vd).sde[ie], "          add_sub : in std_logic;                                   \n" );ie++;
    if(noncout == 1){sprintf((*vd).sde[ie], "--");ie++;}
    sprintf((*vd).sde[ie], "          cout : out std_logic;                                     \n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  component lpm_add_sub                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                    \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: string;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_HINT: string:=\"USED\";                            \n" );ie++;
    sprintf((*vd).sde[ie], "             MAXIMIZE_SPEED: integer:=10);                          \n" );ie++;
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   \n" );ie++;
    sprintf((*vd).sde[ie], "          cin : in std_logic;                                       \n" );ie++;
    if(noncout == 1){sprintf((*vd).sde[ie], "--");ie++;}
    sprintf((*vd).sde[ie], "          cout : out std_logic;                                     \n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "signal sx : std_logic_vector(WIDTH-1 downto 0);                     \n" );ie++;
  sprintf((*vd).sde[ie], "signal zero : std_logic_vector(WIDTH-1 downto 0);                   \n" );ie++;
  sprintf((*vd).sde[ie], "signal addout : std_logic_vector(WIDTH-1 downto 0);                 \n" );ie++;
  sprintf((*vd).sde[ie], "signal run1 : std_logic;                                            \n" );ie++;
  if(noncout != 1){
    sprintf((*vd).sde[ie], "signal cout0 : std_logic;                                         \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "--signal cout0 : std_logic;                                       \n" );ie++;
  }
  for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
    sprintf((*vd).sde[ie], "signal reg_vmp%d : std_logic_vector(WIDTH-1 downto 0);            \n",ivmp);ie++;
  }
  if(boost !=0 ){
    sprintf((*vd).sde[ie],"signal addo,subo : std_logic_vector(WIDTH-1 downto 0); \n" );ie++;
    if(noncout != 1)
      sprintf((*vd).sde[ie],"signal aco,sco : std_logic;                            \n" );ie++;
  }
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "  forgen1 : for i in 0 to WIDTH-1 generate\n" );ie++;
  sprintf((*vd).sde[ie], "    zero(i) <= '0';                       \n" );ie++;
  sprintf((*vd).sde[ie], "  end generate;                           \n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;



  if(nvmp>2){
    /* -----------------------------------------------------  NVMP > 2 --------------------------------- */
    /* VMP REGISTER SET */
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
      char sd1[256];
      strcpy(sd1,"");
      itobc(ivmp,sd1,nbit_count);
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
      sprintf((*vd).sde[ie], "        if(count = \"%s\") then                               \n",sd1 );ie++;
      sprintf((*vd).sde[ie], "          reg_vmp%d <= addout;                                    \n",ivmp);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
      sprintf((*vd).sde[ie], "          reg_vmp%d <= zero;\n"                                     ,ivmp);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "  reg%d <= reg_vmp%d;                                   \n", ivmp,ivmp);ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }
    /* ADDER INPUT SELECT */
    {
      char sd1[256];
      strcpy(sd1,"");
      itobc(0,sd1,nbit_count);
      sprintf((*vd).sde[ie],  "                                   \n" );ie++;
      sprintf((*vd).sde[ie],  "  with count select                \n" );ie++;
      sprintf((*vd).sde[ie],  "    sx <= reg_vmp0 when \"%s\",    \n",sd1);ie++;
      for(ivmp=1;ivmp<(*vd).nvmp;ivmp++){
	strcpy(sd1,"");itobc(ivmp,sd1,nbit_count);
	sprintf((*vd).sde[ie],"          reg_vmp%d when \"%s\",   \n",ivmp,sd1 );ie++;
      }
      sprintf((*vd).sde[ie],  "          reg_vmp0 when others;    \n" );ie++;
    }
    sprintf((*vd).sde[ie], "\n" );ie++;
  }else if(nvmp==2){
    /* -----------------------------------------------------  NVMP == 2 -------------------------------- */
    /* VMP REGISTER SET */
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
      char sd1[256];
      strcpy(sd1,"");
      itobc(ivmp,sd1,nbit_count);
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
      sprintf((*vd).sde[ie], "        if(count = '%s') then                                     \n",sd1 );ie++;
      sprintf((*vd).sde[ie], "          reg_vmp%d <= addout;                                    \n",ivmp);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
      sprintf((*vd).sde[ie], "          reg_vmp%d <= zero;\n"                                     ,ivmp);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "  reg%d <= reg_vmp%d;                                   \n", ivmp,ivmp);ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }
    /* ADDER INPUT SELECT */
    sprintf((*vd).sde[ie],  "                                   \n");ie++;
    sprintf((*vd).sde[ie],  "  with count select                \n");ie++;
    sprintf((*vd).sde[ie],  "    sx <= reg_vmp1 when '1',       \n");ie++;
    sprintf((*vd).sde[ie],  "          reg_vmp0 when others;    \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
  }else if(nvmp==1){
    /* -----------------------------------------------------  NVMP == 1 -------------------------------- */
    /* VMP REGISTER SET */
    sprintf((*vd).sde[ie], "  process(clk) begin                                              \n");ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n");ie++;
    sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n");ie++;
    sprintf((*vd).sde[ie], "        reg_vmp0 <= addout;                                       \n");ie++;
    sprintf((*vd).sde[ie], "      else                                                        \n");ie++;
    sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n");ie++;
    sprintf((*vd).sde[ie], "          reg_vmp0 <= zero;                                       \n");ie++;
    sprintf((*vd).sde[ie], "        end if;                                                   \n");ie++;
    sprintf((*vd).sde[ie], "      end if;                                                     \n");ie++;
    sprintf((*vd).sde[ie], "    end if;                                                       \n");ie++;
    sprintf((*vd).sde[ie], "  end process;                                                    \n");ie++;
    sprintf((*vd).sde[ie], "  reg0 <= reg_vmp0;                                               \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    /* ADDER INPUT SELECT(non sense) */
    sprintf((*vd).sde[ie],  "    sx <= reg_vmp0;\n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
  }else{
    fprintf(stderr,"Err: illegal NVMP %d\n",nvmp);exit(1);
  }

  /* ADDER */
  if(boost==0){
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>WIDTH)\n");ie++;
    sprintf((*vd).sde[ie], "      port map(dataa=>sx,                      \n");ie++;
    sprintf((*vd).sde[ie], "               datab=>indata,add_sub=>addsub,  \n");ie++;
    sprintf((*vd).sde[ie], "               cin=>cin,                       \n");ie++;
    if(noncout == 1){sprintf((*vd).sde[ie], "--");ie++;}
    sprintf((*vd).sde[ie], "               cout=>cout0,                    \n");ie++;
    sprintf((*vd).sde[ie], "               result=>addout);                \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "  u0: lpm_add_sub generic map(LPM_WIDTH=>WIDTH,\n");ie++;
    sprintf((*vd).sde[ie], "        LPM_DIRECTION=>\"ADD\",                \n");ie++;
    sprintf((*vd).sde[ie], "        LPM_HINT=>\"USED\",                   \n");ie++;
    sprintf((*vd).sde[ie], "        MAXIMIZE_SPEED=>%d)              \n",boost);ie++;
    sprintf((*vd).sde[ie], "      port map(dataa=>sx,                      \n");ie++;
    sprintf((*vd).sde[ie], "               datab=>indata,                  \n");ie++;
    sprintf((*vd).sde[ie], "               cin=>cin,                       \n");ie++;
    if(noncout == 1){sprintf((*vd).sde[ie], "--");ie++;}
    sprintf((*vd).sde[ie], "               cout=>aco,                      \n");ie++;
    sprintf((*vd).sde[ie], "               result=>addo);                  \n");ie++;
    sprintf((*vd).sde[ie], "                                               \n");ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>WIDTH,\n");ie++;
    sprintf((*vd).sde[ie], "        LPM_DIRECTION=>\"SUB\",                \n");ie++;
    sprintf((*vd).sde[ie], "        LPM_HINT=>\"USED\",                   \n");ie++;
    sprintf((*vd).sde[ie], "        MAXIMIZE_SPEED=>%d)              \n",boost);ie++;
    sprintf((*vd).sde[ie], "      port map(dataa=>sx,                      \n");ie++;
    sprintf((*vd).sde[ie], "               datab=>indata,                  \n");ie++;
    sprintf((*vd).sde[ie], "               cin=>cin,                       \n");ie++;
    if(noncout == 1){sprintf((*vd).sde[ie], "--");ie++;}
    sprintf((*vd).sde[ie], "               cout=>sco,                      \n");ie++;
    sprintf((*vd).sde[ie], "               result=>subo);                  \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  with addsub select                           \n" );ie++;
    sprintf((*vd).sde[ie], "       addout <= addo when '1',                \n" );ie++;
    sprintf((*vd).sde[ie], "                 subo when others;             \n" );ie++;
    if(noncout != 1){
      sprintf((*vd).sde[ie], "  with addsub select                           \n" );ie++;
      sprintf((*vd).sde[ie], "       cout0  <= aco when '1',                 \n" );ie++;
      sprintf((*vd).sde[ie], "                 sco when others;              \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }
  }

  sprintf((*vd).sde[ie], "  process(clk) begin              \n");ie++;
  sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      run1 <= run;                \n");ie++;
  sprintf((*vd).sde[ie], "    end if;                       \n");ie++;
  sprintf((*vd).sde[ie], "  end process;                    \n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  /* output delay signals */
  if(noncout != 1){
    sprintf((*vd).sde[ie], "  process(clk) begin              \n");ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      addsubd <= addsub;          \n");ie++;
    sprintf((*vd).sde[ie], "      cout <= cout0;              \n");ie++;
    if(nvmp>1){
      sprintf((*vd).sde[ie], "      countd <= count;            \n");ie++;
    }
    sprintf((*vd).sde[ie], "    end if;                       \n");ie++;
    sprintf((*vd).sde[ie], "  end process;                    \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  rund <= run1;\n" );ie++;
  }else{
    sprintf((*vd).sde[ie], "--process(clk) begin              \n");ie++;
    sprintf((*vd).sde[ie], "--  if(clk'event and clk='1') then\n");ie++;
    sprintf((*vd).sde[ie], "--    addsubd <= addsub;          \n");ie++;
    sprintf((*vd).sde[ie], "--    cout <= cout0;              \n");ie++;
    sprintf((*vd).sde[ie], "--    countd <= count;            \n");ie++;
    sprintf((*vd).sde[ie], "--  end if;                       \n");ie++;
    sprintf((*vd).sde[ie], "--end process;                    \n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "--rund <= run1;\n" );ie++;
  }
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                            \n" );ie++;
  sprintf((*vd).sde[ie], "                                    \n" );ie++;
  (*vd).ie = ie;
}


void generate_pg_fix_accum(sdata,vd)
     char sdata[][STRLEN];
     struct vhdl_description *vd;
{
  int i;
  int nbit,nstage,nbits,ndelay;
  char fx[STRLEN],sx[STRLEN];
  char func_name[STRLEN];
  char sd[STRLEN];
  char sd0[STRLEN];	
  char sd1[STRLEN];
  int ic,im,ie,is,nzero,iflag,j;
  int _msb=0,_lsb=0;
  static int flag=1;
  int ivmp,nvmp;
  int nbit_count;
  int nbit_add[100];
  int nboost;

  strcpy(fx,sdata[1]);
  strcpy(sx,sdata[2]);
  nbit = atoi(sdata[3]);
  nbits = atoi(sdata[4]);
  nstage = atoi(sdata[5]);
  ndelay = atoi(sdata[6]);
  nboost = (*vd).nboost_fixaccum;

  nvmp = (*vd).nvmp;
  nbit_count = (int)ceil(log((double)nvmp)/log(2.0)); /* bug-fixed 2003/05/14 */ 

  printf("fix accum: %s += %s : ",fx,sx);
  printf("nbit %d nbit_accum %d nstage %d ndelay %d nvmp %d\t",nbit,nbits,nstage,ndelay,nvmp); 
  printf("boost %d\n",nboost); 

  ic = (*vd).ic;
  sprintf(func_name,"pg_fix_accum_%d_%d_%d",nbit,nbits,nstage);
  sprintf((*vd).sdc[ic], "  component %s\n",func_name);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port (fdata: in std_logic_vector(%d downto 0);\n",nbit-1);ic++;
    if((*vd).nvmp>1){
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        sprintf((*vd).sdc[ic], "          sdata%d: out std_logic_vector(%d downto 0);\n",ivmp,nbits-1);ic++;
      } 
    }else{
      sprintf((*vd).sdc[ic], "          sdata: out std_logic_vector(%d downto 0);\n",nbits-1);ic++;
    }
    sprintf((*vd).sdc[ic], "          run: in std_logic;\n");ic++;
    sprintf((*vd).sdc[ic], "          clk: in std_logic);\n");ic++;
    sprintf((*vd).sdc[ic], "  end component;\n");ic++;
    sprintf((*vd).sdc[ic], "\n");ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  if((*vd).nvmp>1){
    strcpy(sd0,"");
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
      sprintf(sd1,"sdata%d=>%s%d,",ivmp,sx,ivmp);
      strcat(sd0,sd1);
    }
  }else{
    sprintf(sd0,"sdata=>%s,",sx);
  }
  if((*vd).nvmp==1){
    ndelay -= 1;
  }else{
    ndelay += (*vd).nvmp - 1;
  }
  sprintf((*vd).sdm[im], "  u%d: %s",(*vd).ucnt,func_name);im++;
  sprintf((*vd).sdm[im], " port map(fdata=>%s,%srun=>run(%d),clk=>pclk);\n",fx,sd0,ndelay);im++;
  sprintf((*vd).sdm[im], "\n");im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",fx,nbit-1);is++;
  if((*vd).nvmp>1){
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){    
      sprintf((*vd).sds[is], "  signal %s%d: std_logic_vector(%d downto 0);\n",sx,ivmp,nbits-1);is++;
    }
  }else{
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);\n",sx,nbits-1);is++;
  }
  (*vd).is = is;

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "-- pg_fix_accum\n" );ie++;
    sprintf((*vd).sde[ie], "-- Pipelined, Virtual Multiple Pipelined Fixed-Point Accumulator for Programmable GRAPE\n");ie++;
    sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada\n" );ie++;
    sprintf((*vd).sde[ie], "-- Last Modified at May 9 13:33:33\n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_unsigned.all;\n" );ie++;
    //    if((*vd).nvmp>1){
    //      sprintf((*vd).sde[ie], "use ieee.std_logic_unsigned.all;                                    \n" );ie++;  
    //    }
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "entity %s is\n",func_name);ie++;
    sprintf((*vd).sde[ie], "  port (fdata: in std_logic_vector(%d downto 0);\n", nbit-1 );ie++;
    if((*vd).nvmp>1){
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){    
	sprintf((*vd).sde[ie], "        sdata%d: out std_logic_vector(%d downto 0);\n", ivmp,nbits-1 );ie++;
      }
    }else{
      sprintf((*vd).sde[ie], "        sdata: out std_logic_vector(%d downto 0);\n", nbits-1 );ie++;
    }
    sprintf((*vd).sde[ie], "        run : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "        clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "end %s;\n",func_name);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of %s is\n",func_name);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  component fix_accum_reg_%d\n",nvmp);ie++;
    sprintf((*vd).sde[ie], "    generic (WIDTH: integer);\n" );ie++;
    sprintf((*vd).sde[ie], "    port (indata: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    if(nvmp>2){
      sprintf((*vd).sde[ie], "          count : in std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
      sprintf((*vd).sde[ie], "          countd : out std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
    }else if(nvmp==2){
      sprintf((*vd).sde[ie], "          count : in std_logic;\n");ie++;
      sprintf((*vd).sde[ie], "          countd : out std_logic;\n");ie++;
    }
    sprintf((*vd).sde[ie], "          cin : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          run : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          addsub : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          addsubd : out std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          rund : out std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          cout : out std_logic;\n" );ie++;
    for(ivmp=0;ivmp<nvmp;ivmp++){
      sprintf((*vd).sde[ie], "          reg%d : out std_logic_vector(WIDTH-1 downto 0);\n",ivmp);ie++;
    }
    sprintf((*vd).sde[ie], "          clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  component fix_accum_reg_last_%d\n",nvmp);ie++;
    sprintf((*vd).sde[ie], "    generic (WIDTH: integer);\n" );ie++;
    sprintf((*vd).sde[ie], "    port (indata: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    if(nvmp>2){
      sprintf((*vd).sde[ie], "          count : in std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
    }else if(nvmp==2){
      sprintf((*vd).sde[ie], "          count : in std_logic;\n");ie++;
    }
    sprintf((*vd).sde[ie], "          cin : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          run : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "          addsub : in std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "--        countd : out std_logic_vector(%d downto 0);\n",nbit_count-1);ie++;
    sprintf((*vd).sde[ie], "--        addsubd : out std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "--        rund : out std_logic;\n" );ie++;
    sprintf((*vd).sde[ie], "--        cout : out std_logic;\n" );ie++;
    for(ivmp=0;ivmp<nvmp;ivmp++){
      sprintf((*vd).sde[ie], "          reg%d : out std_logic_vector(WIDTH-1 downto 0);\n",ivmp);ie++;
    }
    sprintf((*vd).sde[ie], "          clk : in std_logic);\n" );ie++;
    sprintf((*vd).sde[ie], "  end component;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* 2003/05/07/ */
    /*--------------------------------------------------------------------------*/
    /*  calculate the length of all adders                                      */
    {
      int n_bigger,addlen;
      addlen = (int)(((double)nbits)/((double)nstage));
      n_bigger = nbits - addlen*nstage;
      for(i=0;i<nstage;i++){
	if(i<n_bigger)  nbit_add[i] = addlen+1; else  nbit_add[i] = addlen;
      }
    }
    if(nstage>2){ /* --- swap the second last one for the first one --- */
      int t;
      t = nbit_add[nstage-2];
      nbit_add[nstage-2] = nbit_add[0];
      nbit_add[0] = t;
    }
    /*+------------------------------------------------------------------------+*/

    sprintf((*vd).sde[ie], "  signal fx: std_logic_vector(%d downto 0);\n",nbits-1);ie++;
    for(i=0;i<nstage;i++){
      int j;
      for(j=0;j<=i;j++){
	sprintf((*vd).sde[ie], "  signal fx_%d_%d: std_logic_vector(%d downto 0);\n",i,j,nbit_add[i]-1);ie++;
      }
    }

    sprintf((*vd).sde[ie], "  signal rsig : std_logic_vector(%d downto 0);\n", nstage-1);ie++;
    if(nstage == 1){
      ie--;
      sprintf((*vd).sde[ie], "  signal rsig : std_logic_vector(1 downto 0);\n");ie++;
    }

    sprintf((*vd).sde[ie], "  signal addsub : std_logic_vector(%d downto 0);\n", nstage-1);ie++;
    sprintf((*vd).sde[ie], "  signal carry : std_logic_vector(%d downto 0);\n", nstage-1);ie++;


    if(nvmp>2){
      for(i=0;i<nstage;i++){
	sprintf((*vd).sde[ie], "  signal count%d : std_logic_vector(%d downto 0);\n", i,nbit_count-1);ie++;
      }
    }else if(nvmp==2){
      for(i=0;i<nstage;i++){
	sprintf((*vd).sde[ie], "  signal count%d : std_logic;\n",i);ie++;
      }
    }

    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    nzero = nbits - nbit + 1;
    strcpy(sd,"");
    for(i=0;i<nzero;i++) strcat(sd,"0");
    sprintf((*vd).sde[ie], "  fx <= \"%s\" & fdata(%d downto 0);\n", sd,nbit-2);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    _msb=0,_lsb=0;  // bugfixed 2003/09/23
    for(i=0;i<nstage;i++){
      if(i == 0){
	_lsb = 0;
	_msb = nbit_add[i];
      }else{
	_lsb += nbit_add[i-1];
	_msb += nbit_add[i];
      }
      sprintf((*vd).sde[ie], "  fx_%d_0 <= fx(%d downto %d);\n",i,_msb-1,_lsb);ie++;
    }

    sprintf((*vd).sde[ie], "  addsub(0) <= not fdata(%d);\n",nbit-1);ie++;
    sprintf((*vd).sde[ie], "  carry(0) <= fdata(%d);\n",nbit-1);ie++;
    sprintf((*vd).sde[ie], "  rsig(0)  <= run;\n");ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;

    /* --- RESET VMP COUNTER --- */
    sprintf((*vd).sde[ie], "\n" );ie++;
    if(nvmp>2){
      sprintf((*vd).sde[ie], "  process(clk) begin                        \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then          \n" );ie++;
      sprintf((*vd).sde[ie], "      if(rsig(1) = '1') then                \n" );ie++;
      itobc((*vd).nvmp-1,sd,nbit_count);
      sprintf((*vd).sde[ie], "        if(count0 = \"%s\") then            \n",sd);ie++;
      itobc(0,sd,nbit_count);
      sprintf((*vd).sde[ie], "          count0 <= \"%s\";                 \n",sd);ie++;
      sprintf((*vd).sde[ie], "        else                                \n" );ie++;
      itobc(1,sd,nbit_count);
      sprintf((*vd).sde[ie], "          count0 <= count0 + \"%s\";        \n",sd);ie++;
      sprintf((*vd).sde[ie], "        end if;                             \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                  \n" );ie++;
      sprintf((*vd).sde[ie], "        if(rsig(0) = '1') then              \n" );ie++;
      itobc(0,sd,nbit_count);
      sprintf((*vd).sde[ie], "          count0 <= \"%s\";                 \n",sd);ie++;
      sprintf((*vd).sde[ie], "        end if;                             \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                               \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                 \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                              \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }else if(nvmp==2){
      sprintf((*vd).sde[ie], "  process(clk) begin                        \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then          \n" );ie++;
      sprintf((*vd).sde[ie], "      if(rsig(1) = '1') then                \n" );ie++;
      sprintf((*vd).sde[ie], "        if(count0 = '1') then               \n" );ie++;
      sprintf((*vd).sde[ie], "          count0 <= '0';                    \n" );ie++;
      sprintf((*vd).sde[ie], "        else                                \n" );ie++;
      sprintf((*vd).sde[ie], "          count0 <= '1';                    \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                             \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                  \n" );ie++;
      sprintf((*vd).sde[ie], "        if(rsig(0) = '1') then              \n" );ie++;
      sprintf((*vd).sde[ie], "          count0 <= '0';                    \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                             \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                               \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                 \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                              \n" );ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    /* --- rsig delay when nstage == 1 --- */ 
    if(nstage==1){
      sprintf((*vd).sde[ie], "  process(clk) begin              \n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      sprintf((*vd).sde[ie], "      rsig(1) <= rsig(0);         \n");ie++;
      sprintf((*vd).sde[ie], "    end if;                       \n");ie++;
      sprintf((*vd).sde[ie], "  end process;                    \n");ie++;
      sprintf((*vd).sde[ie], "\n" );ie++;
    }

    /* --- fx delay -- */ 
    for(i=0;i<nstage;i++){
      int j;
      for(j=0;j<i;j++){
	sprintf((*vd).sde[ie], "  process(clk) begin              \n");ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
	sprintf((*vd).sde[ie], "      fx_%d_%d <= fx_%d_%d;       \n",i,j+1,i,j);ie++;
	sprintf((*vd).sde[ie], "    end if;                       \n");ie++;
	sprintf((*vd).sde[ie], "  end process;                    \n");ie++;
	sprintf((*vd).sde[ie], "\n" );ie++;
      }
    }


    _msb=0,_lsb=0;  // bugfixed 2003/09/23
    for(i=0;i<nstage;i++){
      if(i == 0){
	_lsb = nbit_add[i];
	_msb = 0;
      }else{
	_lsb += nbit_add[i];
	_msb += nbit_add[i-1];
      }
      if(i<nstage-1){
	sprintf((*vd).sde[ie], "  u%d: fix_accum_reg_%d                 \n", i,nvmp);ie++;
      }else{
	sprintf((*vd).sde[ie], "  u%d: fix_accum_reg_last_%d            \n", i,nvmp);ie++;
      }
      sprintf((*vd).sde[ie], "    generic map(WIDTH=>%d)                \n", nbit_add[i]);ie++;
      sprintf((*vd).sde[ie], "    port map(indata=>fx_%d_%d,            \n", i,i);ie++;
      if(nvmp>1){
	sprintf((*vd).sde[ie], "             count=>count%d,            \n", i);ie++;
      }
      sprintf((*vd).sde[ie], "             cin=>carry(%d),              \n", i);ie++;
      sprintf((*vd).sde[ie], "             run=>rsig(%d),               \n", i);ie++;
      sprintf((*vd).sde[ie], "             addsub=>addsub(%d),          \n", i);ie++;
      if(i<nstage-1){
	if(nvmp>1){
	  sprintf((*vd).sde[ie], "             countd=>count%d,         \n", i+1);ie++;
	}
	sprintf((*vd).sde[ie], "             addsubd=>addsub(%d),       \n", i+1);ie++;
	sprintf((*vd).sde[ie], "             rund=>rsig(%d),            \n", i+1);ie++;
	sprintf((*vd).sde[ie], "             cout=>carry(%d),           \n", i+1);ie++;
      }else{
	sprintf((*vd).sde[ie], "--           countd=>count%d,           \n", i+1);ie++;
	sprintf((*vd).sde[ie], "--           addsubd=>addsub(%d),       \n", i+1);ie++;
	sprintf((*vd).sde[ie], "--           rund=>rsig(%d),            \n", i+1);ie++;
	sprintf((*vd).sde[ie], "--           cout=>carry(%d),           \n", i+1);ie++;
      }
      if(nvmp>1){
	for(ivmp=0;ivmp<nvmp;ivmp++){
	  sprintf((*vd).sde[ie], "             reg%d=>sdata%d(%d downto %d),\n", ivmp,ivmp,_lsb-1,_msb);ie++;
	}
      }else{
	sprintf((*vd).sde[ie], "             reg0=>sdata(%d downto %d),\n",_lsb-1,_msb);ie++;
      }
      sprintf((*vd).sde[ie], "             clk=>clk);                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                          \n" );ie++;
    }


    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;

    if(nstage > 1) generate_pg_fix_accum_reg(vd,0,nboost); /* with cout version */
    generate_pg_fix_accum_reg(vd,1,nboost);                /* without cout version */
    flag=0;
  }
}  



