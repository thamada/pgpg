// should be implemented
//   PGPG::generate_pg_table(); <- none

/* --- function list -----------------------------
       void PGPG_CONV_LTOF_CREATE_MIF::i2s(...);
       void PGPG_CONV_LTOF_CREATE_MIF::i2s_hex(...);
       void PGPG_CONV_LTOF_CREATE_MIF::generate_mif_file(...);
       void PGPG_CONV_LTOF_CREATE_MIF::create_table_input_file(...);
       void PGPG_CONV_LTOF::generate_pg_shift_ltof(...);
       void std::generate_pg_conv_ltof(...);
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


  namespace PGPG_CONV_LTOF_CREATE_MIF{

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

      string filename = "ltof.mif";  

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
	double x = pow(2.0,((double)input)/(double)memory_depth);
	double y = (((double)memory_depth)*(x-1.0)) +0.5;
	int output = (int)y;
      
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


  namespace PGPG_CONV_LTOF{
    void generate_pg_shift_ltof(vhdl_description *vd)
    {
      int ie;

      static int flag=1; 

      if(flag==1){
	ie = (*vd).ie;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "library ieee;                                                       \n" );ie++;
	sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                        \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "entity shift_ltof is                                                \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  port( indata : in std_logic_vector(5 downto 0);                   \n" );ie++;
	sprintf((*vd).sde[ie], "        control : in std_logic_vector(6 downto 0);                  \n" );ie++;
	sprintf((*vd).sde[ie], "        clk : in std_logic;                                         \n" );ie++;
	sprintf((*vd).sde[ie], "        outdata : out std_logic_vector(47 downto 0));               \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "end shift_ltof;                                                     \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "architecture rtl of shift_ltof is                                   \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  component lpm_clshift                                             \n" );ie++;
	sprintf((*vd).sde[ie], "    generic (LPM_WIDTH : POSITIVE;                                  \n" );ie++;
	sprintf((*vd).sde[ie], "             LPM_WIDTHDIST : POSITIVE);                             \n" );ie++;
	sprintf((*vd).sde[ie], "    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          \n" );ie++;
	sprintf((*vd).sde[ie], "         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  \n" );ie++;
	sprintf((*vd).sde[ie], "         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      \n" );ie++;
	sprintf((*vd).sde[ie], "  end component;                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  signal d0 : std_logic_vector(63 downto 0);                        \n" );ie++;
	sprintf((*vd).sde[ie], "  signal o0 : std_logic_vector(63 downto 0);                        \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "begin                                                               \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "  d0 <= \"0000000000000000000000000000000000000000000000000000000000\"&indata;\n" );ie++;
	sprintf((*vd).sde[ie], "  u1: lpm_clshift generic map (LPM_WIDTH=>64,LPM_WIDTHDIST=>6)      \n" );ie++;
	sprintf((*vd).sde[ie], "                  port map (data=>d0,distance=>control(5 downto 0),result=>o0);\n" );ie++;
	sprintf((*vd).sde[ie], "  outdata <= o0(52 downto 5);                                       \n" );ie++;
	sprintf((*vd).sde[ie], "                                                                    \n" );ie++;
	sprintf((*vd).sde[ie], "end rtl;                                                            \n" );ie++;
	(*vd).ie = ie;  
	flag=0;
      }
    }

  }

  void generate_pg_conv_ltof(char sdata[][STRLEN],vhdl_description *vd)
  {
    int i;
    int nbit_fix,nbit_log,nbit_man,nstage;
    char sx[STRLEN],sy[STRLEN];


    char sd[STRLEN];
    int ic,im,ie,is;
    int iflag, j;

    strcpy(sx,sdata[1]);
    strcpy(sy,sdata[2]);
    nbit_log = atoi(sdata[3]);
    nbit_man = atoi(sdata[4]);
    nbit_fix = atoi(sdata[5]);
    nstage = atoi(sdata[6]);

    printf("conv ltof: %s = %s : ",sx,sy);
    printf("nbit log %d man %d fix %d nstage %d\n",nbit_log,nbit_man,nbit_fix,nstage);

    ic = (*vd).ic;
    sprintf((*vd).sdc[ic], "  component pg_conv_ltof_%d_%d_%d      \n", nbit_log,nbit_man,nbit_fix);ic++;
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
  
    im = (*vd).im;
    sprintf((*vd).sdm[im], "  u%d: pg_conv_ltof_%d_%d_%d\n",(*vd).ucnt,nbit_log,nbit_man,nbit_fix);im++;
    sprintf((*vd).sdm[im], "                  port map (logdata=>%s,fixdata=>%s,clk=>pclk);\n",sx,sy);im++;   
    sprintf((*vd).sdm[im], "                                                                   \n" );im++;
    (*vd).im = im;
    (*vd).ucnt++;

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_log-1);is++;
    (*vd).is = is;

    if(iflag==0){
      ie = (*vd).ie;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "entity pg_conv_ltof_%d_%d_%d is        \n", nbit_log,nbit_man,nbit_fix );ie++;
      sprintf((*vd).sde[ie], "  port(logdata : in std_logic_vector(%d downto 0);\n",nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "       fixdata : out std_logic_vector(%d downto 0);\n,nbit_fix-1",nbit_fix-1);ie++;
      sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;
      sprintf((*vd).sde[ie], "end pg_conv_ltof_%d_%d_%d;             \n", nbit_log,nbit_man,nbit_fix );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "architecture rtl of pg_conv_ltof_%d_%d_%d is \n", nbit_log,nbit_man,nbit_fix );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer ;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "             OUT_WIDTH: integer ;                                  \n" );ie++;
      sprintf((*vd).sde[ie], "             TABLE_FILE: string);                                  \n" );ie++;
      sprintf((*vd).sde[ie], "    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);        \n" );ie++;
      sprintf((*vd).sde[ie], "         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);     \n" );ie++;
      sprintf((*vd).sde[ie], "         clk : in std_logic);                                      \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  component shift_ltof                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    port( indata : in std_logic_vector(%d downto 0);      \n",nbit_man );ie++;
      sprintf((*vd).sde[ie], "          control : in std_logic_vector(%d downto 0);\n", nbit_log-nbit_man-3);ie++;
      sprintf((*vd).sde[ie], "          clk : in std_logic;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "          outdata : out std_logic_vector(%d downto 0)); \n",nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  signal frac1 : std_logic_vector(%d downto 0);         \n",nbit_man-1 );ie++;
      sprintf((*vd).sde[ie], "  signal frac2 : std_logic_vector(%d downto 0);           \n",nbit_man );ie++;
      sprintf((*vd).sde[ie], "  signal sign1 : std_logic;                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  signal nz1 : std_logic;                                          \n" );ie++;
      sprintf((*vd).sde[ie], "  signal exp1 : std_logic_vector(%d downto 0); \n",nbit_log-nbit_man-3 );ie++;
      sprintf((*vd).sde[ie], "  signal fix1 : std_logic_vector(%d downto 0);          \n",nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                               \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                 \n" );ie++;
      sprintf((*vd).sde[ie], "      sign1 <= logdata(%d);                             \n",nbit_log-1 );ie++;
      sprintf((*vd).sde[ie], "      nz1 <= logdata(%d);                               \n",nbit_log-2 );ie++;
      sprintf((*vd).sde[ie], "      exp1 <= logdata(%d downto %d);            \n",nbit_log-3,nbit_man);ie++;
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;


      // ------------------------
      // implementator -> Hamada
      // ------------------------
      PGPG_CONV_LTOF_CREATE_MIF::create_table_input_file(nbit_man,nbit_man);


      sprintf((*vd).sde[ie], "  u1: table generic map (IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"ltof.mif\")\n",nbit_man,nbit_man );ie++;
      sprintf((*vd).sde[ie], "            port map(indata=>logdata(%d downto 0),outdata=>frac1,clk=>clk);\n",nbit_man-1 );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  with nz1 select                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "    frac2 <= '1'&frac1 when '1',                                   \n" );ie++;
      strcpy(sd,"");
      for(i=0;i<(nbit_man+1);i++) strcat(sd,"0");
      sprintf((*vd).sde[ie], "             \"%s\" when others;                                \n",sd );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  u3: shift_ltof port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);\n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clk) begin                                               \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                 \n" );ie++;
      sprintf((*vd).sde[ie], "      fixdata(%d) <= sign1;                                 \n",nbit_fix-1 );ie++;
      sprintf((*vd).sde[ie], "      fixdata(%d downto 0) <= fix1;                         \n",nbit_fix-2 );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;   
      (*vd).ie = ie;
    }

    // ------------------------
    // implementator -> Hamada
    // ------------------------
    //PGPG::generate_pg_table(vd);

    PGPG_CONV_LTOF::generate_pg_shift_ltof(vd);

  }  




}
