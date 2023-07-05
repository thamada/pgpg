/* --- function list -----------------------------
       void std::generate_c_pg_fix_addsub(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"


namespace std{
  void generate_c_pg_function(vector<string> args, GenFile* gfObj)
  {
    //    Convert* conv;

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string unit_name = args[0];  // ADD or SUB
    string argslist_str;
    argslist_str = "(";
    if(args.size()==3){
      argslist_str += "int x, ";
    }else if(args.size()==4){
      argslist_str += "int x, int y,";
    }else{
      for(int i=1;i<(int)(args.size()-1);i++){
	char _param[200];
	sprintf(_param,"x%d",(i-1));
	argslist_str += "int ";
	argslist_str += _param;
	argslist_str += ", ";
      }
    }

    argslist_str += "int* z)";

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name = "pg_function_" + unit_name;

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << argslist_str <<endl;

    // simulator
    string s;
    s += "#include<stdio.h>\n";
    s += "void "+ func_name + argslist_str +"{\n";
    s += "\t\n";
    s += "\t\n";
    s += "\t\n";
    s += "}\n";

    gfObj->generate(func_name,s);

  }


}
