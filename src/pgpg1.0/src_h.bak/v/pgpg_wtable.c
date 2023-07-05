#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy(),strcmp(),strcat()
#include<math.h>
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc()

void create_table_wtable(nbit,nbit_man,nzero,tablecnt)
int nbit,nbit_man,*nzero,tablecnt;
{
  int i,iy,nentry,outmask;
  double inscale,outscale;
  FILE *fopen(),*fp;
  char filename[100];

  sprintf(filename,"wta%1d.mif",tablecnt);
  fp = fopen(filename,"w");
  nentry = 1<<nbit;

  fprintf(fp,"DEPTH = %d;\n",nentry);
  fprintf(fp,"WIDTH = %d;\n\n",nbit);
  fprintf(fp,"ADDRESS_RADIX = HEX;\n");
  fprintf(fp,"DATA_RADIX = HEX;\n\n");
  fprintf(fp,"CONTENT\n");
  fprintf(fp,"  BEGIN\n");

  inscale = (double)nentry;
  outscale = (double)(1<<nbit);

  *nzero = 0;
  for(i=0;i<nentry;i++){
    int ii;
    double x,result;
    if(i>=(nentry/2)){
      ii = -(nentry - i);
    }else{
      ii = i;
    }
    x = pow(2.0,((double)ii)/32.0)/4.0;
    x = sqrt(x);
    if((x>=0.0)&&(x<1.0)){
      result = 1.0 -1.5*x*x + 0.75*x*x*x;
    }else{
      result = 0.25*(2.0-x)*(2.0-x)*(2.0-x);
    }
    iy = 32.0 * log(result * 256.0)/log(2.0) + 0.5 ;
    if(iy<0){
      iy = 0;
      (*nzero)++;
    }
    /*printf("ii %x i %x x %g result %g iy %x\n",ii,i,x,result,iy);*/
    outmask = ((int)outscale)-1;
    fprintf(fp,"    %X : %X ;\n",i,outmask&iy);
  }
  fprintf(fp,"  END;\n");
  fclose(fp);
}

void generate_pg_wtable(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{

        int i;
	int nbit_log,nbit_man,nstage,nbit_table,nbit_exp,offset,itmp;
	char sx[STRLEN],sy[STRLEN];
        char sd[STRLEN];
	int ic,im,ie,is,iflag,j,nzero;
        static int tablecnt=0;

	strcpy(sx,sdata[1]);
	strcpy(sy,sdata[2]);
	nbit_log = atoi(sdata[3]);
	nbit_man = atoi(sdata[4]);
	nstage = atoi(sdata[5]);

        printf("w table: %s => %s : ",sx,sy);
	printf("nbit log %d man %d nstage %d\n",nbit_log,nbit_man,nstage);
        if(nstage<1){ 
	  nstage = 1;
	  printf("nstage: fixed to %d\n",nstage);
	}
        if(nstage>4){ 
	  nstage = 4;
	  printf("nstage: fixed to %d\n",nstage);
	}

  /*** generate component decleration ****/

  ic = (*vd).ic;
  sprintf((*vd).sdc[ic], "  component pg_wtable_%d_%d                       \n",nbit_log,nbit_man);ic++;
  iflag=0;
  for(j=0;j<(ic-1);j++){
    if(strcmp((*vd).sdc[ic-1],(*vd).sdc[j])==0) iflag=1;
  }
  if(iflag==0){
    sprintf((*vd).sdc[ic], "    port(indata  : in std_logic_vector(%d downto 0);     \n",nbit_log-1);ic++;
    sprintf((*vd).sdc[ic], "         outdata : out std_logic_vector(%d downto 0);    \n",nbit_log-1);ic++;
    sprintf((*vd).sdc[ic], "         clk : in std_logic);                                      \n" );ic++;
    sprintf((*vd).sdc[ic], "  end component;                                                   \n" );ic++;
    sprintf((*vd).sdc[ic], "                                                                   \n" );ic++;   
    (*vd).ic = ic;
  }

  /*** generate map ****/

  im = (*vd).im;
  sprintf((*vd).sdm[im], "  u%d: pg_wtable_%d_%d ",                    (*vd).ucnt,nbit_log,nbit_man);im++;
  sprintf((*vd).sdm[im], "  port map (indata=>%s,outdata=>%s,clk=>pclk);                  \n",sx,sy);im++;
  sprintf((*vd).sdm[im], "                                                                     \n" );im++;   
  (*vd).im = im;
  (*vd).ucnt++;

  /*** generate signal decleration ****/

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal %s: std_logic_vector(%d downto 0);    \n",sx,nbit_log-1);is++;
  (*vd).is = is;
	
  /*** generate component ****/

  if(iflag==0){
    ie = (*vd).ie;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "library ieee;                                                      \n" );ie++;   
    sprintf((*vd).sde[ie], "use ieee.std_logic_1164.all;                                       \n" );ie++;   
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "entity pg_wtable_%d_%d is                        \n",nbit_log,nbit_man );ie++;   
    sprintf((*vd).sde[ie], "  port(indata : in std_logic_vector(%d downto 0);       \n", nbit_log-1);ie++;     
    sprintf((*vd).sde[ie], "       outdata : out std_logic_vector(%d downto 0);     \n", nbit_log-1);ie++;     
    sprintf((*vd).sde[ie], "       clk : in std_logic);                                        \n" );ie++;     
    sprintf((*vd).sde[ie], "end pg_wtable_%d_%d;                             \n",nbit_log,nbit_man );ie++;     
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "architecture rtl of pg_wtable_%d_%d is \n",           nbit_log,nbit_man);ie++;   
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;   
    sprintf((*vd).sde[ie], "  component unreg_table                                            \n" );ie++;     
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

    nbit_table = 8;
    nbit_exp = nbit_log - 2 - nbit_man;  
      
    sprintf((*vd).sde[ie], "  signal nz,sexp0,sexp: std_logic;                                 \n" );ie++;
    sprintf((*vd).sde[ie], "  signal exp: std_logic_vector(%d downto 0);\n", nbit_log-nbit_table-3 );ie++;
    sprintf((*vd).sde[ie], "  signal d10,d2: std_logic_vector(%d downto 0);      \n", nbit_table-1 );ie++;
    sprintf((*vd).sde[ie], "  signal d0,d1,d3,d4,offset0,offset1: std_logic_vector(%d downto 0);\n",nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "begin                                                              \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  nz <= indata(%d);                                    \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "  sexp0 <= indata(%d);                                 \n", nbit_log-3 );ie++;

    strcpy(sd,"");
    for(i=0;i<(nbit_exp-2);i++) strcat(sd,"0");
    strcat(sd,"1");
    for(i=0;i<(nbit_man+1);i++) strcat(sd,"0");

    sprintf((*vd).sde[ie], "  d0 <= \"%s\";                                                 \n", sd);ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  u1: lpm_add_sub generic map(LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "                  port map(result=>d1,dataa=>indata(%d downto 0),datab=>d0);\n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  sexp <= d1(%d);                                      \n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "  exp <= d1(%d downto %d);               \n", nbit_log-4, nbit_table-1 );ie++;
    sprintf((*vd).sde[ie], "  d10 <= d1(%d downto 0);                            \n", nbit_table-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    create_table_wtable(nbit_table,nbit_man,&nzero,tablecnt);

    sprintf((*vd).sde[ie], "  u2: unreg_table generic map (IN_WIDTH=>%d,OUT_WIDTH=>%d,TABLE_FILE=>\"wta%1d.mif\") \n",
	                                                             nbit_table,nbit_table,tablecnt);ie++;   
    sprintf((*vd).sde[ie], "                  port map(indata=>d10,outdata=>d2,clk=>clk);      \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    tablecnt++;
 
    strcpy(sd,"");
    for(i=0;i<(nbit_log-nbit_table-2);i++) strcat(sd,"0");

    sprintf((*vd).sde[ie], "  d3 <= \"%s\" & d2;                                           \n", sd );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    offset = 0x880;
       
    sprintf((*vd).sde[ie], "  --  offset0 = 0x%x, offset1 = 0x%x + 0x100        \n", offset,offset );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;

    itobc(offset,sd,nbit_log-2);
    sprintf((*vd).sde[ie], "  offset0 <= \"%s\";                                           \n", sd );ie++;
    offset += 0x100;
    itobc(offset,sd,nbit_log-2);
    sprintf((*vd).sde[ie], "  offset1 <= \"%s\";                                           \n", sd );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  u3: lpm_add_sub generic map (LPM_WIDTH=>%d,LPM_DIRECTION=>\"ADD\")\n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "                   port map(result=>d4,dataa=>d3,datab=>offset0);  \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  process(clk) begin                                               \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                 \n" );ie++;
    sprintf((*vd).sde[ie], "      if(nz = '0') then                                            \n" );ie++;
    sprintf((*vd).sde[ie], "        outdata(%d downto 0) <= offset1;               \n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "      else                                                         \n" );ie++;
    sprintf((*vd).sde[ie], "        if(sexp = '0') then                                        \n" );ie++;
    sprintf((*vd).sde[ie], "          outdata(%d downto 0) <= d4;                  \n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "        else                                                       \n" );ie++;

    strcpy(sd,"");
    for(i=0;i<(nbit_log-nbit_table-2);i++) strcat(sd,"1");

    sprintf((*vd).sde[ie], "          if(exp = \"%s\") then                                \n", sd );ie++;
    sprintf((*vd).sde[ie], "            outdata(%d downto 0) <= d4;                \n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "          else                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "            outdata(%d downto 0) <= offset1;           \n", nbit_log-3 );ie++;
    sprintf((*vd).sde[ie], "          end if;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "        end if;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "      end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  process(clk) begin                                               \n" );ie++;
    sprintf((*vd).sde[ie], "    if(clk'event and clk='1') then                                 \n" );ie++;
    sprintf((*vd).sde[ie], "      if(nz = '0') then                                            \n" );ie++;
    sprintf((*vd).sde[ie], "        outdata(%d) <= '1';                            \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "      else                                                         \n" );ie++;
    sprintf((*vd).sde[ie], "        if(sexp = '0') then                                        \n" );ie++;

    strcpy(sd,"");
    for(i=0;i<(nbit_log-nbit_table-2);i++) strcat(sd,"0");

    sprintf((*vd).sde[ie], "          if(exp = \"%s\") then                                \n", sd );ie++;

    itmp = (1<<(nbit_table-1))-1;
    itobc(itmp,sd,nbit_table);
    sprintf((*vd).sde[ie], "            if(d10 = \"%s\" or                                 \n", sd );ie++;
    for(i=1;i<nzero-1;i++){
      itmp--;
      itobc(itmp,sd,nbit_table);
      sprintf((*vd).sde[ie], "               d10 = \"%s\" or                               \n", sd );ie++;
    }
    itmp--;
    itobc(itmp,sd,nbit_table);
    sprintf((*vd).sde[ie], "               d10 = \"%s\") then                            \n", sd );ie++;
    sprintf((*vd).sde[ie], "              outdata(%d) <= '0';                      \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "            else                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "              outdata(%d) <= '1';                      \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "            end if;                                                \n" );ie++;
    sprintf((*vd).sde[ie], "          else                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "            outdata(%d) <= '0';                        \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "          end if;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "        else                                                       \n" );ie++;
    sprintf((*vd).sde[ie], "          if(sexp0 ='0') then                                      \n" );ie++;
    sprintf((*vd).sde[ie], "            outdata(%d) <= '0';                        \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "          else                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "            outdata(%d) <= '1';                        \n", nbit_log-2 );ie++;
    sprintf((*vd).sde[ie], "          end if;                                                  \n" );ie++;
    sprintf((*vd).sde[ie], "        end if;                                                    \n" );ie++;
    sprintf((*vd).sde[ie], "      end if;                                                      \n" );ie++;
    sprintf((*vd).sde[ie], "    end if;                                                        \n" );ie++;
    sprintf((*vd).sde[ie], "  end process;                                                     \n" );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "  outdata(%d) <= '0';                                  \n", nbit_log-1 );ie++;
    sprintf((*vd).sde[ie], "                                                                   \n" );ie++;
    sprintf((*vd).sde[ie], "end rtl;                                                           \n" );ie++;   
    (*vd).ie = ie;

    generate_pg_unreg_table(vd);

  }
}  
