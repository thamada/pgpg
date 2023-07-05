/* --- function list -----------------------------
       void std::generate_c_pg_fix_addsub(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_log_muldiv(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string muldivflag = args[0];  // ADD or SUB
    string sx = args[1];
    string sy = args[2];
    string sz = args[3];
    int nbit = atoi(args[4].c_str()); // bit-width parameter (integer)
    // args[5]: mantissa-length   -> no need at this program
    // args[6]: pipelining-number -> no need at this program

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    // 2003/09/11 support SDIV
    //    if(muldivflag == "MUL") func_name = "pg_log_mul"; else func_name = "pg_log_div";
    if(muldivflag == "MUL") func_name = "pg_log_mul";
    else if(muldivflag == "SDIV") func_name = "pg_log_sdiv";
    else func_name = "pg_log_div";

    func_name += "_" + conv->i2s(nbit);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int x,int y,int* z)"<<endl;

    // simulator
    string s;
    s += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */                  \n";
    s += "/* MULDIV :   " + muldivflag      + "          */                  \n";

    s += "#include<stdio.h>                                                  \n";
    s += "void " + func_name + "(int x,int y,int* z){                        \n";
    s += "  int _signbit_x=0;                                                \n";
    s += "  int _signbit_y=0;                                                \n";
    s += "  int _signbit_z=0;                                                \n";
    s += "  int _nonzbit_x=0;                                                \n";
    s += "  int _nonzbit_y=0;                                                \n";
    s += "  int _nonzbit_z=0;                                                \n";
    s += "  int _x=0;                                                        \n";
    s += "  int _y=0;                                                        \n";
    s += "  int _z=0;                                                        \n";
    s += "  _signbit_x = 0x1&(x >> " + conv->i2s(nbit-1) + ");               \n";
    s += "  _signbit_y = 0x1&(y >> " + conv->i2s(nbit-1) + ");               \n";
    s += "  _signbit_z = _signbit_x ^ _signbit_y;                            \n";
    s += "  _nonzbit_x = 0x1&(x >> " + conv->i2s(nbit-2) + ");               \n";
    s += "  _nonzbit_y = 0x1&(y >> " + conv->i2s(nbit-2) + ");               \n";
    s += "  _nonzbit_z = _nonzbit_x & _nonzbit_y;                            \n";
    s += "  _x = "+ conv->i2mask(nbit-2) + "&x;                              \n";
    s += "  _y = "+ conv->i2mask(nbit-2) + "&y;                              \n";

    if(muldivflag == "MUL"){
      s += "  _z = _x + _y;                                                  \n";
    }else{
      s += "  _z = _x - _y;                                                  \n";
    }

    if(muldivflag == "SDIV"){
      s += "  if(_y>_x) _z = 0;                                              \n";
    }

    s += "  _z = "+ conv->i2mask(nbit-2) + "&_z;                           \n";
    s += "  *z = _signbit_z << " + conv->i2s(nbit-1)
         + " | _nonzbit_z << "   + conv->i2s(nbit-2)
         + " | _z;                                                           \n";
    s += "  *z = "+conv->i2mask(nbit) + "&(*z);                              \n";
    s += "  return;                                                          \n";
    s += "}                                                                  \n";

    // test driver
    string test;
    test += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */               \n";
    test += "/* MULDIV :   " + muldivflag      + "          */               \n";
    test += "#include<stdio.h>                                               \n";
    test += "void "+func_name+"(int x,int y,int* z);                         \n";
    test += "int main(){                                                     \n";
    test += "  int x=0x0ffffffe;                                             \n";
    test += "  int y=0x00000001;                                             \n";
    test += "  int z;                                                        \n";
    test += "  "+func_name+"(x,y,&z);                                        \n";
    test += "  printf(\"nbit = "+ conv->i2s(nbit) +"\\n\");                  \n";
    test += "  printf(\"MULDIV = "+ muldivflag +"\\n\");                     \n";
    test += "  printf(\"x=0x%X,y=0x%X,z=0x%X\\n\",x,y,z);                    \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+"\n";

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);         // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");  // for debug no problem

  }




}
