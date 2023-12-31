/*                                                                  
   Emulator top structure generated by                              
     pgpgc2 : ver 0.0, Sep 9 2003                                   
     Toshiyuki Fukushige                                            
*/                                                                  
#include <stdio.h>                                                  
#include <math.h>                                                   
                                                                    
void force_gravity_on_pg2(x,m,eps2,a,pot,n)
        double x[][3];
        double m[];
        double eps2;
        double a[][3];
        double pot[];
        int n;
{                                                                   
  int i,j;                                                          
  long long int sx;
  long long int sy;
  long long int sz;
  int xi;
  int xj;
  int yi;
  int yj;
  int zi;
  int zj;
  int xij;
  int yij;
  int zij;
  int dx;
  int dy;
  int dz;
  int ieps2;
  int x2;
  int y2;
  int z2;
  int ieps2r;
  int x2y2;
  int z2e2;
  int r2;
  int r1;
  int mj;
  int mjr;
  int r3;
  int mf;
  int dxr;
  int dyr;
  int dzr;
  int fx;
  int fy;
  int fz;
  long long int ffx;
  long long int ffy;
  long long int ffz;
                                                                    
                                                                    
  for(i=0;i<n;i++){                                               

    xi = ((unsigned int) ((x[i][0] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
    yi = ((unsigned int) ((x[i][1] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
    zi = ((unsigned int) ((x[i][2] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
    if(eps2 == 0.0){                                         
      ieps2 = 0;                                                
    }else if(eps2 > 0.0){                                    
      ieps2 = (((int)(pow(2.0,8.0)*log(eps2*((pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))))/log(2.0))) & 0x7fff) | 0x8000;
    }else{                                                 
      ieps2 = (((int)(pow(2.0,8.0)*log(-eps2*((pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))))/log(2.0))) & 0x7fff) | 0x18000;
    }                                                      
    sx = 0;
    sy = 0;
    sz = 0;
                                                                    
    for(j=0;j<n;j++){                                             

      xj = ((unsigned int) ((x[j][0] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
      yj = ((unsigned int) ((x[j][1] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
      zj = ((unsigned int) ((x[j][2] + ((4.0)/2.0)) * (pow(2.0,20.0)/(4.0)) + 0.5)) & 0xffffffff;
      if(m[j] == 0.0){                                         
        mj = 0;                                                
      }else if(m[j] > 0.0){                                    
        mj = (((int)(pow(2.0,8.0)*log(m[j]*(pow(2.0,60.0)/(1.0/1024.0)))/log(2.0))) & 0x7fff) | 0x8000;
      }else{                                                 
        mj = (((int)(pow(2.0,8.0)*log(-m[j]*(pow(2.0,60.0)/(1.0/1024.0)))/log(2.0))) & 0x7fff) | 0x18000;
      }                                                      
                                                                    
      pg_fix_sub_32(xi,xj,&xij);
      pg_fix_sub_32(yi,yj,&yij);
      pg_fix_sub_32(zi,zj,&zij);
      pg_conv_ftol_fix32_log17_man8(xij,&dx);
      pg_conv_ftol_fix32_log17_man8(yij,&dy);
      pg_conv_ftol_fix32_log17_man8(zij,&dz);
      pg_pdelay_17_np8(dx,&dxr);
      pg_pdelay_17_np8(dy,&dyr);
      pg_pdelay_17_np8(dz,&dzr);
      pg_log_shift_log17_1(dx,&x2);
      pg_log_shift_log17_1(dy,&y2);
      pg_log_shift_log17_1(dz,&z2);
      pg_pdelay_17_np5(ieps2,&ieps2r);
      pg_log_unsigned_add_log17_man8(x2,y2,&x2y2);
      pg_log_unsigned_add_log17_man8(z2,ieps2r,&z2e2);
      pg_log_unsigned_add_log17_man8(x2y2,z2e2,&r2);
      pg_log_shift_log17_m1(r2,&r1);
      pg_log_mul_17(r2,r1,&r3);
      pg_pdelay_17_np12(mj,&mjr);
      pg_log_div_17(mjr,r3,&mf);
      pg_log_mul_17(mf,dxr,&fx);
      pg_log_mul_17(mf,dyr,&fy);
      pg_log_mul_17(mf,dzr,&fz);
      pg_conv_ltof_log17_man8_fix57(fx,&ffx);
      pg_conv_ltof_log17_man8_fix57(fy,&ffy);
      pg_conv_ltof_log17_man8_fix57(fz,&ffz);
      pg_fix_accum_f57_s64(ffx,&sx);
      pg_fix_accum_f57_s64(ffy,&sy);
      pg_fix_accum_f57_s64(ffz,&sz);
                                                                    
    }                                                                 

    a[i][0] = ((double)(sx<<0))*(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))/pow(2.0,0.0);
    a[i][1] = ((double)(sy<<0))*(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))/pow(2.0,0.0);
    a[i][2] = ((double)(sz<<0))*(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))/pow(2.0,0.0);
                                                                    
  }                                                                   
}                                                                   
