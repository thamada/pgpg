#include<stdio.h>
#include "pgpg.h"

void generate_pg_fix_accum(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
	int i,ii;
	int nbit,nstage,nbits,ndelay;
	char fx[STRLEN],sx[STRLEN];
        char sd[STRLEN];
        char sd0[STRLEN];	
        char sd1[STRLEN];
	int ic,im,ie,is,nzero,iflag,j;
	static int flag=1;
	int nbitl,nbith;
	int ivmp;
	
	strcpy(fx,sdata[1]);
	strcpy(sx,sdata[2]);
	nbit = atoi(sdata[3]);
	nbits = atoi(sdata[4]);
	nstage = atoi(sdata[5]);
	ndelay = atoi(sdata[6]);

        printf("fix accum: %s += %s : ",fx,sx);
	printf("nbit %d nbit_accum %d nstage %d ndelay %d\n",nbit,nbits,nstage,ndelay); 

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_fix_accum_%d_%d_%d                          \n", nbit,nbits,nstage);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port (fdata: in std_logic_vector(%d downto 0);    \n", nbit-1 );ic++;
    if((*vd).nvmp>1){
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        sprintf((*vd).sdc[ic], "          sdata%d: out std_logic_vector(%d downto 0);   \n" , ivmp,nbits-1 );ic++;
      } 
    }else{
      sprintf((*vd).sdc[ic], "          sdata: out std_logic_vector(%d downto 0);   \n" , nbits-1 );ic++;
    }
    sprintf((*vd).sdc[ic], "          run: in std_logic;                                        \n" );ic++;
    sprintf((*vd).sdc[ic], "          clk: in std_logic);                                       \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                    \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                    \n" );ic++;
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
  sprintf((*vd).sdm[im], "  u%d: pg_fix_accum_%d_%d_%d\n",(*vd).ucnt,nbit,nbits,nstage);im++;
  sprintf((*vd).sdm[im], "                   port map(fdata=>%s,%srun=>run(%d),clk=>pclk);\n",fx,sd0,ndelay);im++;
  sprintf((*vd).sdm[im], "                                                                    \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",fx,nbit-1);is++;
  if((*vd).nvmp>1){
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){    
      sprintf((*vd).sds[is], "  signal %s%d: std_logic_vector(%d downto 0);    \n",sx,ivmp,nbits-1);is++;
    }
  }else{
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbits-1);is++;
  }
  (*vd).is = is;

  if(iflag==0){
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  if((*vd).nvmp>1){
    sprintf((*vd).sde[ie], "use ieee.std_logic_unsigned.all;                                    \n" );ie++;  
  }
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity pg_fix_accum_%d_%d_%d is                     \n",nbit,nbits,nstage );ie++;
  sprintf((*vd).sde[ie], "  port (fdata: in std_logic_vector(%d downto 0);      \n", nbit-1 );ie++;
  if((*vd).nvmp>1){
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){    
      sprintf((*vd).sde[ie], "        sdata%d: out std_logic_vector(%d downto 0);     \n", ivmp,nbits-1 );ie++;
    }
  }else{
    sprintf((*vd).sde[ie], "        sdata: out std_logic_vector(%d downto 0);     \n", nbits-1 );ie++;
  }
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
    sprintf((*vd).sde[ie], "  signal fxl,sxl1: std_logic_vector(%d downto 0);          \n", nbits-1 );ie++;
    sprintf((*vd).sde[ie], "  signal sxl2: std_logic_vector(%d downto 0);              \n", nbits-1 );ie++;    
    if((*vd).nvmp>1){
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        sprintf((*vd).sde[ie], "  signal sxl2%d: std_logic_vector(%d downto 0);   \n", ivmp,nbits-1 );ie++;    
      }
      sprintf((*vd).sde[ie], "  signal count: std_logic_vector(3 downto 0);                     \n" );ie++;    
    }
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  process(clk) begin                                                \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                  \n" );ie++;
    sprintf((*vd).sde[ie], "      run1 <= run;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
    if((*vd).nvmp>1){
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
      itobc((*vd).nvmp-1,sd,4);
      sprintf((*vd).sde[ie], "        if(count = \"%s\") then                               \n", sd );ie++;
      sprintf((*vd).sde[ie], "          count <= \"0000\";                                      \n" );ie++;
      sprintf((*vd).sde[ie], "        else                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "          count <= count + \"0001\";                              \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
      sprintf((*vd).sde[ie], "          count <= \"0000\";                                      \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
    }
    nzero = nbits - nbit + 1;
    strcpy(sd,"");
    for(i=0;i<nzero;i++) strcat(sd,"0");
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
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
    strcpy(sd,"");
    for(i=0;i<nbits;i++) strcat(sd,"0");
    if((*vd).nvmp==1){
      sprintf((*vd).sde[ie], "  process(clk) begin                                                \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                  \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                           \n" );ie++;
      sprintf((*vd).sde[ie], "        sxl2 <= sxl1;                                               \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                          \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run='1') then                                            \n" );ie++;
      sprintf((*vd).sde[ie], "          sxl2 <= \"%s\";\n"                                        , sd);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "  sdata <= sxl2;                                                    \n" );ie++;
    }else{
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        strcpy(sd1,"");
	itobc(ivmp,sd1,4);
        sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
        sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
        sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
        sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
        sprintf((*vd).sde[ie], "        if(count = \"%s\") then                               \n",sd1 );ie++;
        sprintf((*vd).sde[ie], "          sxl2%d <= sxl1;                                    \n",ivmp );ie++;
        sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
        sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
        sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
        sprintf((*vd).sde[ie], "          sxl2%d <= \"%s\";\n"                               ,ivmp, sd);ie++;
        sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
        sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
        sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
        sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  sdata%d <= sxl2%d;                                  \n", ivmp,ivmp);ie++;
      }
      sprintf((*vd).sde[ie], "                                                               \n" );ie++;
      sprintf((*vd).sde[ie], "  with count select                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    sxl2 <= sxl20 when \"0000\",                                \n" );ie++;      
      for(ivmp=1;ivmp<(*vd).nvmp;ivmp++){
        strcpy(sd1,"");
	itobc(ivmp,sd1,4);
        sprintf((*vd).sde[ie], "            sxl2%d when \"%s\",                       \n",ivmp,sd1 );ie++;
      }
      sprintf((*vd).sde[ie], "            sxl20 when others;                                   \n" );ie++;
    }
  }
  if(nstage==2){
    sprintf((*vd).sde[ie], "  signal fx: std_logic_vector(%d downto 0);               \n", nbits-1);ie++;
    sprintf((*vd).sde[ie], "  signal run2,run1 : std_logic;                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  signal signx,signx1,cx1,cx2: std_logic;                         \n" );ie++;
    nbitl = nbits/2;
    nbith = nbits - nbitl;
    sprintf((*vd).sde[ie], "  signal fxl,sxl1,sxl2: std_logic_vector(%d downto 0);            \n",nbitl-1);ie++;
    sprintf((*vd).sde[ie], "  signal fxh,fxh1,sxh1,sxh2 : std_logic_vector(%d downto 0);      \n",nbith-1);ie++;
    if((*vd).nvmp>1){
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
	sprintf((*vd).sde[ie], "  signal sxl2%d: std_logic_vector(%d downto 0);   \n", ivmp,nbitl-1 );ie++;
	sprintf((*vd).sde[ie], "  signal sxh2%d: std_logic_vector(%d downto 0);   \n", ivmp,nbith-1 );ie++;
      }
      sprintf((*vd).sde[ie], "  signal count,count2: std_logic_vector(3 downto 0);              \n" );ie++;
    }
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
    if((*vd).nvmp>1){
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
      itobc((*vd).nvmp-1,sd,4);
      sprintf((*vd).sde[ie], "        if(count = \"%s\") then                               \n", sd );ie++;
      sprintf((*vd).sde[ie], "          count <= \"0000\";                                      \n" );ie++;
      sprintf((*vd).sde[ie], "        else                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "          count <= count + \"0001\";                              \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
      sprintf((*vd).sde[ie], "          count <= \"0000\";                                      \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run2 = '1') then                                         \n" );ie++;
      itobc((*vd).nvmp-1,sd,4);
      sprintf((*vd).sde[ie], "        if(count2 = \"%s\") then                              \n", sd );ie++;
      sprintf((*vd).sde[ie], "          count2 <= \"0000\";                                     \n" );ie++;
      sprintf((*vd).sde[ie], "        else                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "          count2 <= count2 + \"0001\";                            \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run1 = '1') then                                       \n" );ie++;
      sprintf((*vd).sde[ie], "          count2 <= \"0000\";                                     \n" );ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
    }
    nzero = nbits - nbit + 1;
    strcpy(sd,"");
    for(i=0;i<nzero;i++) strcat(sd,"0");
    sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
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
    sprintf((*vd).sde[ie], "      cx2 <= cx1;                                                 \n" );ie++;
    sprintf((*vd).sde[ie], "      fxh1 <= fxh;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "      signx1 <= signx;                                            \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
    strcpy(sd,"");
    for(i=0;i<nbitl;i++) strcat(sd,"0");
    if((*vd).nvmp==1){
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
      sprintf((*vd).sde[ie], "        sxl2 <= sxl1;                                             \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run='1') then                                          \n" );ie++;
      sprintf((*vd).sde[ie], "          sxl2 <= \"%s\";\n",                                       sd);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  sdata(%d downto 0) <= sxl2;                                     \n", nbitl-1);ie++;
    }else{
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
	strcpy(sd1,"");
	itobc(ivmp,sd1,4);
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
	sprintf((*vd).sde[ie], "      if(run1 = '1') then                                         \n" );ie++;
	sprintf((*vd).sde[ie], "        if(count = \"%s\") then                               \n",sd1 );ie++;
	sprintf((*vd).sde[ie], "          sxl2%d <= sxl1;                                    \n",ivmp );ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
	sprintf((*vd).sde[ie], "        if(run = '1') then                                        \n" );ie++;
	sprintf((*vd).sde[ie], "          sxl2%d <= \"%s\";\n"                               ,ivmp, sd);ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
        sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
        sprintf((*vd).sde[ie], "  sdata%d(%d downto 0) <= sxl2%d;               \n", ivmp,nbitl-1,ivmp);ie++;
      }
      sprintf((*vd).sde[ie], "  with count select                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    sxl2 <= sxl20 when \"0000\",                                \n" );ie++;
      for(ivmp=1;ivmp<(*vd).nvmp;ivmp++){
	strcpy(sd1,"");
	itobc(ivmp,sd1,4);
	sprintf((*vd).sde[ie], "            sxl2%d when \"%s\",                       \n",ivmp,sd1 );ie++;
      }
      sprintf((*vd).sde[ie], "            sxl20 when others;                                   \n" );ie++;
    }
    sprintf((*vd).sde[ie], "                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d)                      \n",nbith);ie++;
    sprintf((*vd).sde[ie], "      port map(dataa=>sxh2,                                       \n" );ie++;
    sprintf((*vd).sde[ie], "               datab=>fxh1,add_sub=>signx1,                       \n" );ie++;
    sprintf((*vd).sde[ie], "               cin=>cx2,result=>sxh1);                            \n" );ie++;
    strcpy(sd,"");
    for(i=0;i<nbith;i++) strcat(sd,"0");
    if((*vd).nvmp==1){
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
      sprintf((*vd).sde[ie], "      if(run2 = '1') then                                         \n" );ie++;
      sprintf((*vd).sde[ie], "        sxh2 <= sxh1;                                             \n" );ie++;
      sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "        if(run1='1') then                                         \n" );ie++;
      sprintf((*vd).sde[ie], "          sxh2 <= \"%s\";\n",                                       sd);ie++;
      sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  sdata(%d downto %d) <= sxh2;                      \n", nbits-1,nbitl);ie++;
    }else{
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
	strcpy(sd1,"");
	itobc(ivmp,sd1,4);
	sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                \n" );ie++;
	sprintf((*vd).sde[ie], "      if(run2 = '1') then                                         \n" );ie++;
	sprintf((*vd).sde[ie], "        if(count2 = \"%s\") then                              \n",sd1 );ie++;
	sprintf((*vd).sde[ie], "          sxh2%d <= sxh1;                                    \n",ivmp );ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      else                                                        \n" );ie++;
	sprintf((*vd).sde[ie], "        if(run1 = '1') then                                       \n" );ie++;
	sprintf((*vd).sde[ie], "          sxh2%d <= \"%s\";\n"                               ,ivmp, sd);ie++;
	sprintf((*vd).sde[ie], "        end if;                                                   \n" );ie++;
	sprintf((*vd).sde[ie], "      end if;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                    \n" );ie++;
        sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
        sprintf((*vd).sde[ie], "  sdata%d(%d downto %d) <= sxh2%d;        \n", ivmp,nbits-1,nbitl,ivmp);ie++;
        sprintf((*vd).sde[ie], "                                                                  \n" );ie++;
      }
      sprintf((*vd).sde[ie], "  with count2 select                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    sxh2 <= sxh20 when \"0000\",                                 \n" );ie++;
      for(ivmp=1;ivmp<(*vd).nvmp;ivmp++){
	strcpy(sd1,"");
	itobc(ivmp,sd1,4);
	sprintf((*vd).sde[ie], "            sxh2%d when \"%s\",                       \n",ivmp,sd1 );ie++;
      }
      sprintf((*vd).sde[ie], "            sxh20 when others;                                   \n" );ie++;
    }
  }
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
  (*vd).ie = ie;
  flag=0;
  }
}  







