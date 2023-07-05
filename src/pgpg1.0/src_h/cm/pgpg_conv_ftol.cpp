/* --- function list -----------------------------
       void PGPG_CONV_FTOL_CREATE_MIF::i2s(...);
       void PGPG_CONV_FTOL_CREATE_MIF::i2s_hex(...);
       void PGPG_CONV_FTOL_CREATE_MIF::generate_mif_file(...);
       void PGPG_CONV_FTOL_CREATE_MIF::create_table_input_file(...);
       void PGPG_CONV_FTOL::generate_pg_shift_ftol(...);
       void PGPG_CONV_FTOL::generate_pg_penc(...);
       void std::generate_pg_conv_ftol(...);
   -----------------------------------------------*/ 

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>

#include<string>
#include<fstream>
#include<cmath>

#include "pgpg.h"
namespace std{

  namespace PGPG_CONV_FTOL_CREATE_MIF{

    string i2s(int i){
      char strbuf[STRLEN];
      string s;
      sprintf(strbuf ,"%d",i);
      s = strbuf;
      return s;
    }

    string i2s_hex(int i){
      char strbuf[STRLEN];
      string s;
      sprintf(strbuf ,"%X",i);
      s = strbuf;
      return s;
    }

    void generate_mif_file(string &contents){

      string filename = "ftol.mif";  

      string basedir = "./";      // define base directory
      string fstring = basedir + filename; //filename(string)
      const char* fname;                           //filename(char *)
      // string to char* convert
      fname = new char[fstring.size()];
      fname = fstring.c_str();
      ofstream fout(fname); // file instance
      if(!fout){
	cout << "Error:\n";
	cout << "Cannot open output file.\n";
	cout << "Contact to hamada@provence.c.u-tokyo.ac.jp\n";
	exit(1) ;
      }
      fout << contents;
      cout << fname <<endl;
    }

    void create_table_input_file(int input_width,int output_width)
    {
      string mif_contents;
      int memory_depth = (int)(pow(2.0,(double)input_width));

      mif_contents = "-- *** table Memory Initialize File for PROGRAPE *****\n\n";
      mif_contents += "DEPTH = " + i2s(memory_depth) +";\n";
      mif_contents += "WIDTH = " + i2s(output_width) +";\n";
      mif_contents += "\n";
      mif_contents += "ADDRESS_RADIX = HEX;\n";
      mif_contents += "DATA_RADIX    = HEX;\n";
      mif_contents += "\n";
      mif_contents += "CONTENT\n";
      mif_contents += "  BEGIN\n";

      for(int input=0;input<memory_depth;input++){
	double d_fix = (double)input;
	double d_adr = 1.0 + (d_fix + 0.5)/pow(2.0,(double)input_width);
	double d_data = log(d_adr)/log(2.0);
	int output = (int)(pow(2.0,(double)output_width)*d_data +0.5);

	// overflow check--------------------------------------------
	int overflow_mask = 0x1<<output_width;
	int overflow      = (overflow_mask & output) >> output_width;
	//-----------------------------------------------------------

	if(overflow >0)
	  mif_contents += "    " + i2s_hex(input) + " : 0 ;\n";
	else
	  mif_contents += "    " + i2s_hex(input) + " : " + i2s_hex(output) + " ;\n";
      }
      mif_contents += "  END;\n";
      generate_mif_file(mif_contents);

    }
  }

  namespace PGPG_CONV_FTOL{
    void generate_pg_shift_ftol(vhdl_description *vd)
    {
      int ie;
      static int flag=1;

      if(flag==1){
	ie = (*vd).ie;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "entity shift_ftol is                                                \n" );ie++;
	sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(17 downto 0);                  \n" );ie++;
	sprintf((*vd).sde[ie], "        control : in std_logic_vector(4 downto 0);                  \n" );ie++;
	sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(6 downto 0));                \n" );ie++;
	sprintf((*vd).sde[ie], "end shift_ftol;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "architecture rtl of shift_ftol is                                   \n" );ie++;
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
	sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(24 downto 0);                        \n" );ie++;
	sprintf((*vd).sde[ie], "  signal o0 : std_logic_vector(24 downto 0);                        \n" );ie++;
	sprintf((*vd).sde[ie], "  signal one : std_logic;                                           \n" );ie++;
	sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  one <= '1';                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  d0 <= indata & \"0000000\";                                       \n" );ie++;
	sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>25,LPM_WIDTHDIST=>5)      \n" );ie++;
	sprintf((*vd).sde[ie], "                  port map (data=>d0,distance=>control,result=>o0,direction=>one);\n" );ie++;
	sprintf((*vd).sde[ie], "  outdata <= o0(6 downto 0);                                        \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
	(*vd).ie = ie;
	flag=0;
      }
    }
  }

  namespace PGPG_CONV_FTOL{
    void generate_pg_penc(vhdl_description *vd)
    {
      int ie;
      static int flag=1;

      if(flag==1){
	ie = (*vd).ie;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "entity penc is                                                      \n" );ie++;
	sprintf((*vd).sde[ie], "port( a : in std_logic_vector(18 downto 0);                         \n" );ie++;
	sprintf((*vd).sde[ie], "      c : out std_logic_vector(4 downto 0));                        \n" );ie++;
	sprintf((*vd).sde[ie], "end penc;                                                           \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "architecture rtl of penc is                                         \n" );ie++;
	sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  process(a) begin                                                  \n" );ie++;
	sprintf((*vd).sde[ie], "    if(a(18)='1') then                                              \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"10010\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(17)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"10001\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(16)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"10000\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(15)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01111\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(14)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01110\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(13)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01101\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(12)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01100\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(11)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01011\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(10)='1') then                                           \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01010\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(9)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01001\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(8)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"01000\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(7)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00111\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(6)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00110\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(5)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00101\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(4)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00100\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(3)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00011\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(2)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00010\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    elsif(a(1)='1') then                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00001\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    else                                                            \n" );ie++;
	sprintf((*vd).sde[ie], "      c <= \"00000\";                                               \n" );ie++;
	sprintf((*vd).sde[ie], "    end if;                                                         \n" );ie++;
	sprintf((*vd).sde[ie], "  end process;                                                      \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
	(*vd).ie = ie;
	flag=0;
      }

    }
  }

  void generate_pg_conv_ftol(char sdata[][STRLEN],vhdl_description *vd)
  {
    int i;
    int nbit_fix,nbit_log,nbit_man,nstage;
    int nbit_penc=0;
    char sx[STRLEN],sy[STRLEN];
    char sd[STRLEN];
    int ic,im,ie,is,iflag,j;

    strcpy(sx,sdata[1]);
    strcpy(sy,sdata[2]);
    nbit_fix = atoi(sdata[3]);
    nbit_log = atoi(sdata[4]);
    nbit_man = atoi(sdata[5]);
    nstage = atoi(sdata[6]);

    printf("conv ftol: %s = %s : ",sx,sy);
    printf("nbit fix %d log %d man %d nstage %d\n",nbit_fix,nbit_log,nbit_man,nstage);

    ic = (*vd).ic;
    sprintf((*vd).sdc[ic], "  component pg_conv_ftol_%d_%d_%d                       \n",nbit_fix,nbit_log,nbit_man);ic++;
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

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  u%d: pg_conv_ftol_%d_%d_%d ",     (*vd).ucnt,nbit_fix,nbit_log,nbit_man);im++;
    sprintf((*vd).sdm[im], "  port map (fixdata=>%s,logdata=>%s,clk=>pclk);                 \n",sx,sy);im++;
    sprintf((*vd).sdm[im], "                                                                     \n" );im++;   
    (*vd).im = im;
    (*vd).ucnt++;

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_fix-1);is++;
    (*vd).is = is;

    if(iflag==0){
      ie = (*vd).ie;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;   
      sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "entity pg_conv_ftol_%d_%d_%d is         \n",nbit_fix,nbit_log,nbit_man );ie++;   
      sprintf((*vd).sde[ie], "  port(fixdata : in std_logic_vector(%d downto 0);      \n", nbit_fix-1);ie++;     
      sprintf((*vd).sde[ie], "       logdata : out std_logic_vector(%d downto 0);     \n", nbit_log-1);ie++;     
      sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;     
      sprintf((*vd).sde[ie], "end pg_conv_ftol_%d_%d_%d;              \n",nbit_fix,nbit_log,nbit_man );ie++;     
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "architecture rtl of pg_conv_ftol_%d_%d_%d is \n", nbit_fix,nbit_log,nbit_man);ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;     
      sprintf((*vd).sde[ie], "  generic (IN_WIDTH: integer ;                                     \n" );ie++;     
      sprintf((*vd).sde[ie], "           OUT_WIDTH: integer ;                                    \n" );ie++;     
      sprintf((*vd).sde[ie], "           TABLE_FILE: string);                                    \n" );ie++;     
      sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;     
      sprintf((*vd).sde[ie], "      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        \n" );ie++;     
      sprintf((*vd).sde[ie], "      clk : in std_logic);                                         \n" );ie++;     
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;   
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;   
      sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;   
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;   
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  component penc                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "    port( a : in std_logic_vector(%d downto 0);        \n", nbit_fix-2 );ie++;   

      if(nbit_fix>32) nbit_penc = nbit_penc = 6;
      if((nbit_fix<33)&&(nbit_fix>16)) nbit_penc = 5;
      if((nbit_fix<17)&&(nbit_fix>8)) nbit_penc = 4;
      if(nbit_fix<9) nbit_penc = 3;

      sprintf((*vd).sde[ie], "          c : out std_logic_vector(%d downto 0));      \n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  component shift_ftol                                             \n" );ie++;   
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);   \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);  \n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0));\n", nbit_man+1 );ie++;   
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  signal d1,d0,d0r: std_logic_vector(%d downto 0);     \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "  signal d2: std_logic_vector(%d downto 0);            \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "  signal d3,d3r: std_logic_vector(%d downto 0);        \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "  signal one: std_logic_vector(%d downto 0);           \n", nbit_fix-2 );ie++;   
      sprintf((*vd).sde[ie], "  signal sign: std_logic;                                          \n" );ie++;   
      sprintf((*vd).sde[ie], "  signal c1: std_logic_vector(%d downto 0);            \n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "  signal d4: std_logic_vector(%d downto 0);            \n", nbit_fix-3 );ie++;   
      sprintf((*vd).sde[ie], "  signal c2,c3,c4,add: std_logic_vector(%d downto 0);  \n", nbit_penc-1);ie++;   
      sprintf((*vd).sde[ie], "  signal d5,d6: std_logic_vector(%d downto 0);         \n", nbit_man+1 );ie++;   
      sprintf((*vd).sde[ie], "  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                \n" );ie++;   
      sprintf((*vd).sde[ie], "  signal nz0,nz1,nz2: std_logic;                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "  d1 <=  NOT fixdata(%d downto 0);                     \n", nbit_fix-2 );ie++;   
      strcpy(sd,"");
      for(i=0;i<(nbit_fix-2);i++) strcat(sd,"0");
      strcat(sd,"1");
      sprintf((*vd).sde[ie], "  one <= \"%s\";                                                \n", sd);ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_fix-1);ie++;
      sprintf((*vd).sde[ie], "                  port map(result=>d2,dataa=>d1,datab=>one);       \n" );ie++;
      sprintf((*vd).sde[ie], "  d0 <= fixdata(%d downto 0);                        \n", nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "  sign0 <= fixdata(%d);                              \n", nbit_fix-1 );ie++;
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  with sign0 select                                              \n" );ie++;   
      sprintf((*vd).sde[ie], "    d3 <= d0 when '0',                                           \n" );ie++;   
      sprintf((*vd).sde[ie], "    d2 when others;                                              \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;   
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;   
      sprintf((*vd).sde[ie], "      d3r <= d3;                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "      sign1 <= sign0;                                            \n" );ie++;   
      sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;   
      sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  u2: penc port map (a=>d3r,c=>c1);                              \n" );ie++;   
      sprintf((*vd).sde[ie], "  with d3r select                                                \n" );ie++;   
      strcpy(sd,"");
      for(i=0;i<(nbit_fix-1);i++) strcat(sd,"0");
      sprintf((*vd).sde[ie], "    nz0 <= '0' when \"%s\",                                  \n", sd );ie++;   
      sprintf((*vd).sde[ie], "           '1' when others;                                      \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;
      sprintf((*vd).sde[ie], "      d4 <= d3r(%d downto 0);                        \n", nbit_fix-3 );ie++;
      sprintf((*vd).sde[ie], "      c2 <= c1;                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "      sign2 <= sign1;                                            \n" );ie++;
      sprintf((*vd).sde[ie], "      nz1 <= nz0;                                                \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;
      sprintf((*vd).sde[ie], "  u3: shift_ftol port map (indata=>d4,control=>c2,outdata=>d5);  \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;   
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;   
      sprintf((*vd).sde[ie], "      d6 <= d5;                                                  \n" );ie++;   
      sprintf((*vd).sde[ie], "      sign3 <= sign2;                                            \n" );ie++;   
      sprintf((*vd).sde[ie], "      nz2 <= nz1;                                                \n" );ie++;   
      sprintf((*vd).sde[ie], "      c3 <= c2;                                                  \n" );ie++;   
      sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;   
      sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   


      // ------------------------
      // implementator -> Hamada
      // ------------------------
      PGPG_CONV_FTOL_CREATE_MIF::create_table_input_file(nbit_man+2,nbit_man);

      sprintf((*vd).sde[ie], "  u4: table generic map (IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"ftol.mif\") \n", nbit_man+2,nbit_man );ie++;   
      sprintf((*vd).sde[ie], "            port map(indata=>d6,outdata=>logdata(%d downto 0),clk=>clk);\n", nbit_man-1 );ie++;     sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  with d6 select                                                 \n" );ie++;   
      /****** here need check **************/
      strcpy(sd,"");
      for(i=0;i<nbit_penc-1;i++) strcat(sd,"0");
      strcat(sd,"1");
      sprintf((*vd).sde[ie], "    add <= \"%s\" when \"1111111\",                          \n", sd );ie++;   
      sprintf((*vd).sde[ie], "           \"%s\" when \"1111110\",                          \n", sd );ie++;   
      sprintf((*vd).sde[ie], "           \"%s\" when \"1111101\",                          \n", sd );ie++;   
      strcpy(sd,"");
      for(i=0;i<nbit_penc;i++) strcat(sd,"0");
      sprintf((*vd).sde[ie], "           \"%s\" when others;                               \n", sd );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;
      sprintf((*vd).sde[ie], "  u5: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_penc );ie++;
      sprintf((*vd).sde[ie], "                  port map(result=>c4,dataa=>c3,datab=>add);     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      strcpy(sd,"");
      for(i=0;i<(nbit_log-nbit_man-nbit_penc-2);i++) strcat(sd,"0");
      sprintf((*vd).sde[ie], "  logdata(%d downto %d) <= \"%s\";                    \n", nbit_man+nbit_penc,nbit_log-3,sd );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "  process(clk) begin                                             \n" );ie++;   
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                               \n" );ie++;   
      sprintf((*vd).sde[ie], "      logdata(%d) <= sign3 ;                          \n",nbit_log-1 );ie++;   
      sprintf((*vd).sde[ie], "      logdata(%d) <= nz2;                             \n",nbit_log-2 );ie++;   
      sprintf((*vd).sde[ie], "      logdata(%d downto %d) <= c4;                    \n",nbit_man+nbit_penc-1,nbit_man);ie++;   
      sprintf((*vd).sde[ie], "    end if;                                                      \n" );ie++;   
      sprintf((*vd).sde[ie], "  end process;                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "                                                                 \n" );ie++;   
      sprintf((*vd).sde[ie], "end rtl;                                                         \n" );ie++;   
      (*vd).ie = ie;
    }


    // ------------------------
    // implementator -> Hamada
    // ------------------------
    PGPG::generate_pg_table(vd);
  
    PGPG_CONV_FTOL::generate_pg_penc(vd);
    PGPG_CONV_FTOL::generate_pg_shift_ftol(vd);

  }





}
