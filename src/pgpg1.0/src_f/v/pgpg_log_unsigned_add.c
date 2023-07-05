#include<stdio.h>
#include "pgpg.h"

void create_table_log_unsigned_add( nbit_man, nbit_tin,tablecnt)
int nbit_man;
int *nbit_tin;
int tablecnt;
{
  int i,iy,nentry,outmask;
  double inscale,outscale,log(),pow(),x;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"add%1d.mif",tablecnt);
  fp = fopen(filename,"w");
  inscale = (double)(1<<nbit_man);

  i = 0;
  do{
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    i++; 
  }while(iy>0);

  *nbit_tin = (int) (log((double)i)/log(2.0)+1.0);
  nentry = (int) pow(2.0, (double)(*nbit_tin));

  fprintf(fp,"DEPTH = %d;\n",nentry);
  fprintf(fp,"WIDTH = %d;\n\n",nbit_man+1);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  for(i=0;i<nentry;i++){
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    fprintf(fp,"    %X : %X ;\n",i,iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}

void generate_pg_log_unsigned_add(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
	int i,ii,j;
	int nbit_fix,nbit_log,nbit_man,nstage,nbit_tin,nbit_tout;
	char sx[STRLEN],sy[STRLEN],sz[STRLEN];
        char muldiv_flag[STRLEN];
        char snshift[STRLEN];
        static char sd[1000][STRLEN];
	int ic,im,ie,is;
	int iflag=1;
        static int tablecnt=0;

	strcpy(sx,sdata[1]);
	strcpy(sy,sdata[2]);
	strcpy(sz,sdata[3]);
	nbit_log = atoi(sdata[4]);
	nbit_man = atoi(sdata[5]);
	nstage = atoi(sdata[6]);

        printf("log unsigned add : %s + %s = %s : ",sx,sy,sz);
	printf("nbit log %d man %d nstage %d\n",nbit_log,nbit_man,nstage);
        if(nstage<1) {
	  nstage = 1;
	  printf("nstage: fixed to %d\n",nstage);
	}
        if(nstage>3) {
	  nstage = 3;
	  printf("nstage: fixed to %d\n",nstage);
	}

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_log_unsigned_add_%d_%d                    \n", nbit_log, nbit_man );ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port( x,y : in std_logic_vector(%d downto 0);      \n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          z : out std_logic_vector(%d downto 0);       \n", nbit_log-1 );ic++;
    sprintf((*vd).sdc[ic], "          clock : in std_logic);                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;
    (*vd).ic = ic;
  }

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_log_unsigned_add_%d_%d      \n",(*vd).ucnt,nbit_log,nbit_man );im++;
  sprintf((*vd).sdm[im], "            port map(x=>%s,y=>%s,z=>%s,clock=>pclk);       \n",sx,sy,sz);im++;
  sprintf((*vd).sdm[im], "                                                                   \n" );im++;
  (*vd).im = im;
  (*vd).ucnt++;

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);           \n",sx,nbit_log-1);is++;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);           \n",sy,nbit_log-1);is++;
  (*vd).is = is;

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "entity pg_log_unsigned_add_%d_%d is            \n", nbit_log, nbit_man );ie++;
    sprintf((*vd).sde[ie], "  port( x,y : in std_logic_vector(%d downto 0);        \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "        z : out std_logic_vector(%d downto 0);         \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "        clock : in std_logic);                                     \n" );ie++;
    sprintf((*vd).sde[ie], "end pg_log_unsigned_add_%d_%d;                 \n", nbit_log, nbit_man );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "architecture rtl of pg_log_unsigned_add_%d_%d is\n", nbit_log, nbit_man );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  component lpm_add_sub                                            \n" );ie++;
    sprintf((*vd).sde[ie], "    generic (LPM_WIDTH: integer;                                   \n" );ie++;
    sprintf((*vd).sde[ie], "             LPM_DIRECTION: string);                               \n" );ie++;
    sprintf((*vd).sde[ie], "    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  \n" );ie++;
    sprintf((*vd).sde[ie], "          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    if(nstage==3){
    sprintf((*vd).sde[ie], "  component table                                                  \n" );ie++;
    }else{
    sprintf((*vd).sde[ie], "  component unreg_table                                            \n" );ie++;
    }
    sprintf((*vd).sde[ie], "    generic (IN_WIDTH: integer ;                                   \n" );ie++;
    sprintf((*vd).sde[ie], "	     OUT_WIDTH: integer ;                                      \n" );ie++;
    sprintf((*vd).sde[ie], "	     TABLE_FILE: string);                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          \n" );ie++;
    sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);       \n" );ie++;
    sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end component;                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    nbit_tout = nbit_man + 1;
    
    sprintf((*vd).sde[ie], "  signal x1,y1,xy,yx : std_logic_vector(%d downto 0);  \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "  signal x2,x3,x4 : std_logic_vector(%d downto 0);     \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal d0,d1,d4 : std_logic_vector(%d downto 0);     \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal df0,df1 : std_logic;                                      \n" );ie++;
    sprintf((*vd).sde[ie], "  signal z0 : std_logic_vector(%d downto 0);           \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  signal d2 : std_logic_vector(%d downto 0);          \n", nbit_tout-1 );ie++;
    sprintf((*vd).sde[ie], "  signal sign0,sign1,sign2 : std_logic;                            \n" );ie++;
    sprintf((*vd).sde[ie], "  signal signxy : std_logic_vector(1 downto 0);                    \n" );ie++;        
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  x1 <= '0' & x(%d downto 0);                          \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  y1 <= '0' & y(%d downto 0);                          \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log );ie++;
    sprintf((*vd).sde[ie], "       port map(dataa=>x1,datab=>y1,result=>xy);                   \n" );ie++;
    sprintf((*vd).sde[ie], "  u2: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"SUB\")\n", nbit_log );ie++;
    sprintf((*vd).sde[ie], "       port map(dataa=>y1,datab=>x1,result=>yx);                   \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "  x2 <= x(%d downto 0) when yx(%d)='1' else y(%d downto 0);\n", nbit_log-2, nbit_log-1,nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  d0 <= xy(%d downto 0) when yx(%d)='1' else yx(%d downto 0);\n",
                                                                nbit_log-2, nbit_log-1, nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  signxy <= x(%d)&y(%d);                    \n", nbit_log-1,nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "  with signxy select                                               \n" );ie++;    
    sprintf((*vd).sde[ie], "    sign0 <= y(%d) when \"01\",                        \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "             x(%d) when others;                        \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    if(nstage>1){
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
      sprintf((*vd).sde[ie], "      x3 <= x2;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      d1 <= d0;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      sign1 <= sign0;                                              \n" );ie++;     
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  x3 <= x2;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  d1 <= d0;                                                       \n" );ie++;
      sprintf((*vd).sde[ie], "  sign1 <= sign0;                                                 \n" );ie++;      
    }      
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

#ifdef PGPGVM
    create_table_log_unsigned_add(nbit_man,&nbit_tin,tablecnt);
#endif
    
    itobc(0,sd,nbit_log-1-nbit_tin);
    sprintf((*vd).sde[ie], "  df0 <= '1' when d1(%d downto %d)=\"%s\" else '0';\n", nbit_log-2, nbit_tin, sd );ie++;
    if(nstage==3){ 
      sprintf((*vd).sde[ie], "  u3: table generic map(IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"add%1d.mif\")\n",
                                                                       nbit_tin,nbit_tout,tablecnt );ie++;
      tablecnt++;
    }else{
      sprintf((*vd).sde[ie], "  u3: unreg_table generic map(IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"add%1d.mif\")\n",
                                                                       nbit_tin,nbit_tout,tablecnt );ie++;
      tablecnt++;
    }
    sprintf((*vd).sde[ie], "            port map(indata=>d1(%d downto 0),outdata=>d2,clk=>clock);\n", nbit_tin-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    if(nstage==3){
      sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
      sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
      sprintf((*vd).sde[ie], "      df1 <= df0;                                                  \n" );ie++;
      sprintf((*vd).sde[ie], "      x4 <= x3;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "      sign2 <= sign1;                                              \n" );ie++;      
      sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
      sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    }else{
      sprintf((*vd).sde[ie], "  df1 <= df0;                                                    \n" );ie++;
      sprintf((*vd).sde[ie], "  x4 <= x3;                                                      \n" );ie++;
      sprintf((*vd).sde[ie], "  sign2 <= sign1;                                                \n" );ie++;      
    }
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    itobc(0,sd,nbit_tout);
    sprintf((*vd).sde[ie], "  d4(%d downto 0) <= d2 when (df1 = '1') else \"%s\";\n", nbit_tout-1, sd );ie++;
    itobc(0,sd,nbit_log-1-nbit_tout);
    sprintf((*vd).sde[ie], "  d4(%d downto %d) <= \"%s\";           \n", nbit_log-2, nbit_tout, sd );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "  u4: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "       port map(dataa=>x4,datab=>d4,result=>z0);                   \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    sprintf((*vd).sde[ie], "  process(clock) begin                                             \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clock'event and clock='1') then                             \n" );ie++;
    sprintf((*vd).sde[ie], "      z(%d downto 0) <= z0;                            \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "      z(%d) <= sign2;                                  \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;   
    (*vd).ie = ie;

    if(nstage==3){
      generate_pg_table(vd);
    }else{
      generate_pg_unreg_table(vd);
    }
  }
}  




