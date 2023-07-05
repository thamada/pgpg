/* --- function list -----------------------------
       string PGPG_PG_TABLE::get_contents();
       void PGPG::generate_pg_table(...);
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

namespace PGPG_PG_TABLE{
  string get_contents(){
    string s;
    s  ="library ieee;                                                               \n";
    s +="use ieee.std_logic_1164.all;                                                \n";
    s +="                                                                            \n";
    s +="entity table is                                                             \n";
    s +="generic (                                                                   \n";
    s +="      IN_WIDTH : INTEGER :=13;                                              \n";
    s +="      OUT_WIDTH : INTEGER :=13;                                             \n";
    s +="      TABLE_FILE: STRING :=\"addt.mif\";                                    \n";
    s +="      NP : INTEGER := 0;                                                    \n";
    s +="      MAXIMIZE_SPEED : INTEGER :=6);                                        \n";
    s +="port( indata : in std_logic_vector(IN_WIDHT-1 downto 0);                    \n";
    s +="      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);                 \n";   
    s +="      clk : in std_logic);                                                  \n";
    s +="end table;                                                                  \n";
    s +="                                                                            \n";    
    s +="architecture rtl of table is                                                \n";
    s +="                                                                            \n";
    s +="  component lpm_rom                                                         \n";
    s +="    generic (                                                               \n";
    s +="      LPM_WIDTH: POSITIVE;                                                  \n";
    s +="      LPM_WIDTHAD: POSITIVE;                                                \n";
    s +="      LPM_ADDRESS_CONTROL: STRING;                                          \n";
    s +="      LPM_OUTDATA: STRING;                                                  \n";
    s +="      LPM_FILE: STRING);                                                    \n";
    s +="    port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);             \n";
    s +="          q: out std_logic_vector(LPM_WIDTH-1 downto 0));                   \n";
    s +="  end component;                                                            \n";
    s +="                                                                            \n";
    s +="  signal x0 : std_logic_vector(IN_WIDTH-1 downto 0);                        \n";
    s +="  signal y0 : std_logic_vector(OUT_WIDTH-1 downto 0);                       \n";
    s +="                                                                            \n";
    s +="begin                                                                       \n";
    s +="                                                                            \n";
    s +="  u1: lpm_rom generic map(LPM_WIDTHAD=>IN_WIDTH,                            \n";
    s +="                        LPM_WIDTH=>OUT_WIDTH,                               \n";
    s +="                        LPM_ADDRESS_CONTROL=>\"UNREGISTERED\",              \n";
    s +="                        LPM_OUTDATA=>\"UNREGISTERED\",                      \n";
    s +="                        LPM_FILE=>\"addtab.mif\")                           \n";
    s +="            port map(address=>x0,q=>y0);                                    \n";
    s +="                                                                            \n";
    s +="--  process(clock) begin                                                    \n";
    s +="--    if(clock'event and clock='1') then                                    \n";
    s +="      x0 <= indata;                                                         \n";
    s +="      outdata  <=y0;                                                        \n";
    s +="--    end if;                                                               \n";
    s +="--  end process;                                                            \n";
    s +="                                                                            \n";
    s +="end rtl;                                                                    \n";
    return s; 
  }
}

namespace std{
  namespace PGPG{

    void generate_pg_table(vhdl_description *vd_unuse){

      string filename = "table.vhd";  

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
      fout << PGPG_PG_TABLE::get_contents();
    }

  }





}


