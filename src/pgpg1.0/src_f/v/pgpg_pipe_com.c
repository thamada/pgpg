#include<stdio.h>
#include "pgpg.h"

void generate_pipe_com(fp,vd)
FILE *fp;
struct vhdl_description *vd;
{
  int ii,i;
  char sd0[1000],sd1[100],sd2[100];
  
  fprintf(fp,"library ieee;                                                  \n");  
  fprintf(fp,"use ieee.std_logic_1164.all;                                   \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"entity pg_pipe is                                              \n");  
  fprintf(fp,"  generic(JDATA_WIDTH : integer := 72);                        \n");  
  fprintf(fp,"  port(                                                        \n");  
  fprintf(fp,"    p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);     \n");  
  fprintf(fp,"    p_run : in std_logic;                                      \n");  
  fprintf(fp,"    p_we :  in std_logic;                                      \n");  
  fprintf(fp,"    p_adri : in std_logic_vector(11 downto 0);                 \n");  
  fprintf(fp,"    p_datai : in std_logic_vector(31 downto 0);                \n");  
  fprintf(fp,"    p_adro : in std_logic_vector(11 downto 0);                 \n");  
  fprintf(fp,"    p_datao : out std_logic_vector(31 downto 0);               \n");  
  fprintf(fp,"    p_runret : out std_logic;                                  \n");  
  fprintf(fp,"    rst,clk : in std_logic                                     \n");  
  fprintf(fp,"  );                                                           \n");  
  fprintf(fp,"end pg_pipe;                                                   \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"architecture std of pg_pipe is                                 \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  component pipe                                               \n");
  fprintf(fp,"    generic(JDATA_WIDTH : integer );                           \n");
  fprintf(fp,"    port(                                                      \n");  
  fprintf(fp,"      p_jdata: in std_logic_vector(JDATA_WIDTH-1 downto 0);    \n");  
  fprintf(fp,"      p_run : in std_logic;                                    \n");  
  fprintf(fp,"      p_we : in std_logic;                                     \n");  
  fprintf(fp,"      p_adri : in std_logic_vector(3 downto 0);                \n");  
  fprintf(fp,"      p_adrivp : in std_logic_vector(3 downto 0);              \n");  
  fprintf(fp,"      p_datai : in std_logic_vector(31 downto 0);              \n");  
  fprintf(fp,"      p_adro : in std_logic_vector(3 downto 0);                \n");  
  fprintf(fp,"      p_adrovp : in std_logic_vector(3 downto 0);              \n");  
  fprintf(fp,"      p_datao : out std_logic_vector(31 downto 0);             \n");  
  fprintf(fp,"      p_runret : out std_logic;                                \n");  
  fprintf(fp,"      rst,pclk : in std_logic );                                \n");  
  fprintf(fp,"  end component;                                               \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  signal u_adri,u_adro: std_logic_vector(7 downto 0);          \n");
  fprintf(fp,"  signal adrivp,adrovp: std_logic_vector(3 downto 0);          \n");  
  fprintf(fp,"  signal we,runret: std_logic_vector(%d downto 0);\n", (*vd).npipe-1 );
  fprintf(fp,"  signal datao: std_logic_vector(%d downto 0);\n",32*((*vd).npipe)-1 );
  fprintf(fp,"  signal l_adro: std_logic_vector(3 downto 0);                 \n");
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"begin                                                          \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  u_adri <= p_adri(11 downto 4);                               \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  u_adro <= p_adro(11 downto 4);                               \n");
  fprintf(fp,"  l_adro <= p_adro(3 downto 0);                                \n");    
  fprintf(fp,"                                                               \n");  
  for(ii=0;ii<(*vd).npipe;ii++){
    fprintf(fp,"  process(u_adri,p_we) begin                                 \n");  
    fprintf(fp,"    if(p_we = '1') then                                      \n");
    itobc(ii*(*vd).nvmp,sd2,8);
    sprintf(sd0,"     if(u_adri = \"%s\" ",sd2);
    for(i=1;i<(*vd).nvmp;i++){
      itobc(ii*(*vd).nvmp+i,sd2,8);
      sprintf(sd1,"or u_adri = \"%s\" ",sd2);    
      strcat(sd0,sd1);
    }
    sprintf(sd1,") then\n");
    strcat(sd0,sd1);
    fprintf(fp,"%s",sd0);    
    fprintf(fp,"        we(%d) <= '1';                                    \n",ii);  
    fprintf(fp,"      else                                                   \n");  
    fprintf(fp,"        we(%d) <= '0';                                    \n",ii);  
    fprintf(fp,"      end if;                                                \n");  
    fprintf(fp,"    else                                                     \n");  
    fprintf(fp,"      we(%d) <= '0';                                      \n",ii);  
    fprintf(fp,"    end if;                                                  \n");  
    fprintf(fp,"  end process;                                               \n");  
    fprintf(fp,"                                                             \n");
  }

  fprintf(fp,"  with u_adri select                                           \n");
  for(ii=0;ii<(*vd).npipe;ii++){
    for(i=0;i<(*vd).nvmp;i++){
      if((ii==0)&&(i==0)){
        fprintf(fp,"    adrivp <= \"0000\" when \"00000000\", \n");
      }else{
	itobc(i,sd0,4);
	itobc((ii*((*vd).nvmp))+i,sd1,8);	
        fprintf(fp,"              \"%s\" when \"%s\", \n",sd0,sd1);
      }
    }  
  }
  fprintf(fp,"              \"0000\" when others;                            \n");
  fprintf(fp,"                                                               \n");

  fprintf(fp,"  with u_adro select                                           \n");
  for(ii=0;ii<(*vd).npipe;ii++){
    for(i=0;i<(*vd).nvmp;i++){
      if((ii==0)&&(i==0)){
        fprintf(fp,"    adrovp <= \"0000\" when \"00000000\", \n");
      }else{
	itobc(i,sd0,4);
	itobc((ii*((*vd).nvmp))+i,sd1,8);	
        fprintf(fp,"              \"%s\" when \"%s\", \n",sd0,sd1);
      }
    }  
  }
  fprintf(fp,"              \"0000\" when others;                            \n");
  fprintf(fp,"                                                               \n");
  fprintf(fp,"  forgen1: for i in 0 to %d generate           \n", (*vd).npipe-1 );
  fprintf(fp,"    upipe: pipe GENERIC MAP(JDATA_WIDTH=>JDATA_WIDTH)          \n");  
  fprintf(fp,"	      PORT MAP(p_jdata=>p_jdata, p_run=>p_run,               \n");  
  fprintf(fp,"                 p_we=>we(i),p_adri=>p_adri(3 downto 0),p_adrivp=>adrivp,\n");  
  fprintf(fp,"	               p_datai=>p_datai,p_adro=>l_adro,p_adrovp=>adrovp, \n");  
  fprintf(fp,"	               p_datao=>datao(32*(i+1)-1 downto 32*i), p_runret=>runret(i), \n");  
  fprintf(fp,"		       rst=>rst,pclk=>clk);                           \n");  
  fprintf(fp,"  end generate forgen1;                                        \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  p_runret <= runret(0);                                       \n");  
  fprintf(fp,"                                                               \n");  
  fprintf(fp,"  with u_adro select                                           \n");  

  for(ii=0;ii<(*vd).npipe;ii++){
    for(i=0;i<(*vd).nvmp;i++){
      if((ii==0)&&(i==0)){
        fprintf(fp,"    p_datao <= datao(31 downto 0) when \"00000000\", \n");
      }else{
	itobc((ii*((*vd).nvmp))+i,sd1,8);	
        fprintf(fp,"               datao(%d downto %d) when \"%s\", \n",32*(ii+1)-1,32*ii,sd1);
      }
    }  
  }
  fprintf(fp,"               datao(31 downto 0) when others;                  \n");
  fprintf(fp,"                                                               \n");
  fprintf(fp,"end std;                                               \n");  
  fprintf(fp,"\n");
}
