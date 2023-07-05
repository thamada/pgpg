/* --- function list -----------------------------
       void std::generate_c_pdelay(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_pdelay(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sz = args[1];
    int nbit = atoi(args[2].c_str());
    int ndelay = atoi(args[3].c_str());

    if(nbit < 1) {cerr << "pg_pdelay: length of nbit is too small:" << nbit <<endl;exit(1);}
    if(ndelay < 1) {cerr << "pg_pdelay: length of ndelay is too small:" << nbit <<endl;exit(1);}


    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_pdelay";
    func_name += "_" + conv->i2s(nbit);
    func_name += "_np" + conv->i2s(ndelay);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int indata, int* outdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* PIPELINE DELAY */\n";
    s += "/* nbit   :   " + conv->i2s(nbit)   + "-bit    */\n";
    s += "/* npipe  :   " + conv->i2s(ndelay) + "-stage  */\n";
    s += " \n";

    s += "#include<stdio.h>                                \n";
    s += "void " + func_name + "(int indata, int* outdata){\n";
    s += "  *outdata = "+conv->i2mask(nbit)+"&indata;      \n";
    s += "  return;                                        \n";
    s += "}          \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* PIPELINE DELAY */\n";
    test += "/* nbit   :   " + conv->i2s(nbit)   + "-bit    */\n";
    test += "/* npipe  :   " + conv->i2s(ndelay) + "-stage  */\n";
    test += " \n";

    test += "#include<stdio.h>                              \n";
    test += "#include<math.h>                               \n";
    test += "void "+ func_name +"(int indata, int* outdata);\n";
    test += "int main(){                                    \n";
    test += "  int i;                                       \n";
    test += "  for(i=0;i<="+conv->i2mask(nbit)+";i++){      \n";
    test += "    int indata=i;                              \n";
    test += "    int outdata=0;                             \n";
    test += "    "+func_name+"(indata,&outdata);            \n";
    test += "    printf(\"(in)x=0x%X, (out)z=0x%X\\n\",indata,outdata);\n";
    test += "  }                                            \n";
    test += "  return 1;                                    \n";
    test += "}                                              \n";

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
