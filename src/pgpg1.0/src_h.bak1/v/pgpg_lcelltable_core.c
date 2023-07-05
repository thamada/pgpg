/*
  PGPGV library: Core routine for Lcell-Table generation
  Author: Tsuyoshi Hamada
  Last modified at Sep 23, 2003. : bug-fixed
  Last modified at Jul  1, 2003.
*/
#include <stdio.h>
#include <math.h>   // ceil()
#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include "pgpg.h"   // struct vhdl_description,STRLEN

/* Check the size for sde */
static void ckln(int ie)
{
  if(ie>=SDE_MAX){
    printf("Err at %s ",__FILE__);
    printf("line %d.\n",__LINE__+3);
    printf("Sorry, the size of pg_lcell_rom is over the system size.\n");
    printf("(%d)\n",ie);
    exit(1);
  }
}

void generate_pg_lcell_rom(vd,ilen,olen,depth,nstage,name,long_type_rom)
struct vhdl_description *vd;
int ilen,olen,depth,nstage;
char* name;
long long int* long_type_rom;
{
  int i,ie,j;
  int nentry,imax;
  int rom[131072];
  int lut_mask[256];
  int is_pipe[128];
  ie = (*vd).ie;

  // ------- PIPELINE MAP ----------------------------------
  // The lcell_rom has (ilen-4) register-points.
  // The lcell_rom can't have input/output register.
  //                     is_pipe[0]      : point of input register 
  //                     is_pipe[ilen-3] : point of output register

  /* --- 2003/09/23 bugfixed --- */
  //  for(i=0;i<=ilen-4;i++) is_pipe[i] = 0;  // <- bug
  for(i=0;i<=ilen-3;i++) is_pipe[i] = 0;  // the output register of (is_pipe[ilen-3]) should be 0.

  // is_pipe[0] is not used (fixed to 0).
  if(nstage==0){
  }else if(nstage==1){
    is_pipe[ilen-4] =1; // this is the last reg-point( not output reg).
  }else{
    fprintf(stderr,"Err at %s ",__FILE__);fprintf(stderr,"line %d.\n",__LINE__);exit(1);
  }
  // ------- END PIPELINE MAP ------------------------------

  sprintf((*vd).sde[ie], "                                   \n");ckln(++ie);
  sprintf((*vd).sde[ie], "-- ROM using Lcell not ESB         \n");ckln(++ie);
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada         \n");ckln(++ie);
  sprintf((*vd).sde[ie], "-- Last Modified at May 29,2003    \n");ckln(++ie);
  sprintf((*vd).sde[ie], "-- In %d Out %d Stage %d Type\"%s\"\n",ilen,olen,nstage,name);ckln(++ie);
  sprintf((*vd).sde[ie], "library ieee;                      \n" );ckln(++ie);
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;       \n" );ckln(++ie);
  sprintf((*vd).sde[ie], "                                   \n" );ckln(++ie);
  sprintf((*vd).sde[ie], "entity lcell_rom_%s_%d_%d_%d is\n",name,ilen,olen,nstage);ckln(++ie);
  for(j=0;j<(ie-1);j++){if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) return;}

  // --- prepare the local values ---
  imax = 16 * (int)ceil(((double)depth)/16.0);
  nentry = 0x1<<ilen;
  for(i=0;i<depth;i++) rom[i] = (int)(long_type_rom[i]);
  // --------------------------------

  sprintf((*vd).sde[ie],"  port( indata : in std_logic_vector(%d downto 0);\n",ilen-1);ckln(++ie);
  sprintf((*vd).sde[ie],"        clk : in std_logic;\n");ckln(++ie);
  sprintf((*vd).sde[ie],"        outdata : out std_logic_vector(%d downto 0));\n",olen-1);ckln(++ie);
  sprintf((*vd).sde[ie],"end lcell_rom_%s_%d_%d_%d;\n",name,ilen,olen,nstage);ckln(++ie);
  sprintf((*vd).sde[ie],"\n");ckln(++ie);
  sprintf((*vd).sde[ie],"architecture rtl of lcell_rom_%s_%d_%d_%d is\n",name,ilen,olen,nstage);ckln(++ie); 
  sprintf((*vd).sde[ie],"\n");ckln(++ie);

  sprintf((*vd).sde[ie],"  component pg_lcell\n");ckln(++ie);
  sprintf((*vd).sde[ie],"    generic (MASK : string  := \"ffff\";\n");ckln(++ie);
  sprintf((*vd).sde[ie],"             FF   : integer :=%d);\n",is_pipe[1]);ckln(++ie);
  sprintf((*vd).sde[ie],"    port (x   : in  std_logic_vector(3 downto 0);\n");ckln(++ie);
  sprintf((*vd).sde[ie],"          z   : out std_logic;\n");ckln(++ie);
  sprintf((*vd).sde[ie],"          clk : in  std_logic);\n");ckln(++ie);
  sprintf((*vd).sde[ie],"  end component;\n\n");ckln(++ie);


  if(ilen>4){
    // ----------------------- START if(ilen>4) --------------------------------------------------------------
    for(i=0;i<ilen-3;i++){
      sprintf((*vd).sde[ie],"  signal adr%d : std_logic_vector(%d downto 0);\n",i,ilen-1);ckln(++ie);
    }

    for(j=0;j<(nentry>>4);j++){
      sprintf((*vd).sde[ie],"  signal lc_%x_%x : std_logic_vector(%d downto 0);\n",4,j,olen-1);ckln(++ie);
    }
    for(i=5;i<=ilen;i++){
      for(j=0;j<(nentry>>i);j++){
	sprintf((*vd).sde[ie],"  signal lut_%x_%x,lc_%x_%x : std_logic_vector(%d downto 0);\n",i,j,i,j,olen-1);ckln(++ie);
      }
    }

    sprintf((*vd).sde[ie],"\n");ckln(++ie);
    sprintf((*vd).sde[ie],"begin\n\n");ckln(++ie);

    // --- LCell arrays --------------------------------------------------------
    for(i=0;i<nentry;i++){ /* start with i=0 not 1 !! */
      int block_num = i>>4;
      int k = 0xf & i;
      if(k == 0x0){
	for(j=0;j<olen;j++) lut_mask[j] = 0; // clear all the lut_mask s
      }

      for(j=0;j<olen;j++){
	int lcell;
	if(i<depth) lcell = rom[i]>>j; else lcell = 0x0;
	lcell &= 0x1;
	lut_mask[j] |= lcell<<k;
      }

      if(k == 0xf){
	for(j=0;j<olen;j++){
	  if(lut_mask[j] == 0x0){
	    sprintf((*vd).sde[ie],"--LC_%03X_%02d \n",block_num,j);ckln(++ie);
	    sprintf((*vd).sde[ie],"  lc_4_%x(%d) <= '0';\n",block_num,j);ckln(++ie);
	  }else if(lut_mask[j] == 0xffff){
	    sprintf((*vd).sde[ie],"--LC_%03X_%02d \n",block_num,j);ckln(++ie);
	    sprintf((*vd).sde[ie],"  lc_4_%x(%d) <= '1';\n",block_num,j);ckln(++ie);
	  }else{
	    int is_reged=0;
	    if(is_pipe[1]) is_reged=1; else is_reged=0;
	    sprintf((*vd).sde[ie],"  LC_%03X_%02d : pg_lcell\n",block_num,j);ckln(++ie);
	    sprintf((*vd).sde[ie],"  generic map(MASK=>\"%04X\",FF=>%d)\n",lut_mask[j],is_reged);ckln(++ie);
	    sprintf((*vd).sde[ie],"  port map( x=>adr0(3 downto 0),clk=>clk,\n");ckln(++ie);
	    sprintf((*vd).sde[ie],"            z=>lc_4_%x(%d));\n",block_num,j);ckln(++ie);
	  }
	  sprintf((*vd).sde[ie],"\n");ckln(++ie);
	}
      }
    }
    // --- END LCell arrays ----------------------------------------------------


    // --- address (reg/wire) ---
    sprintf((*vd).sde[ie], "  adr0 <= indata;\n");ckln(++ie);
    for(i=0;i<ilen-4;i++){
      if(is_pipe[i+1]){
	sprintf((*vd).sde[ie], "  process(clk) begin\n");ckln(++ie);
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ckln(++ie);
	if(ilen == i+5){
	  sprintf((*vd).sde[ie], "      adr%d(%d) <= adr%d(%d);\n",i+1,ilen-1,i,ilen-1);ckln(++ie);
	}else{
	  sprintf((*vd).sde[ie], "      adr%d(%d downto %d) <= adr%d(%d downto %d);\n",i+1,ilen-1,i+4,i,ilen-1,i+4);ckln(++ie);
	}
	sprintf((*vd).sde[ie], "    end if;\n");ckln(++ie);
	sprintf((*vd).sde[ie], "  end process;\n");ckln(++ie);
      }else{
	if(ilen == i+5){
	  sprintf((*vd).sde[ie], "  adr%d(%d) <= adr%d(%d);\n",i+1,ilen-1,i,ilen-1);ckln(++ie);
	}else{
	  sprintf((*vd).sde[ie], "  adr%d(%d downto %d) <= adr%d(%d downto %d);\n",i+1,ilen-1,i+4,i,ilen-1,i+4);ckln(++ie);
	}
      }
    }

    // --- Next Selectors ---
    sprintf((*vd).sde[ie],"\n");ckln(++ie);
    for(i=5;i<=ilen;i++){
      sprintf((*vd).sde[ie],"--  =================================\n");ckln(++ie);
      for(j=0;j<(nentry>>(i-1));j+=2){
	sprintf((*vd).sde[ie],"  with adr%d(%d) select\n",i-4,i-1);ckln(++ie);
	sprintf((*vd).sde[ie],"    lut_%x_%x <= lc_%x_%x when '0',\n", i,j>>1,i-1,j);ckln(++ie);
	sprintf((*vd).sde[ie],"              lc_%x_%x when others;\n\n",i-1,j+1);ckln(++ie);
      }
    }

    // --- LUT register ---
    sprintf((*vd).sde[ie],"\n");ckln(++ie);
    for(i=5;i<=ilen;i++){
      sprintf((*vd).sde[ie],"--  =================================\n");ckln(++ie);
      if(is_pipe[i-3]){
	sprintf((*vd).sde[ie], "  process(clk) begin\n");ckln(++ie);
	//      sprintf((*vd).sde[ie], "  process(clk) begin -- is_pipe[%d]=%d\n",i-3,is_pipe[i-3]);ckln(++ie);
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ckln(++ie);
	for(j=0;j<(nentry>>(i-1));j+=2){
	  sprintf((*vd).sde[ie],"      lc_%x_%x <= lut_%x_%x;\n",i,j>>1,i,j>>1);ckln(++ie);
	}
	sprintf((*vd).sde[ie], "    end if;\n");ckln(++ie);
	sprintf((*vd).sde[ie], "  end process;\n");ckln(++ie);
      }else{
	for(j=0;j<(nentry>>(i-1));j+=2){
	  sprintf((*vd).sde[ie],"    lc_%x_%x <= lut_%x_%x;\n",i,j>>1,i,j>>1);ckln(++ie);
	}
      }
    }

    // --- output connection ---
    sprintf((*vd).sde[ie],"  outdata <= lc_%x_0;\n",ilen);ckln(++ie);
    // ----------------------- END if(ilen>4) --------------------------------------------------------------//
  }else{
    sprintf((*vd).sde[ie],"  signal adr0 : std_logic_vector(3 downto 0);\n");ckln(++ie);
    sprintf((*vd).sde[ie],"\n");ckln(++ie);
    sprintf((*vd).sde[ie],"begin\n\n");ckln(++ie);
    if(ilen == 4){
      sprintf((*vd).sde[ie],"  adr0 <= indata;\n");ckln(++ie);
    }else if(ilen == 3){
      sprintf((*vd).sde[ie],"  adr0(3) <= '0';\n");ckln(++ie);
      sprintf((*vd).sde[ie],"  adr0(2 downto 0) <= indata;\n");ckln(++ie);
    }else if(ilen == 2){
      sprintf((*vd).sde[ie],"  adr0(3 downto 2) <= \"00\";\n");ckln(++ie);
      sprintf((*vd).sde[ie],"  adr0(1 downto 0) <= indata;\n");ckln(++ie);
    }else{
      printf("Worning at lcell generation: rom input len < 2 ??\n");
    }
    // --- LCell arrays --------------------------------------------------------
    for(i=0;i<16;i++){ /* start with i=0 not 1 !! */
      int k = 0xf & i;
      if(k == 0x0){
	for(j=0;j<olen;j++) lut_mask[j] = 0; // clear all the lut_mask s
      }
      for(j=0;j<olen;j++){
	int lcell;
	if(i>=nentry) lcell = 0x0;
	else{
	  if(i<depth) lcell = rom[i]>>j; else lcell = 0x0;
	}
	lcell &= 0x1;
	lut_mask[j] |= lcell<<k;
      }
      if(k == 0xf){
	for(j=0;j<olen;j++){
	  if(lut_mask[j] == 0x0){
	    sprintf((*vd).sde[ie],"  outdata(%d) <= '0';\n",j);ckln(++ie);
	  }else if(lut_mask[j] == 0xffff){
	    sprintf((*vd).sde[ie],"  outdata(%d) <= '1';\n",j);ckln(++ie);
	  }else{
	    int is_reged=0;
	    if(nstage==1) is_reged=1;
	    else if(nstage==0) is_reged=0;
	    else{
	     fprintf(stderr,"Error @ pgpg_lcelltable_core.c:\n");
	     fprintf(stderr,"lcell_rom of %d-bit adr bit can't have %d-bit pipeline stage.\n",ilen,nstage);
	     exit(1);
	    }
	    sprintf((*vd).sde[ie],"  LC_%02d : pg_lcell",j);ckln(++ie);
	    sprintf((*vd).sde[ie],"  generic map(MASK=>\"%04X\",FF=>%d)",lut_mask[j],is_reged);ckln(++ie);
	    sprintf((*vd).sde[ie],"  port map( x=>adr0(3 downto 0),clk=>clk,");ckln(++ie);
	    sprintf((*vd).sde[ie],"z=>outdata(%d));\n",j);ckln(++ie);
	  }
	}
      }
    }
    // --- END LCell arrays ----------------------------------------------------
  }

  sprintf((*vd).sde[ie],"end rtl;\n");ckln(++ie);
  (*vd).ie = ie;  

}



// debug line
//  printf("Err at %s ",__FILE__);printf("line %d.\n",__LINE__);exit(1);
//
