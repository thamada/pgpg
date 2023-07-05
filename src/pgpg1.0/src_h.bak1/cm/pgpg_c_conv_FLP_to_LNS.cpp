/* --- function list -----------------------------
       void std::generate_c_pg_conv_FLP_to_LNS(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_FLP_to_LNS(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    int nbit_log = atoi(args[2].c_str());  // LNS bit size
    int nbit_man = atoi(args[3].c_str());  // Mantissa of LNS bit size
    if(nbit_log > 32){
      cerr << "conv_FLP_to_LNS: the size of log is out of range.: "<< nbit_log <<endl;
      exit(1);
    }
    if(nbit_log <= nbit_man){
      cerr << "conv_FLP_to_LNS: the size of log is less than that of mantissa.\n";
      exit(1);
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_FLP_to_LNS";
    func_name += "_log"+conv->i2s(nbit_log);
    func_name += "_man"+conv->i2s(nbit_man);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(double flp, int* lns)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* FLP  :  double  */                                   \n";
    s += "/* LNS of log :  " + conv->i2s(nbit_log) + "-bit */     \n";
    s += "/* LNS of mantissa :  " + conv->i2s(nbit_man) + "-bit */\n";

    s += "#include<stdio.h>                            \n";
    s += "#include<math.h>                             \n";
    s += "void " + func_name + "(double flp, int* lns){\n";
    s += "  double abs=0.0;                            \n";
    s += "  int logpart=0;                             \n";
    s += "  int sign=0;                                \n";
    s += "  int nonzero=0;                             \n";
    s += "  int lnsimage=0;                            \n";

    s += "\n";
    s += "  if(flp == 0.0){                            \n";
    s += "    *lns=0;                                  \n";
    s += "    return;                                  \n";
    s += "  }else{                                     \n";
    s += "    nonzero=0x1;                             \n";
    s += "    abs = flp;                               \n";
    s += "    if(flp <0.0){                            \n";
    s += "      sign = 0x1;                            \n";
    s += "      abs = -1.0 * flp;                      \n";
    s += "    }                                        \n";
    s += "    logpart = (int)(0.5 + "+conv->i2ds(nbit_man)+"*(log(abs)/log(2.0)));\n";
    s += "    if((unsigned int)logpart <= "+conv->i2mask(nbit_log)+"){  \n";
    s += "      lnsimage |= sign << "+conv->i2s(nbit_log-1)+";         \n";
    s += "      lnsimage |= nonzero << "+conv->i2s(nbit_log-2)+";      \n";
    s += "      lnsimage |= "+conv->i2mask(nbit_log-2)+"&logpart;      \n";
    s += "      (*lns) = "+conv->i2mask(nbit_log)+"&lnsimage;          \n";
    s += "      return;                                                \n";
    s += "    }else{                                                   \n";
    s += "      printf(\"conv_FLP_to_LNS: Overflow.\\n\");             \n";
    s += "      exit(1);                                               \n";
    s += "    }                                                        \n";
    s += "  }                                                          \n";
    s += "  printf(\"conv_FLP_to_LNS: Something bad.\\n\");            \n";
    s += "  exit(1);                                                   \n";
    s += "}                                                             \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* FLP  :  double  */                                   \n";
    test += "/* LNS of log :  " + conv->i2s(nbit_log) + "-bit */     \n";
    test += "/* LNS of mantissa :  " + conv->i2s(nbit_man) + "-bit */\n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(double flp,int* lns);                        \n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "  printf(\"nbit_log = "+ conv->i2s(nbit_log) +"\\n\");          \n";
    test += "  printf(\"nbit_man = "+ conv->i2s(nbit_man) +"\\n\");          \n";
    test += "  for(i=0;i<="+conv->i2mask(nbit_log-2)+";i++){                 \n";
    test += "    int lns=0;                                                  \n";
    test += "    double flp = (double)i;                                     \n";
    test += "    double reconv_flp = 0.0;                                    \n";
    test += "    "+func_name+"(flp,&lns);                                    \n";

    test += "    if( 0x1&lns>>"+conv->i2s(nbit_log-2)+" == 0x1){                            \n";
    test += "      reconv_flp=0.5+";
    test += "pow(2.0,((double)("+conv->i2mask(nbit_log-2)+"&lns))/"+conv->i2ds(nbit_man)+");\n";
    test += "      if( 0x1&lns>>"+conv->i2s(nbit_log-1)+" == 0x1) reconv_flp *= -1.0;       \n";
    test += "    }else{                                                      \n";
    test += "      reconv_flp =0.0;                                          \n";
    test += "    }                                                           \n";

    test += "    printf(\"flp=%le, lns=0x%X, (reconv)flp=%le, ERR(relative)=%lf\\n\",flp,lns,reconv_flp,(reconv_flp-flp)/flp);\n";
    test += "  }                                                             \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -g -c test_"+func_name+".c\n";
    shell += "gcc -g -c "+func_name+".c\n";
    shell += "gcc -g test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";

    gfObj->generate(func_name,s);
    gfObj->generate("test_"+func_name,test);
    gfObj->generate("make_"+func_name,shell,".sh");

  }




}
