/* --- function list -----------------------------
       void std::generate_c_bits_delay(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_bits_delay(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sz = args[1];
    int nbit = atoi(args[2].c_str());
    int ndelay = atoi(args[3].c_str());

    if(nbit < 1) {cerr << "pg_bits_delay: length of nbit is too small:" << nbit <<endl;exit(1);}
    if(ndelay < 1) {cerr << "pg_bits_delay: length of ndelay is too small:" << nbit <<endl;exit(1);}


    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_delay";
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
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";
    s += "void " + func_name + "(PGINT indata, PGINT* outdata){\n";
    s += "  *outdata = "+conv->i2lmask(nbit)+"&indata;     \n";
    s += "  return;                                        \n";
    s += "}\n";

    gfObj->generate(func_name,s);

  }




}
