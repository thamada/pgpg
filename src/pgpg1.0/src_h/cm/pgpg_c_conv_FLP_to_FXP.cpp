/* --- function list -----------------------------
       void std::generate_c_pg_conv_FLP_to_FXP(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_FLP_to_FXP(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    int nbit_fix = atoi(args[2].c_str());

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_FLP_to_FXP";
    func_name += conv->i2s(nbit_fix);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(double flp, int* fxp)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* flp  :  double  */          \n";
    s += "/* fxp  :  " + conv->i2s(nbit_fix) + "-bit unsigned integer  */    \n";

    s += "#include<stdio.h>                            \n";
    s += "#include<math.h>                             \n";
    s += "void " + func_name + "(double flp, int* fxp){\n";
    s += "  double dx=0.0;                             \n";
    s += "  int ix=0;                                  \n";

    s += "\n";
    s += "  dx = "+conv->i2ds(nbit_fix)+" * flp; /* [dx = pow(2.0,"+conv->i2s(nbit_fix)+".0)*flp] */\n";
    s += "  ix = (int)dx;                           \n";
    s += "  ix &= "+conv->i2mask(nbit_fix)+";       \n";
    s += "  *fxp = ix;                              \n";

    s += "\n";
    s += "  return;                                   \n";
    s += "}                                           \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_fxp   :   " + conv->i2s(nbit_fix) + "-bit      */       \n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(double flp,int* fxp);                        \n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "  printf(\"nbit_fxp = "+ conv->i2s(nbit_fix) +"\\n\");          \n";
    test += "  for(i=0;i<="+conv->i2mask(nbit_fix)+";i++){                   \n";
    test += "    int fxp=0;                                                  \n";
    test += "    double flp = (double)i;                                     \n";
    test += "    double true_val =(flp/"+conv->i2ds(nbit_fix)+");            \n";
    test += "    "+func_name+"(true_val,&fxp);                               \n";
    test += "    printf(\"flp=%lf,fxp=0x%LX\\n\",true_val,fxp);              \n";
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
