/* --- function list -----------------------------
       void std::generate_c_pg_bits_join(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{
  void generate_c_pg_bits_join(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();
    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];  // Name of x-coordinate
    string sy = args[1];  // Name of y-coordinate
    string sz = args[2];  // Name of z-coordinate
    int nbitx = atoi(args[3].c_str());  // bit-width parameter (integer)
    int nbity = atoi(args[4].c_str());  // bit-width parameter (integer)
    int nbitz = nbitx + nbity;
    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_join";
    func_name += "_" + cv->i2s(nbitx);
    func_name += "_" + cv->i2s(nbity);

    // simulator
    string s;
    s += "/* opx nbit  :   " + cv->i2s(nbitx) + "-bit      */\n";
    s += "/* opy nbit  :   " + cv->i2s(nbity) + "-bit      */\n";
    s += "/* z...z = x...x y...y  (" + cv->i2s(nbitz) + "-bit)*/\n";
    s += "#include<stdio.h>\n";
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";

    s += "void "+func_name+"(PGINT x,PGINT y,PGINT* z){\n";
    s += "  PGINT _x;\n";
    s += "  PGINT _y;\n";
    s += "  PGINT _z;\n";
    s += "  _x = "+ cv->i2lmask(nbitx) + "&(PGINT)x;\n";
    s += "  _y = "+ cv->i2lmask(nbity) + "&(PGINT)y;\n";
    s += "  _z = (_x <<"+cv->i2s(nbity)+")| _y;\n";
    s += "  *z = "+ cv->i2lmask(nbitz) + "&_z;\n";
    s += "}\n";

    gfObj->generate(func_name,s);
  }
}
