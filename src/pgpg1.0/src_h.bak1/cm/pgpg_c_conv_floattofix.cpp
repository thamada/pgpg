/* --- function list -----------------------------
       void std::generate_c_pg_conv_fixtofloat(...);
   2002/07/17 coding start
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_floattofix(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+

    string sx = args[0];
    string sy = args[1];
    int nbit_float = atoi(args[2].c_str());
    int nbit_man   = atoi(args[3].c_str());
    int nbit_fix   = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str());   nstage +=0;

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_floattofix";
    func_name += "_float" + cv->i2s(nbit_float);
    func_name += "_man" + cv->i2s(nbit_man);
    func_name += "_fix" + cv->i2s(nbit_fix);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int floatdata, int* fixdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* PGPG Floating-Point to Fixed-Point Format Converter */\n";
    s += "/* by Tsuyoshi Hamada (2002/07)                        */\n";
    s += "/* nbit_float :   " + cv->i2s(nbit_float)+"-bit      */\n";
    s += "/* nbit_man   :   " + cv->i2s(nbit_man) + "-bit      */\n";
    s += "/* nbit_fix   :   " + cv->i2s(nbit_fix) + "-bit      */\n";
    s += "#include<stdio.h>                                    \n";
    s += "#include<math.h>   /* pow() */                       \n";
    s += "#ifndef LONG                                         \n";
    s += "#define LONG long long int                           \n";
    s += "#endif                                               \n";
    s += "void " + func_name + "(int floatdata, int* fixdata){ \n";
    s += "  LONG flp;\n";
    s += "  LONG sign;\n";
    s += "  LONG exp; /* NOT BIASED!! */\n";
    s += "  LONG fixabs;\n";
    s += "  LONG man;\n";
    s += "  LONG nonz;\n";
    s += "  LONG fix;\n";
    s += "  flp = (LONG)floatdata;\n";
    s += "  if(((0x1LL&(flp>>"+cv->i2s(nbit_float-2)+"))==1) && (("+cv->i2lmask(nbit_float-2)+"&flp)==0)) nonz=0;\n";
    s += "  else nonz=1;\n";
    s += "\n";
    s += "  sign = 0x1LL&(flp>>"+cv->i2s(nbit_float-1)+");\n";
    s += "  exp  = "+cv->i2lmask(nbit_float-nbit_man-1)+"&(flp>>"+cv->i2s(nbit_man)+");\n";
    s += "  man  = (0x1LL<<"+cv->i2s(nbit_man)+") | ("+cv->i2lmask(nbit_man)+"&flp);\n";
    s += "  fixabs = (man << exp)>>"+cv->i2s(nbit_man)+";\n";
    s += "  if(sign==1) fixabs = (fixabs^"+cv->i2lmask(64)+")+1;\n";
    s += "  fix = "+cv->i2lmask(nbit_fix)+"&fixabs;\n";
    s += "  if(nonz==0) fix=0;\n";
    s += "                    \n";
    s += "                    \n";
    s += "  *fixdata=(int)fix;\n";
    s += "}\n";

    gfObj->generate(func_name,s);

  }




}
