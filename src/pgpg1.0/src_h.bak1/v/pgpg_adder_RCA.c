#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy()
#include<math.h>   // ceil()
#include "pgpg.h"
#include "pgpg_submodule.h" // generate_pg_lcell_fulladder()

static void calc_slice(int width, int np, int* nbit_small, int* nbit_long, int* nblock_small, int* nblock_long);

void generate_pg_adder_RCA(vd,nbit,nstage,is_sub)
struct vhdl_description *vd;
int nbit;
int nstage;
int is_sub;
{
  int ie;
  int iflag;
  int j;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "--+--------------------------+\n" );ie++;
  sprintf((*vd).sde[ie], "--| PGPG Ripple-Carry Adder  |\n" );ie++;
  sprintf((*vd).sde[ie], "--|  2003/10/31              |\n" );ie++;
  sprintf((*vd).sde[ie], "--|      by Tsuyoshi Hamada  |\n" );ie++;
  sprintf((*vd).sde[ie], "--+--------------------------+\n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  if(is_sub==0){
    sprintf((*vd).sde[ie], "entity pg_adder_RCA_ADD_%d_%d is\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "entity pg_adder_RCA_SUB_%d_%d is\n",nbit,nstage);ie++;
  }
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }

  if(iflag !=0) return;
  if(is_sub==0){
    printf("generate RCA(ADD): %d-bit %d-stage\n",nbit,nstage);
  }else{
    printf("generate RCA(SUB): %d-bit %d-stage\n",nbit,nstage);
  }
  sprintf((*vd).sde[ie], "  port(x,y : in std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "       z : out std_logic_vector(%d downto 0);\n",nbit-1);ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);\n");ie++;
  if(is_sub==0){
    sprintf((*vd).sde[ie], "end pg_adder_RCA_ADD_%d_%d;\n\n",nbit,nstage);ie++;
    sprintf((*vd).sde[ie], "architecture rtl of pg_adder_RCA_ADD_%d_%d is\n",nbit,nstage);ie++;
  }else{
    sprintf((*vd).sde[ie], "end pg_adder_RCA_SUB_%d_%d;\n\n",nbit,nstage);ie++;
    sprintf((*vd).sde[ie], "architecture rtl of pg_adder_RCA_SUB_%d_%d is\n",nbit,nstage);ie++;
  }
  sprintf((*vd).sde[ie], "  component pg_lcell_ari                    \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (MASK: string := \"96E8\";      \n" );ie++;
  sprintf((*vd).sde[ie], "             FF: integer  := 1);            \n" );ie++;
  sprintf((*vd).sde[ie], "    port (x,y,ci,clk: in std_logic;         \n" );ie++;
  sprintf((*vd).sde[ie], "          z,co: out std_logic);             \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                           \n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  //================================================================================
  if(nstage<=1){ // NSTAGE=0,1
    int stage,nslice,k;
    int nbit_s,nbit_l,nbs,nbl;
    int nuni=0;
    int first_cin_val;
    char mask_val[STRLEN];
    sprintf((*vd).sde[ie], "  signal co : std_logic_vector(%d downto 0);\n",nbit-2);ie++;
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    if(is_sub==0){
      first_cin_val=0;         //ADD
      strcpy(mask_val,"96E8"); //ADD
    }else{
      first_cin_val=1;         //SUB
      strcpy(mask_val,"69B2"); //SUB
    }

    calc_slice(nbit,nstage,&nbit_s,&nbit_l,&nbs,&nbl);
    sprintf((*vd).sde[ie], "-- ADDER: (%d-b)*%d + (%d-b)*%d\n",nbit_s,nbs,nbit_l,nbl);ie++;
    sprintf((*vd).sde[ie], "-- [ADD: MASK=\"96E8\", u0:ci='0'],[SUB: MASK=\"69B2\", u0:ci='1']\n");ie++;
    stage=0;
    do{
      if(stage<nbs){
	/* SMALLER WIDTH BLOCKS */
	nslice=nbit_s;
	sprintf((*vd).sde[ie], "--  %d-stage (%d-bit)--\n",stage,nslice);ie++;
	for(k=0;k<nslice;k++){
	  sprintf((*vd).sde[ie], "  u%d: pg_lcell_ari generic map (MASK=>\"%s\", FF=>%d)\n",nuni,mask_val,nstage);ie++;
	  if(nuni != 0){
	    sprintf((*vd).sde[ie], "     port map(x=>x(%d),y=>y(%d),ci=>co(%d),clk=>clk,",nuni,nuni,nuni-1 );ie++;
	  }else{
	    sprintf((*vd).sde[ie], "     port map(x=>x(%d),y=>y(%d),ci=>'%d',clk=>clk,",nuni,nuni,first_cin_val);ie++;
	  }
	  if(nuni != (nbit-1)){
	    sprintf((*vd).sde[ie], "z=>z(%d),co=>co(%d));\n",nuni,nuni);ie++;
	  }else{
	    sprintf((*vd).sde[ie], "z=>z(%d));\n",nuni);ie++; // NPIPE==0 
	  }
	  nuni++;
	}
      }else{
	/* LONGER WIDTH BLOCKS */
	nslice=nbit_l;
	sprintf((*vd).sde[ie], "--  %d-stage (%d-bit)--\n",stage,nslice);ie++;
	for(k=0;k<nslice;k++){
	  sprintf((*vd).sde[ie], "  u%d: pg_lcell_ari generic map (MASK=>\"%s\", FF=>%d)\n",nuni,mask_val,nstage);ie++;
	  sprintf((*vd).sde[ie], "     port map(x=>x(%d),y=>y(%d),ci=>co(%d),clk=>clk,",nuni,nuni,nuni-1 );ie++;
	  if(nuni != (nbit-1)){
	    sprintf((*vd).sde[ie], "z=>z(%d),co=>co(%d));\n",nuni,nuni);ie++;
	  }else{
	    sprintf((*vd).sde[ie], "z=>z(%d));\n",nuni);ie++;
	  }
	  nuni++;
	}
      }
      stage++;
    }while(stage<nstage);
  }else if(nstage==2){ // NSTAGE=2
    //================================================================================
    // DAMEDAME Algorithm (2003/10/31)
    int stage,k;
    int nbit_s,nbit_l,nbs,nbl;
    int nuni=0;
    int first_cin_val;
    char mask_val[STRLEN];
    int slen[2]; // n-th slice length
    if(is_sub==0){
      first_cin_val=0;         //ADD
      strcpy(mask_val,"96E8"); //ADD
    }else{
      first_cin_val=1;         //SUB
      strcpy(mask_val,"69B2"); //SUB
    }
    calc_slice(nbit,nstage,&nbit_s,&nbit_l,&nbs,&nbl);
    if(nbl==0){
      slen[0]=nbit_s; // 1-st stage adder length
      slen[1]=nbit_s; // 2-nd stage adder length
    }else{
      slen[0]=nbit_l; // 1-st stage adder length
      slen[1]=nbit_s; // 2-nd stage adder length
    }

    sprintf((*vd).sde[ie], "  --                             \n");ie++;
    sprintf((*vd).sde[ie], "  --  <-MSB               LSB->  \n");ie++;
    sprintf((*vd).sde[ie], "  --  x1_1| |y1_0   x0_0| |y0_0  \n");ie++;
    sprintf((*vd).sde[ie], "  --      | |       +---ADD(FF)  \n");ie++;
    sprintf((*vd).sde[ie], "  --     FF FF     FF    |z0_0   \n");ie++;
    sprintf((*vd).sde[ie], "  --  x1_1| |y1_1   |    |       \n");ie++;
    sprintf((*vd).sde[ie], "  --      ADD(FF)---+    FF      \n");ie++;
    sprintf((*vd).sde[ie], "  --       |z1_1         |z0_1   \n");ie++;
    sprintf((*vd).sde[ie], "  --                             \n");ie++;
    sprintf((*vd).sde[ie], "  signal x0_0      : std_logic_vector(%d downto 0);\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  signal y0_0      : std_logic_vector(%d downto 0);\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  signal z0_0,z0_1 : std_logic_vector(%d downto 0);\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  signal x1_0,x1_1 : std_logic_vector(%d downto 0);\n",slen[1]-1);ie++;
    sprintf((*vd).sde[ie], "  signal y1_0,y1_1 : std_logic_vector(%d downto 0);\n",slen[1]-1);ie++;
    sprintf((*vd).sde[ie], "  signal      z1_1 : std_logic_vector(%d downto 0);\n",slen[1]-1);ie++;
    sprintf((*vd).sde[ie], "  signal co    : std_logic_vector(%d downto 0);\n",nbit-2);ie++;
    sprintf((*vd).sde[ie], "  signal co_%dr: std_logic;\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "begin\n" );ie++;

    sprintf((*vd).sde[ie], "  x0_0<=x(%d downto 0);\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  y0_0<=y(%d downto 0);\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  x1_0<=x(%d downto %d);\n",nbit-1,slen[0]);ie++;
    sprintf((*vd).sde[ie], "  y1_0<=y(%d downto %d);\n",nbit-1,slen[0]);ie++;

    sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      x1_1 <= x1_0;\n");ie++;
    sprintf((*vd).sde[ie], "      y1_1 <= y1_0;\n");ie++;
    sprintf((*vd).sde[ie], "    end if;\n");ie++;
    sprintf((*vd).sde[ie], "  end process;\n");ie++;

    sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      z0_1 <= z0_0;\n");ie++;
    sprintf((*vd).sde[ie], "    end if;\n");ie++;
    sprintf((*vd).sde[ie], "  end process;\n");ie++;

    sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      co_%dr <= co(%d);\n",slen[0]-1,slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "    end if;\n");ie++;
    sprintf((*vd).sde[ie], "  end process;\n");ie++;

    sprintf((*vd).sde[ie], "-- ADDER: %d-bit + %d-bit\n",slen[0],slen[1]);ie++;
    sprintf((*vd).sde[ie], "-- [ADD: MASK=\"96E8\", u0:ci='0'],[SUB: MASK=\"69B2\", u0:ci='1']\n");ie++;
    for(stage=0;stage<nstage;stage++){
      sprintf((*vd).sde[ie], "--  %d-stage (%d-bit)--\n",stage,slen[stage]);ie++;
      for(k=0;k<slen[stage];k++){
	sprintf((*vd).sde[ie], "  u%d: pg_lcell_ari generic map (MASK=>\"%s\", FF=>%d)\n",nuni,mask_val,nstage);ie++;
	if(stage==0){ // IF (1-st STAGE)
	  if(k == 0){ // the first Slice
	    sprintf((*vd).sde[ie], "     port map(x=>x0_0(%d),y=>y0_0(%d),ci=>'%d',clk=>clk,",k,k,first_cin_val);ie++;
	  }else{
	    sprintf((*vd).sde[ie], "     port map(x=>x0_0(%d),y=>y0_0(%d),ci=>co(%d),clk=>clk,",k,k,nuni-1 );ie++;
	  }
	  sprintf((*vd).sde[ie], "z=>z0_0(%d),co=>co(%d));\n",k,nuni);ie++;
	}else{        // IF Not(1-st STAGE)
	  if(k == 0){ // the first Slice
	    sprintf((*vd).sde[ie], "     port map(x=>x1_1(%d),y=>y1_1(%d),ci=>co_%dr,clk=>clk,",k,k,nuni-1 );ie++;
	  }else{
	    sprintf((*vd).sde[ie], "     port map(x=>x1_1(%d),y=>y1_1(%d),ci=>co(%d),clk=>clk,",k,k,nuni-1 );ie++;
	  }
	  if(nuni != (nbit-1)){
	    sprintf((*vd).sde[ie], "z=>z1_1(%d),co=>co(%d));\n",k,nuni);ie++;
	  }else{
	    sprintf((*vd).sde[ie], "z=>z1_1(%d));\n",k);ie++; // THE LAST UNIT
	  }
	}
	nuni++;
      }
    }
    sprintf((*vd).sde[ie], "\n");ie++;
    sprintf((*vd).sde[ie], "  z(%d downto 0)  <= z0_1;\n",slen[0]-1);ie++;
    sprintf((*vd).sde[ie], "  z(%d downto %d) <= z1_1;\n",nbit-1,slen[0]);ie++;
  }else{ // NSTAGE > 2 : Error
  //================================================================================
    printf("Error: pgpg_adder_RCA is not support nstage=%d\n",nstage);
    exit(0);
  }


  sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
  (*vd).ie = ie;
}
  


static void calc_slice(int width, int np, int* nbit_small, int* nbit_long, int* nblock_small, int* nblock_long)
{
  int Ns,Nl,x,y;
  if(np>0){
    Ns = (int)(width/np);
    Nl = Ns+1;
    x = (int)((Nl*np-width)/(Nl-Ns));
    y = np -x;
  }else{
    Ns=width;
    Nl=0;
    x=1;
    y=0;
  }
  (*nbit_small)=Ns;
  (*nbit_long)=Nl;
  (*nblock_small)=x;
  (*nblock_long)=y;
  return;
}
