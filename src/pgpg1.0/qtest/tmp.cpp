# 15 "list.g5"
/NVMP 2
/NPIPE 1
/JPSET xj,0,0,31,x[][0],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/JPSET yj,0,32,63,x[][1],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/JPSET zj,1,0,31,x[][2],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/JPSET mj,1,32,48,m[],log,17,8,(pow(2.0,60.0)/(1.0/1024.0))
/IPSET xi,x[][0],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/IPSET yi,x[][1],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/IPSET zi,x[][2],ufix,32,(pow(2.0,20.0)/(4.0)),((4.0)/2.0)
/IPSET ieps2,eps2,log,17,8,((pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0)))
/FOSET sx,a[][0],fix,64,(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))
/FOSET sy,a[][1],fix,64,(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))
/FOSET sz,a[][2],fix,64,(-(pow(2.0,20.0)/(4.0))*(pow(2.0,20.0)/(4.0))/(pow(2.0,60.0)/(1.0/1024.0)))

void force_gravity_on_pg2(x,m,eps2,a,pot,n)
        double x[][3];
        double m[];
        double eps2;
        double a[][3];
        double pot[];
        int n;
{
  pg_rundelay(16);

  pg_fix_addsub(SUB,xi,xj,xij,32,1);
  pg_fix_addsub(SUB,yi,yj,yij,32,1);
  pg_fix_addsub(SUB,zi,zj,zij,32,1);

  pg_conv_ftol(xij,dx,32,17,8,4);
  pg_conv_ftol(yij,dy,32,17,8,4);
  pg_conv_ftol(zij,dz,32,17,8,4);

  pg_pdelay(dx,dxr,17,8);
  pg_pdelay(dy,dyr,17,8);
  pg_pdelay(dz,dzr,17,8);

  pg_log_shift(1,dx,x2,17);
  pg_log_shift(1,dy,y2,17);
  pg_log_shift(1,dz,z2,17);

  pg_pdelay(ieps2,ieps2r,17,5);
  pg_log_unsigned_add(x2,y2,x2y2,17,8,3);
  pg_log_unsigned_add(z2,ieps2r,z2e2,17,8,3);
  pg_log_unsigned_add(x2y2,z2e2,r2,17,8,3);

  pg_log_shift(-1,r2,r1,17);
  pg_log_muldiv(MUL,r2,r1,r3,17,1);

  pg_pdelay(mj,mjr,17,12);
  pg_log_muldiv(DIV,mjr,r3,mf,17,1);

  pg_log_muldiv(MUL,mf,dxr,fx,17,1);
  pg_log_muldiv(MUL,mf,dyr,fy,17,1);
  pg_log_muldiv(MUL,mf,dzr,fz,17,1);

  pg_conv_ltof(fx,ffx,17,8,57,2);
  pg_conv_ltof(fy,ffy,17,8,57,2);
  pg_conv_ltof(fz,ffz,17,8,57,2);

  pg_fix_accum(ffx,sx,57,64,2,16);
  pg_fix_accum(ffy,sy,57,64,2,16);
  pg_fix_accum(ffz,sz,57,64,2,16);

}
