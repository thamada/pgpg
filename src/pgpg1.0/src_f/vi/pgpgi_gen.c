#include<stdio.h>
#include "pgpgi.h"

void generate_pg_pipe(vd,filename)
struct vhdl_description *vd;
char filename[];
{

  int ii,i,ivmp,i0,lsb,msb;
  static char sd[20000][STRLEN];
  char sdjpd[100][2][STRLEN];  
  char sdtmp[STRLEN];
  FILE *fp,*fopen();
  int ndip,ndfo,npipe; 
  double pow();
  
  ii = 0;
  ndfo = (*vd).ifod;
  ndip = (*vd).iipd;

  /*  header */

  sprintf(sd[ii], "/*                                                                  \n" );ii++;
  sprintf(sd[ii], "   Interface Program for PG2                                        \n" );ii++;
  sprintf(sd[ii], "     generated by pgpgi : ver 0.0, Dec 29 2004                      \n" );ii++;
  sprintf(sd[ii], "     Toshiyuki Fukushige                                            \n" );ii++;
  sprintf(sd[ii], "*/                                                                  \n" );ii++;
  sprintf(sd[ii], "#include <stdio.h>                                                  \n" );ii++;
  sprintf(sd[ii], "#include <math.h>                                                   \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;    

    /* function */

  for(i=0;i<(*vd).ifunc;i++) sprintf(sd[i+ii],"%s",(*vd).sdfunc[i]);
  ii += (*vd).ifunc;
  sprintf(sd[ii], "{                                                                   \n" );ii++;

  /* define statement */
  sprintf(sd[ii], "  int i,j,ii,nn;                                                    \n" );ii++;
  sprintf(sd[ii], "  int devid,npipe;                                                  \n" );ii++;
  sprintf(sd[ii], "  static int initflag=1;                                            \n" );ii++;  
  sprintf(sd[ii], "  int ndj,ni,nd,nword,retword;                                      \n" );ii++;
  sprintf(sd[ii], "  unsigned int ipdata[2048];                                        \n" );ii++;    
  sprintf(sd[ii], "  unsigned int jpdata[2048];                                        \n" );ii++;
  sprintf(sd[ii], "  unsigned int fodata[2048];                                        \n" );ii++;  
  
  for(i=0;i<(*vd).idef;i++){
    int iflag,j;
    iflag = 0;
    for(j=0;j<i;j++){
      if(strcmp((*vd).sddef[i],(*vd).sddef[j])==0) iflag=1;
    }
    if(iflag==0){
      strcpy(sd[ii],"  ");
      sprintf(sdtmp,"%s",(*vd).sddef[i]);
      strcat(sd[ii],sdtmp);
      ii++;
    }
  }
  sprintf(sd[ii], "                                                                    \n" );ii++;
  
    /* setting (num, init, JPREG, FOREG) */ 

  npipe = (*vd).npipe*(*vd).nvmp;
  sprintf(sd[ii], "  devid = 0;                                                        \n" );ii++;
  sprintf(sd[ii], "  npipe = %d;                                                 \n",npipe );ii++;  
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "  if(initflag == 1){                                                \n" );ii++;
  sprintf(sd[ii], "    g6_init(devid);                                                 \n" );ii++;
  sprintf(sd[ii], "    g6_send_fpga_data(devid,\"pgfpga.ttf\");                        \n" );ii++;
  sprintf(sd[ii], "    initflag = 0;                                                   \n" );ii++;
  sprintf(sd[ii], "  }                                                                 \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "  ndj = 2;                                                          \n" );ii++;
  sprintf(sd[ii], "  ipdata[0] = 0x2000;                                               \n" );ii++;
  sprintf(sd[ii], "  ipdata[1] = 0x2;                                                  \n" );ii++;
  sprintf(sd[ii], "  ipdata[2] = ndj;                                                  \n" );ii++;
  sprintf(sd[ii], "  ipdata[3] = 0x0;                                                  \n" );ii++;
  sprintf(sd[ii], "  g6_set_ipdata(devid,ipdata);                                      \n" );ii++;  
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "  ni = %d;                                                    \n", npipe);ii++;
  sprintf(sd[ii], "  nd = %d;                                                    \n",2*ndfo);ii++;    
  sprintf(sd[ii], "  ipdata[0] = 0x3002;                                               \n" );ii++;
  sprintf(sd[ii], "  ipdata[1] = 0x2;                                                  \n" );ii++;
  sprintf(sd[ii], "  ipdata[2] = ni;                                                   \n" );ii++;
  sprintf(sd[ii], "  ipdata[3] = nd;                                                   \n" );ii++;
  sprintf(sd[ii], "  g6_set_ipdata(devid,ipdata);                                      \n" );ii++;  
  sprintf(sd[ii], "                                                                    \n" );ii++;
  
    /* j-loop */    
  
  sprintf(sd[ii], "  for(j=0;j<n;j++){                                             \n\n" );ii++;
  
    /* jpdata */

  for(i=0;i<(*vd).ijp;i++){
    strcpy(sd[i+ii],"    ");
    sprintf(sdtmp,"%s",(*vd).sdjp[i]);
    strcat(sd[i+ii],sdtmp);
  }
  ii += (*vd).ijp;
  sprintf(sd[ii], "                                                                    \n" );ii++;

  for(i=0;i<(*vd).ijpd;i++){
    double nmask;
    unsigned int imask;
    sscanf((*vd).sdjpd[i][1],"%d",&ivmp);
    sscanf((*vd).sdjpd[i][2],"%d",&lsb);
    sscanf((*vd).sdjpd[i][3],"%d",&msb);        

    if(ivmp>=((*vd).nvmp)){
      printf("pgpgvi error: ivmp >= nvmp\n");
      exit(1);
    }
    if(msb>63){
      printf("pgpgvi error: jdata width = 64\n");
      exit(1);
    }
    
    if(lsb<32){
      if(msb<32){ 
        nmask = msb - lsb + 1;
        imask = pow(2.0,nmask)-1;
        sprintf(sdtmp,"| ((0x%x & %s) << %d) ",imask,(*vd).sdjpd[i][0],lsb);
        strcat(sdjpd[ivmp][0],sdtmp);
      }else{
	nmask = 32 - lsb;
        imask = pow(2.0,nmask)-1;
        sprintf(sdtmp,"| ((0x%x & %s) << %d) ",imask,(*vd).sdjpd[i][0],lsb);
        strcat(sdjpd[ivmp][0],sdtmp);
	nmask = msb - 31;
        imask = pow(2.0,nmask)-1;
        sprintf(sdtmp,"| (0x%x & (%s >> %d)) ",imask,(*vd).sdjpd[i][0],32-lsb);
        strcat(sdjpd[ivmp][1],sdtmp);
      }
    }else{
      nmask = msb - lsb + 1;      
      imask = pow(2.0,nmask)-1;
      sprintf(sdtmp,"| ((0x%x & %s) << %d) ",imask,(*vd).sdjpd[i][0],lsb-32);
      strcat(sdjpd[ivmp][1],sdtmp);
    }
  }
  
  sprintf(sd[ii], "    nword = 4;                                                      \n" );ii++;
  sprintf(sd[ii], "    jpdata[0] = 0xffc00;                                            \n" );ii++;
  for(i=0;i<(*vd).nvmp;i++){ 
    sprintf(sd[ii], "    jpdata[1] = %d*j+%d;                 \n",(*vd).nvmp,(*vd).nvmp-i-1);ii++;
    sprintf(sd[ii], "    jpdata[2] = 0x0 %s;  \n",sdjpd[i][0] );ii++;
    sprintf(sd[ii], "    jpdata[3] = 0x0 %s;  \n",sdjpd[i][1] );ii++;
    sprintf(sd[ii], "    g6_set_jpdata(devid,nword,jpdata);                              \n" );ii++;    
    sprintf(sd[ii], "                                                                    \n" );ii++;
  }
  
    /* end of j-loop */

  sprintf(sd[ii], "  }                                                                 \n\n" );ii++;

    /* i-loop */

  sprintf(sd[ii], "  for(i=0;i<n;i+=npipe){                                              \n" );ii++;
  sprintf(sd[ii], "    if((i+npipe)>n){                                                  \n" );ii++;
  sprintf(sd[ii], "      nn = n - i;                                                     \n" );ii++;
  sprintf(sd[ii], "    }else{                                                            \n" );ii++;
  sprintf(sd[ii], "      nn = npipe;                                                     \n" );ii++;
  sprintf(sd[ii], "    }                                                                 \n" );ii++;
  sprintf(sd[ii], "    for(ii=0;ii<nn;ii++){                                             \n" );ii++;          
  
    /* ipdata */

  for(i=0;i<(*vd).iip;i++){
    strcpy(sd[i+ii],"      ");
    sprintf(sdtmp,"%s",(*vd).sdip[i]);
    strcat(sd[i+ii],sdtmp);
  }
  ii += (*vd).iip;

  if(ndip>16){
    printf("pgpgi does not support ndip > 16\n");
    exit(1);
  }
  sprintf(sd[ii], "      ipdata[0] = 0x0 | (ii<<4);                                     \n" );ii++;
  sprintf(sd[ii], "      ipdata[1] = %d;                                            \n",ndip);ii++;  
  for(i=0;i<ndip;i++){
    int imask,nbit;
    sscanf((*vd).sdipd[i][1],"%d",&nbit);
    imask = ((unsigned int)pow(2.0,(double)nbit))-1;
    sprintf(sd[ii], "      ipdata[%d] = 0x%x & %s;          \n",i+2,imask,(*vd).sdipd[i][0]);ii++;
  }
  sprintf(sd[ii], "      g6_set_ipdata(devid,ipdata);                                  \n" );ii++;
  sprintf(sd[ii], "    }                                                               \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

    /* n and run */

  sprintf(sd[ii], "    ipdata[0] = 0x4000;                                             \n" );ii++;
  sprintf(sd[ii], "    ipdata[1] = 0x1;                                                \n" );ii++;
  sprintf(sd[ii], "    ipdata[2] = %d*n;                                     \n",(*vd).nvmp);ii++;
  sprintf(sd[ii], "    g6_set_ipdata(devid,ipdata);                                    \n" );ii++;  
  sprintf(sd[ii], "                                                                    \n" );ii++;

    /* fodata */

  sprintf(sd[ii], "    nword = ni * nd;                                                \n" );ii++;  
  sprintf(sd[ii], "    retword = g6_get_fodata(devid,nword,fodata);                    \n" );ii++;  
  sprintf(sd[ii], "                                                                    \n" );ii++;
  sprintf(sd[ii], "    for(ii=0;ii<nn;ii++){                                           \n" );ii++;  

  for(i=0;i<ndfo;i++){
    sprintf(sd[ii], "      %s = ((long long int)fodata[%d+nd*ii] << 32)\n",(*vd).sdfod[i],2*i+1);ii++;
    sprintf(sd[ii], "           | (long long int)fodata[%d+nd*ii];                     \n",2*i);ii++;    
  }

  for(i=0;i<(*vd).ifo;i++){
    strcpy(sd[i+ii],"      ");
    sprintf(sdtmp,"%s",(*vd).sdfo[i]);
    strcat(sd[i+ii],sdtmp);
  }
  ii += (*vd).ifo;

  sprintf(sd[ii], "    }                                                               \n" );ii++;
  sprintf(sd[ii], "                                                                    \n" );ii++;

    /* end of i-loop */

  sprintf(sd[ii], "  }                                                                   \n" );ii++;

    /* end of function */

  sprintf(sd[ii], "}                                                                   \n" );ii++;  
  
    /* print out */

  strcat((*vd).filename,".c");
  fp = fopen((*vd).filename,"w");
  for(i=0;i<ii;i++) fprintf(fp,"%s",sd[i]);
  fclose(fp);

}



     

		     
