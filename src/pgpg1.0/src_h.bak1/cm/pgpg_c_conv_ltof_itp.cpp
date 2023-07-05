/* --- function list -----------------------------
       void std::generate_c_pg_conv_ltof_itp(...);
       Last modified at 2003/04/23 23:00 bug fixed at shifter
       Last modified at 2003/04/09 11:00 first release
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_ltof_itp(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    int nbit_log = atoi(args[2].c_str());
    int nbit_man = atoi(args[3].c_str());
    int nbit_fix = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str()); nstage += 0;
    int ncut     = atoi(args[6].c_str());
    int bit_ext  = 2; // fixed length

    if((ncut<2)||(ncut>nbit_man)){
      ncut = (int)(nbit_man/2.0 +0.5);
      cerr << "\tWorning: Set NCUT for pg_conv_ltof_itp to "+conv->i2s(ncut)+".\n";
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_ltof_itp";
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);
    func_name += "_fix" + conv->i2s(nbit_fix);
    func_name += "_cut" + conv->i2s(ncut);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int logdata, long long int* fixdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */          \n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */          \n";
    s += "/* nbit_fix   :   " + conv->i2s(nbit_fix) + "-bit      */          \n";
    s += "/* ncut       :   " + conv->i2s(ncut) + "-bit          */          \n";

    s += "#include<stdio.h>                                                  \n";
    s += "#include<math.h>                                                   \n";
    s += "void " + func_name + "(int logdata, long long int* fixdata){       \n";
    s += "  int logdata_sign=0;                                              \n";
    s += "  int logdata_nonzero=0;                                           \n";
    s += "  int logdata_exponent=0;                                          \n";
    s += "  int logdata_mantissa=0;                                          \n";
    s += "  int fixdata_sign=0;                                              \n";
    s += "  unsigned long long int fixdata_absolute=0;                       \n";
    s += "  int table_adr=0;                                                 \n";
    s += "  int table_data=0;                                                \n";
    s += "  int shift_indata=0;                                              \n";
    s += "  int shift_control=0;                                             \n";


    s += "\n";
    s += "  /* VECTOR PREPARATION */                                         \n";
    s += "  logdata_sign=0x1&(logdata>>"    +conv->i2s(nbit_log-1)  +");     \n";
    s += "  logdata_nonzero=0x1&(logdata>>" +conv->i2s(nbit_log-2)  +");     \n";
    s += "  logdata_exponent="+conv->i2mask(nbit_log-nbit_man-2);
    s +=                      "&(logdata>>" +conv->i2s(nbit_man)  +");       \n";
    s += "  logdata_mantissa="+conv->i2mask(nbit_man) +"&logdata;            \n";

    s += "\n";
    s += "  /* SIGN BIT */                                                   \n";
    s += "  fixdata_sign=logdata_sign;                                       \n";

    s += "\n";
    s += "  /* TABLE */                                                      \n";
    int nbit_fixedpoint_mantissa = nbit_man;  // why nbit_man = nbit_fixedpoint_mantissa ?
    s += "  table_adr = logdata_mantissa;                                    \n";
    s += "  {                                                                \n";
    s += "    int adr,dx,rom_c0,rom_c1,data;\n";
    if(nbit_man>31){cerr << "Error : table adr width at conv_ltof_itp is out of range."; exit(1);}
    s += "    adr = "+conv->i2mask(nbit_man-ncut)+"& (table_adr>>"+conv->i2s(ncut)+");";
    s += "\t/* high part of table address */\n";
    s += "    dx = "+conv->i2mask(ncut)+"& table_adr;";
    s += "\t/* low part of table address */\n";
    s += "    { /* --- calc chebyshev coefficients --- */\n";
    s += "      long double xmin,xmax;\n";
    s += "      long double x0,x1,bma,bpa,c0,c1,cc0,cc1,coef0,coef1;\n";
    s += "      xmin = (long double)(adr<<"+conv->i2s(ncut)+");\n";
    s += "      xmax = (long double)(((adr+1)<<"+conv->i2s(ncut)+")-1);\n";
    s += "      xmin = (xmin+0.0)/(long double)(0x1<<"+conv->i2s(nbit_man)+");\n";
    s += "      xmax = (xmax+0.0)/(long double)(0x1<<"+conv->i2s(nbit_man)+");\n";
    s += "      x0 = -0.5*sqrtl(2.0);\n";
    s += "      x1 =  0.5*sqrtl(2.0);\n";
    s += "      bma = 0.5*(xmax-xmin);\n";
    s += "      bpa = 0.5*(xmax+xmin);\n";
    s += "      c0 = powl(2.0,x0*bma+bpa)-1.0;\n";
    s += "      c1 = powl(2.0,x1*bma+bpa)-1.0;;\n";
    s += "      cc0 = 0.5*(c0+c1);\n";
    s += "      cc1 = 0.5*sqrtl(2.0)*(c1-c0);\n";
    s += "      coef0 = cc0 - cc1;    /* 0-dim chebyshev coefficient */\n";
    s += "      coef1 = cc1/bma;      /* 1-dim chebyshev coefficient */\n";
    s += "      rom_c0 = (int)(coef0*powl(2.0,"+conv->i2s(nbit_man+bit_ext)+".0)+0.5);\n";
    s += "      rom_c1 = (int)(coef1*powl(2.0,"+conv->i2s(nbit_man+bit_ext)+".0)+0.5);\n";
    s += "    }\n";
    int trnd;
    if(bit_ext>0) trnd = 0x1<<(bit_ext-1); else trnd = 0x0;
    s += "    data = (rom_c0+((rom_c1*dx)>>"+conv->i2s(nbit_man)+")+"+conv->i2s(trnd)+")>>"+conv->i2s(bit_ext)+";\n";
    s += "    table_data = "+conv->i2mask(nbit_man)+"&data;\n";
    s += "  }\n";

    s += "\n";
    s += "  /* MULTIPLEXOR */                                                \n";
    s += "  {                                                                \n";
    s += "    int muxout=0;                                                  \n";
    s += "    if(logdata_nonzero == 0x1){                                    \n";
    s += "      muxout = (0x1 << "+conv->i2s(nbit_fixedpoint_mantissa)+") | table_data;\n";
    s += "    }else{                                                         \n";
    s += "      muxout = 0x0;                                                \n";
    s += "    }                                                              \n";
    s += "    shift_indata = muxout;                                         \n";
    s += "  }                                                                \n";


    s += "\n";
    s += "  /* SHIFTER */                                                    \n";
    s += "  shift_control = logdata_exponent;                                \n";
    s += "  {                                                                \n";
    s += "    unsigned long long int shift_out=0;                            \n";
    s += "    unsigned long long int seed=0;                                 \n";
    s += "    if(shift_control < 8*sizeof(long long int)){\n";  /* bug-fixed at 2003/04/23 */
    s += "      seed = ((unsigned long long int)shift_indata) << shift_control;\n";
    s += "    }else{\n";
    s += "      seed = 0x0LL;\n";
    s += "    }\n";
    s += "    shift_out = seed >> "+conv->i2s(nbit_fixedpoint_mantissa)+";   \n";
    s += "    fixdata_absolute = "+conv->i2mask(nbit_fix-1)+" & shift_out;   \n";
    s += "  }                                                                \n";

    s += "\n";
    s += "  *fixdata = (long long int)fixdata_sign << "+conv->i2s(nbit_fix-1)+" | fixdata_absolute;\n";

    s += "\n";
    s += "  return;                                                          \n";
    s += "}                                                                  \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */       \n";
    test += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */       \n";
    test += "/* nbit_fix   :   " + conv->i2s(nbit_fix) + "-bit      */       \n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(int log,long long int* fix);                 \n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "  printf(\"nbit_log = "+ conv->i2s(nbit_log) +"\\n\");          \n";
    test += "  printf(\"nbit_man = "+ conv->i2s(nbit_man) +"\\n\");          \n";
    test += "  printf(\"nbit_fix = "+ conv->i2s(nbit_fix) +"\\n\");          \n";
    test += "  for(i=0;i<="+conv->i2mask(nbit_log)+";i++){                   \n";
    test += "    int log=i;                                                  \n";
    test += "    long long int fix=0;                                        \n";
    test += "    double real_fix=0.0;                                        \n";
    test += "    double real_log=0.0;                                        \n";
    test += "    "+func_name+"(log,&fix);                                    \n";
    test += "    real_fix = (double)("+conv->i2mask(nbit_fix-1)+"&fix);      \n";
    test += "    real_log = pow(2.0,(double)("+conv->i2mask(nbit_log-2)+"&log)/pow(2.0,"+conv->i2s(nbit_man)+".0)); \n";

    test += "    if(0x1&(fix>>"+conv->i2s(nbit_fix-1)+")==0x1){   \n";
    test += "      real_fix *= -1.0;                              \n";
    test += "    }                                                \n";

    test += "    if(0x1&(log>>"+conv->i2s(nbit_log-1)+")==0x1){ real_log = real_log * -1.0;}\n";
    test += "    if(0x1&(log>>"+conv->i2s(nbit_log-2)+")==0x0){ real_log = 0.0;}\n";

    test += "    printf(\"log=0x%X,fix=0x%LX\\t\",log,fix);                   \n";
    test += "    printf(\"(Re)fix=%f,(Re)log=%f\\t\",real_fix,real_log);     \n";
    test += "    printf(\"Error(Abs)%f\\t\",(real_log-real_fix));\n";
    test += "    printf(\"Error(Rel)%f\\n\",(real_log-real_fix)/real_log);\n";
    test += "  }                                                             \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";

    gfObj->generate(func_name,s); 
    //    gfObj->generate("test_"+func_name,test);        // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh"); // for debug no problem

  }




}
