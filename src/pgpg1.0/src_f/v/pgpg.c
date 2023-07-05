#include<stdio.h>
#include "pgpg.h"

void itobc(idata,sdata,nbit)
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

void generate_pg_table(vd)
struct vhdl_description *vd;
{
  int ii,i,ie;
  static char sd[1000][STRLEN];
  static int flag=0;
  
  if(flag==0){
    if((*vd).alteraesbflag==0){
      /*      generate_pg_table_lcell(vd);*/
    }else{
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
    }
    flag = 1;
  }
}

void generate_pg_unreg_table(vd)
struct vhdl_description *vd;
{
  int ii,i,ie;
  static char sd[1000][STRLEN];
  static int flag=0;

  if(flag==0){
    if((*vd).alteraesbflag==0){
      /*      generate_pg_unreg_table_lcell(vd);*/
    }else{
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
    }
    flag = 1;
  }
}

void generate_pg_pipe(vd)
struct vhdl_description *vd;
{
  int ii,i,ivmp,i0;
  static char sd[20000][STRLEN];
  FILE *fp,*fopen();

  ii = 0;

#ifdef PGPGVT

  sprintf(sd[ii], "library ieee;                                                       \n" );ii++;
  sprintf(sd[ii], "use ieee.std_logic_1164.all;                                        \n" );ii++;
  if((*vd).nvmp>1){
    sprintf(sd[ii], "use ieee.std_logic_unsigned.all;                                    \n" );ii++;  
  }
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "entity pipe is                                                      \n" );ii++;
  sprintf(sd[ii], "  generic(JDATA_WIDTH : integer :=72) ;                             \n" );ii++;
  sprintf(sd[ii], "port(p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);         \n" );ii++;
  sprintf(sd[ii], "     p_run : in std_logic;                                          \n" );ii++;
  sprintf(sd[ii], "     p_we :  in std_logic;                                          \n" );ii++;
  sprintf(sd[ii], "     p_adri : in std_logic_vector(3 downto 0);                      \n" );ii++;
  sprintf(sd[ii], "     p_adrivp : in std_logic_vector(3 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_datai : in std_logic_vector(31 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_adro : in std_logic_vector(3 downto 0);                      \n" );ii++;
  sprintf(sd[ii], "     p_adrovp : in std_logic_vector(3 downto 0);                    \n" );ii++;
  sprintf(sd[ii], "     p_datao : out std_logic_vector(31 downto 0);                   \n" );ii++;
  sprintf(sd[ii], "     p_runret : out std_logic;                                      \n" );ii++;
  sprintf(sd[ii], "     rst,pclk : in std_logic);                                      \n" );ii++;
  sprintf(sd[ii], "end pipe;                                                           \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  sprintf(sd[ii], "architecture std of pipe is                                         \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  for(i=0;i<(*vd).ic;i++) sprintf(sd[i+ii],"%s",(*vd).sdc[i]);
  ii += (*vd).ic;

  for(i=0;i<(*vd).is;i++){
    int iflag,j;
    iflag = 0;
    for(j=0;j<i;j++){
      if(strcmp((*vd).sds[i],(*vd).sds[j])==0) iflag=1;
    }
    if(iflag==0){
      sprintf(sd[i+ii],"%s",(*vd).sds[i]);
      ii += (*vd).is;
    }
  }
  if((*vd).nvmp>1){
    for(i=0;i<(*vd).iip;i++){
      sscanf((*vd).sdip[i][1],"%d",&i0);
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        sprintf(sd[ii], "  signal %s%d: std_logic_vector(%d downto 0);\n",(*vd).sdip[i][0],ivmp,i0-1);ii++;
      }
    }
    sprintf(sd[ii], "  signal vmp_phase: std_logic_vector(3 downto 0);                 \n" );ii++;
    sprintf(sd[ii], "  signal irun: std_logic_vector(%d downto 0);      \n",(*vd).nvmp-1 );ii++;    
    for(i=1;i<(*vd).nvmp;i++){
      sprintf(sd[ii], "  signal jdata%d: std_logic_vector(JDATA_WIDTH-1 downto 0);      \n",i );ii++;
    }
  }
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "begin                                                               \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  generate_pg_set_data(&sd,vd,&ii);

  generate_pg_set_jdata(&sd,vd,&ii);

  generate_pg_set_idata(&sd,vd,&ii);  
  
  for(i=0;i<(*vd).im;i++) sprintf(sd[i+ii],"%s",(*vd).sdm[i]);
  ii += (*vd).im;

  generate_pg_set_fodata(&sd,vd,&ii);

  sprintf(sd[ii], "end std;                                                            \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  fp = fopen("pg_pipe.vhd","w");
  generate_pipe_com(fp,vd);
  for(i=0;i<ii;i++) fprintf(fp,"%s",sd[i]);
  fclose(fp);
#endif

#ifdef PGPGVM  
  fp = fopen("pg_module.vhd","w");
  for(i=0;i<(*vd).ie;i++) sprintf(sd[i+ii],"%s",(*vd).sde[i]);
  ii += (*vd).ie;
  for(i=0;i<ii;i++) fprintf(fp,"%s",sd[i]);
  fclose(fp);
#endif
  
}


void sdtest(sd)
struct vhdl_description *sd;
{
  printf("%s\n",(*sd).sdc[0]);
  sprintf((*sd).sdc[0],"test test 2\n");
  (*sd).ic = 10;
}

main(argc,argv)
int argc;
char *argv[];
{
	char isdata[STRLEN];
	static char sdata[100][STRLEN];
	char *p;
	int i,j,ii;
	FILE *fp,*fopen();
        static struct vhdl_description sd;
        sd.ic = 0;
        sd.im = 0;
        sd.ie = 0;
        sd.is = 0;
        sd.ucnt = 0;
        sd.nvmp = 1;
	sd.npipe = 1;
	sd.ijp = sd.iip = sd.ifo = sd.ida = 0;
	sd.alteraesbflag = 1;
	
        if(argc!=2){
	  printf("pgpg filename\n");
	  exit(0);
	}else{
  	  fp = fopen(argv[1],"r");
	}
	
	/*	while(fscanf(fp,"%s",isdata)!=EOF){*/
	while(fgets(isdata,100,fp)!=NULL){	  
	  i = 0;
	  p = (char*)strtok(isdata,"( ");
	  strcpy(sdata[i],p);
	  i++;
	  while((p = (char*)strtok(NULL, ",(); "))!=NULL){
	    strcpy(sdata[i],p);
	    i++;
	  }
	  /*	  for(j=0;j<i;j++) printf("%d %s\n",j,sdata[j]);*/
	
	  if(strcmp(sdata[0],"/NVMP")==0){
	    sscanf(sdata[1],"%d",&(sd.nvmp));
	    printf("nvmp %d\n",sd.nvmp);
	  }
	  if(strcmp(sdata[0],"/NPIPE")==0){
	    sscanf(sdata[1],"%d",&(sd.npipe));
	    printf("npipe %d\n",sd.npipe);
	  }
	  if(strcmp(sdata[0],"/ALTERAESB")==0){
	    if((strcmp(sdata[1],"off")==0)||(strcmp(sdata[1],"OFF")==0)){
	      sd.alteraesbflag = 0;
	    }
	  }
	  if(strcmp(sdata[0],"/JPSET")==0){
	    ii = sd.ijp;
	    strcpy(sd.sdjp[ii][0],sdata[1]);
	    strcpy(sd.sdjp[ii][1],sdata[2]);
	    strcpy(sd.sdjp[ii][2],sdata[3]);
	    strcpy(sd.sdjp[ii][3],sdata[4]);
	    (sd.ijp)++;
	  }
	  if(strcmp(sdata[0],"/IPSET")==0){
	    ii = sd.iip;
	    strcpy(sd.sdip[ii][0],sdata[1]);
	    strcpy(sd.sdip[ii][1],sdata[4]);
	    (sd.iip)++;
	  }
	  if(strcmp(sdata[0],"/FOSET")==0){
	    ii = sd.ifo;
	    strcpy(sd.sdfo[ii][0],sdata[1]);
	    strcpy(sd.sdfo[ii][1],sdata[4]);
	    (sd.ifo)++;
	  }
	  if(strcmp(sdata[0],"/VALSET")==0){
	    int ixtmp,itmp;
	    char sdtmp[100];
	    sscanf(sdata[2],"%x",&ixtmp);
	    itmp = atoi(sdata[4])-atoi(sdata[3])+1;
	    itobc(ixtmp,sdata[2],itmp);
	    ii = sd.ida;
	    strcpy(sd.sdda[ii][0],sdata[1]);
	    strcpy(sd.sdda[ii][1],sdata[2]);
	    strcpy(sd.sdda[ii][2],sdata[3]);
	    strcpy(sd.sdda[ii][3],sdata[4]);	    	    
	    (sd.ida)++;
	  }
	  if(strcmp(sdata[0],"pg_fix_addsub")==0){
	    generate_pg_fix_addsub(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_log_muldiv")==0){
	    generate_pg_log_muldiv(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_log_shift")==0){
	    generate_pg_log_shift(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_pdelay")==0){
	    generate_pg_pdelay(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_conv_ftol")==0){
	    generate_pg_conv_ftol(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_conv_ltof")==0){
	    generate_pg_conv_ltof(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_log_add")==0){
	    generate_pg_log_add(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_log_unsigned_add")==0){
            generate_pg_log_unsigned_add(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_fix_accum")==0){
	    generate_pg_fix_accum(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_rundelay")==0){
	    generate_pg_rundelay(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_wtable")==0){
	    generate_pg_wtable(sdata,&sd);
	  }
	  if(strcmp(sdata[0],"pg_dwtable")==0){
	    generate_pg_dwtable(sdata,&sd);
	  }
	}

	fclose(fp);

	generate_pg_pipe(&sd);

}










