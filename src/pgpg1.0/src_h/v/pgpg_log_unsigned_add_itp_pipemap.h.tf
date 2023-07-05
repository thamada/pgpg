/* 
 *   pipeline map
 *
 */

// Stratix, ESB, log17, man8, cut6 : 2004/04/20
static void generate_fixed_pipelinemap_stratix_nolc_log17_man_8_cut6(nstage)
     int* nstage;
{
  int npipe;
  int i,pmax;
  //  printf("pipelinemap : stratix nolcrom log17 man8 cut6\n");
  npipe = (*nstage);
  pmax = this.npmax;
  (this.is_pipe)[0] = 0; // this is not used.
  for(i=1;i<=(pmax-1);i++) (this.is_pipe)[i] = 0;
  if(npipe == 1){                             //---- NP : 1 -> FIXED TO 2
    npipe=2;
    printf("nstage: fixed to %d\n",npipe);
    (this.is_pipe[3]) = 1;
    (this.is_pipe)[pmax] = 1;
  }else if(npipe == 2){                       //---- NP : 2
    (this.is_pipe[3]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 3){                       //---- NP : 3
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 4){                       //---- NP : 4
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 5){                       //---- NP : 5
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 6){                       //---- NP : 6
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 7){                       //---- NP : 7
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe >= 7){                       //---- NP : >7
    npipe=7;
    printf("nstage: fixed to %d\n",npipe);
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3])=1;
    (this.is_pipe[4])=1;
    (this.is_pipe[6])=1;
    (this.is_pipe[7])=1;
    (this.is_pipe[pmax]) = 1;
  }else{                                      //---- NP : 0
    // null
  }
  (*nstage)=npipe;
  return;
}

// Stratix, LCELL, log17, man8, cut6 : 2004/04/20
static void generate_fixed_pipelinemap_stratix_lc_log17_man_8_cut6(nstage)
     int* nstage;
{
  int npipe;
  int i,pmax;
  npipe = (*nstage);
  pmax = this.npmax;
  for(i=0;i<=pmax;i++) (this.is_pipe)[i] = 0;
  if(npipe == 1){                             //---- NP : 1
    (this.is_pipe)[pmax] = 1;
  }else if(npipe == 2){                       //---- NP : 2
    (this.is_pipe[3]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 3){                       //---- NP : 3
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 4){                       //---- NP : 4
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 5){                       //---- NP : 5
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 6){                       //---- NP : 6
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe >= 7){                       //---- NP : >6
    npipe=6;
    printf("nstage: fixed to %d\n",npipe);
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else{                                      //---- NP : 0
    // null
  }
  (*nstage)=npipe;
  return;
}


// default
// pipelining map fixed for pg_log_unsigned_add_itp(log17,man8,cut6)
static void generate_fixed_pipelinemap(npipe)
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
    (this.is_pipe[5]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 3){                       //---- NP : 3 ,2003/12/16
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 4){                       //---- NP : 4 ,2003/12/16
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 5){                       //---- NP : 5
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 6){                       //---- NP : 6
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 7){                       //---- NP : 7
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == 8){                       //---- NP : 8
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
    (this.is_pipe[1]) = 1;
    (this.is_pipe[2]) = 1;
    (this.is_pipe[3]) = 1;
    (this.is_pipe[4]) = 1;
    (this.is_pipe[5]) = 1;
    (this.is_pipe[6]) = 1;
    (this.is_pipe[7]) = 1;
    (this.is_pipe[pmax]) = 1;
  }else if(npipe == pmax){                    //---- NP : MAX(=9, Apr/30/2003)
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 1;
  }else{                                      //---- NP : 0
    for(i=1;i<=pmax;i++) (this.is_pipe)[i] = 0;
  }
  return;
}

