/* * * * * * * * * * * * * * * * * * * * * * * * * * 
   This is unsinged logarithmic adder module
   supported the Chebyshev linear interpolation.
   Author : Tsuyoshi Hamada.
   * * * * * * * * * * * * * * * * * * * * * * * * */

/* --- function list -----------------------------
   void std::generate_c_pg_log_unsigned_add_itp(...);

   Last modified at Fri Sep 11 13:00:00 JST 2003
   -----------------------------------------------*/ 
#include<iostream>
#include<cmath>
#include "pgpgc.h"
#define LONG long long int
#define REAL long double
// static private members
namespace{
  using namespace std;

  // PRIORITY ENCODER
  int priority_encoder(int input)
  { 
    int out=0;
    for(int i=31;i >=0;i--){
        int buf = 0x1 & (input >>i);
        if(buf == 0x1){ out = i; break;}
        out = i;
    }
    if(out == 0){
      cerr << "error"<<endl;
      exit(0);
    }
    return out;
  }

  // i.e.  calc_bitlength(4) => 3
  LONG calc_bitlength(LONG input)
  {
    LONG i;
    LONG out=0LL;

    if(input <0){input=0;}

    for(i=61LL;i >=0LL;i--){
      LONG buf;
      buf = 0x1LL & (input >>i);
      if(buf == 0x1LL){ out = (i+0x1LL); break;}
      out = (i+0x1LL);
    }
    if(out == 0LL){fprintf(stderr,"error at calc_bitlength.\n");exit(0);}
    return out;
  }

  // table function for logarithmic adder
  REAL function(REAL x)
  {
    REAL f;
    f = logl(1.0+powl(2.0,-1.0*x))/logl(2.0);
    return f;
  }

  string paste_static_routine()
  {
    string s;
    s += "static long double log_tab_plus_func(long double x)\n";
    s += "{                                           \n";
    s += "  long double f;                            \n";
    s += "  f = logl(1.0+powl(2.0,-1.0*x))/logl(2.0); \n";
    s += "  return f;                                 \n";
    s += "}                                           \n";
    s += "\n";
    s += "static void calc_chebyshev_coefficient(\n";
    s += "  long double xk_min,               \n";
    s += "  long double xk_max,               \n";
    s += "  long double* coef0,               \n";
    s += "  long double* coef1)               \n";
    s += "{                                   \n";
    s += "  long double (*_func)(long double);\n";
    s += "  long double x0,x1,bma,bpa,cc0,cc1;\n";
    s += "  long double f[2];                 \n";
    s += "  x0 = -0.5*sqrtl(2.0);\n";
    s += "  x1 =  0.5*sqrtl(2.0);\n";
    s += "  bma=0.5*(xk_max-xk_min);\n";
    s += "  bpa=0.5*(xk_max+xk_min);\n";
    s += "  _func = &log_tab_plus_func;\n";
    s += "  f[0] = (*_func)(x0*bma+bpa);\n";
    s += "  f[1] = (*_func)(x1*bma+bpa);\n";
    s += "  cc0 = 0.5*(f[0]+f[1]);\n";
    s += "  cc1 = 0.5*sqrtl(2.0)*(f[1]-f[0]);\n";
    s += "  (*coef0) = cc0 - cc1;\n";
    s += "  (*coef1) = (-1.0)*cc1/bma;\n";
    s += "}\n";
    s += "\n";
    return s;
  }

  // calculate coefficient by chebyshev fitting algorithm
  void calc_chebyshev_coefficient(REAL xk_min, REAL xk_max, REAL* coef0, REAL* coef1)
  {
    REAL (*_func)(REAL);
    _func = &function;
    REAL x0 = -0.5*sqrtl(2.0);
    REAL x1 =  0.5*sqrtl(2.0);
    REAL bma=0.5*(xk_max-xk_min);
    REAL bpa=0.5*(xk_max+xk_min);
    REAL f[2];
    f[0] = (*_func)(x0*bma+bpa);
    f[1] = (*_func)(x1*bma+bpa);
    REAL cc0 = 0.5*(f[0]+f[1]);
    REAL cc1 = 0.5*sqrtl(2.0)*(f[1]-f[0]);
    (*coef0) = cc0 - cc1;
    (*coef1) = (-1.0)*cc1/bma;
  }


  LONG rom_c0_ilen;
  LONG rom_c1_ilen;
  LONG rom_c0_olen;
  LONG rom_c1_olen;

  void calc_rom_iowidth(LONG nlog, LONG nman, LONG ncut, LONG bit_ext)
  {
    LONG imax = (LONG)pow(2.0,(double)(nlog-1-ncut));
    for(LONG i=0;i<imax;i++){
      REAL x = ((REAL)i)/(REAL)(1<<(nman-ncut));
      if((LONG)(function(x)*powl(2.0,(REAL)nman)+0.5)==0){imax = i;break;}
    }

    // check all of the address-length 'coef0 and coef1'.
    LONG c0_olen = 0x0LL;
    LONG c1_olen = 0x0LL;
    for(LONG i=0;i<imax;i++){
      REAL xmin = (REAL)(i<<ncut);
      REAL xmax = (REAL)(((i+0x1LL)<<ncut)-0x1LL);
      xmin = (xmin+0.25) / (REAL)(0x1LL<<nman);
      xmax = (xmax+0.25) / (REAL)(0x1LL<<nman);
      REAL coef0,coef1;
      calc_chebyshev_coefficient(xmin,xmax,&coef0,&coef1);
      LONG rom_c0,rom_c1;
      rom_c0 = (LONG)(coef0*powl(2.0,(REAL)(nman+bit_ext))+0.5);
      rom_c1 = (LONG)(coef1*powl(2.0,(REAL)(nman+bit_ext))+0.5);
      LONG tmp_c0_olen = calc_bitlength(rom_c0);
      LONG tmp_c1_olen = calc_bitlength(rom_c1);
      if(tmp_c0_olen > c0_olen) c0_olen = tmp_c0_olen;
      if(tmp_c1_olen > c1_olen) c1_olen = tmp_c1_olen;
    }

    rom_c0_ilen = calc_bitlength(imax-1);
    rom_c1_ilen = calc_bitlength(imax-1);
    rom_c0_olen = c0_olen;
    rom_c1_olen = c1_olen;

    return;
  }




  // Get "LNSimage[(Real Z)/(Real X)]" from "(Logimage X) - (Logimage Y)"
  int get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(
						      int logimage_XY,  // (LNSimage)X - (LNSimage)Y
						      int nbit_man)
  {
    double D = ((double)logimage_XY+0.25)/pow(2.0,(double)nbit_man );
    double z_x_double = (log(1.0+pow(2.0,-1.0*D ))/log(2.0)); // (Real)X - (Real)Y
    z_x_double *= pow(2.0,(double)nbit_man );
    return (  (int)(z_x_double+0.5)  );
  }

  // calculate the width of table input
  int get_ladd_table_input_width(
			   int nbit_log,
			   int nbit_man)
  {
    int table_width=0;
    int i=0;
    int index=0;
    int lastflag=0;

    int imax = (int)pow(2.0,(double)(nbit_log-1));
    while(lastflag==0||i==imax){
      int LNS_z_x = get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(i,nbit_man);
      if(LNS_z_x == 0){
	index = i;
	lastflag=1;
      }
      i++;
    }

    table_width = 1 + priority_encoder(index);

    return table_width;
  }

  // calculate the width of table output
  int get_ladd_table_output_width(int nbit_man)
  {
    int LNS_z_x = get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(0x0,nbit_man);
    return (  1 + priority_encoder(LNS_z_x)  );
  }

}

namespace std{

  void generate_c_pg_log_unsigned_add_itp(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    string sz = args[2];
    int nbit_log = atoi(args[3].c_str());
    int nbit_man = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str()); nstage += 0;
    int nbit_cut = atoi(args[6].c_str());
    LONG bit_ext = 0x2LL    ;// = (LONG)atoi(args[7].c_str()); //  fixed to 2

    if((nbit_cut<2)||(nbit_cut>nbit_man)){
      nbit_cut = (int)(nbit_man/2.0 +0.5);
      cerr << "\tWorning: Set NCUT for pg_log_unsigned_itp to "+conv->i2s(nbit_cut)+".\n";
    }


    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_log_unsigned_add_itp";
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);
    func_name += "_cut" + conv->i2s(nbit_cut);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int logx, int logy, int* logz)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* logarithmic UNSIGNED adder using interpolated table */          \n";
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */          \n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */          \n";

    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is ";
    s += conv->i2s(nbit_log-1) + "-bit.      */\n\n";

    s += "#include<stdio.h>                                                  \n";
    s += "#include<math.h>                                                   \n";
    static int is_pasted=0;
    if(is_pasted==0){
      s += paste_static_routine();
      is_pasted=1;
    }
    s += "void " + func_name + "(int logimage_x, int logimage_y, int* logimage_z){\n";
    s += "  int signz=0;  /* 1-bit */                                        \n";
    s += "  int x1=0;     /* "+conv->i2s(nbit_log)+"-bit */                  \n";
    s += "  int y1=0;     /* "+conv->i2s(nbit_log)+"-bit */                  \n";
    s += "  int xy=0;     /* "+conv->i2s(nbit_log)+"-bit (x1 - y1) */        \n";
    s += "  int yx=0;     /* "+conv->i2s(nbit_log)+"-bit (y1 - x1) */        \n";
    s += "  int yx_msb=0; /*  1-bit SIGN of yx */                            \n";
    s += "  int x2=0; /* "+conv->i2s(nbit_log-1)+"-bit output of Left-MUX  */\n";
    s += "  int d0=0; /* "+conv->i2s(nbit_log-1)+"-bit output of Right-MUX */\n";
    s += "  int d0_low_part=0;    /* TABLE INPUT */                          \n";
    s += "  int d0_high_part=0;   /* MUX Middle INPUT */                     \n";
    s += "  int df=0;             /* MUX Middle OUTPUT */                    \n";
    s += "  int table_data=0;     /* TABLE OUTPUT */                         \n";
    s += "  int logimage_ZperX=0; /* Logimage of (Real)Z/X */                \n";
    s += "\n";

    s += "  /* SIGN EVALUATION                      \n";
    s += "          ----------------------------    \n";
    s += "          nonz(X) | nonz(Y) -> sign(Z)    \n";
    s += "          --------+-------------------    \n";
    s += "             0    |   0     ->   0        \n";
    s += "             0    |   1     -> sign(Y)    \n";
    s += "             1    |   0     -> sign(X)    \n";
    s += "             1    |   1     -> sign(X)    \n";
    s += "          ----------------------------  */\n";
    s += "  {\n";
    s += "    int nonzx,nonzy,signx,signy;\n";
    s += "    signx = (logimage_x>>"+conv->i2s(nbit_log-1)+")&0x1;\n";
    s += "    signy = (logimage_y>>"+conv->i2s(nbit_log-1)+")&0x1;\n";
    s += "    nonzx = (logimage_x>>"+conv->i2s(nbit_log-2)+")&0x1;\n";
    s += "    nonzy = (logimage_y>>"+conv->i2s(nbit_log-2)+")&0x1;\n";
    s += "    if(      (nonzx==0x0)&&(nonzy==0x0)){ signz = 0x0;   }\n";
    s += "    else if((nonzx==0x0)&&(nonzy==0x1)){  signz = signy; }\n";
    s += "    else if((nonzx==0x1)&&(nonzy==0x0)){  signz = signx; }\n";
    s += "    else{ /*(nonzx==0x1)&&(nonzy==0x1) */ signz = signx; }\n";
    s += "  }\n";
    s += "\n";

    s += "  /* RESET FUNCTION */ \n";
    s += "  {\n";
    s += "    int nonzx;\n";
    s += "    int nonzy;\n";
    s += "    nonzx = 0x1&(logimage_x>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    nonzy = 0x1&(logimage_y>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    if(nonzx == 0x0) logimage_x &= 0x0;\n";
    s += "    if(nonzy == 0x0) logimage_y &= 0x0;\n";
    s += "  }\n";

    s += "\n";
    s += "  x1 = "+conv->i2mask(nbit_log-1)+"&logimage_x; /* The MSB("+conv->i2s(nbit_log)+"th-bit) is zero. */\n";
    s += "  y1 = "+conv->i2mask(nbit_log-1)+"&logimage_y; /* The MSB("+conv->i2s(nbit_log)+"th-bit) is zero. */\n";

    s += "\n";
    s += "  /* SUB (Y-X),(X-Y) */                                            \n";
    s += "  yx = "+conv->i2mask(nbit_log)+"&(y1 - x1);                       \n";
    s += "  xy = "+conv->i2mask(nbit_log)+"&(x1 - y1);                       \n";

    s += "\n";
    s += "  yx_msb = 0x1 & (yx >>"+conv->i2s(nbit_log-1)+");                 \n";

    s += "\n";
    s += "  /* MUX Left */\n";
    s += "  if(yx_msb==0x1){x2="+conv->i2mask(nbit_log-1)+"&logimage_x;}else{x2="+conv->i2mask(nbit_log-1)+"&logimage_y;}\n";

    s += "\n";
    s += "  /* MUX Right */\n";
    s += "  if(yx_msb==0x1){d0="+conv->i2mask(nbit_log-1)+"&xy;}else{d0="+conv->i2mask(nbit_log-1)+"&yx;}\n";

    s += "\n";

    calc_rom_iowidth((LONG)nbit_log,(LONG)nbit_man,(LONG)nbit_cut,bit_ext); // calculate table i/o with (2003/03/14)

    LONG nbit_rom_c0_ilen = rom_c0_ilen;
    LONG nbit_rom_c1_ilen = rom_c1_ilen;
    LONG nbit_rom_c0_olen = rom_c0_olen;
    LONG nbit_rom_c1_olen = rom_c1_olen;
    s += "  /* ===========================\n";
    s += "     ROM C0 input = "+conv->i2s(nbit_rom_c0_ilen)+" bits                  \n";
    s += "     ROM C1 input = "+conv->i2s(nbit_rom_c1_ilen)+" bits                  \n";
    s += "     ROM C0 output = "+conv->i2s(nbit_rom_c0_olen)+" bits                 \n";
    s += "     ROM C1 output = "+conv->i2s(nbit_rom_c1_olen)+" bits                 \n";
    s += "     BIT EXTENSION = "+conv->i2s(bit_ext)+" bits                          \n";
    s += "     =========================== */\n";

    int nbit_table_in  = nbit_cut + (int)rom_c0_ilen; // (= rom_c1_ilen)
    int nbit_table_out = nbit_man + 1;

    s += "  /* ===========================\n";
    s += "     table input  = "+conv->i2s(nbit_table_in)+" bits                   \n";
    s += "     table output = "+conv->i2s(nbit_table_out)+" bits                  \n";
    s += "     =========================== */\n";


    s += "  d0_low_part  = "+conv->i2mask(nbit_table_in)+"&d0;      /* "+conv->i2s(nbit_table_in)+"-bit */\n";
    if(nbit_log-1-nbit_table_in <= 0){
      cerr << "\n\nPGPG error at generation of "<<func_name<< ".c:\n\t d0_high_part becomes under 0-bit.\n\n";
      exit(1);
    }
    s += "  d0_high_part = "+conv->i2mask(nbit_log-1-nbit_table_in)+"&(d0>>"+conv->i2s(nbit_table_in)+"); \n";

    s += "\n";
    s += "  /* MUX Middle */\n";
    s += "  if(d0_high_part == 0x0){ df=0x1; }else{ df=0x0; }                \n";

    s += "\n";
    s += "  /* TABLE LOG_ADD */\n";
    s += "  {                                                                                     \n";
    s += "    long long int adr,dx,rom_c0,rom_c1;                                  \n";
    s += "    long double xk_min,xk_max,coef0,coef1;                               \n";
    s += "    long long int f;                                                     \n";
    s += "    adr=(long long int)(d0_low_part>>"+conv->i2s(nbit_cut)+");           \n";
    s += "    dx =(long long int)("+conv->i2mask(nbit_cut)+"&d0_low_part);         \n";
    s += "    xk_min = (long double)(adr<<"+conv->i2s(nbit_cut)+");                \n";
    s += "    xk_max = (long double)(((adr+0x1LL)<<"+conv->i2s(nbit_cut)+")-0x1LL);\n";
    s += "    xk_min = (xk_min+0.25)/(long double)(0x1LL<<"+conv->i2s(nbit_man)+");\n";
    s += "    xk_max = (xk_max+0.25)/(long double)(0x1LL<<"+conv->i2s(nbit_man)+");\n";
    s += "    calc_chebyshev_coefficient(xk_min,xk_max,&coef0,&coef1);             \n";
    s += "    rom_c0 = (long long int)(coef0*powl(2.0,"+conv->i2s(nbit_man+(int)bit_ext)+".0)+0.5);\n";
    s += "    rom_c1 = (long long int)(coef1*powl(2.0,"+conv->i2s(nbit_man+(int)bit_ext)+".0)+0.5);\n";
    s += "    f = (rom_c0 - ((rom_c1*dx)>>"+conv->i2s(nbit_man)+"))>>"+conv->i2s(bit_ext)+";       \n";
    s += "    if(f<0) f = 0x0LL;                                             \n";
    s += "    f &= "+conv->i2mask(nbit_man)+"LL;                             \n";
    s += "    if(d0_low_part == 0) f = 0x1LL <<"+conv->i2s(nbit_man)+";      \n";
    s += "    table_data = (int)(f);  ";
    s += "/* TABLE OUTPUT : "+conv->i2s(nbit_man+1)+"-bit */\n";
    s += "  }                                               \n";

    s += "\n";
    s += "  /* MUX Last */\n";
    s += "  if(df == 0x1){ logimage_ZperX = table_data; }else{ logimage_ZperX = 0;}\n";

    s += "\n";
    s += "  /* ADDER :"+conv->i2s(nbit_log-1)+"-bit width */ \n";
    s += "  {                                                \n";
    s += "    int add_out=0;                                 \n";
    s += "    add_out = x2 + logimage_ZperX;                 \n";
    s += "    add_out &= "+conv->i2mask(nbit_log-1)+";       \n";
    s += "                   /* WITH SIGN-BIT (2002/07/26) */\n";
    s += "    *logimage_z = (signz<<"+conv->i2s(nbit_log-1)+") | add_out;\n";
    s += "  }                                                \n";


    s += "\n";
    s += "  return;  \n";
    s += "}          \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */       \n";
    test += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */       \n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(int logimage_x,int logimage_y,int* logimage_z);\n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "\n\n";
    test += "#ifndef GNUPLOT\n";
    test += "  printf(\"nbit_log = "+ conv->i2s(nbit_log) +"\\n\");          \n";
    test += "  printf(\"nbit_man = "+ conv->i2s(nbit_man) +"\\n\");          \n";
    test += "#endif\n\n";
    test += "  for(i=0;i<="+conv->i2mask(/* must be nbit_fix */ 19)+";i+=32){\n";
    test += "    int logimage_x=0;                                           \n";
    test += "    int logimage_y=0;                                           \n";
    test += "    int logimage_z=0;                                           \n";
    test += "    double real_x =(double)i;                                   \n";
    test += "    double real_y =(double)i;                                   \n";
    test += "    double real_z = 0.0;                                        \n";
    test += "    double real_z_byladd = 0.0;                                 \n";
    test += "\n\n";
    test += "    real_z = real_x + real_y;                                   \n";
    test += "    logimage_x = (int)("+conv->i2ds(nbit_man)+"*log(real_x)/log(2.0)+0.5); \n";
    test += "    logimage_x &= "+conv->i2mask(nbit_log-2)+";                 \n";
    test += "    if(i>0){  /* Non-Zero bit addition */                       \n";
    test += "      logimage_x = 0x1<<"+conv->i2s(nbit_log-2)+"|logimage_x;   \n";
    test += "    }                                                           \n";
    test += "    logimage_y = logimage_x;                                    \n";

    test += "    "+func_name+"(logimage_x,logimage_y,&logimage_z);           \n";
    test += "    {                                                                   \n";
    test += "      int logimage_z_nonzero=0;                                         \n";
    test += "      int logimage_z_body="+conv->i2mask(nbit_log-2)+"&logimage_z;      \n";
    test += "      logimage_z_nonzero=0x1&(logimage_z>>"+conv->i2s(nbit_log-2)+");   \n";
    test += "      real_z_byladd= pow(2.0, logimage_z_body/"+conv->i2ds(nbit_man)+");\n";
    test += "      if(logimage_z_nonzero ==0){ real_z_byladd = 0.0;}                 \n";
    test += "    }                                                                   \n";

    test += "#ifdef GNUPLOT\n";
    test += "    if(real_z>0.0){\n";
    test += "      printf(\"%f\\t\",real_x); \n";
    test += "      printf(\"%f\\n\",sqrt(pow((real_z-real_z_byladd)/real_z,2.0)) );\n";
    test += "    }\n";
    test += "#else\n";
    test += "    printf(\"(LOG)x=0x%X,(LOG)y=0x%X,(LOG)z=0x%X\\t\",logimage_x,logimage_y,logimage_z); \n";
    test += "    printf(\"(Re)z=%f,(Re')z=%f\\t\",real_z,real_z_byladd);\n";
    test += "    printf(\"Error(Abs)%f\\t\",(real_z-real_z_byladd));\n";
    test += "    printf(\"Error(Rel)%f\\n\",(real_z-real_z_byladd)/real_z);\n";
    test += "#endif\n";

    test += "  }                                                             \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c -DGNUPLOT test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";
    shell += "./test_"+func_name+">./test_"+func_name+".log\n";
    shell += "gnuplot ./test_"+func_name+".plot > ./test_"+func_name+".ps\n"; // --- generate psfile by "gnuplot"
    shell += "gv -scale -3 -landscape ./test_"+func_name+".ps\n";             // --- open psfile by "gv"

    // gnuplot script
    string gnuplot;
    gnuplot +="set terminal postscript\n";
    gnuplot +="set title '" +func_name+ "'\n";
    gnuplot +="set key title \"Relative error: ({(exact(z) - practical(z))/exact(z)}^2)^0.5,\\n z = x + x\"\n";
    gnuplot +="set logscale x\n";
    gnuplot +="set grid\n";
    gnuplot +="set xlabel 'real x'\n";
    gnuplot +="set ylabel 'Re'\n";
    gnuplot +="plot \"./test_"+func_name+".log\"  u 1:2 t \"\"  with lines\n";
    gnuplot +="save \"./test_"+func_name+".gp\" \n";  // save gpfile by "gnuplot"

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);            // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");     // for debug no problem
    //    gfObj->generate("test_"+func_name,gnuplot,".plot"); // for debug no problem

  }




}
