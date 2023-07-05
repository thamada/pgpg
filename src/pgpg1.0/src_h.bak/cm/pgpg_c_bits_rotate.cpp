/* --- function list -----------------------------
       void std::generate_c_pg_bits_and(...);
   -----------------------------------------------*/ 
#include<iostream>
#include<cmath>
#include "pgpgc.h"


namespace std{
  void generate_c_pg_bits_rotate(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    int nrot   = atoi(args[0].c_str());
    string sx  = args[1];  // Name of x-coordinate
    string sz  = args[2];  // Name of z-coordinate
    int nbit   = atoi(args[3].c_str());  // bit-width parameter (integer)

    string snrotate;
    {
      snrotate = "";
      if(nrot<0){
	snrotate = "m";
	snrotate += cv->i2s(-nrot);
      }else{
	snrotate += cv->i2s(nrot);
      }
      int nrabs = abs(nrot);
      if(nrabs>=nbit){
	cerr << "pg_bits_rotate: nrotate(="<< nrabs << ") must be less than nbit(="<<nbit <<")" <<endl; 
	exit(1);
      }
      if(nrabs==0){
	cerr << "pg_bits_rotate: nrotate is ZERO. " <<endl; 
	exit(1);
      }
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_bits_rotate";
    func_name += "_" + cv->i2s(nbit);
    func_name += "_" + snrotate;

    // simulator
    string s;
    s += "/* nrotate : " + cv->i2s(nrot) + " */\n";
    s += "/* nbit  :   " + cv->i2s(nbit) + "-bit */\n";
    s += "#include<stdio.h>\n";
    s += "#ifndef PGINT\n";
    s += "#define PGINT long long int\n";
    s += "#endif\n";

    s += "void "+func_name+"(PGINT x,PGINT* z){\n";
    string snbit = cv->i2s(nbit);
    string snrot;
    if(nrot>0) snrot=cv->i2s(nrot); else snrot=cv->i2s(-nrot);

    s += "  int i;\n";
    s += "  PGINT bit["+snbit+"];\n";
    s += "  PGINT _x;\n";
    s += "  PGINT _z;\n";
    s += "  _x = "+ cv->i2lmask(nbit) + "&(PGINT)x;\n";
    s += "  _z=0;\n";
    if(nrot>0){
      s += "  for(i=0;i<"+snbit+";i++) bit[i] = 0x1LL&(_x>>i);\n";
      s += "  for(i=0;i<"+snrot+";i++) _z |= (bit["+snbit+"-i-1]<<("+snrot+"-i-1));\n";
      s += "  for(i="+snrot+";i<"+snbit+";i++) _z |= (bit[i-"+snrot+"]<<i);\n";
    }else{
      s += "  for(i=0;i<"+snbit+";i++) bit[i] = 0x1LL&(_x>>i);\n";
      s += "  for(i=0;i<"+cv->i2s(nbit+nrot)+";i++) _z |= (bit[i+"+snrot+"]<<i);\n";
      s += "  for(i="+cv->i2s(nbit+nrot)+";i<"+snbit+";i++) _z |= (bit[i-"
	+cv->i2s(nbit+nrot)+"]<<i);\n";
    }
    s += "  *z = "+ cv->i2lmask(nbit) + "&_z;\n";
    s += "}\n";

    gfObj->generate(func_name,s);
  }
}
