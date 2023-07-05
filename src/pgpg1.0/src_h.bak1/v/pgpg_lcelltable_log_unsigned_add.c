/*
  PGPGV library: Lcell-Table generation routines for pg_log_unsigned_add
  Author: Tsuyoshi Hamada 
  Last modified at Sep 19, 2003.
*/
#include<stdio.h>
#include<stdlib.h>                 // malloc(),free()
#include<string.h>                 // strcpy()
#include<math.h>                   // log(),pow()
#include "pgpg.h"                  // struct vhdl_description,STRLEN
#include "pgpg_lcelltable_core.h"  // generate_pg_lcell_rom()
#define LONG long long int

static struct {
  char lcromname[256];
  LONG* rom;
  int rom_nstage;
  int rom_ilen;
  int rom_olen;
  int rom_depth; // entries of 'int rom[]' array (not entries of ROM)
} this;

static void create_lcell_rom(nbit_man, nbit_tin)
     int nbit_man;
     int* nbit_tin;
{
  int i,iy,nentry;
  double inscale,x;
  inscale = (double)(1<<nbit_man);
  i = 0;
  do{
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    i++; 
  }while(iy>0);
  i--;          /* cut a address at iy=0 */

  *nbit_tin = (int) (log((double)i)/log(2.0)+1.0);
  nentry = (int) pow(2.0, (double)(*nbit_tin));

  this.rom = (LONG*)malloc(sizeof(LONG) * nentry);
  this.rom[0] = 0x0;
  for(i=1;i<nentry;i++){
    x = 1.0 + pow(2.0, - ((double)i+0.25)/inscale);
    iy = (int) (0.5 + inscale*log(x)/log(2.0));   
    (this.rom)[i] = (LONG)iy;
  }
  this.rom_ilen = *nbit_tin;
  this.rom_olen = nbit_man;
  this.rom_depth = nentry;

  /* 
     ROM IDENTIFICATION
     ROM name must be 4 words(16-bit) as follows.
     fixed(1-bit) | Table Type id(4-bit) | Object information(11-bit)|   
     | Nman(6-bit) | Ncut(5-bit) | 
  */
  {
    int fname_vec;
    int type = 3; /* 3: pg_log_unsigned_add */
    fname_vec =  (0x1<<15)| (type<<11) | ((0x3f&nbit_man)<<5) | (0x1f&0);
    sprintf(this.lcromname,"%04x",fname_vec);
  }

  return;
}










/***********************************
 *  PUBLIC MEMBERS ARE AS FOLLOWS  *
 ***********************************/


/* +------------------------------------------------------------------+
   | calculate the table data and return nbit_tin for upper VHDL code |
   +------------------------------------------------------------------+
   You MUST call this function firstly.
 */
void create_table_lcell_log_unsigned_add(nbit_man,nstage,nbit_tin,mname)
     int nbit_man;
     int nstage;
     int* nbit_tin;
     char* mname;
{
  /* set pipeline stage for other API "generate_pg_table_lcell(sd)" */
  this.rom_nstage = nstage; 

  /* initialize table data and get the input-length */
  create_lcell_rom(nbit_man, nbit_tin);

  /* set module name to "nmame" */
  {
    int nbit_in,nbit_out,nstage;
    char lcell_index[256];
    nbit_in  = this.rom_ilen;
    nbit_out = this.rom_olen;
    nstage   = this.rom_nstage;
    strcpy(lcell_index,this.lcromname);  
    sprintf(mname,"lcell_rom_%s_%d_%d_%d",lcell_index,nbit_in,nbit_out,nstage);
  }
}


/* +------------------------------------------------------------------+
   | generate the lcell table                                         |
   +------------------------------------------------------------------+ */
void generate_pg_table_lcell_of_log_unsigned_add(sd)
     struct vhdl_description *sd;
{
  int ilen,olen,depth,nstage;
  char name[256];
  LONG* rom;
  rom   = this.rom;
  ilen  = this.rom_ilen;
  olen  = this.rom_olen;
  depth = this.rom_depth;
  nstage = this.rom_nstage;
  strcpy(name,this.lcromname);
  generate_pg_lcell_rom(sd,ilen,olen,depth,nstage,name,rom);
  free(this.rom);
}




/* --- SAMPLE CODE ---
   int nbit_man,nstage,nbit_tin;
   struct vhdl_description *sd;
   char lcname[256];
   nbit_man=8;
   nstage=1;

   //--- API(1) ---
   create_table_lcell_log_unsigned_add(nbit_man,nstage,&nbit_tin,lcname);
   //--- API(2) ---
   generate_pg_table_lcell_of_log_unsigned_add(&sd);
*/








