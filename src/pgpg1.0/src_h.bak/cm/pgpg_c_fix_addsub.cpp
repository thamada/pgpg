/* --- function list -----------------------------
       void std::generate_c_pg_fix_addsub(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"


namespace std{
  void generate_c_pg_fix_addsub(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string addsubflag = args[0];  // ADD or SUB
    string sx         = args[1];  // Name of x-coordinate
    string sy         = args[2];  // Name of y-coordinate
    string sz         = args[3];  // Name of z-coordinate
    int nbit   = atoi(args[4].c_str());  // bit-width parameter (integer)
    int nstage = atoi(args[5].c_str());  // pipelined parameter (integer)
    nstage += 0;

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    if(addsubflag == "ADD") func_name = "pg_fix_add"; else func_name = "pg_fix_sub";
    func_name += "_" + conv->i2s(nbit);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int x,int y,int* z)"<<endl;

    // simulator
    string s;
    s += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */\n";
    s += "/* ADDSUB :   " + addsubflag      + "          */\n";

    s += "#include<stdio.h>\n";
    s += "void "+func_name+"(int x,int y,int* z){\n";
    s += "  unsigned int _x;\n";
    s += "  unsigned int _y;\n";
    s += "  int _z;\n";
    s += "  _x = "+ conv->i2mask(nbit) + "&x;\n";
    s += "  _y = "+ conv->i2mask(nbit) + "&y;\n";
    if(addsubflag == "ADD"){
      s += "  _z = (int)(_x + _y);\n";
      s += "  *z = "+ conv->i2mask(nbit) + "&_z;\n";
    }else{
      s += "  _z = (int)(_x - _y);\n";
      s += "  *z = "+ conv->i2mask(nbit) + "&_z;\n";
    }
    s += "}\n";
    // test driver
    string test;
    test += "/*-- none --*/\n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh\n";
    shell += "echo 'none'\n";

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);           // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");    // for debug no problem

  }




}
