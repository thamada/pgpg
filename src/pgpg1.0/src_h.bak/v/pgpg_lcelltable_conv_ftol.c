/*
  PGPGV library: Lcell-Table generation routines for pg_conv_ftol
  Author: Tsuyoshi Hamada 
  Last modified at Sep 19, 2003.
*/
#include<stdio.h>
#include<stdlib.h>                 // malloc(),free()
#include<string.h>                 // strcpy()
#include<math.h>                   // log()
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

static void create_lcell_rom( nfix,nbit_in,nbit_out,nadd)
     int nfix,nbit_in,nbit_out,*nadd;
{
  int i,iy,nentry,outmask;
  double inscale,outscale;

  nentry = 1<<nbit_in;
  inscale = (double)nentry;
  outscale = (double)(1<<nbit_out);

  this.rom = (LONG*)malloc(sizeof(LONG) * nentry);

  *nadd = 0;
  for(i=0;i<nentry;i++){
    iy = outscale*log(1.0+(((double)i)+0.5)/inscale)/log(2.0)+0.5;
    if(iy==((int)outscale)) (*nadd)++;
    outmask = ((int)outscale)-1;
    this.rom[i] = (LONG)(outmask&iy);
  }

  this.rom_ilen = nbit_in;
  this.rom_olen = nbit_out;
  this.rom_depth = nentry;

  /* 
     ROM IDENTIFICATION
     ROM name must be 4 words(16-bit) as follows.
     fixed(1-bit) | Table Type id(4-bit) | Object information(11-bit)|   
                                         | Nfix(6-bit) | Nman(5-bit) | 
  */
  {
    int fname_vec;
    int type = 0; /* 0: conv_ftol */
    int nman = nbit_out;
    fname_vec =  (0x1<<15)| (type<<11) | ((0x3f&nfix)<<5) | (0x1f&nman);
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
void create_table_lcell_conv_ftol(nfix,nbit_man,nstage,nadd,mname)
     int nfix;     // fixed-point format length (this is only used in module name)
     int nbit_man; // mantissa length (= table output length)
     int nstage;   // pipeline stages
     int* nadd;    // table overflow counts
     char* mname;  // module name
{

  /* set pipeline stage for other API "generate_pg_table_lcell(sd)" */
  this.rom_nstage = nstage;

  /* initialize table data and get the add-length */
  {
    int nbit_in;
    nbit_in = nbit_man + 2;
    create_lcell_rom(nfix,nbit_in,nbit_man,nadd);
  }

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
void generate_pg_table_lcell_of_conv_ftol(sd)
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
   int nbit_man,nstage,nadd;
   struct vhdl_description *sd;
   char lcname[256];
   nbit_man=8;
   nstage=1;

   //--- API(1) ---
   create_table_lcell_conv_ftol(nfix,nbit_man,nstage,&nadd,lcname);

   //--- API(2) ---
   generate_pg_table_lcell_of_conv_ftol(&sd);
*/







