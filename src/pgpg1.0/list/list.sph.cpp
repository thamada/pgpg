# 20 "../list/list.sph"
/NPIPE 1
/NVMP 3
/JPSET xj,0,0,23,x[][0],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/JPSET yj,0,24,47,x[][1],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/JPSET zj,1,0,23,x[][2],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/JPSET vxj,1,24,37,v[][0],fix,14,(pow(2.0,14)/4.0)
/JPSET vyj,1,38,51,v[][1],fix,14,(pow(2.0,14)/4.0)
/JPSET vzj,2,0,13,v[][2],fix,14,(pow(2.0,14)/4.0)
/JPSET hj,2,14,27,h[],log,14,5,(pow(2.0,24)/16.0)
/JPSET pj,2,28,41,p[],log,14,5,(1024.0)
/JPSET qj,2,42,55,q[],log,14,5,((1024.0)*(1024.0))
/IPSET xi,x[][0],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/IPSET yi,x[][1],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/IPSET zi,x[][2],ufix,24,(pow(2.0,24)/16.0),(16.0/2.0)
/IPSET vxi,v[][0],fix,14,(pow(2.0,14)/4.0)
/IPSET vyi,v[][1],fix,14,(pow(2.0,14)/4.0)
/IPSET vzi,v[][2],fix,14,(pow(2.0,14)/4.0)
/IPSET pi,p[],log,14,5,((1024.0)*2.0)
/IPSET qi,q[],log,14,5,((1024.0)*(1024.0))
/IPSET hi,h[],log,14,5,(pow(2.0,24)/16.0)
/FOSET srho,rho[],fix,64,(M_1_PI/pow(2.0,76.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*pow(2.0,8.0))
/FOSET sdivv,divv[],fix,64,(M_1_PI/(pow(2.0,120.0)/3.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)/(pow(2.0,14)/4.0)*pow(2.0,48.0))
/FOSET sax,a[][0],fix,64,(M_1_PI/(pow(2.0,120.0)/3.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)/((1024.0)*(1024.0))*pow(2.0,56.0))
/FOSET say,a[][1],fix,64,(M_1_PI/(pow(2.0,120.0)/3.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)/((1024.0)*(1024.0))*pow(2.0,56.0))
/FOSET saz,a[][2],fix,64,(M_1_PI/(pow(2.0,120.0)/3.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)/((1024.0)*(1024.0))*pow(2.0,56.0))
/FOSET sdudt,dudt[],fix,64,(M_1_PI/(pow(2.0,120.0)/3.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)*(pow(2.0,24)/16.0)/(pow(2.0,14)/4.0)/((1024.0)*(1024.0))*pow(2.0,64.0))
/VALSET rhoshift,0x1100,0,13
/VALSET dshift,0x1600,0,13
/VALSET fshift,0x1700,0,13
/VALSET ushift,0x1800,0,13

/ALTERAESB on

void sph_pipeline_on_emulator(x,v,a,n,h,p,q,rho,divv,dudt)
        double x[][3];
        double v[][3];
        double a[][3];
        int n;
        double h[];
        double p[];
        double q[];
        double rho[];
        double divv[];
        double dudt[];
{
  pg_rundelay(22);

-- distance

  pg_fix_addsub(SUB,xi,xj,xij,24,1);
  pg_fix_addsub(SUB,yi,yj,yij,24,1);
  pg_fix_addsub(SUB,zi,zj,zij,24,1);

  pg_conv_ftol(xij,dx,24,14,5,4);
  pg_conv_ftol(yij,dy,24,14,5,4);
  pg_conv_ftol(zij,dz,24,14,5,4);

  pg_pdelay(dx,dxr,14,14);
  pg_pdelay(dy,dyr,14,14);
  pg_pdelay(dz,dzr,14,14);

  pg_log_shift(1,dx,x2,14);
  pg_log_shift(1,dy,y2,14);
  pg_log_shift(1,dz,z2,14);

  pg_log_unsigned_add(x2,y2,x2y2,14,5,3);
  pg_pdelay(z2,z2r,14,3);
  pg_log_unsigned_add(x2y2,z2r,r2,14,5,3);

-- inner product

  pg_fix_addsub(SUB,vxi,vxj,vxij,14,1);
  pg_fix_addsub(SUB,vyi,vyj,vyij,14,1);
  pg_fix_addsub(SUB,vzi,vzj,vzij,14,1);

  pg_conv_ftol(vxij,dvx,14,14,5,4);
  pg_conv_ftol(vyij,dvy,14,14,5,4);
  pg_conv_ftol(vzij,dvz,14,14,5,4);

  pg_log_muldiv(MUL,dvx,dx,vxx,14,1);
  pg_log_muldiv(MUL,dvy,dy,vyy,14,1);
  pg_log_muldiv(MUL,dvz,dz,vzz,14,1);

  pg_log_add(vxx,vyy,vxy,14,5,3);
  pg_pdelay(vzz,vzzr,14,3);
  pg_log_add(vxy,vzzr,vdotr,14,5,3);

-- kernel

  pg_pdelay(hi,hir,14,11);
  pg_log_shift(1,hir,hii2,14);
  pg_log_muldiv(MUL,hir,hii2,hii3,14,1);
  pg_pdelay(hii2,hii2r,14,1);
  pg_log_muldiv(MUL,hii3,hii2r,hii5,14,1);
  pg_pdelay(hii3,hii3r,14,1);

  pg_log_muldiv(DIV,r2,hii2,rhi2,14,1);
  pg_wtable(rhi2,wi,14,5,1);
  pg_log_muldiv(DIV,wi,hii3r,whi,14,1);
  pg_dwtable(rhi2,dwi,14,5,1);
  pg_log_muldiv(DIV,dwi,hii5,dwhi,14,1);

  pg_pdelay(hj,hjr,14,11);
  pg_log_shift(1,hjr,hj2,14);
  pg_log_muldiv(MUL,hjr,hj2,hj3,14,1);
  pg_pdelay(hj2,hj2r,14,1);
  pg_log_muldiv(MUL,hj3,hj2r,hj5,14,1);
  pg_pdelay(hj3,hj3r,14,1);

  pg_log_muldiv(DIV,r2,hj2,rhj2,14,1);
  pg_wtable(rhj2,wj,14,5,1);
  pg_log_muldiv(DIV,wj,hj3r,whj,14,1);
  pg_dwtable(rhj2,dwj,14,5,1);
  pg_log_muldiv(DIV,dwj,hj5,dwhj,14,1);

  pg_log_unsigned_add(whi,whj,w,14,5,3);
  pg_log_unsigned_add(dwhi,dwhj,dw,14,5,3);

-- density

  pg_log_muldiv(SDIV,w,rhoshift,wrhos,14,1);
  pg_conv_ltof(wrhos,frho,14,5,57,2);
  pg_fix_accum(frho,srho,57,64,2,20);

-- divv
  pg_pdelay(vdotr,vdotrr,14,5);
  pg_log_muldiv(MUL,vdotrr,dw,vrdw0,14,1);
  pg_log_muldiv(SDIV,vrdw0,dshift,vrdw,14,1);
  pg_conv_ltof(vrdw,fdivv,14,5,57,2);
  pg_fix_accum(fdivv,sdivv,57,64,2,21);

-- artificial viscosity

  pg_pdelay(pj,pjr,14,13);
  pg_pdelay(pi,pir,14,13);
  pg_log_muldiv(MUL,pir,pjr,pij,14,1);

  pg_pdelay(qj,qjr,14,11);
  pg_pdelay(qi,qir,14,11);
  pg_log_unsigned_add(qir,qjr,qij,14,5,3);
  pg_log_unsigned_add(pij,qij,avij,14,5,3);

-- acceleration

  pg_log_muldiv(MUL,dw,avij,avdw0,14,1);

  pg_log_muldiv(SDIV,avdw0,fshift,avdw,14,1);

  pg_log_muldiv(MUL,dxr,avdw,avdwx,14,1);
  pg_log_muldiv(MUL,dyr,avdw,avdwy,14,1);
  pg_log_muldiv(MUL,dzr,avdw,avdwz,14,1);

  pg_conv_ltof(avdwx,ax,14,5,57,2);
  pg_conv_ltof(avdwy,ay,14,5,57,2);
  pg_conv_ltof(avdwz,az,14,5,57,2);

  pg_fix_accum(ax,sax,57,64,2,22);
  pg_fix_accum(ay,say,57,64,2,22);
  pg_fix_accum(az,saz,57,64,2,22);

-- du/dt

  pg_log_muldiv(SDIV,avdw0,ushift,avdw1,14,1);

  pg_pdelay(vdotrr,vdotrrr,14,2);
  pg_log_muldiv(MUL,vdotrrr,avdw1,dudt0,14,1);

  pg_conv_ltof(dudt0,fdudt,14,5,57,2);
  pg_fix_accum(fdudt,sdudt,57,64,2,22);

}
