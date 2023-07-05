/* --- function list -----------------------------
       void std::generate_c_pg_bits_and(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"


namespace std{
  void generate_c_pg_bits_inv(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];  // Name of x-coordinate
    string sz = args[1];  // Name of z-coordinate
    int nbit  = atoi(args[2].c_str());  // bit-width parameter (integer)

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_inv";
    func_name += "_" + cv->i2s(nbit);

    // simulator
    string s;
    s += "/* nbit  :   " + cv->i2s(nbit) + "-bit */\n";
    s += "#include<stdio.h>\n";
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";

    s += "void "+func_name+"(PGINT x,PGINT* z){\n";
    s += "  PGINT _x;\n";
    s += "  PGINT _z;\n";
    s += "  _x = "+ cv->i2lmask(nbit) + "&(PGINT)x;\n";
    s += "  _z = ~(_x);\n";
    s += "  *z = "+ cv->i2lmask(nbit) + "&_z;\n";
    s += "}\n";

    gfObj->generate(func_name,s);
  }
}
