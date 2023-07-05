#include<iostream>
#include<cmath>
#include "pgpgc.h"

namespace std{
  void generate_c_pg_bits_shift(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();
    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    int nshift = atoi(args[0].c_str());
    string sx  = args[1];  // Name of x-coordinate
    string sz  = args[2];  // Name of z-coordinate
    int nbit   = atoi(args[3].c_str());  // bit-width parameter (integer)

    string snshift;
    {
      snshift = "";
      if(nshift<0){
	snshift = "m";
	snshift += cv->i2s(-nshift);
      }else{
	snshift += cv->i2s(nshift);
      }
      int nsabs = abs(nshift);
      if(nsabs>=nbit){
	cerr << "pg_bits_shift: nshift(="<< nsabs << ") must be less than nbit(="<<nbit <<")" <<endl; 
	exit(1);
      }
      if(nsabs==0){
	cerr << "pg_bits_shift: nshift is ZERO. " <<endl; 
	exit(1);
      }
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_shift";
    func_name += "_" + cv->i2s(nbit);
    func_name += "_" + snshift;

    // simulator
    string s;
    s += "/* nshift  :   " + cv->i2s(nshift) + " */\n";
    s += "/* nbit  :   " + cv->i2s(nbit) + "-bit */\n";
    s += "#include<stdio.h>\n";
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";

    s += "void "+func_name+"(PGINT x, PGINT* z){\n";
    s += "  PGINT _x;\n";
    s += "  PGINT _z;\n";
    s += "  _x = "+ cv->i2lmask(nbit) + "&(PGINT)x;\n";

    if(nshift>0){
      s += "  _z = _x<<"+ cv->i2s(nshift) +";\n";
    }else{
      s += "  _z = _x>>"+ cv->i2s(-nshift) +";\n";
    }
    s += "  *z = "+ cv->i2lmask(nbit) + "&_z;\n";
    s += "}\n";
    gfObj->generate(func_name,s);
  }
}
