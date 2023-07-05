#include<stdio.h>
#include "pgpgi.h"

/***** from http://www.hidecnet.ne.jp/~sinzan/tips/c *******************/

char *StrShift( char *String, size_t nShift )
{
    char *start = String;
    char *stop  = String + strlen( String );
    memmove( start + nShift, start, stop-start+1 );
    return String + nShift;

}//StrShift

char *StrReplace( char *String, const char *From, const char *To )
{
       int   nToLen;
       int   nFromLen;
       int   nShift;
       char *start;
       char *stop;
       char *p;
       
       nToLen   = strlen( To );
       nFromLen = strlen( From );
       nShift   = nToLen - nFromLen;
       start    = String;
       stop     = String + strlen( String );

       while( NULL != ( p = (char*)strstr( start, From ) ) )
       {
          start = StrShift( p + nFromLen, nShift );
          stop  = stop + nShift;
          memmove( p, To, nToLen );
       }
       return String;

}//StrReplace

/***********************************/
	
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


void ufix_conversion(psd,i,xufix,xreal,xscale,xoffset,nbit)
     char psd[][STRLEN];
     int *i;
     char xufix[];
     char xreal[];     
     char xscale[];
     char xoffset[];
     int nbit;
{
  int mask;
  if(nbit==32){
    sprintf(psd[*i],"%s = ((unsigned int) ((%s + %s) * %s + 0.5)) & 0xffffffff;\n",xufix,xreal,xoffset,xscale);
  }else{
    mask  = (1<<nbit) - 1; 
    sprintf(psd[*i],"%s = ((unsigned int) ((%s + %s) * %s + 0.5)) & 0x%x ;\n",xufix,xreal,xoffset,xscale,mask);
  }
  (*i)++;
}

void fix_conversion(psd,i,xufix,xreal,xscale,nbit)
     char psd[][STRLEN];
     int *i;
     char xufix[];
     char xreal[];     
     char xscale[];
     int nbit;
{
  int mask;
  if(nbit==32){
    sprintf(psd[*i],"%s = ((int) (%s * %s + 0.5)) & 0xffffffff;\n",xufix,xreal,xscale);
  }else{
    mask  = (1<<nbit) - 1; 
    sprintf(psd[*i],"%s = ((int) (%s * %s + 0.5)) & 0x%x ;\n",xufix,xreal,xscale,mask);
  }
  (*i)++;
}

void log_conversion(psd,i,xlog,xreal,xscale,nbit,nman)
     char psd[][STRLEN];
     int *i;
     char xlog[];
     char xreal[];     
     char xscale[];
     int nbit;
     int nman;     
{
  int mask,bsnz,bsnzm;

  mask  = (1<<(nbit-2)) - 1; 
  bsnz =  1<<(nbit-2);
  bsnzm =  3<<(nbit-2);  
  sprintf(psd[*i],"if(%s == 0.0){                                         \n",xreal);(*i)++;
  sprintf(psd[*i],"  %s = 0;                                                \n",xlog);(*i)++;
  sprintf(psd[*i],"}else if(%s > 0.0){                                    \n",xreal);(*i)++;
  sprintf(psd[*i],"  %s = (((int)(pow(2.0,%d.0)*log(%s*%s)/log(2.0))) & 0x%x) | 0x%x;\n",xlog,nman,xreal,xscale,mask,bsnz);(*i)++;
  sprintf(psd[*i],"}else{                                                 \n");(*i)++;
  sprintf(psd[*i],"  %s = (((int)(pow(2.0,%d.0)*log(-%s*%s)/log(2.0))) & 0x%x) | 0x%x;\n",xlog,nman,xreal,xscale,mask,bsnzm);(*i)++;
  sprintf(psd[*i],"}                                                      \n");(*i)++;
  
}

main(argc,argv)
int argc;
char *argv[];
{
	char isdata[STRLEN];
	char isdata2[STRLEN];
	static char sdata[100][STRLEN];
	char *p;
	int i,j,ii,jj;
	FILE *fp,*fopen();
        static struct vhdl_description sd;
	int itmp;
	char stmp[STRLEN];
	int dtmp;
   	char filename[STRLEN];
	    
        sd.nvmp = 1;
	sd.npipe = 1;
        sd.ifunc = 0;
        sd.ipipe = 0;
        sd.idef = 0;	
	sd.ijp = sd.iip = sd.ifo = 0;
        sd.ijpd = 0;
	
        if(argc!=2){
	  printf("pgpgc2 filename\n");
	  exit(0);
	}else{
  	  fp = fopen(argv[1],"r");
	}
	
	while(fgets(isdata,STRLEN,fp)!=NULL){	  
	  i = 0;
	  strcpy(isdata2,isdata);
	  p = (char*)strtok(isdata,"( ");
	  strcpy(sdata[i],p);
	  i++;
	  if((strcmp(sdata[0],"/JPSET")==0)||(strcmp(sdata[0],"/IPSET")==0)||(strcmp(sdata[0],"/FOSET")==0)){
	    StrReplace(isdata2,"pow(2.0,","pow(2.0");
	    p = (char*)strtok(isdata2,"( ");
	    strcpy(sdata[0],p);
	    i=1;
	    while((p = (char*)strtok(NULL, ",; "))!=NULL){
	      strcpy(sdata[i],p);
	      i++;
	    }
	    for(j=0;j<i;j++){
	      StrReplace(sdata[j],"pow(2.0","pow(2.0,");
	    }
	    StrReplace(sdata[i-1],"\n","");	      
	  }else{
	    while((p = (char*)strtok(NULL, ",(); "))!=NULL){
	      strcpy(sdata[i],p);
	      i++;
	    }
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

          if(strcmp(sdata[0],"void")==0) strcpy(sd.filename,sdata[1]);
	     
          if((strcmp(sdata[0],"void")==0)||(strcmp(sdata[0],"int")==0)||(strcmp(sdata[0],"double")==0)){
	    strcpy(sd.sdfunc[sd.ifunc],isdata2);
	    (sd.ifunc)++;
	  }

	  if(strcmp(sdata[0],"/JPSET")==0){
	    StrReplace(sdata[5],"[]","[j]");
	    if(strcmp(sdata[6],"ufix")==0){
	      ufix_conversion(&(sd.sdjp),&(sd.ijp),sdata[1],sdata[5],sdata[8],sdata[9],atoi(sdata[7]));
	    }
	    if(strcmp(sdata[6],"fix")==0){
	      fix_conversion(&(sd.sdjp),&(sd.ijp),sdata[1],sdata[5],sdata[8],atoi(sdata[7]));
	    }
	    if(strcmp(sdata[6],"log")==0){
	      log_conversion(&(sd.sdjp),&(sd.ijp),sdata[1],sdata[5],sdata[9],atoi(sdata[7]),atoi(sdata[8]));
	    }
            sprintf(sd.sddef[sd.idef],"int %s;\n",sdata[1]);(sd.idef)++;
	    strcpy(sd.sdjpd[sd.ijpd][0],sdata[1]);
	    strcpy(sd.sdjpd[sd.ijpd][1],sdata[2]);
	    strcpy(sd.sdjpd[sd.ijpd][2],sdata[3]);
	    strcpy(sd.sdjpd[sd.ijpd][3],sdata[4]);
	    (sd.ijpd)++;	    	    
	  }

	  if(strcmp(sdata[0],"/IPSET")==0){
	    StrReplace(sdata[2],"[]","[i+ii]");
	    if(strcmp(sdata[3],"ufix")==0){
	      ufix_conversion(&(sd.sdip),&(sd.iip),sdata[1],sdata[2],sdata[5],sdata[6],atoi(sdata[4]));
	    }
	    if(strcmp(sdata[3],"fix")==0){
	      fix_conversion(&(sd.sdip),&(sd.iip),sdata[1],sdata[2],sdata[5],atoi(sdata[4]));
	    }
	    if(strcmp(sdata[3],"log")==0){
	      log_conversion(&(sd.sdip),&(sd.iip),sdata[1],sdata[2],sdata[6],atoi(sdata[4]),atoi(sdata[5]));
	    }
            sprintf(sd.sddef[sd.idef],"int %s;\n",sdata[1]);(sd.idef)++;
	    strcpy(sd.sdipd[sd.iipd][0],sdata[1]);
	    strcpy(sd.sdipd[sd.iipd][1],sdata[4]);
	    (sd.iipd)++;	    	    
	  }

	  if(strcmp(sdata[0],"/FOSET")==0){
	    StrReplace(sdata[2],"[]","[i+ii]");
	    itmp = atoi(sdata[4]);
	    if(itmp<33){
	      printf("pgpgi does'n support 32bit-interger for fodata\n");
	      exit(0);
	    }
            sprintf(sd.sdfo[sd.ifo],"%s = ((double)(%s<<%d))*%s/pow(2.0,%d.0);\n",sdata[2],sdata[1],64-itmp,sdata[5],64-itmp);(sd.ifo)++;
            sprintf(sd.sddef[sd.idef],"long long int %s;\n",sdata[1]);(sd.idef)++;
            strcpy(sd.sdfod[sd.ifod],sdata[1]);
	    (sd.ifod)++;	    	    
	  }

	}

	fclose(fp);

	generate_pg_pipe(&sd);

}

     

		     



