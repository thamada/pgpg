/*
  Sub modules used in pgpg_modules.
  This header file must be included at "pgpg.c".
  Last modified at July 1, 2003.

  --- SUB MODULE LIST ---
  void itobit(int idata, char sdata[], int nbit); // hamada version
  void itobc(int idata, char sdata[], int nbit);  // fukushige version @ pgpg.c
  void generate_pg_table(vd);
  void generate_pg_reged_penc(vd,nbit_in,nbit_out,npipe);
  void generate_pg_reged_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl);
  void generate_pg_penc(vd,nbit_in,nbit_out);
  void generate_pg_shift_ftol_round(vd,nbit_in,nbit_out,nbit_ctl);
  void generate_pg_unreg_table(vd);
  void generate_reged_add_sub(vd);
  void generate_unreg_add_sub(vd);
  void generate_reged_add_sub_cout(vd);
  void generate_unreg_add_sub_cout(vd);
  void generate_pg_reged_mult(vd);
  void generate_pg_unreg_mult(vd);
  void generate_pg_reged_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_unreg_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_shift_fixtofloat_withSbitGbit(vd,nbit_in,nbit_out,nbit_ctl,nstage);

*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // exit()
#include <string.h> // strcmp(),strcpy()
#include "pgpg.h"


/* hamada version of itobc (itobc is f version) */
void i2bit(idata,sdata,nbit)
int idata;
char sdata[];
int nbit;
{
  int i,itmp;
  strcpy(sdata,"");
  for(i=0;i<nbit;i++){
    itmp =0x1&(idata>>(nbit-1-i));
    if(itmp == 1) strcat(sdata,"1");
    if(itmp == 0) strcat(sdata,"0");
  }
}


// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_table(vd)
struct vhdl_description *vd;
{
  int ie;
  static int flag=0;

  if(flag==0){
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity table is                                                     \n" );ie++;
  sprintf((*vd).sde[ie], "  generic(IN_WIDTH: integer := 7;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "       OUT_WIDTH: integer := 5;                                     \n" );ie++;
  sprintf((*vd).sde[ie], "       TABLE_FILE: string := \"ftol.mif\");                         \n" );ie++;
  sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);           \n" );ie++;
  sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        \n" );ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);                                         \n" );ie++;
  sprintf((*vd).sde[ie], "end table;                                                          \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "architecture rtl of table is                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  component lpm_rom                                                 \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                                 \n" );ie++;
  sprintf((*vd).sde[ie], "          LPM_ADDRESS_CONTROL: STRING;                              \n" );ie++;
  sprintf((*vd).sde[ie], "          LPM_FILE: STRING);                                        \n" );ie++;
  sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);      \n" );ie++;
  sprintf((*vd).sde[ie], "         outclock: in std_logic;                                    \n" );ie++;
  sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));            \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_rom generic map (LPM_WIDTH=>OUT_WIDTH,                    \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>IN_WIDTH,                   \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\",   \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_FILE=>TABLE_FILE)                    \n" );ie++;
  sprintf((*vd).sde[ie], "  port map(address=>indata,outclock=>clk,q=>outdata);               \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  (*vd).ie = ie;
  flag = 1;
  }
}



// pgpg_conv_ftol.c
void generate_pg_reged_penc(vd,nbit_in,nbit_out,npipe)
struct vhdl_description *vd;
int nbit_in,nbit_out,npipe;
{
  int ie;

  if(npipe != 1){fprintf(stderr,"pg_reged_penc is not support npipe>1.\n"); exit(1);}
  if(nbit_in != 31){fprintf(stderr,"pg_reged_penc is not support nbit_in != 31.\n"); exit(1);}
  if(nbit_out != 5){fprintf(stderr,"pg_reged_penc is not support nbit_out != 5.\n"); exit(1);}

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n");ie++;
  sprintf((*vd).sde[ie], "-- The Pipelined Priority Encoder                                   \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                                     \n");ie++;
  sprintf((*vd).sde[ie], "-- 81 LEs, 185.67Mhz                                                \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n");ie++;
  sprintf((*vd).sde[ie], "                                                                    \n");ie++;
  sprintf((*vd).sde[ie], "entity reged_penc_31_5_1 is                        \n");ie++;
  sprintf((*vd).sde[ie], "port( a : in std_logic_vector(30 downto 0);        \n");ie++;
  sprintf((*vd).sde[ie], "      clock: in std_logic;                         \n");ie++;
  sprintf((*vd).sde[ie], "      c : out std_logic_vector(4 downto 0));       \n");ie++;
  sprintf((*vd).sde[ie], "end reged_penc_31_5_1;                             \n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "architecture rtl of reged_penc_31_5_1 is           \n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  signal ah0 : std_logic_vector(13 downto 0);      \n");ie++;
  sprintf((*vd).sde[ie], "  signal al0,al1 : std_logic_vector(16 downto 0);  \n");ie++;
  sprintf((*vd).sde[ie], "  signal o0,o1 : std_logic_vector(3 downto 0);     \n");ie++;
  sprintf((*vd).sde[ie], "  signal o1_nonz : std_logic;                      \n");ie++;
  sprintf((*vd).sde[ie], "  signal oo0,oo1,o2 : std_logic_vector(4 downto 0);\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "begin                                              \n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  c <= o2;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  ah0 <= a(30 downto 17);\n");ie++;
  sprintf((*vd).sde[ie], "  al0 <= a(16 downto 0);\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  process(clock) begin\n");ie++;
  sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      al1 <= al0;\n");ie++;
  sprintf((*vd).sde[ie], "      o1 <= o0; \n");ie++;
  sprintf((*vd).sde[ie], "    end if;\n");ie++;
  sprintf((*vd).sde[ie], "  end process;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  o1_nonz <= o1(3) or o1(2) or o1(1) or o1(0);\n");ie++;
  sprintf((*vd).sde[ie], "  oo1 <= '1' & o1;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  with o1_nonz select\n");ie++;
  sprintf((*vd).sde[ie], "      o2 <= oo0 when '0',\n");ie++;
  sprintf((*vd).sde[ie], "            oo1 when others;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- second stage\n");ie++;
  sprintf((*vd).sde[ie], "  with al1(16) select\n");ie++;
  sprintf((*vd).sde[ie], "    oo0(4) <= '1' when '1',\n");ie++;
  sprintf((*vd).sde[ie], "              '0' when others;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  process(al1(15 downto 0)) begin\n");ie++;
  sprintf((*vd).sde[ie], "    if(al1(15)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1111\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(14)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1110\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(13)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1101\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(12)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1100\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(11)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1011\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(10)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1010\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(9)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1001\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(8)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"1000\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(7)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0111\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(6)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0110\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(5)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0101\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(4)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0100\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(3)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0011\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(2)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0010\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(al1(1)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0001\";\n");ie++;
  sprintf((*vd).sde[ie], "    else\n");ie++;
  sprintf((*vd).sde[ie], "      oo0(3 downto 0) <= \"0000\";\n");ie++;
  sprintf((*vd).sde[ie], "    end if;\n");ie++;
  sprintf((*vd).sde[ie], "  end process;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  -- first stage\n");ie++;
  sprintf((*vd).sde[ie], "  process(ah0) begin\n");ie++;
  sprintf((*vd).sde[ie], "    if(ah0(13)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1110\";       -- output <= '1' & \"1110\"\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(12)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1101\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(11)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1100\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(10)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1011\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(9)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1010\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(8)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1001\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(7)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"1000\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(6)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0111\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(5)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0110\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(4)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0101\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(3)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0100\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(2)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0011\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(1)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0010\";\n");ie++;
  sprintf((*vd).sde[ie], "    elsif(ah0(0)='1') then\n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0001\";\n");ie++;
  sprintf((*vd).sde[ie], "    else         \n");ie++;
  sprintf((*vd).sde[ie], "      o0 <= \"0000\";          -- ah = \"00000000000000\"\n");ie++;
  sprintf((*vd).sde[ie], "    end if;\n");ie++;
  sprintf((*vd).sde[ie], "  end process;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "end rtl;\n");ie++;


  (*vd).ie = ie;
}

// pgpg_conv_ftol.c
void generate_pg_reged_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl,npipe)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl,npipe;
{
  int ie,j;

  if(npipe != 1){fprintf(stderr,"pg_reged_shift_ftol is not support npipe>1.\n"); exit(1);}
  if(nbit_ctl != 5){fprintf(stderr,"pg_reged_shift_ftol is not support nbit_ctl != 5.\n"); exit(1);}

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- The Pipelined Shifter for Fix to Log Converter                   \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Jul 1,2003                                      \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity reged_shift_ftol_%d_%d_%d is\n", nbit_in,nbit_out,npipe );ie++;
  for(j=0;j<(ie-1);j++){if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) return;}

  sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);       \n", nbit_in-1 );ie++;
  sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);     \n", nbit_ctl-1 );ie++;
  sprintf((*vd).sde[ie], "        clock : in std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));   \n", nbit_out-1 );ie++;
  sprintf((*vd).sde[ie], "end reged_shift_ftol_%d_%d_%d;\n", nbit_in,nbit_out,npipe);ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "architecture rtl of reged_shift_ftol_%d_%d_%d is\n", nbit_in, nbit_out, npipe );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  signal cntd_4 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(%d downto 0);    \n", nbit_in+nbit_out-1 );ie++;
  {
    int c,cmax;
    cmax = (0x1<<nbit_ctl)-1;
    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(c=0;c<cmax;c++){sprintf((*vd).sde[ie], "s%d,",c);ie++;}
    sprintf((*vd).sde[ie], "s%d : std_logic_vector(%d downto 0);\n",cmax,nbit_out-1);ie++;
  }
  sprintf((*vd).sde[ie], "  signal c0xxxx, c1xxxx : std_logic_vector(%d downto 0);\n",nbit_out-1 );ie++;
  sprintf((*vd).sde[ie], "  signal c0xxxxd,c1xxxxd : std_logic_vector(%d downto 0);\n",nbit_out-1 );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "begin\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "  with cntd_4 select               \n");ie++;
  sprintf((*vd).sde[ie], "    outdata <= c0xxxxd when '0',   \n");ie++;
  sprintf((*vd).sde[ie], "               c1xxxxd when others;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  process(clock) begin                 \n");ie++;
  sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then \n");ie++;
  sprintf((*vd).sde[ie], "      cntd_4 <= control(4);           \n");ie++;
  sprintf((*vd).sde[ie], "      c0xxxxd <= c0xxxx;               \n");ie++;
  sprintf((*vd).sde[ie], "      c1xxxxd <= c1xxxx;               \n");ie++;
  sprintf((*vd).sde[ie], "    end if;                            \n");ie++;
  sprintf((*vd).sde[ie], "  end process;                         \n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;

  /*--- change non-rounding version (2003/07/07) ---*/
  //  {
  //    char sd[100];
  //    i2bit(0,sd,nbit_out-1);
  //    sprintf((*vd).sde[ie], "  d0 <= indata & \"1%s\";\n", sd );ie++;
  //  }
  {
    char sd[100];
    i2bit(0,sd,nbit_out);
    sprintf((*vd).sde[ie], "  d0 <= indata & \"%s\";\n", sd );ie++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;


  {
    int c,cmax;
    cmax = (0x1<<nbit_ctl)-1;
    for(c=0;c<cmax;c++){
      sprintf((*vd).sde[ie], "  s%d <= d0(%d downto %d);\n",c,nbit_out+c-1,c);ie++;
    }
    sprintf((*vd).sde[ie], "  s%d <= '0' & d0(%d downto %d);\n",cmax,nbit_out+cmax-2,c);ie++;    
  }
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "-- Specified to LCELL 4-bit inputs.\n");ie++;
  sprintf((*vd).sde[ie], "  with control(3 downto 0) select\n");ie++;
  sprintf((*vd).sde[ie], "    c0xxxx <= s0 when \"0000\",  \n");ie++;
  sprintf((*vd).sde[ie], "           s1 when \"0001\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s2 when \"0010\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s3 when \"0011\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s4 when \"0100\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s5 when \"0101\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s6 when \"0110\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s7 when \"0111\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s8 when \"1000\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s9 when \"1001\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s10 when \"1010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s11 when \"1011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s12 when \"1100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s13 when \"1101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s14 when \"1110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s15 when others;      \n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  with control(3 downto 0) select\n");ie++;
  sprintf((*vd).sde[ie], "    c1xxxx <= s16 when \"0000\", \n");ie++;
  sprintf((*vd).sde[ie], "           s17 when \"0001\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s18 when \"0010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s19 when \"0011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s20 when \"0100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s21 when \"0101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s22 when \"0110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s23 when \"0111\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s24 when \"1000\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s25 when \"1001\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s26 when \"1010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s27 when \"1011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s28 when \"1100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s29 when \"1101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s30 when \"1110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s31 when others;      \n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
  (*vd).ie = ie;
}


// pgpg_conv_ftol.c
void generate_pg_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl;
{
  int ie,j;
  char sd[STRLEN];
  int iflag;

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity shift_ftol_%d_%d is                       \n", nbit_in, nbit_out );ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);       \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);     \n", nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));   \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end shift_ftol_%d_%d;                            \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of shift_ftol_%d_%d is          \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  component lpm_clshift                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH : POSITIVE;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_WIDTHDIST : POSITIVE);                             \n" );ie++;
    sprintf((*vd).sde[ie], "    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "         direction : in std_logic;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(%d downto 0);    \n", nbit_in+nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "  signal o0 : std_logic_vector(%d downto 0);    \n", nbit_in+nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "  signal one : std_logic;                                           \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  one <= '1';                                                       \n" );ie++;
    i2bit(0,sd,nbit_out);
    sprintf((*vd).sde[ie], "  d0 <= indata & \"%s\";                                        \n", sd );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>%d,LPM_WIDTHDIST=>%d)\n",
                                                                           nbit_in+nbit_out,nbit_ctl);ie++;
    sprintf((*vd).sde[ie], "         port map (data=>d0,distance=>control,result=>o0,direction=>one);\n" );ie++;
    sprintf((*vd).sde[ie], "  outdata <= o0(%d downto 0);                           \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;
  }
}

// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_penc(vd,nbit_in,nbit_out)
struct vhdl_description *vd;
int nbit_in,nbit_out;
{
  int ii,ie,iflag,j;
  char sd[STRLEN];

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity penc_%d_%d is                             \n", nbit_in, nbit_out );ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "port( a : in std_logic_vector(%d downto 0);              \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "      c : out std_logic_vector(%d downto 0));           \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end penc_%d_%d;                                  \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of penc_%d_%d is                \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  process(a) begin                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "    if(a(%d)='1') then                                    \n",nbit_in-1 );ie++;
    i2bit(nbit_in-1,sd,nbit_out);
    sprintf((*vd).sde[ie], "      c <= \"%s\";                                              \n", sd );ie++;
    for(ii=(nbit_in-2);ii>0;ii--){
      sprintf((*vd).sde[ie], "    elsif(a(%d)='1') then                                      \n",ii );ie++;
      i2bit(ii,sd,nbit_out);
      sprintf((*vd).sde[ie], "      c <= \"%s\";                                             \n",sd );ie++;
    }
    sprintf((*vd).sde[ie], "    else                                                            \n" );ie++;
    i2bit(ii,sd,nbit_out);
    sprintf((*vd).sde[ie], "      c <= \"%s\";                                               \n",sd );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;
  }
}

// 2003/07/01 not used anywhere
void generate_pg_shift_ftol_round(vd,nbit_in,nbit_out,nbit_ctl)
     struct vhdl_description *vd;
     int nbit_in,nbit_out,nbit_ctl;
{
  int i,ie,j;
  int iflag;

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- The unpipelined Shifter(with rounding) for Fix to Log Converter  \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                                     \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity shift_ftol_round_%d_%d is                       \n", nbit_in, nbit_out );ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);       \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);     \n", nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));   \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end shift_ftol_round_%d_%d;                            \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of shift_ftol_round_%d_%d is          \n", nbit_in, nbit_out );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  component lpm_clshift                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH : POSITIVE;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_WIDTHDIST : POSITIVE);                             \n" );ie++;
    sprintf((*vd).sde[ie], "    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "         direction : in std_logic;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(%d downto 0);    \n", nbit_in+nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "  signal o0 : std_logic_vector(%d downto 0);    \n", nbit_in+nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "  signal one : std_logic;                                           \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  one <= '1';                                                       \n" );ie++;
    {
      char _half[256];
      strcpy(_half,"");
      strcat(_half,"1");
      for(i=0;i<(nbit_out-1);i++) strcat(_half,"0");
      sprintf((*vd).sde[ie], "  d0 <= indata & \"%s\"; -- 2003/03/31\n",_half);ie++;
    }
    sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>%d,LPM_WIDTHDIST=>%d)\n",
	    nbit_in+nbit_out,nbit_ctl);ie++;
    sprintf((*vd).sde[ie], "         port map (data=>d0,distance=>control,result=>o0,direction=>one);\n" );ie++;
    sprintf((*vd).sde[ie], "  outdata <= o0(%d downto 0);                           \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;
  }
}


// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_unreg_table(vd)
struct vhdl_description *vd;
{
  int ie;
  static int flag=0;

  if(flag==0){
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity unreg_table is                                               \n" );ie++;
  sprintf((*vd).sde[ie], "  generic(IN_WIDTH: integer := 7;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "       OUT_WIDTH: integer := 5;                                     \n" );ie++;
  sprintf((*vd).sde[ie], "       TABLE_FILE: string := \"ftol.mif\");                         \n" );ie++;
  sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);           \n" );ie++;
  sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        \n" );ie++;
  sprintf((*vd).sde[ie], "       clk : in std_logic);                                         \n" );ie++;
  sprintf((*vd).sde[ie], "end unreg_table;                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "architecture rtl of unreg_table is                                  \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  component lpm_rom                                                 \n" );ie++;
  sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: POSITIVE;                                   \n" );ie++;
  sprintf((*vd).sde[ie], "             LPM_WIDTHAD: POSITIVE;                                 \n" );ie++;
  sprintf((*vd).sde[ie], "          LPM_ADDRESS_CONTROL: STRING;                              \n" );ie++;
  sprintf((*vd).sde[ie], "          LPM_OUTDATA: STRING;                                      \n" );ie++;
  sprintf((*vd).sde[ie], "          LPM_FILE: STRING);                                        \n" );ie++;
  sprintf((*vd).sde[ie], "   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);      \n" );ie++;
  sprintf((*vd).sde[ie], "         q: out std_logic_vector(LPM_WIDTH-1 downto 0));            \n" );ie++;
  sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "  u1: lpm_rom generic map (LPM_WIDTH=>OUT_WIDTH,                    \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_WIDTHAD=>IN_WIDTH,                   \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_ADDRESS_CONTROL=>\"UNREGISTERED\",   \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_OUTDATA=>\"UNREGISTERED\",           \n" );ie++;
  sprintf((*vd).sde[ie], "                           LPM_FILE=>TABLE_FILE)                    \n" );ie++;
  sprintf((*vd).sde[ie], "  port map(address=>indata,q=>outdata);                             \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  (*vd).ie = ie;
  flag = 1;
  }
}

// pgpg_log_unsigned_add_itp.c
// pgpg_conv_ftol_itp.c
void generate_reged_add_sub(vd) /* 2003/03/31 by T.Hamada */
     struct vhdl_description *vd;
{
  int ie,j;
  int iflag;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- The Pipelined Add/Sub (LPM Wrapper)                              \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                                     \n");ie++;
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
    sprintf((*vd).sde[ie], "LPM_WIDTH=>WIDTH,LPM_DIRECTION=>DIRECTION,LPM_PIPELINE=>PIPELINE)\n");ie++;
    sprintf((*vd).sde[ie], "  PORT MAP(dataa=>dataa,datab=>datab,result=>result,clock=>clock);\n" );ie++;
    sprintf((*vd).sde[ie], "END rtl;\n\n");ie++;
    (*vd).ie = ie;
  }
}

// pgpg_conv_ltof_itp.c
void generate_unreg_add_sub(vd)
     struct vhdl_description *vd;
{
  int ie,j;
  int iflag;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "-- The Unregisterd Add/Sub (LPM Wrapper)\n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada\n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003\n");ie++;
  sprintf((*vd).sde[ie], "library ieee;\n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "ENTITY unreg_add_sub IS\n");ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  GENERIC(WIDTH : INTEGER;DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0));\n" );ie++;
    sprintf((*vd).sde[ie], "END unreg_add_sub;\n");ie++;
    sprintf((*vd).sde[ie], "ARCHITECTURE rtl OF unreg_add_sub IS\n");ie++;
    sprintf((*vd).sde[ie], "  COMPONENT lpm_add_sub\n" );ie++;
    sprintf((*vd).sde[ie], "    GENERIC (LPM_WIDTH: INTEGER;    \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));\n");ie++;
    sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
    sprintf((*vd).sde[ie], "BEGIN\n");ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub GENERIC MAP(");ie++;
    sprintf((*vd).sde[ie], "LPM_WIDTH=>WIDTH,LPM_DIRECTION=>DIRECTION)\n");ie++;
    sprintf((*vd).sde[ie], "  PORT MAP(dataa=>dataa,datab=>datab,result=>result);\n" );ie++;
    sprintf((*vd).sde[ie], "END rtl;\n\n");ie++;
    (*vd).ie = ie;
  }
}

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_reged_add_sub_cout(vd) /* 2003/03/31 by T.Hamada */
     struct vhdl_description *vd;
{
  int ie,j;
  int iflag;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- The Pipelined Add/Sub with carry-out (LPM Wrapper)               \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                                     \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "ENTITY reged_add_sub_cout IS\n");ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  GENERIC(WIDTH : INTEGER;DIRECTION: STRING;PIPELINE: INTEGER);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          cout: out std_logic);\n");ie++;
    sprintf((*vd).sde[ie], "END reged_add_sub_cout;\n");ie++;
    sprintf((*vd).sde[ie], "ARCHITECTURE rtl OF reged_add_sub_cout IS\n");ie++;
    sprintf((*vd).sde[ie], "  COMPONENT lpm_add_sub\n" );ie++;
    sprintf((*vd).sde[ie], "    GENERIC (LPM_WIDTH: INTEGER;    \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_PIPELINE: INTEGER; \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          clock: in std_logic;\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          cout: out std_logic);\n");ie++;
    sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
    sprintf((*vd).sde[ie], "BEGIN\n");ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub GENERIC MAP(");ie++;
    sprintf((*vd).sde[ie], "LPM_WIDTH=>WIDTH,LPM_DIRECTION=>DIRECTION,LPM_PIPELINE=>PIPELINE)\n");ie++;
    sprintf((*vd).sde[ie], "  PORT MAP(dataa=>dataa,datab=>datab,result=>result,clock=>clock,cout=>cout);\n" );ie++;
    sprintf((*vd).sde[ie], "END rtl;\n\n");ie++;
    (*vd).ie = ie;
  }
}

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_unreg_add_sub_cout(vd) /* 2003/03/31 by T.Hamada */
     struct vhdl_description *vd;
{
  int ie,j;
  int iflag;
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- The Unpipelined Add/Sub with carry-out (LPM Wrapper)             \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                                     \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "ENTITY unreg_add_sub_cout IS\n");ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sde[ie], "  GENERIC(WIDTH : INTEGER;DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "          cout: out std_logic);\n");ie++;
    sprintf((*vd).sde[ie], "END unreg_add_sub_cout;\n");ie++;
    sprintf((*vd).sde[ie], "ARCHITECTURE rtl OF unreg_add_sub_cout IS\n");ie++;
    sprintf((*vd).sde[ie], "  COMPONENT lpm_add_sub\n" );ie++;
    sprintf((*vd).sde[ie], "    GENERIC (LPM_WIDTH: INTEGER;    \n");ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: STRING);\n");ie++;
    sprintf((*vd).sde[ie], "    PORT (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0);\n");ie++;
    sprintf((*vd).sde[ie], "          cout: out std_logic);\n");ie++;
    sprintf((*vd).sde[ie], "  END COMPONENT;\n" );ie++;
    sprintf((*vd).sde[ie], "BEGIN\n");ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub GENERIC MAP(");ie++;
    sprintf((*vd).sde[ie], "LPM_WIDTH=>WIDTH,LPM_DIRECTION=>DIRECTION)\n");ie++;
    sprintf((*vd).sde[ie], "  PORT MAP(dataa=>dataa,datab=>datab,result=>result,cout=>cout);\n" );ie++;
    sprintf((*vd).sde[ie], "END rtl;\n\n");ie++;
    (*vd).ie = ie;
  }
}

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_pg_reged_mult(vd) /* 2003/03/31 by T.Hamada */
     struct vhdl_description *vd;
{
  int ie;
  static int flag=0;

  if(flag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "-- The Pipelined Multiplier (LPM Wrapper)                   \n");ie++;
    sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                  \n");ie++;
    sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                             \n");ie++;
    sprintf((*vd).sde[ie], "library ieee;                                               \n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "entity reged_mult is                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  generic(IN_WIDTHA: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          IN_WIDTHB: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          OUT_WIDTH: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          REPRESENTATION: string :=\"UNSIGNED\";            \n" );ie++;
    sprintf((*vd).sde[ie], "          PIPELINE : integer);                              \n" );ie++;
    sprintf((*vd).sde[ie], "  port(dataa : IN STD_LOGIC_VECTOR(IN_WIDTHA-1 downto 0);   \n" );ie++;
    sprintf((*vd).sde[ie], "       datab : IN STD_LOGIC_VECTOR (IN_WIDTHB-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "       result : OUT STD_LOGIC_VECTOR (OUT_WIDTH-1 downto 0);\n" );ie++;
    sprintf((*vd).sde[ie], "       clock : in std_logic);                               \n" );ie++;
    sprintf((*vd).sde[ie], "end reged_mult;                                             \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of reged_mult is                           \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
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
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_mult generic map (              \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHA=>IN_WIDTHA, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHB=>IN_WIDTHB, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHP=>OUT_WIDTH, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHS=>OUT_WIDTH, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_TYPE=>\"LPM_MULT\",\n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_REPRESENTATION=>REPRESENTATION,\n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_PIPELINE=>PIPELINE,\n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_HINT=>\"MAXIMIZE_SPEED=1\")\n");ie++;
    sprintf((*vd).sde[ie], "               port map(clock=>clock,dataa=>dataa,datab=>datab,result=>result);\n");ie++;
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    (*vd).ie = ie;
    flag = 1;
  }
}

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_pg_unreg_mult(vd) /* 2003/03/31 by T.Hamada */
     struct vhdl_description *vd;
{
  int ie;
  static int flag=0;

  if(flag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "-- The Unpipelined Multiplier (LPM Wrapper)                 \n");ie++;
    sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                  \n");ie++;
    sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003                             \n");ie++;
    sprintf((*vd).sde[ie], "library ieee;                                               \n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "entity unreg_mult is                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  generic(IN_WIDTHA: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          IN_WIDTHB: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          OUT_WIDTH: integer;                               \n" );ie++;
    sprintf((*vd).sde[ie], "          REPRESENTATION: string :=\"UNSIGNED\");           \n" );ie++;
    sprintf((*vd).sde[ie], "  port(dataa : IN STD_LOGIC_VECTOR(IN_WIDTHA-1 downto 0);   \n" );ie++;
    sprintf((*vd).sde[ie], "       datab : IN STD_LOGIC_VECTOR (IN_WIDTHB-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "       result : OUT STD_LOGIC_VECTOR (OUT_WIDTH-1 downto 0));\n" );ie++;
    sprintf((*vd).sde[ie], "end unreg_mult;                                             \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of unreg_mult is                           \n" );ie++;
    sprintf((*vd).sde[ie], "                                                            \n" );ie++;
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
    sprintf((*vd).sde[ie], "begin\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_mult generic map (              \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHA=>IN_WIDTHA, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHB=>IN_WIDTHB, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHP=>OUT_WIDTH, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_WIDTHS=>OUT_WIDTH, \n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_TYPE=>\"LPM_MULT\",\n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_REPRESENTATION=>REPRESENTATION,\n");ie++;
    sprintf((*vd).sde[ie], "                   LPM_HINT=>\"MAXIMIZE_SPEED=1\")\n");ie++;
    sprintf((*vd).sde[ie], "               port map(dataa=>dataa,datab=>datab,result=>result);\n");ie++;
    sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    (*vd).ie = ie;
    flag = 1;
  }
}

// pg_conv_ltof_itp.c
void generate_pg_reged_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl,npipe)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl,npipe;
{
  int i,ie,j,nbit_sin,nbit_smax;
  int cnt_bit;
  int is_pipe[10];
  char sd[STRLEN];
  int iflag;
  double log();

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "-- Pipelined Shifter for Log to Fix Converter\n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                   \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Apr 11,2003              \n");ie++;
  sprintf((*vd).sde[ie], "                                             \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                \n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                 \n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "entity reged_shift_ltof_%d_%d_%d_%d is\n", nbit_in,nbit_ctl,nbit_out,npipe);ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    nbit_sin = nbit_out + 2*(nbit_in-1);
    nbit_smax =  nbit_in + nbit_out - 1;
    //    nbit_shift = (int)(log((double)(nbit_smax))/log(2.0) + 1.0);

    sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);       \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);     \n", nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "        clk : in std_logic;                                         \n" );ie++;
    sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));   \n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end reged_shift_ltof_%d_%d_%d_%d;\n", nbit_in,nbit_ctl,nbit_out,npipe );ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of reged_shift_ltof_%d_%d_%d_%d is\n",nbit_in,nbit_ctl,nbit_out,npipe);ie++; 
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;

    cnt_bit = (int)(log((double)nbit_out)/log(2.0) + 1.0);
    {
      int nbit_inc=0;
      for(i=0;i<cnt_bit;i++){
	sprintf((*vd).sde[ie], "  signal c%d : std_logic_vector(%d downto 0);\n",i,cnt_bit-1 );ie++;
      }
      for(i=0;i<=cnt_bit;i++){
	if(i>0) nbit_inc += (0x1<<(i-1));
	sprintf((*vd).sde[ie], "  signal o%d,o%dd : std_logic_vector(%d downto 0);\n",i,i,nbit_in+nbit_inc-1 );ie++;
      }
    }

    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;

    i2bit(0,sd,nbit_out-1);
    sprintf((*vd).sde[ie], "  c0 <= control(%d downto 0);\n",cnt_bit-1);ie++;
    sprintf((*vd).sde[ie], "  outdata <= o%d(%d downto %d);\n",cnt_bit,nbit_out+nbit_in-2,nbit_in-1);ie++;
    sprintf((*vd).sde[ie], "\n");ie++;

    sprintf((*vd).sde[ie], "  o0d <= indata;\n");ie++;

    for(i=0;i<cnt_bit;i++){
      int k;
      char zero[512];
      strcpy(zero,"");
      if(i!=0) strcat(zero,"\""); else strcat(zero,"'");
      for(k=0;k<(0x1<<i);k++) strcat(zero,"0");
      if(i!=0) strcat(zero,"\""); else strcat(zero,"'");
      sprintf((*vd).sde[ie], "  with c%d(%d) select\n",i,i);ie++;
      sprintf((*vd).sde[ie], "    o%d <= o%dd & %s when '1',\n",i+1,i,zero);ie++;
      sprintf((*vd).sde[ie], "          %s & o%dd when others;\n",zero,i);ie++;
      sprintf((*vd).sde[ie], "\n");ie++;
    }

    /* Err: npipe must be under (cnt_bit-1). */
    if(npipe > cnt_bit-1){
      fprintf(stderr,"Err: at reged_shift_ltof.\n npipe %d is over %d.\n",npipe,cnt_bit-1);exit(1);
    }
 
    for(i=0;i<cnt_bit-npipe-1;i++) is_pipe[i] = 0;
    for(i=cnt_bit-npipe-1;i<cnt_bit-1;i++) is_pipe[i] = 1;

    for(i=0;i<cnt_bit-1;i++){
      if(is_pipe[i]){
	sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
	sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then \n");ie++;
	sprintf((*vd).sde[ie], "      o%dd <= o%d;\n",i+1,i+1);ie++;
	sprintf((*vd).sde[ie], "      c%d <= c%d;\n",i+1,i);ie++;
	sprintf((*vd).sde[ie], "    end if;\n");ie++;
	sprintf((*vd).sde[ie], "  end process;\n\n");ie++;
      }else{
	sprintf((*vd).sde[ie], "--  process(clk) begin\n");ie++;
	sprintf((*vd).sde[ie], "--    if(clk'event and clk='1') then \n");ie++;
	sprintf((*vd).sde[ie], "      o%dd <= o%d;\n",i+1,i+1);ie++;
	sprintf((*vd).sde[ie], "      c%d <= c%d;\n",i+1,i);ie++;
	sprintf((*vd).sde[ie], "--    end if;\n");ie++;
	sprintf((*vd).sde[ie], "--  end process;\n\n");ie++;
      }
    }

    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;  
  }
}

// pg_conv_ltof_itp.c
void generate_pg_unreg_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl;
{
  int ie,j,nbit_sin,nbit_smax;
  char sd[STRLEN];
  int iflag;
  //  double log();
  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "-- Unpipelined Shifter for Log to Fix Converter(using lpm_clshift)\n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                   \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Mar 31,2003              \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
  sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
  sprintf((*vd).sde[ie], "entity unreg_shift_ltof_%d_%d_%d is\n", nbit_in,nbit_ctl,nbit_out);ie++;
  iflag=0;
  for(j=0;j<(ie-1);j++){
    if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) iflag=1;
  }
  if(iflag==0){
    nbit_sin = nbit_out + 2*(nbit_in-1);
    nbit_smax =  nbit_in + nbit_out - 1;
    //    nbit_shift = (int)(log((double)(nbit_smax))/log(2.0) + 1.0);

    sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(%d downto 0);   \n", nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "        control : in std_logic_vector(%d downto 0);  \n", nbit_ctl-1 );ie++;
    sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(%d downto 0));\n", nbit_out-1 );ie++;
    sprintf((*vd).sde[ie], "end unreg_shift_ltof_%d_%d_%d;\n", nbit_in,nbit_ctl,nbit_out);ie++;
    sprintf((*vd).sde[ie], "\n" );ie++;
    sprintf((*vd).sde[ie], "architecture rtl of unreg_shift_ltof_%d_%d_%d is\n",nbit_in,nbit_ctl,nbit_out);ie++; 
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  component lpm_clshift                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH : POSITIVE;                                  \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_WIDTHDIST : POSITIVE);                             \n" );ie++;
    sprintf((*vd).sde[ie], "    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "  signal d0,o0 : std_logic_vector(%d downto 0);          \n",nbit_sin-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    i2bit(0,sd,nbit_sin-nbit_in);
    sprintf((*vd).sde[ie], "  d0 <= \"%s\" & indata;                                        \n", sd );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>%d,LPM_WIDTHDIST=>%d)\n", nbit_sin,nbit_ctl);ie++;
    sprintf((*vd).sde[ie], "          port map (data=>d0,distance=>control,result=>o0);\n");ie++;
    sprintf((*vd).sde[ie], "  outdata <= o0(%d downto %d);            \n",nbit_smax-1,nbit_in-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
    (*vd).ie = ie;  
  }
}


// pg_log_unsigned_add_itp.c
void generate_pg_lcell_rom_old(vd,ilen,olen,depth,nstage,name,long_type_rom)
struct vhdl_description *vd;
int ilen,olen,depth,nstage;
char* name;
long long int* long_type_rom;
{
  int i,ie,j;
  int nentry,imax;
  //  int rom[65536];
  int rom[131072];
  int lut_mask[256];
  int is_pipe[128];
  ie = (*vd).ie;

  // ------- PIPELINE MAP ----------------------------------
  // The lcell_rom has (ilen-4) register-points.
  // The lcell_rom can't have input/output register.
  for(i=0;i<=ilen-4;i++) is_pipe[i] = 0;
  // is_pipe[0] is not used.
  if(nstage==0){
    //    is_pipe[2] =1; //hogehoge
  }else if(nstage==1){
    is_pipe[ilen-4] =1; // this is the last reg-point( not output reg).
  }else{
    fprintf(stderr,"Err at %s ",__FILE__);fprintf(stderr,"line %d.\n",__LINE__);exit(1);
  }
  // ------- END PIPELINE MAP ------------------------------

  sprintf((*vd).sde[ie], "                                   \n");ie++;
  sprintf((*vd).sde[ie], "-- ROM using Lcell not ESB         \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada         \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at May 29,2003    \n");ie++;
  sprintf((*vd).sde[ie], "-- In %d Out %d Stage %d Type\"%s\"\n",ilen,olen,nstage,name);ie++;
  sprintf((*vd).sde[ie], "library ieee;                      \n" );ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;       \n" );ie++;
  sprintf((*vd).sde[ie], "library wysiwyg;                   \n" );ie++;
  sprintf((*vd).sde[ie], "use wysiwyg.apex20k_components.all;\n" );ie++;
  sprintf((*vd).sde[ie], "                                   \n" );ie++;
  sprintf((*vd).sde[ie], "entity lcell_rom_%s_%d_%d_%d is\n",name,ilen,olen,nstage);ie++;
  for(j=0;j<(ie-1);j++){if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) return;}

  // --- prepare the local values ---
  imax = 16 * (int)ceil(((double)depth)/16.0);
  nentry = 0x1<<ilen;
  if(depth>65536){
    printf("Err at %s ",__FILE__);
    printf("line %d.\n",__LINE__+3);
    exit(1);
  }
  for(i=0;i<depth;i++) rom[i] = (int)(long_type_rom[i]);
  // --------------------------------

  sprintf((*vd).sde[ie],"  port( indata : in std_logic_vector(%d downto 0);\n",ilen-1);ie++;
  sprintf((*vd).sde[ie],"        clk : in std_logic;\n");ie++;
  sprintf((*vd).sde[ie],"        outdata : out std_logic_vector(%d downto 0));\n",olen-1);ie++;
  sprintf((*vd).sde[ie],"end lcell_rom_%s_%d_%d_%d;\n",name,ilen,olen,nstage);ie++;
  sprintf((*vd).sde[ie],"\n");ie++;
  sprintf((*vd).sde[ie],"architecture rtl of lcell_rom_%s_%d_%d_%d is\n",name,ilen,olen,nstage);ie++; 
  sprintf((*vd).sde[ie],"\n");ie++;

  sprintf((*vd).sde[ie],"  component apex20k_lcell\n");ie++;
  sprintf((*vd).sde[ie],"    generic (operation_mode    : string := \"normal\";\n");ie++;
  sprintf((*vd).sde[ie],"        output_mode   : string := \"comb_and_reg\";\n");ie++;
  sprintf((*vd).sde[ie],"        packed_mode   : string := \"false\";\n");ie++;
  sprintf((*vd).sde[ie],"        lut_mask       : string := \"ffff\";\n");ie++;
  sprintf((*vd).sde[ie],"        power_up : string := \"low\";\n");ie++;
  sprintf((*vd).sde[ie],"        cin_used       : string := \"false\");\n");ie++;
  sprintf((*vd).sde[ie],"    port (clk     : in std_logic := '0';\n");ie++;
  sprintf((*vd).sde[ie],"          dataa     : in std_logic := '1';\n");ie++;
  sprintf((*vd).sde[ie],"          datab     : in std_logic := '1';\n");ie++;
  sprintf((*vd).sde[ie],"          datac     : in std_logic := '1';\n");ie++;
  sprintf((*vd).sde[ie],"          datad     : in std_logic := '1';\n");ie++;
  sprintf((*vd).sde[ie],"          aclr    : in std_logic := '0';\n");ie++;
  sprintf((*vd).sde[ie],"          ena : in std_logic := '1';\n");ie++;
  if(is_pipe[1]){
    sprintf((*vd).sde[ie],"          regout    : out std_logic);\n");ie++;
  }else{
    sprintf((*vd).sde[ie],"          combout   : out std_logic);\n");ie++;
  }
  sprintf((*vd).sde[ie],"  end component;\n\n");ie++;

  for(i=0;i<ilen-3;i++){
    sprintf((*vd).sde[ie],"  signal adr%d : std_logic_vector(%d downto 0);\n",i,ilen-1);ie++;
  }

  for(j=0;j<(nentry>>4);j++){
    sprintf((*vd).sde[ie],"  signal lc_%x_%x : std_logic_vector(%d downto 0);\n",4,j,olen-1);ie++;
  }
  for(i=5;i<=ilen;i++){
    for(j=0;j<(nentry>>i);j++){
      sprintf((*vd).sde[ie],"  signal lut_%x_%x,lc_%x_%x : std_logic_vector(%d downto 0);\n",i,j,i,j,olen-1);ie++;
    }
  }

  sprintf((*vd).sde[ie],"\n");ie++;
  sprintf((*vd).sde[ie],"begin\n\n");ie++;

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
	  sprintf((*vd).sde[ie],"--LC_%03X_%02d \n",block_num,j);ie++;
	  sprintf((*vd).sde[ie],"  lc_4_%x(%d) <= '0';\n",block_num,j);ie++;
	}else if(lut_mask[j] == 0xffff){
	  sprintf((*vd).sde[ie],"--LC_%03X_%02d \n",block_num,j);ie++;
	  sprintf((*vd).sde[ie],"  lc_4_%x(%d) <= '1';\n",block_num,j);ie++;
	}else{
	  if(is_pipe[1]){
	    sprintf((*vd).sde[ie],"  LC_%03X_%02d : apex20k_lcell\n",block_num,j);ie++;
	    sprintf((*vd).sde[ie],"  generic map(lut_mask=>\"%04X\")\n",lut_mask[j]);ie++;
	    sprintf((*vd).sde[ie],"  port map( dataa=>adr0(0),datab=>adr0(1),datac=>adr0(2),datad=>adr0(3),clk=>clk,\n");ie++;
	    sprintf((*vd).sde[ie],"            regout=>lc_4_%x(%d));\n",block_num,j);ie++;
	  }else{
	    sprintf((*vd).sde[ie],"  LC_%03X_%02d : apex20k_lcell\n",block_num,j);ie++;
	    sprintf((*vd).sde[ie],"  generic map(lut_mask=>\"%04X\")\n",lut_mask[j]);ie++;
	    sprintf((*vd).sde[ie],"  port map( dataa=>adr0(0),datab=>adr0(1),datac=>adr0(2),datad=>adr0(3),clk=>clk,\n");ie++;
	    sprintf((*vd).sde[ie],"            combout=>lc_4_%x(%d));\n",block_num,j);ie++;
	  }
	}
	sprintf((*vd).sde[ie],"\n");ie++;
      }
    }
  }
  // --- END LCell arrays ----------------------------------------------------


  // --- address (reg/wire) ---
  sprintf((*vd).sde[ie], "  adr0 <= indata;\n");ie++;
  for(i=0;i<ilen-4;i++){
    if(is_pipe[i+1]){
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      if(ilen == i+5){
	sprintf((*vd).sde[ie], "      adr%d(%d) <= adr%d(%d);\n",i+1,ilen-1,i,ilen-1);ie++;
      }else{
	sprintf((*vd).sde[ie], "      adr%d(%d downto %d) <= adr%d(%d downto %d);\n",i+1,ilen-1,i+4,i,ilen-1,i+4);ie++;
      }
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
    }else{
      if(ilen == i+5){
	sprintf((*vd).sde[ie], "  adr%d(%d) <= adr%d(%d);\n",i+1,ilen-1,i,ilen-1);ie++;
      }else{
	sprintf((*vd).sde[ie], "  adr%d(%d downto %d) <= adr%d(%d downto %d);\n",i+1,ilen-1,i+4,i,ilen-1,i+4);ie++;
      }
    }
  }

  // --- Next Selectors ---
  sprintf((*vd).sde[ie],"\n");ie++;
  for(i=5;i<=ilen;i++){
    sprintf((*vd).sde[ie],"--  =================================\n");ie++;
    for(j=0;j<(nentry>>(i-1));j+=2){
      sprintf((*vd).sde[ie],"  with adr%d(%d) select\n",i-4,i-1);ie++;
      sprintf((*vd).sde[ie],"    lut_%x_%x <= lc_%x_%x when '0',\n", i,j>>1,i-1,j);ie++;
      sprintf((*vd).sde[ie],"              lc_%x_%x when others;\n\n",i-1,j+1);ie++;
    }
  }

  // --- LUT register ---
  sprintf((*vd).sde[ie],"\n");ie++;
  for(i=5;i<=ilen;i++){
    sprintf((*vd).sde[ie],"--  =================================\n");ie++;
    if(is_pipe[i-3]){
      sprintf((*vd).sde[ie], "  process(clk) begin\n");ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then\n");ie++;
      for(j=0;j<(nentry>>(i-1));j+=2){
	sprintf((*vd).sde[ie],"      lc_%x_%x <= lut_%x_%x;\n",i,j>>1,i,j>>1);ie++;
      }
      sprintf((*vd).sde[ie], "    end if;\n");ie++;
      sprintf((*vd).sde[ie], "  end process;\n");ie++;
    }else{
      for(j=0;j<(nentry>>(i-1));j+=2){
	sprintf((*vd).sde[ie],"    lc_%x_%x <= lut_%x_%x;\n",i,j>>1,i,j>>1);ie++;
      }
    }
  }

  // --- output connection ---
  sprintf((*vd).sde[ie],"  outdata <= lc_%x_0;\n",ilen);ie++;


  sprintf((*vd).sde[ie],"end rtl;\n");ie++;
  (*vd).ie = ie;  
}



// pgpg_conv_fixtofloat.c
void generate_pg_shift_fixtofloat_withSbitGbit(vd,nbit_in,nbit_out,nbit_ctl,nstage)
struct vhdl_description *vd;
int nbit_in,nbit_out,nbit_ctl,nstage;
{
  int ie,j;
  char func_name[STRLEN];

  if(nstage > 1){fprintf(stderr,"pg_shift_fixtofloat is not support nstage>1.\n"); exit(1);}
  if(nbit_ctl != 5){fprintf(stderr,"pg_shift_fixtofloat is not support nbit_ctl != 5.\n"); exit(1);}

  sprintf(func_name,"shift_fixtofloat_%d_%d_%d_%d",nbit_in,nbit_out,nbit_ctl,nstage);

  ie = (*vd).ie;
  sprintf((*vd).sde[ie], "                                                                    \n");ie++;
  sprintf((*vd).sde[ie], "-- The Pipelined Shifter for Fix to Float Converter                 \n");ie++;
  sprintf((*vd).sde[ie], "-- Author: Tsuyoshi Hamada                                          \n");ie++;
  sprintf((*vd).sde[ie], "-- Last Modified at Dec 10,2003                                     \n");ie++;
  sprintf((*vd).sde[ie], "library ieee;                                                       \n");ie++;
  sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n");ie++;
  sprintf((*vd).sde[ie], "entity %s is\n",func_name);ie++;
  for(j=0;j<(ie-1);j++){if(strcmp((*vd).sde[ie-1],(*vd).sde[j])==0) return;}

  sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(%d downto 0);\n",nbit_in-1);ie++;
  sprintf((*vd).sde[ie], "       control: in std_logic_vector(%d downto 0);\n",nbit_ctl-1);ie++;
  sprintf((*vd).sde[ie], "       clock  : in std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "       outdata: out std_logic_vector(%d downto 0));\n",nbit_out-1);ie++;
  sprintf((*vd).sde[ie], "end %s;\n",func_name);ie++;
  sprintf((*vd).sde[ie], "architecture rtl of %s is\n",func_name);ie++;
  sprintf((*vd).sde[ie], "  signal cntd_4 : std_logic;\n");ie++;
  sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(%d downto 0);\n",nbit_in+nbit_out-1);ie++;
  {
    int c,cmax;
    cmax = (0x1<<nbit_ctl)-1;
    sprintf((*vd).sde[ie], "  signal ");ie++;
    for(c=0;c<cmax;c++){sprintf((*vd).sde[ie], "s%d,",c);ie++;}
    sprintf((*vd).sde[ie], "s%d : std_logic_vector(%d downto 0);\n",cmax,nbit_out-1);ie++;
  }
  sprintf((*vd).sde[ie], "  signal c0xxxx, c1xxxx : std_logic_vector(%d downto 0);\n",nbit_out-1 );ie++;
  sprintf((*vd).sde[ie], "  signal c0xxxxd,c1xxxxd : std_logic_vector(%d downto 0);\n",nbit_out-1 );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "begin\n" );ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "  with cntd_4 select               \n");ie++;
  sprintf((*vd).sde[ie], "    outdata <= c0xxxxd when '0',   \n");ie++;
  sprintf((*vd).sde[ie], "               c1xxxxd when others;\n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;

  if(nstage>1){
    sprintf((*vd).sde[ie], "  process(clock) begin\n");ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      cntd_4 <= control(4);\n");ie++;
    sprintf((*vd).sde[ie], "      c0xxxxd <= c0xxxx;\n");ie++;
    sprintf((*vd).sde[ie], "      c1xxxxd <= c1xxxx;\n");ie++;
    sprintf((*vd).sde[ie], "    end if;\n");ie++;
    sprintf((*vd).sde[ie], "  end process;\n");ie++;
  }else{
    sprintf((*vd).sde[ie], "--  process(clock) begin\n");ie++;
    sprintf((*vd).sde[ie], "--    if(clock'event and clock='1') then\n");ie++;
    sprintf((*vd).sde[ie], "      cntd_4 <= control(4);\n");ie++;
    sprintf((*vd).sde[ie], "      c0xxxxd <= c0xxxx;\n");ie++;
    sprintf((*vd).sde[ie], "      c1xxxxd <= c1xxxx;\n");ie++;
    sprintf((*vd).sde[ie], "--    end if;\n");ie++;
    sprintf((*vd).sde[ie], "--  end process;\n");ie++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;

  /*--- change non-rounding version (2003/07/07) ---*/
  //  {
  //    char sd[100];
  //    i2bit(0,sd,nbit_out-1);
  //    sprintf((*vd).sde[ie], "  d0 <= indata & \"1%s\";\n", sd );ie++;
  //  }
  {
    char sd[100];
    i2bit(0,sd,nbit_out);
    sprintf((*vd).sde[ie], "  d0 <= indata & \"%s\";\n", sd );ie++;
  }
  sprintf((*vd).sde[ie], "\n");ie++;


  {
    int c,cmax,k;
    char zero[STRLEN];
    cmax = (0x1<<nbit_ctl)-1;
    for(c=0;c<cmax;c++){
      char gbit[STRLEN];
      strcpy(gbit,"");
      for(k=c;k>0;k--){
	char buf[STRLEN];
	sprintf(buf," d0(%d) or",k);
	strcat(gbit,buf);
      }
      sprintf((*vd).sde[ie], "  s%d <= d0(%d downto %d)&(%s d0(0) );\n",c,nbit_out+c-1,c+1,gbit);ie++;
    }
    strcpy(zero,"");
    for(k=0;k<nbit_out;k++) strcat(zero,"0");
    sprintf((*vd).sde[ie], "  s%d <= \"%s\";\n",cmax,zero);ie++;    
  }
  sprintf((*vd).sde[ie], "\n");ie++;

  sprintf((*vd).sde[ie], "-- Specified to LCELL 4-bit inputs.\n");ie++;
  sprintf((*vd).sde[ie], "  with control(3 downto 0) select\n");ie++;
  sprintf((*vd).sde[ie], "    c0xxxx <= s0 when \"0000\",  \n");ie++;
  sprintf((*vd).sde[ie], "           s1 when \"0001\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s2 when \"0010\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s3 when \"0011\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s4 when \"0100\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s5 when \"0101\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s6 when \"0110\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s7 when \"0111\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s8 when \"1000\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s9 when \"1001\",     \n");ie++;
  sprintf((*vd).sde[ie], "           s10 when \"1010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s11 when \"1011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s12 when \"1100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s13 when \"1101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s14 when \"1110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s15 when others;      \n");ie++;
  sprintf((*vd).sde[ie], "\n");ie++;
  sprintf((*vd).sde[ie], "  with control(3 downto 0) select\n");ie++;
  sprintf((*vd).sde[ie], "    c1xxxx <= s16 when \"0000\", \n");ie++;
  sprintf((*vd).sde[ie], "           s17 when \"0001\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s18 when \"0010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s19 when \"0011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s20 when \"0100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s21 when \"0101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s22 when \"0110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s23 when \"0111\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s24 when \"1000\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s25 when \"1001\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s26 when \"1010\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s27 when \"1011\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s28 when \"1100\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s29 when \"1101\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s30 when \"1110\",    \n");ie++;
  sprintf((*vd).sde[ie], "           s31 when others;      \n");ie++;
  sprintf((*vd).sde[ie], "\n" );ie++;
  sprintf((*vd).sde[ie], "end rtl;\n" );ie++;
  (*vd).ie = ie;
}


// debug line
//  printf("Err at %s ",__FILE__);printf("line %d.\n",__LINE__);exit(1);
//
