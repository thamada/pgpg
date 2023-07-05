/* --- function list -----------------------------
       void std::generate_c_pg_conv_ltof(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_ltof(vector<string> args, GenFile* gfObj)
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
    int nstage = atoi(args[5].c_str()); nstage += 0;

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_ltof";
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);
    func_name += "_fix" + conv->i2s(nbit_fix);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int logdata, long long int* fixdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */          \n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */          \n";
    s += "/* nbit_fix   :   " + conv->i2s(nbit_fix) + "-bit      */          \n";

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
    s += "    double x     = 0.0;                                            \n";
    s += "    double y     = 0.0;                                            \n";
    s += "    double depth = " +conv->i2ds(nbit_man)+ ";                     \n";
    s += "    x = pow(2.0,((double)table_adr)/depth);                        \n";
    s += "    y = (" +conv->i2ds(nbit_fixedpoint_mantissa)+ "*(x-1.0)) +0.5; \n";
    s += "    table_data = (int)y;                                           \n";
    s += "    table_data &= " +conv->i2mask(nbit_man)+ ";                    \n";
    s += "  }                                                                \n";

    s += "\n";
    s += "  /* MULTIPLEXOR */                                                \n";
    s += "  {                                                                \n";
    s += "    int muxout=0;                                                  \n";
    s += "    if(logdata_nonzero == 0x1){                                    \n";
    s += "      muxout = 0x1 << "+conv->i2s(nbit_fixedpoint_mantissa)+" | table_data;\n";
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
    s += "    if(shift_control < 8*sizeof(long long int)){\n";  /* bug-fixed at 2003/06/19 */
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
