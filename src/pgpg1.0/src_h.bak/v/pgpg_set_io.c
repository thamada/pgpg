#include<stdio.h>
#include<stdlib.h> // atoi()
#include<string.h> // strcpy()
#include "pgpg.h"
#include "pgpg_submodule.h" // itobc()

void generate_pg_set_fodata(sd,vd,ii)
char sd[][STRLEN];
struct vhdl_description *vd;
int *ii;
{
  int i,i0,ivmp,iadr;
  char sd0[100],sb[100];
  
  sprintf(sd[(*ii)],"  process(pclk) begin\n");(*ii)++;
  sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then\n");(*ii)++;

  iadr = 0;
  for(i=0;i<(*vd).ifo;i++){
    if(i==0){  
      sprintf(sd[(*ii)],"      if(p_adro = \"0000\") then\n");(*ii)++;    
      iadr++;
    }else{
      itobc(iadr,sd0,4);
      sprintf(sd[(*ii)],"      elsif(p_adro = \"%s\") then\n",sd0);(*ii)++;
      iadr++;
    }
    sscanf((*vd).sdfo[i][1],"%d",&i0);
    if(i0>32){
      if((*vd).nvmp==1){
          sprintf(sd[(*ii)],"        p_datao <=  %s(31 downto 0);\n",(*vd).sdfo[i][0]);(*ii)++;
      }else{
        for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
          if(ivmp==0){  
            sprintf(sd[(*ii)],"        if(p_adrovp = \"0000\") then\n");(*ii)++;    
          }else{
            itobc(ivmp,sd0,4);
            sprintf(sd[(*ii)],"        elsif(p_adrovp = \"%s\") then\n",sd0);(*ii)++;
          }
          sprintf(sd[(*ii)],"          p_datao <= %s%d(31 downto 0);\n",(*vd).sdfo[i][0],ivmp);(*ii)++; 	
        }
        sprintf(sd[(*ii)],"        end if;\n");(*ii)++;
      }

      itobc(iadr,sd0,4);
      sprintf(sd[(*ii)],"      elsif(p_adro = \"%s\") then\n",sd0);(*ii)++;
      iadr++;
      if((*vd).nvmp==1){
        if(i0==64){
	  sprintf(sb,"%s(63 downto 32)",(*vd).sdfo[i][0]);
        }else{
          itobc(0,sd0,64-i0);
          sprintf(sb,"\"%s\" & %s(%d downto 32)",sd0,(*vd).sdfo[i][0],i0-1);
        }
        sprintf(sd[(*ii)],"        p_datao <=  %s;\n",sb);(*ii)++;
      }else{
        for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
          if(ivmp==0){  
            sprintf(sd[(*ii)],"        if(p_adrovp = \"0000\") then\n");(*ii)++;    
          }else{
            itobc(ivmp,sd0,4);
            sprintf(sd[(*ii)],"        elsif(p_adrovp = \"%s\") then\n",sd0);(*ii)++;
          }
          if(i0==64){
	    sprintf(sb,"%s%d(63 downto 32)",(*vd).sdfo[i][0],ivmp);
          }else{
            itobc(0,sd0,64-i0);
            sprintf(sb,"\"%s\" & %s%d(%d downto 32)",sd0,(*vd).sdfo[i][0],ivmp,i0-1);
          }
          sprintf(sd[(*ii)],"          p_datao <= %s;\n",sb);(*ii)++; 	
        }
        sprintf(sd[(*ii)],"        end if;\n");(*ii)++;
      }
    }else{
      if(i0==32){
	strcpy(sb,(*vd).sdfo[i][0]);
      }else{
        itobc(0,sd0,32-i0);
        sprintf(sb,"\"%s\" & %s",sd0,(*vd).sdfo[i][0]);
      }
      if((*vd).nvmp==1){
          sprintf(sd[(*ii)],"        p_datao <=  %s;\n",sb);(*ii)++;
      }else{
        for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
          if(ivmp==0){  
            sprintf(sd[(*ii)],"        if(p_adrovp = \"0000\") then\n");(*ii)++;    
          }else{
            itobc(ivmp,sd0,4);
            sprintf(sd[(*ii)],"        elsif(p_adrovp = \"%s\") then\n",sd0);(*ii)++;
          }
          sprintf(sd[(*ii)],"          p_datao <= %s%d;\n",sb,ivmp);(*ii)++; 	
        }
        sprintf(sd[(*ii)],"        end if;\n");(*ii)++;
      }
    }
  }

  sprintf(sd[(*ii)],"      else\n");(*ii)++;
  sprintf(sd[(*ii)],"        p_datao <= \"00000000000000000000000000000000\";\n");(*ii)++;
  sprintf(sd[(*ii)],"      end if;\n");(*ii)++;  
  sprintf(sd[(*ii)],"    end if;\n");(*ii)++;
  sprintf(sd[(*ii)],"  end process;\n");(*ii)++;    
  sprintf(sd[(*ii)],"\n");(*ii)++;

}  

void generate_pg_set_idata(sd,vd,ii)
char sd[][STRLEN];
struct vhdl_description *vd;
int *ii;
{
  int i,i0,ivmp;
  char sd0[100];
  
  sprintf(sd[(*ii)],"  process(pclk) begin\n");(*ii)++;
  sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then\n");(*ii)++;
  sprintf(sd[(*ii)],"      if(p_we ='1') then\n");(*ii)++;  

  for(i=0;i<(*vd).iip;i++){
    if(i==0){  
      sprintf(sd[(*ii)],"        if(p_adri = \"0000\") then\n");(*ii)++;    
    }else{
      itobc(i,sd0,4);
      sprintf(sd[(*ii)],"        elsif(p_adri = \"%s\") then\n",sd0);(*ii)++;
    }
    if((*vd).nvmp==1){
      sscanf((*vd).sdip[i][1],"%d",&i0);
      sprintf(sd[(*ii)],"          %s <=  p_datai(%d downto 0);\n",(*vd).sdip[i][0],i0-1);(*ii)++; 
    }else{
      for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){
        if(ivmp==0){  
          sprintf(sd[(*ii)],"          if(p_adrivp = \"0000\") then\n");(*ii)++;    
        }else{
          itobc(ivmp,sd0,4);
          sprintf(sd[(*ii)],"          elsif(p_adrivp = \"%s\") then\n",sd0);(*ii)++;
        }
        sscanf((*vd).sdip[i][1],"%d",&i0);
        sprintf(sd[(*ii)],"            %s%d <=  p_datai(%d downto 0);\n",(*vd).sdip[i][0],ivmp,i0-1);(*ii)++; 	
      }
      sprintf(sd[(*ii)],"          end if;\n");(*ii)++;
    }
  }

  sprintf(sd[(*ii)],"        end if;\n");(*ii)++;
  sprintf(sd[(*ii)],"      end if;\n");(*ii)++;  
  sprintf(sd[(*ii)],"    end if;\n");(*ii)++;
  sprintf(sd[(*ii)],"  end process;\n");(*ii)++;    
  sprintf(sd[(*ii)],"\n");(*ii)++;

  if((*vd).nvmp>1){

    sprintf(sd[(*ii)],"  process(pclk) begin\n");(*ii)++;
    sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then\n");(*ii)++;
    for(ivmp=0;ivmp<(*vd).nvmp;ivmp++){ 
      if(ivmp==0){
        sprintf(sd[(*ii)],"      if(vmp_phase = \"0000\") then\n");(*ii)++;	
      }else{
	itobc(ivmp,sd0,4);
        sprintf(sd[(*ii)],"      elsif(vmp_phase = \"%s\") then\n",sd0);(*ii)++;
      }
      for(i=0;i<(*vd).iip;i++){
        sprintf(sd[(*ii)],"        %s <= %s%d;\n",(*vd).sdip[i][0],(*vd).sdip[i][0],ivmp);(*ii)++; 	
      }
    }
    sprintf(sd[(*ii)],"      end if;\n");(*ii)++;
    sprintf(sd[(*ii)],"    end if;\n");(*ii)++;
    sprintf(sd[(*ii)],"  end process;\n");(*ii)++;        
    sprintf(sd[(*ii)],"\n");(*ii)++;    

    sprintf(sd[(*ii)],"  process(pclk) begin                                 \n");(*ii)++;
    sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then                  \n");(*ii)++;
    sprintf(sd[(*ii)],"      irun(0) <= p_run;                               \n");(*ii)++;
    for(i=0;i<(*vd).nvmp-1;i++){
      sprintf(sd[(*ii)],"      irun(%d) <= irun(%d);                   \n",i+1,i);(*ii)++;        
    }
    sprintf(sd[(*ii)],"    end if;                                           \n");(*ii)++;
    sprintf(sd[(*ii)],"  end process;                                        \n");(*ii)++;        
    sprintf(sd[(*ii)],"                                                      \n");(*ii)++;    

    sprintf(sd[(*ii)],"  process(pclk) begin                                 \n");(*ii)++;
    sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then                  \n");(*ii)++;
    sprintf(sd[(*ii)],"      if(irun(%d) = '1') then           \n",(*vd).nvmp-1 );(*ii)++;    
    itobc((*vd).nvmp-1,sd0,4);     
    sprintf(sd[(*ii)],"        if(vmp_phase = \"%s\") then              \n", sd0);(*ii)++;
    sprintf(sd[(*ii)],"          vmp_phase <= \"0000\";                      \n");(*ii)++;
    sprintf(sd[(*ii)],"        else                                          \n");(*ii)++;
    sprintf(sd[(*ii)],"          vmp_phase <= vmp_phase + \"0001\";          \n");(*ii)++;
    sprintf(sd[(*ii)],"        end if;                                       \n");(*ii)++;    
    sprintf(sd[(*ii)],"      else                                            \n");(*ii)++;
    sprintf(sd[(*ii)],"        if(p_run = '1') then                          \n");(*ii)++;
    sprintf(sd[(*ii)],"          vmp_phase <= \"0000\";                      \n");(*ii)++;
    sprintf(sd[(*ii)],"        end if;                                       \n");(*ii)++;    
    sprintf(sd[(*ii)],"      end if;                                         \n");(*ii)++;        
    sprintf(sd[(*ii)],"    end if;                                           \n");(*ii)++;
    sprintf(sd[(*ii)],"  end process;                                        \n");(*ii)++;        
    sprintf(sd[(*ii)],"                                                      \n");(*ii)++;    
  }

}  

void generate_pg_set_jdata(sd,vd,ii)
char sd[][STRLEN];
struct vhdl_description *vd;
int *ii;
{
  int i,i0,i1,i2;
  
  if((*vd).nvmp==1){
    for(i=0;i<(*vd).ijp;i++){
      sscanf((*vd).sdjp[i][2],"%d",&i0);
      sscanf((*vd).sdjp[i][3],"%d",&i1);      
      sprintf(sd[(*ii)],"  %s(%d downto 0) <= p_jdata(%d downto %d);\n",(*vd).sdjp[i][0],i1-i0,i1,i0);(*ii)++; 
    }
  }else{
    sprintf(sd[(*ii)],"  process(pclk) begin\n");(*ii)++;
    sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then\n");(*ii)++;
    sprintf(sd[(*ii)],"      jdata1 <= p_jdata;            \n");(*ii)++;
    for(i=1;i<(*vd).nvmp-1;i++){
      sprintf(sd[(*ii)],"      jdata%d <= jdata%d;            \n",i+1,i);(*ii)++;
    }
    sprintf(sd[(*ii)],"    end if;\n");(*ii)++;
    sprintf(sd[(*ii)],"  end process;\n");(*ii)++;
    sprintf(sd[(*ii)],"              \n");(*ii)++;        

    sprintf(sd[(*ii)],"  process(pclk) begin\n");(*ii)++;
    sprintf(sd[(*ii)],"    if(pclk'event and pclk='1') then\n");(*ii)++;
    sprintf(sd[(*ii)],"      if(vmp_phase = \"0000\") then\n");(*ii)++;    
    for(i=0;i<(*vd).ijp;i++){
      sscanf((*vd).sdjp[i][2],"%d",&i0);
      sscanf((*vd).sdjp[i][3],"%d",&i1);
      sscanf((*vd).sdjp[i][1],"%d",&i2);      
      if(i2==((*vd).nvmp-1)){
        sprintf(sd[(*ii)],"        %s(%d downto 0) <= p_jdata(%d downto %d);\n",(*vd).sdjp[i][0],i1-i0,i1,i0);(*ii)++;
      }else{
        sprintf(sd[(*ii)],"        %s(%d downto 0) <= jdata%d(%d downto %d);\n",(*vd).sdjp[i][0],i1-i0,(*vd).nvmp-i2-1,i1,i0);(*ii)++;
      }
    }
    sprintf(sd[(*ii)],"      end if;\n");(*ii)++;
    sprintf(sd[(*ii)],"    end if;\n");(*ii)++;
    sprintf(sd[(*ii)],"  end process;\n");(*ii)++;    
  }
  if((*vd).ijp>0) sprintf(sd[(*ii)],"\n");(*ii)++;
}  

void generate_pg_set_data(sd,vd,ii)
char sd[][STRLEN];
struct vhdl_description *vd;
int *ii;
{
  int i,i0,i1;
  
  for(i=0;i<(*vd).ida;i++){
    sscanf((*vd).sdda[i][2],"%d",&i0);
    sscanf((*vd).sdda[i][3],"%d",&i1);    
    if(i0==i1){ 
      sprintf(sd[(*ii)],"  %s(%d) <= '%s';\n",(*vd).sdda[i][0],i0,(*vd).sdda[i][1]);(*ii)++;
    }else{
      sprintf(sd[(*ii)],"  %s(%d downto %d) <= \"%s\";\n",(*vd).sdda[i][0],i1,i0,(*vd).sdda[i][1]);(*ii)++;
    }
  }
  if((*vd).ida>0) sprintf(sd[(*ii)],"\n");(*ii)++;
}  

void generate_pg_rundelay(sdata,vd)
char sdata[][STRLEN];
struct vhdl_description *vd;
{
	int ndelay;
        int im,is;

	ndelay = atoi(sdata[1]);

  is = (*vd).is;
  sprintf((*vd).sds[is], "  signal run: std_logic_vector(%d downto 0);\n",ndelay+(*vd).nvmp);is++;
  (*vd).is = is; 

  im = (*vd).im;

  sprintf((*vd).sdm[im], "  process(pclk) begin\n" );im++;
  sprintf((*vd).sdm[im], "    if(pclk'event and pclk='1') then\n" );im++;
  sprintf((*vd).sdm[im], "      run(0) <= p_run;\n" );im++;
  sprintf((*vd).sdm[im], "      for i in 0 to %d loop\n",ndelay+(*vd).nvmp-1);im++;
  sprintf((*vd).sdm[im], "        run(i+1) <= run(i);\n" );im++;
  sprintf((*vd).sdm[im], "      end loop;\n" );im++;
  sprintf((*vd).sdm[im], "      p_runret <= run(%d);\n",ndelay+(*vd).nvmp);im++;
  sprintf((*vd).sdm[im], "    end if;\n" );im++;
  sprintf((*vd).sdm[im], "  end process;\n" );im++;
  sprintf((*vd).sdm[im], "\n" );im++;
  (*vd).im = im; 

}  

void conv_to_binary(i,bdata)
int i;
char bdata[STRLEN];
{
  if(i==0) strcpy(bdata,"0000");
  if(i==1) strcpy(bdata,"0001");
  if(i==2) strcpy(bdata,"0010");
  if(i==3) strcpy(bdata,"0011");
  if(i==4) strcpy(bdata,"0100");
  if(i==5) strcpy(bdata,"0101");
  if(i==6) strcpy(bdata,"0110");
  if(i==7) strcpy(bdata,"0111");
  if(i==8) strcpy(bdata,"1000");
  if(i==9) strcpy(bdata,"1001");
  if(i==10) strcpy(bdata,"1010");
  if(i==11) strcpy(bdata,"1011");
  if(i==12) strcpy(bdata,"1100");
  if(i==13) strcpy(bdata,"1101");
  if(i==14) strcpy(bdata,"1110");
  if(i==15) strcpy(bdata,"1111");
}

