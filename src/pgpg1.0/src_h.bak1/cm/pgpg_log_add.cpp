// should be implemented
//     PGPG::generate_pg_table();                 <- none

/* --- function list -----------------------------
       void PGPG_LOG_ADD_CREATE_MIF::i2s(...);
       void PGPG_LOG_ADD_CREATE_MIF::i2s_hex(...);
       void PGPG_LOG_ADD_CREATE_MIF::generate_mif_file(...);
       void PGPG_LOG_ADD_CREATE_MIF::create_table_input_file(...);
       void std::generate_pg_log_add(...);
   -----------------------------------------------*/ 

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>

#include<string>
#include<fstream>
#include<cmath>

#include "pgpg.h"
using namespace std;

namespace PGPG_LOG_ADD_CREATE_MIF{

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

    string filename = "addt.mif";

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

  void create_table_input_file(int input_width,int output_width,int logfrac_width)
  {
    string mif_contents;
    int memory_depth = (int)(pow(2.0,(double)input_width));
    int logfrac_width_convi =  (int)(pow(2.0,(double)logfrac_width));
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
      // I forgot the reason why 0.25 must be added.
      double x = 1.0 + pow(2.0, -((double)input+0.25)/(double)logfrac_width_convi);
      double y = 0.5 + ((double)logfrac_width_convi)*log(x)/log(2.0);
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


namespace std{
  using namespace PGPG_LOG_ADD_CREATE_MIF;
  void generate_pg_log_add(char sdata[][STRLEN],struct vhdl_description *vd)
  {

    int nbit_log,nbit_man,nstage;
    char sx[STRLEN],sy[STRLEN],sz[STRLEN];



    int ic,im,ie,is;
    static int flag=1;

    strcpy(sx,sdata[1]);
    strcpy(sy,sdata[2]);
    strcpy(sz,sdata[3]);
    nbit_log = atoi(sdata[4]);
    nbit_man = atoi(sdata[5]);
    nstage = atoi(sdata[6]);

    printf("log add : %s + %s = %s : ",sx,sy,sz);
    printf("nbit log %d man %d nstage %d\n",nbit_log,nbit_man,nstage);

    if(flag==1){
      ic = (*vd).ic;
      sprintf((*vd).sdc[ic], "  component pg_log_add                                             \n" );ic++;
      sprintf((*vd).sdc[ic], "    generic(PG_WIDTH_LOG: integer;                                 \n" );ic++;
      sprintf((*vd).sdc[ic], "            PG_WIDTH_MAN: integer;                                 \n" );ic++; 
      sprintf((*vd).sdc[ic], "            PG_PIPELINE: integer);                                 \n" );ic++;
      sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(PG_WIDTH_LOG-1 downto 0);      \n" );ic++;
      sprintf((*vd).sdc[ic], "          z : out std_logic_vector(PG_WIDTH_LOG-1 downto 0);       \n" );ic++;
      sprintf((*vd).sdc[ic], "          clock : in std_logic);                                   \n" );ic++;
      sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
      sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;
      (*vd).ic = ic;
    }

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  u%d: pg_log_add generic map(PG_WIDTH_LOG=>%d,PG_WIDTH_MAN=>%d,\n",(*vd).ucnt,nbit_log,nbit_man);im++;
    sprintf((*vd).sdm[im], "                             PG_PIPELINE=>%d)                      \n",nstage);im++;
    sprintf((*vd).sdm[im], "                 port map(x=>%s,y=>%s,z=>%s,clock=>pclk);\n",sx,sy,sz);im++;
    sprintf((*vd).sdm[im], "                                                                   \n" );im++;
    (*vd).im = im;
    (*vd).ucnt++;

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_log-1);is++;
    sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sy,nbit_log-1);is++;
    (*vd).is = is;

    if(flag==1){
      ie = (*vd).ie;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "entity pg_log_add is                                               \n" );ie++;
      sprintf((*vd).sde[ie], "  generic(PG_WIDTH_LOG: integer;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "          PG_WIDTH_MAN: integer;                                   \n" );ie++; 
      sprintf((*vd).sde[ie], "          PG_PIPELINE: integer);                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(PG_WIDTH_LOG-1 downto 0);        \n" );ie++;
      sprintf((*vd).sde[ie], "        z : out std_logic_vector(PG_WIDTH_LOG-1 downto 0);         \n" );ie++;
      sprintf((*vd).sde[ie], "	clock : in std_logic);                                       \n" );ie++;
      sprintf((*vd).sde[ie], "end pg_log_add;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "architecture rtl of pg_log_add is                                  \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;
      sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
      sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer ;                                   \n" );ie++;
      sprintf((*vd).sde[ie], "	     OUT_WIDTH: integer ;                                     \n" );ie++;
      sprintf((*vd).sde[ie], "	     TABLE_FILE: string);                                     \n" );ie++;
      sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;
      sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);       \n" );ie++;
      sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  signal x1,y1,xy,yx : std_logic_vector(13 downto 0);              \n" );ie++;
      sprintf((*vd).sde[ie], "  signal x2,x3,x4 : std_logic_vector(12 downto 0);                 \n" );ie++;
      sprintf((*vd).sde[ie], "  signal d0,d1,d4 : std_logic_vector(12 downto 0);                 \n" );ie++;
      sprintf((*vd).sde[ie], "  signal d2 : std_logic_vector(5 downto 0);                        \n" );ie++;
      sprintf((*vd).sde[ie], "  signal df0,df1 : std_logic;                                      \n" );ie++;
      sprintf((*vd).sde[ie], "  signal z0 : std_logic_vector(12 downto 0);                       \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  x1 <= '0' & x;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  y1 <= '0' & y;                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>14,LPM_DIRECTION=>\"SUB\")\n" );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy);                   \n" );ie++;
      sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>14,LPM_DIRECTION=>\"SUB\")\n" );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>y1,datab=>x1,result=>yx);                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  x2 <= x when yx(13)='1' else y;                                  \n" );ie++;
      sprintf((*vd).sde[ie], "  d0 <= xy(12 downto 0) when yx(13)='1' else yx(12 downto 0);      \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
      sprintf((*vd).sde[ie], "      x3 <= x2;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      d1 <= d0;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  df0 <= '1' when d1(12 downto 8)=\"00000\" else '0';              \n" );ie++;


      // ------------------------
      // implementator -> Hamada
      // ------------------------
      PGPG_LOG_ADD_CREATE_MIF::create_table_input_file(8,6,nbit_man);

      sprintf((*vd).sde[ie], "  u3: table generic map(IN_WIDTH=>8,OUT_WIDTH=>6,TABLE_FILE=>\"addt.mif\")\n" );ie++;
      sprintf((*vd).sde[ie], "            port map(indata=>d1(7 downto 0),outdata=>d2,clk=>clock);      \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
      sprintf((*vd).sde[ie], "      df1 <= df0;                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "      x4 <= x3;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  d4(5 downto 0) <= d2 when (df1 = '1') else \"000000\";           \n" );ie++;
      sprintf((*vd).sde[ie], "  d4(12 downto 6) <= \"0000000\";                                  \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  u4: lpm_add_sub generic map(LPM_WIDTH=>13,LPM_DIRECTION=>\"ADD\")\n" );ie++;
      sprintf((*vd).sde[ie], "       port map(dataa=>x4,datab=>d4,result=>z0);                   \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then z <= z0;                    \n" );ie++;
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
      sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
      sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;   
      (*vd).ie = ie;
      flag =0;
    }


    // ------------------------
    // implementator -> Hamada
    // ------------------------
    //PGPG::generate_pg_table(vd);


  }  


}
