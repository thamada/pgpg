/*
 This program is for checking the following modules :
   pg_bits_join
   pg_bits_part
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
  PGINT x,xx; // 12-bit
  PGINT y,yy; // 21-bit
  PGINT z;    // 33-bit
  srand(0x1234);

  fprintf(stderr,"CHECK: pg_bits_join, pg_bits_part ..............");

  for(i=0;i<0x400000;i++){
    x =   (PGINT)(((double)0x1000)*rand()/(RAND_MAX+1.0));
    y =   (PGINT)(((double)0x200000)*rand()/(RAND_MAX+1.0));
    yy = y;
    xx = x;
    {
      PGINT zz;
      pg_bits_join_12_21(x,y,&z);
      pg_bits_part_33_20_0(z,&y);   /* y <= z(20 downto 0)  */
      pg_bits_part_33_32_21(z,&x);  /* x <= z(32 downto 21) */
      pg_bits_join_12_21(x,y,&zz);
      if(z != zz)  is_err=1;
      //      print_lintImage(z,33);
    }
    if((x != xx)||(y != yy)){
      fprintf(stderr,"Cheking is not completed at pg_bits_part.\n");
      exit(1);
      //      print_lintImage(x,12);
      //      print_lintImage(y,21);
    }
  }
  if(is_err >0){
    //    fprintf(stderr,"Cheking is not completed at pg_bits_part.\n");
    fprintf(stderr," FAILED.\n");
  }else{
    fprintf(stderr," OK.\n");
  }
  return 0;
}


// --- debug routines ---
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
