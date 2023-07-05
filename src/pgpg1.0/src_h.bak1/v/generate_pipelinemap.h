/* 
   Last modufied at Mar 31, 2003
   Author: Tsuyoshi Hamada

   --- NOTE ---
   This code needs 'this' object which has the following members:
     int is_pipe[NPMAX];
     int npmax;
     double pipe_delay[NPMAX];
     double reg_reg_delay; .
   And these(except for is_pipe[]) are specified before the following subroutines called. 
*/

#include<stdlib.h> //exit(),malloc(),free(),

// This subroutine is called only by generate_pipelinemap().
static void recursive_pipeline_search(int npipe,
				      int jindex,
				      int* j,
				      int pmax,
				      double* sum,
				      double* delay_minmax,
				      int* crit_path,
				      int* np_point)
{
  if(jindex == 0){
    for(j[0]=1;j[0]<pmax;j[0]++)
      recursive_pipeline_search(npipe,jindex+1,j,pmax,sum,delay_minmax,crit_path,np_point);
  }else if(jindex < npipe-2){
    for(j[jindex]=j[jindex-1]+1;j[jindex]<pmax; j[jindex]++)
      recursive_pipeline_search(npipe,jindex+1,j,pmax,sum,delay_minmax,crit_path,np_point);
  }else{
    // 'jindex' equals to 'npipe-2'
    for(j[jindex]=j[jindex-1]+1;j[jindex]<pmax;j[jindex]++){
      double delay_max;
      int cpath,i,k;
      for(k=0;k<npipe;k++) sum[k] = 0.0; // init
      for(k=0;k<=j[0];k++)          sum[0] += (this.pipe_delay)[k];
      for(i=0;i<jindex;i++)
	for(k=j[i]+1;k<=j[i+1];k++) sum[i+1] += (this.pipe_delay)[k];
      for(k=j[jindex]+1;k<=pmax;k++) sum[jindex+1] += (this.pipe_delay)[k];
      delay_max = sum[0]; cpath=0;
      for(k=0;k<npipe;k++) if(sum[k]>delay_max){ delay_max = sum[k]; cpath=k;}
      if(*delay_minmax>delay_max){
	*delay_minmax = delay_max;
	*crit_path = cpath;
	for(k=0;k<npipe;k++) np_point[k] = j[k];
      }
      //    for(k=0;k<npipe;k++) printf("%d\t",j[k]);
      //    for(k=0;k<npipe;k++) printf("%f\t",sum[k]);
      //    printf("%f\n",*delay_minmax);
    }
  }
}

static void generate_pipelinemap(npipe)
     int npipe;
{
  int i,pmax;
  double total_delay,interval;
  pmax = this.npmax;
  if(pmax < npipe ){fprintf(stderr,"Err at %s ",__FILE__);printf("line %d.\n",__LINE__);exit(1);}
  total_delay = 0.0;
  for(i=0;i<=pmax;i++){
    (this.is_pipe)[i] = 0;  // set all of the pipelines off
    total_delay += (this.pipe_delay)[i];
  }
  // --- NP0,1 ---
  if(npipe > 0){
    (this.is_pipe)[pmax] = 1; // set the last pipeline on
    interval = total_delay /((double)npipe);
  }else{ return;}
  if(npipe == 1) return;

  // --- NP2 ---
  if(npipe == 2){
    int k,crit_path;
    int* j;
    int* np_point;
    double delay_minmax;
    double* sum;
    delay_minmax = total_delay*2.0;
    j = (int*)malloc(sizeof(int) * npipe);
    np_point = (int*)malloc(sizeof(int) * npipe);
    sum = (double*)malloc(sizeof(double) * npipe);
    j[npipe-1] = pmax;
    for(j[0]=1;j[0]<(pmax-2); j[0]++){
      double delay_max;
      int cpath;
      for(k=0;k<npipe;k++) sum[k] = 0.0; // init
      for(k=0;k<=j[0];k++)    sum[0] += (this.pipe_delay)[k];
      for(k=j[0]+1;k<=pmax;k++) sum[1] += (this.pipe_delay)[k];
      delay_max = sum[0]; cpath=0;
      for(k=0;k<npipe;k++) if(sum[k]>delay_max){ delay_max = sum[k]; cpath=k;}
      if(delay_minmax>delay_max){
	delay_minmax = delay_max;
	crit_path = cpath;
	for(k=0;k<npipe;k++) np_point[k] = j[k];
      }
      //	  for(k=0;k<npipe;k++) printf("%d\t",j[k]);
      //	  for(k=0;k<npipe;k++) printf("%f\t",sum[k]);
      //	  printf("%f\n",delay_minmax);
    }
    for(k=0;k<npipe;k++) (this.is_pipe)[np_point[k]] = 1;
#ifdef DEBUG
    printf("\tPipeline Points:",np_point[k]);
    for(k=0;k<npipe;k++) printf("\tPip(%d)",np_point[k]);
    printf("\n",np_point[k]);
    if(crit_path>0)
      printf("\tCritical path: Pip(%d) to Pip(%d)\n",np_point[crit_path-1],np_point[crit_path]);
    else
      printf("\tCritical path: Input to Pip(%d)\n",np_point[crit_path]);
    //    printf("\tMax delay(estimated): %f(segs)\n",delay_minmax+this.reg_reg_delay);
    printf("\tMax delay(estimated): %f(segs)\n",delay_minmax);
#endif
  }

  // --- NP>2 ---
  if(npipe > 2){
    int k,crit_path;
    int* j;
    int* np_point;
    double delay_minmax;
    double* sum;
    delay_minmax = total_delay*2.0;
    j = (int*)malloc(sizeof(int) * npipe);
    np_point = (int*)malloc(sizeof(int) * npipe);
    sum = (double*)malloc(sizeof(double) * npipe);
    j[npipe-1] = pmax;

    recursive_pipeline_search(npipe,0,j,pmax,sum,&delay_minmax,&crit_path,np_point);
    for(k=0;k<npipe;k++) (this.is_pipe)[np_point[k]] = 1;

#ifdef DEBUG
    printf("\tPipeline Points:",np_point[k]);
    for(k=0;k<npipe;k++) printf("\tPip(%d)",np_point[k]);
    printf("\n",np_point[k]);
    if(crit_path>0)
      printf("\tCritical path: Pip(%d) to Pip(%d)\n",np_point[crit_path-1],np_point[crit_path]);
    else
      printf("\tCritical path: Input to Pip(%d)\n",np_point[crit_path]);
    //    printf("\tMax delay(estimated): %f(segs)\n",delay_minmax+this.reg_reg_delay);
    printf("\tMax delay(estimated): %f(segs)\n",delay_minmax);
#endif
  }

  //-------------------------------------------
  //  if(1){
  if(0){
    int i,from;
    double length=0.0;
    from=0;
    printf("\t");
    for(i=1;i<=pmax;i++){
      if((this.is_pipe)[i]||i==pmax){
	length +=(this.pipe_delay)[i];
	printf("Pipe(%d)-%f-",from,length);
	length = 0.0;
	from = i;
      }else{
	length +=(this.pipe_delay)[i];
      }
    }
    printf("Pipe(%d)\n",pmax);
  }
  //-------------------------------------------

}

// pipelining manually (for debugging)
//static void generate_pipelinemap(npipe)
#ifdef DEBUG_PIPELINING
static void generate_pipelinemap_manual(npipe)
     int npipe;
{
  int i,pmax;
  pmax = this.npmax;
  (this.is_pipe)[0] = 0; // this is not used.
  if(npipe == 1){                             //---- NP : 1
    for(i=1;i<=(pmax-1);i++) (this.is_pipe)[i] = 0;
    (this.is_pipe)[pmax] = 1;
  }else if(npipe == 2){                       //---- NP : 2
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 3){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 4){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 5){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 6){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 7){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == 8){                       //---- NP : 3
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[8]) = 1;
    (this.is_pipe[9]) = 1;
  }else if(npipe == pmax){                    //---- NP : MAX
    //    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
  }else{                                      //---- NP : 0
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  }


  //  for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  //  (this.is_pipe[2]) = 1;
  //  (this.is_pipe[6]) = 1;
  //  (this.is_pipe[9]) = 1;

  return;
}
#endif
