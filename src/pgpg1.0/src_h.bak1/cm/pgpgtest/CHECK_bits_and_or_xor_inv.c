/*
 This program is for checking the following modules :
   pg_bits_and
   pg_bits_or
   pg_bits_xor
   pg_bits_inv
 .
*/
#include<stdio.h>
#include<stdlib.h>
#include "pg_module.c"

void print_lintImage(PGINT x, int nbit);

int main()
{
  int is_err=0;
  int i;
  PGINT x; // 12-bit
  PGINT y; // 21-bit
  srand(0x1234);
  //.............................................................."
  fprintf(stderr,"CHECK: pg_bits_and,or,inv,xor ..................");

  for(i=0;i<0x800000;i++){
    x = (PGINT)(((double)0x1000000)*rand()/(RAND_MAX+1.0));
    y = (PGINT)(((double)0x1000000)*rand()/(RAND_MAX+1.0));
    {
      PGINT xny,inv_x,inv_y,z0,z1;
      PGINT z1inv,z1xor;
      pg_bits_and_24(x,y,&xny);
      pg_bits_inv_24(xny,&z0);
      pg_bits_inv_24(x,&inv_x);
      pg_bits_inv_24(y,&inv_y);
      pg_bits_or_24(inv_x,inv_y,&z1);
      if(z0 != z1)  is_err=1;

      pg_bits_inv_24(z1,&z1inv);
      pg_bits_xor_24(z1,z1inv,&z1xor);
      //      print_lintImage(z1xor,24);
      if(z1xor != 0xffffffLL) is_err=1;
    }
  }
  if(is_err >0){
    fprintf(stderr," FAILED.\n");
  }else{
    fprintf(stderr," OK.\n");
  }
  return 0;
}




// -- debug routine
void print_lintImage(PGINT x, int nbit)
{
  int i;
  for(i=(nbit-1);i>=0;i--){
    PGINT bit = 0x1LL&(x>>i);
    //    if(i!=31) if(i%4==3) printf(" ");
    if(bit==0x1LL)  printf("H");
    else printf("_");
  }
  printf("\n");
}
