/* --- function list -----------------------------
       void PGPG_SET_IO::conv_to_binary(...);
       void std::generate_pg_set_jdata(...);
       void std::generate_pg_rundelay(...);
       void std::generate_pg_set_fodata(...);
       void std::generate_pg_set_idata(...);
   -----------------------------------------------*/ 

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include "pgpg.h"
using namespace std;

namespace PGPG_SET_IO{
  void conv_to_binary(int i,char bdata[STRLEN])
  {
    if(i==0) std::strcpy(bdata,"0000");
    if(i==1) std::strcpy(bdata,"0001");
    if(i==2) std::strcpy(bdata,"0010");
    if(i==3) std::strcpy(bdata,"0011");
    if(i==4) std::strcpy(bdata,"0100");
    if(i==5) std::strcpy(bdata,"0101");
    if(i==6) std::strcpy(bdata,"0110");
    if(i==7) std::strcpy(bdata,"0111");
    if(i==8) std::strcpy(bdata,"1000");
    if(i==9) std::strcpy(bdata,"1001");
    if(i==10) std::strcpy(bdata,"1010");
    if(i==11) std::strcpy(bdata,"1011");
    if(i==12) std::strcpy(bdata,"1100");
    if(i==13) std::strcpy(bdata,"1101");
    if(i==14) std::strcpy(bdata,"1110");
    if(i==15) std::strcpy(bdata,"1111");
  }
}

namespace std{
  using namespace PGPG_SET_IO;
  void generate_pg_set_jdata(char sdata[][STRLEN],vhdl_description *vd,int n)
  {
    int bit_from;
    int bit_to;
    int bit2_from=0;
    int bit2_to=0;
    char sx[STRLEN];
    int im;

    strcpy(sx,sdata[1]);
    bit_from = atoi(sdata[2]);
    bit_to = atoi(sdata[3]);
    if(n>4){
      bit2_from = atoi(sdata[4]);
      bit2_to = atoi(sdata[5]);
    }

    im = (*vd).im;
    if(n>4){
      sprintf((*vd).sdm[im], "  %s <= p_jdata(%d downto %d) & p_jdata(%d downto %d);\n",sx,bit_from,bit_to,bit2_from,bit2_to);im++;
    }else{
      sprintf((*vd).sdm[im], "  %s <= p_jdata(%d downto %d);      \n",sx,bit_from,bit_to);im++;
    }
    (*vd).im = im; 

  }  

  void generate_pg_rundelay(char sdata[][STRLEN],vhdl_description *vd)
  {
    int ndelay;
    int im,is;

    ndelay = atoi(sdata[1]);

    is = (*vd).is;
    sprintf((*vd).sds[is], "  signal run: std_logic_vector(%d downto 0);                        \n",ndelay+2);is++;
    sprintf((*vd).sds[is], "  signal runr: std_logic;                                           \n" );is++;
    (*vd).is = is; 

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  run(0) <= p_run;                                                  \n" );im++;
    sprintf((*vd).sdm[im], "  process(pclk) begin                                               \n" );im++;
    sprintf((*vd).sdm[im], "    if(pclk'event and pclk='1') then                                \n" );im++;
    sprintf((*vd).sdm[im], "      for i in 0 to %d loop                                         \n",ndelay+1);im++;
    sprintf((*vd).sdm[im], "        run(i+1) <= run(i);                                         \n" );im++;
    sprintf((*vd).sdm[im], "      end loop;                                                     \n" );im++;
    sprintf((*vd).sdm[im], "    end if;                                                         \n" );im++;
    sprintf((*vd).sdm[im], "  end process;                                                      \n" );im++;
    sprintf((*vd).sdm[im], "  runr <= run(%d);                                                  \n",ndelay);im++;
    sprintf((*vd).sdm[im], "  p_runret <= run(%d);                                              \n",ndelay+2);im++;
    sprintf((*vd).sdm[im], "                                                                    \n" );im++;
    (*vd).im = im; 

  }  


  void generate_pg_set_fodata(char sdata[][STRLEN],vhdl_description *vd,int n)
  {
    int i;

    char bdata[STRLEN];
    int im;


    im = (*vd).im;
    sprintf((*vd).sdm[im], "  process(pclk) begin                                              \n");im++;
    sprintf((*vd).sdm[im], "    if(pclk'event and pclk = '1') then                             \n");im++;
    i=1;  
    sprintf((*vd).sdm[im], "      if(p_adro = \"0000\") then                                   \n");im++;
    sprintf((*vd).sdm[im], "        p_datao <= %s & %s(%s downto %s);   \n",sdata[i],sdata[i+1],sdata[i+2],sdata[i+3]);im++;
    for(i=5;i<n;i+=4){
      PGPG_SET_IO::conv_to_binary(i/4,bdata);
      sprintf((*vd).sdm[im], "      elsif(p_adro = \"%s\") then                                \n",bdata);im++;
      sprintf((*vd).sdm[im], "        p_datao <= %s & %s(%s downto %s); \n",sdata[i],sdata[i+1],sdata[i+2],sdata[i+3]);im++;
    }
    sprintf((*vd).sdm[im], "      else                                                         \n");im++;
    sprintf((*vd).sdm[im], "        p_datao <= \"00000000000000000000000000000000\";           \n");im++;
    sprintf((*vd).sdm[im], "      end if;                                                      \n");im++;
    sprintf((*vd).sdm[im], "    end if;                                                        \n");im++;
    sprintf((*vd).sdm[im], "  end process;                                                     \n");im++;
    sprintf((*vd).sdm[im], "                                                                   \n");im++;

    (*vd).im = im; 
    (*vd).ucnt++;

  }  

  void generate_pg_set_idata(char sdata[][STRLEN],vhdl_description *vd,int n)
  {
    int i;

    char bdata[STRLEN];
    int im;

    im = (*vd).im;
    sprintf((*vd).sdm[im], "  process(pclk) begin                                              \n");im++;
    sprintf((*vd).sdm[im], "    if(pclk'event and pclk = '1' and p_we = '1') then              \n");im++;
    i=1;  
    sprintf((*vd).sdm[im], "      if(p_adri = \"0000\") then                                   \n");im++;
    sprintf((*vd).sdm[im], "        %s(%s downto %s) <= p_datai(%s downto %s); \n",sdata[i],sdata[i+1],sdata[i+2],sdata[i+3],sdata[i+4]);im++;
    for(i=6;i<n;i+=5){
      PGPG_SET_IO::conv_to_binary(i/5,bdata);
      sprintf((*vd).sdm[im], "      elsif(p_adri = \"%s\") then                                \n",bdata);im++;
      sprintf((*vd).sdm[im], "        %s(%s downto %s) <= p_datai(%s downto %s); \n",sdata[i],sdata[i+1],sdata[i+2],sdata[i+3],sdata[i+4]);im++;
    }
    sprintf((*vd).sdm[im], "      end if;                                                      \n");im++;
    sprintf((*vd).sdm[im], "    end if;                                                        \n");im++;
    sprintf((*vd).sdm[im], "  end process;                                                     \n");im++;
    sprintf((*vd).sdm[im], "                                                                   \n");im++;

    (*vd).im = im; 
    (*vd).ucnt++;

  }  




}
