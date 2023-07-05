/* --- function list -----------------------------
       void std::generate_c_pg_bits_part(...);
       PGPG description
         pg_bits_part(x,z,32,15,0);
       This means as follows:
       x(31 downto 0);
       z <= x(15 downto 0);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{
  void generate_c_pg_bits_part(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();
    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];  // Name of x-coordinate
    string sz = args[1];  // Name of y-coordinate
    int nbitx = atoi(args[2].c_str());  // bit-width parameter (integer)
    int nmsb  = atoi(args[3].c_str());  // bit-width parameter (integer)
    int nlsb  = atoi(args[4].c_str());  // bit-width parameter (integer)
    int nbitz = nmsb-nlsb+1;
    if((nmsb<nlsb)||(nlsb<0)||(nbitz>=nbitx)){
      cerr << "Error at pg_c_bits_part:\n";
      cerr << "  z <= x("+ cv->i2s(nmsb)+" downto "+cv->i2s(nlsb)+") ?\n";
      cerr << cv->i2s(nbitx) +" "+cv->i2s(nbitz)+"\n";
      cerr << "pgpgc_module stoped uncompletely.\n";
      exit(1);
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_part";
    func_name += "_" + cv->i2s(nbitx);
    func_name += "_" + cv->i2s(nmsb);
    func_name += "_" + cv->i2s(nlsb);

    // simulator
    string s;
    s += "/* opx nbit  :   " + cv->i2s(nbitx) + "-bit */\n";
    s += "/* opz nbit  :   " + cv->i2s(nbitz)  + "-bit */\n";
    s += "/* z <= x(" + cv->i2s(nmsb) + " downto "+cv->i2s(nlsb)+"); */\n";
    s += "#include<stdio.h>\n";
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";

    s += "void "+func_name+"(PGINT x,PGINT* z){\n";
    s += "  PGINT _x;\n";
    s += "  PGINT _z;\n";
    s += "  _x = "+ cv->i2lmask(nbitx) + "&(PGINT)x;\n";
    s += "  _z=0;\n";
    s += "  _z = _x >>"+cv->i2s(nlsb)+";\n";
    s += "  *z = "+ cv->i2lmask(nbitz) + "&_z;\n";
    s += "}\n";

    gfObj->generate(func_name,s);
  }
}
