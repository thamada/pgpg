/* --- function list -----------------------------
       void std::generate_c_pg_fix_accum(...);
   -----------------------------------------------*/ 
#include<iostream>
#include<cmath>
#include "pgpgc.h"


namespace std{

  void generate_c_pg_fix_accum(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string fx = args[0];
    string sx = args[1];
    int nbit_fx  = atoi(args[2].c_str());
    int nbit_sx  = atoi(args[3].c_str());
    int nstage   = atoi(args[4].c_str()); nstage += 0;

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_fix_accum";
    func_name += "_f" + conv->i2s(nbit_fx);
    func_name += "_s" + conv->i2s(nbit_sx);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(long long int fdata, long long int* sdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* Fixed-Point Accumulate Registor*/                          \n";
    s += "/* nbit_fx(input)   :   " + conv->i2s(nbit_fx) + "-bit      */\n";
    s += "/* nbit_sx(register):   " + conv->i2s(nbit_sx) + "-bit      */\n";

    s += "/* --- [NOTE!] ---\n";
    s += "   The format of input data is not the 2's complement one.\n";
    s += "   MSB means a sign flag, and the other part means abusolute(not<0) integer. */\n\n";

    s += "#include<stdio.h>                                                  \n";
    s += "#include<math.h>                                                   \n";
    s += "void " + func_name + "(long long int fdata, long long int* sdata){ \n";
    s += "  long long int fx=0;     /* "+conv->i2s(nbit_fx)+"-bit */         \n";
    s += "  long long int sx=0;     /* "+conv->i2s(nbit_sx)+"-bit */         \n";
    s += "  long long int fx_sign=0;     /* 1-bit : sign flag of fdata */     \n";
    s += "  long long int fx_abs=0;      /* "+conv->i2s(nbit_fx-1)+"-bit : absolute part of fdata */\n";

    s += "  fx= "+conv->i2mask(nbit_fx)+"&fdata;                             \n";
    s += "  sx= "+conv->i2mask(nbit_sx)+"&(*sdata);                          \n";

    s += "  fx_sign = 0x1&(fdata >>"+conv->i2s(nbit_fx-1)+");               \n";
    s += "  fx_abs  = "+conv->i2mask(nbit_fx-1)+"&fdata;                    \n";

    s += "  if(fx_sign == 0x0){                                              \n";
    s += "    sx = sx + fx_abs;                                              \n";
    s += "  }else{                                                           \n";
    s += "    sx = sx - fx_abs;                                              \n";
    s += "  }                                                                \n";

    s += "\n";
    s += "  *sdata = "+conv->i2mask(nbit_sx)+"&sx;\n";
    s += "  return;  \n";
    s += "}          \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* Fixed-Point Accumulate Registor*/                          \n";
    test += "/* nbit_fx(input)   :   " + conv->i2s(nbit_fx) + "-bit      */\n";
    test += "/* nbit_sx(register):   " + conv->i2s(nbit_sx) + "-bit      */\n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(long long int fdata,long long int *sdata);   \n";
    test += "int main(){                                                     \n";
    test += "  long long int i;                                              \n";
    test += "  long long int sdata=0LL;                                      \n";
    test += "\n\n";
    test += "  printf(\"nbit_fx = "+ conv->i2s(nbit_fx) +"\\n\");            \n";
    test += "  printf(\"nbit_sx = "+ conv->i2s(nbit_sx) +"\\n\");            \n";


    test += "  for(i=0;i<="+conv->i2mask(nbit_sx)+";i++){\n";
    test += "    long long int fdata=0LL;                                    \n";
    test += "    fdata=i;                                                    \n";
    test += "\n\n";
    test += "    "+func_name+"(fdata,&sdata);                                \n";
    test += "    printf(\"fdata=0x%X\\t\",fdata);        \n";
    test += "    printf(\"sdata=0x%X\\n\",sdata);        \n";
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
    shell += "./test_"+func_name+"|less\n";

    // gnuplot script
    string gnuplot;
    gnuplot = "\n\n";
    gnuplot += "# ================================================= \n";
    gnuplot += "#      This component has no gnuplot script.        \n";
    gnuplot += "# ================================================= \n";

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);            // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");     // for debug no problem
    //    gfObj->generate("test_"+func_name,gnuplot,".plot"); // for debug no problem
  }

}

