#include<stdio.h>
#include "pgpg.h"

void generate_pg_log_muldiv(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
	int i,ii;
	int nbit,nstage,istage;
	char sx[STRLEN],sy[STRLEN],sz[STRLEN];
        char muldiv_flag[STRLEN];
	char sdtmp[STRLEN];
	int ic,im,ie,is;
        int iflag,j,sflag=0;

	strcpy(muldiv_flag,sdata[1]);
	strcpy(sx,sdata[2]);
	strcpy(sy,sdata[3]);
	strcpy(sz,sdata[4]);
	nbit = atoi(sdata[5]);
	nstage = atoi(sdata[6]);

        printf("log muldiv: %s %s %s = %s : ",muldiv_flag,sx,sy,sz);
	printf("nbit %d nstage %d\n",nbit,nstage); 

  if(strcmp(muldiv_flag,"MUL")==0) strcpy(muldiv_flag,"ADD");
  if(strcmp(muldiv_flag,"DIV")==0) strcpy(muldiv_flag,"SUB");
  if(strcmp(muldiv_flag,"SDIV")==0) {
    strcpy(muldiv_flag,"SUB");  
    sflag = 1;
  }
  ic = (*vd).ic;

  if(sflag==1){
    sprintf((*vd).sdc[ic], "  component pg_log_smuldiv_%d_%d                     \n", nstage,nbit);ic++;
  }else{
    sprintf((*vd).sdc[ic], "  component pg_log_muldiv_%d                             \n", nstage );ic++;
  }
  iflag=0;
  for(j=0;j<ic-1;j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
  sprintf((*vd).sdc[ic], "    generic (PG_WIDTH : integer;                                 \n" );ic++;
  sprintf((*vd).sdc[ic], "             PG_MULDIV : string);                                \n" );ic++;
  sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        \n" );ic++;
  sprintf((*vd).sdc[ic], "	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        \n" );ic++;
  sprintf((*vd).sdc[ic], "	     clk: in std_logic);                                   \n" );ic++;
  sprintf((*vd).sdc[ic], "  end component;                                                 \n" );ic++;
  sprintf((*vd).sdc[ic], "                                                                 \n" );ic++;
  (*vd).ic = ic;
  }

  im = (*vd).im;
  if(sflag==1){
    sprintf((*vd).sdm[im], "  u%d: pg_log_smuldiv_%d_%d generic map(PG_WIDTH=>%d,\n",(*vd).ucnt,nstage,nbit,nbit);im++;
  }else{
    sprintf((*vd).sdm[im], "  u%d: pg_log_muldiv_%d generic map(PG_WIDTH=>%d,\n",(*vd).ucnt,nstage,nbit);im++;
  }
  sprintf((*vd).sdm[im], "                                PG_MULDIV=>\"%s\")\n",muldiv_flag);im++;
  sprintf((*vd).sdm[im], "                    port map(x=>%s,y=>%s,z=>%s,clk=>pclk);\n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "                                                                  \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sy,nbit-1);is++;
  (*vd).is = is;

  if(iflag==0){
  ie= (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(sflag==1){
    sprintf((*vd).sde[ie], "entity pg_log_smuldiv_%d_%d is                         \n",nstage,nbit );ie++;
  }else{
    sprintf((*vd).sde[ie], "entity pg_log_muldiv_%d is                                  \n",nstage );ie++;  
  }
  sprintf((*vd).sde[ie], "  generic (PG_WIDTH: integer;                                      \n" );ie++;
  sprintf((*vd).sde[ie], "           PG_MULDIV: string);                                     \n" );ie++;
  sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);            \n" );ie++;
  sprintf((*vd).sde[ie], "	   z : out std_logic_vector(PG_WIDTH-1 downto 0);            \n" );ie++;
  sprintf((*vd).sde[ie], "	   clk : in std_logic);                                      \n" );ie++;
  if(sflag==1){
    sprintf((*vd).sde[ie], "end pg_log_smuldiv_%d_%d;                               \n",nstage,nbit );ie++;
  }else{
    sprintf((*vd).sde[ie], "end pg_log_muldiv_%d;                                       \n",nstage );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(sflag==1){
    sprintf((*vd).sde[ie], "architecture rtl of pg_log_smuldiv_%d_%d is          \n",nstage,nbit );ie++;
  }else{
    sprintf((*vd).sde[ie], "architecture rtl of pg_log_muldiv_%d is                   \n",nstage );ie++;  
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_PIPELINE: integer;                                \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;  
  sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
  sprintf((*vd).sde[ie], "          clock: in std_logic;                                     \n" );ie++;
  sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(nstage>=2){
    for(istage=0;istage<(nstage-1);istage++){
      sprintf((*vd).sde[ie], " signal sign%d,nz%d : std_logic;               \n", istage, istage );ie++;      
    }
  }
  if(sflag==1){
    sprintf((*vd).sde[ie], " signal dataa,datab,result: std_logic_vector(PG_WIDTH-2 downto 0); \n" );ie++;
  }else{
    sprintf((*vd).sde[ie], " signal dataa,datab,result: std_logic_vector(PG_WIDTH-3 downto 0); \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(nstage==0){
    sprintf((*vd).sde[ie], "  z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);                \n" );ie++;
    sprintf((*vd).sde[ie], "  z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);                \n" );ie++;
  }
  if(nstage==1){
    sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
    sprintf((*vd).sde[ie], "      z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);            \n" );ie++;
    sprintf((*vd).sde[ie], "      z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);            \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
  }
  if(nstage>=2){
    sprintf((*vd).sde[ie], "  process(clk) begin                                              \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
    sprintf((*vd).sde[ie], "      sign0 <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);                    \n" );ie++;
    sprintf((*vd).sde[ie], "      nz0 <= x(PG_WIDTH-2) and y(PG_WIDTH-2);                      \n" );ie++;
    for(istage=1;istage<(nstage-1);istage++){
      sprintf((*vd).sde[ie], "      sign%d <= sign%d;                        \n", istage, istage-1 );ie++;
      sprintf((*vd).sde[ie], "      nz%d <= nz%d;                            \n", istage, istage-1 );ie++;
    }
    sprintf((*vd).sde[ie], "      z(PG_WIDTH-1) <= sign%d;                          \n", nstage-2);ie++;
    sprintf((*vd).sde[ie], "      z(PG_WIDTH-2) <= nz%d;                            \n", nstage-2);ie++;
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  if(sflag==1){
    sprintf((*vd).sde[ie], "  dataa <= '0' & x(PG_WIDTH-3 downto 0);                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  datab <= '0' & y(PG_WIDTH-3 downto 0);                                 \n" );ie++;  
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;  
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-1,LPM_PIPELINE=>%d,\n", nstage );ie++;
    sprintf((*vd).sde[ie], "                              LPM_DIRECTION=>PG_MULDIV)           \n" );ie++;
    sprintf((*vd).sde[ie], "       port map(dataa=>dataa,datab=>datab,clock=>clk,result=>result);\n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    itobc(0,sdtmp,nbit-2);
    sprintf((*vd).sde[ie], "  with result(PG_WIDTH-2) select                                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "    z(PG_WIDTH-3 downto 0) <= \"%s\" when '1',     \n", sdtmp);ie++;
    sprintf((*vd).sde[ie], "                           result(PG_WIDTH-3 downto 0) when others;\n" );ie++;    
  }else{
    sprintf((*vd).sde[ie], "  dataa <= x(PG_WIDTH-3 downto 0);                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  datab <= y(PG_WIDTH-3 downto 0);                                 \n" );ie++;  
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;  
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-2,LPM_PIPELINE=>%d,\n", nstage );ie++;
    sprintf((*vd).sde[ie], "                              LPM_DIRECTION=>PG_MULDIV)           \n" );ie++;
    sprintf((*vd).sde[ie], "       port map(dataa=>dataa,datab=>datab,clock=>clk,result=>result);\n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  z(PG_WIDTH-3 downto 0) <= result;                                \n" );ie++;
  }
  sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;
  (*vd).ie = ie;
  }
}  














