/* --- function list -----------------------------
       void std::generate_c_pg_conv_fixtofloat(...);
   2003/11/23 bugfix : priority encoder check msb for 0x80...0
   2003/11/22 bugfix : add "exp adjust adder"
   2003/07/17 coding start
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_conv_fixtofloat(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();
    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    int nbit_fix   = atoi(args[2].c_str());
    int nbit_float = atoi(args[3].c_str());
    int nbit_man = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str()); nstage += 0; // not used
    int round_mode    = atoi(args[6].c_str());
    // round_mode 0=Truncation                ,inc=0
    //            1=Truncation to Zero        ,inc=F(1-(1-G)(1-R))
    //            2=Rounding to Plus infinity ,inc=G
    //            3=Rounding to Minus infinity,inc=GR
    //            4=Rounding to Infinity      ,inc=G(1-F(1-R))
    //            5=Rounding to Zero          ,inc=G(1-(1-F)(1-R))
    //            6=Rounding to Even          ,inc=G(1-(1-U)(1-R))
    //            7=Rounding to Odd           ,inc=G(1-U(1-R))
    if(round_mode>7){
      cerr << "Error at conv_fixtofloat. \n";
      cerr << "Rounding Mode of ["+cv->i2s(round_mode)+"] is not supported. \n";
      exit(0);
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_conv_fixtofloat";
    func_name += "_fix" + cv->i2s(nbit_fix);
    func_name += "_float" + cv->i2s(nbit_float);
    func_name += "_man" + cv->i2s(nbit_man);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int fixdata, int* floatdata)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "// PGPG Fixed-Point to Floating-Point Format Converter\n";
    s += "// by Tsuyoshi Hamada (2003/11)               \n";
    s += "// nbit_fix   : " + cv->i2s(nbit_fix) + "-bit \n";
    s += "// nbit_float : " + cv->i2s(nbit_float)+"-bit \n";
    s += "// nbit_man   : " + cv->i2s(nbit_man) + "-bit \n";
    s += "// This converter; \n";
    s += "// (1) don't treat exceptions; (+/-)inf, NaNs, NaNq.\n";
    s += "// (2) don't treat denormalized numbers.\n";
    s += "// (3) treat no-biased numbers as the exponent.\n";
    s += "// (4) treat (+/-)Zero as : (sign<<"+cv->i2s(nbit_float-1)+") "
      +  "|"+cv->i2slx(0x1<<(nbit_float-2))+"\n";
    s += "// (5) treat rounding (mode="+cv->i2s(round_mode)+").\n";

    s += "#include<stdio.h>                             \n";
    s += "#include<math.h>   /* pow() */                \n";
    s += "#ifndef LONG                                  \n";
    s += "#define LONG long long int                    \n";
    s += "#endif                                        \n";
    s += "void " + func_name + "(int fixdata, int* floatdata){   \n";
    s += "  LONG fix;\n";
    s += "  LONG sign;\n";
    s += "  LONG exp; /* NOT BIASED!! */\n";
    s += "  LONG fixabs;\n";
    s += "  LONG man;\n";
    s += "  LONG nonz;\n";
    s += "  fix = (LONG)fixdata;\n";
    s += "  if(fix == 0x0) nonz=0; else nonz=1;\n";
    s += "  sign = 0x1LL&(fix>>"+cv->i2s(nbit_fix-1)+");\n";
    s += "  if(sign==1) fixabs = (fix^"+cv->i2lmask(nbit_fix)+")+1;\n";
    s += "  else fixabs= fix;\n";
    s += "  { /* PRIORITY ENCODER */\n";
    s += "    int i;\n";
    s += "    int count=0;\n";
    s += "    for(i="+cv->i2s(nbit_fix-1)+";i>=0;i--){\n"; /* bug-fixed :2003/11/23 */
    s += "      LONG buf;\n";
    s += "      buf = 0x1LL & (fixabs>>i);\n";
    s += "      if(buf==0x1LL){count = i;break;}\n";
    s += "      count=i;\n";
    s += "    }\n";
    s += "    exp=(LONG)count;\n";
    s += "    exp &="+cv->i2lmask(nbit_float-nbit_man-1)+";";
    s += "    /*  "+cv->i2s(nbit_float-nbit_man-1)+"-bit  */\n";
    s += "  }\n";
    s += "\n";
    s += "  { /* SHIFTER */\n";
    s += "    if(exp>="+cv->i2s(nbit_man)+"){\n";
    s += "      LONG ulp,gbit,rbit,mask;\n";
    s += "      LONG inc;\n";
    s += "      ulp = 0x1LL&(fixabs>>(exp-"+cv->i2s(nbit_man)+"));\n";
    s += "      if(exp>"+cv->i2s(nbit_man)+") gbit=0x1LL&(fixabs>>(exp-"
      +cv->i2s(nbit_man+1)+"));\n";
    s += "      else gbit=0;\n";
    s += "      if(exp>"+cv->i2s(nbit_man+1)+"){\n";
    s += "        mask =(LONG)(pow(2.0,(exp-"+cv->i2s(nbit_man+1)+")))-1;\n";
    s += "        if((fixabs&mask)==0) rbit=0; else rbit=1;\n";
    s += "      }else rbit=0;\n";
    s += "      man = "+cv->i2lmask(nbit_man)+"&(fixabs>>(exp-"+cv->i2s(nbit_man)+"));\n";
    if(round_mode==6)      s += "      inc = gbit*(1-(1-ulp)*(1-rbit));  /* Rounding to Even   */\n";
    else if(round_mode==0) s += "      inc = 0;                          /* Truncation         */\n";
    else if(round_mode==1) s += "      inc = sign*(1-(1-gbit)*(1-rbit)); /* Truncation to Zero */\n";
    else if(round_mode==2) s += "      inc = gbit;                       /* Rounding to Plus Infinity */\n";
    else if(round_mode==3) s += "      inc = gbit*rbit;                  /* Rounding to Minus Infinity */\n";
    else if(round_mode==4) s += "      inc = gbit*(1-sign*(1-rbit));     /* Rounding to Infinity */\n";
    else if(round_mode==5) s += "      inc = gbit*(1-(1-sign)*(1-rbit)); /* Rounding to Zero     */\n";
    else if(round_mode==7) s += "      inc = gbit*(1-ulp*(1-rbit));      /* Rounding to Odd      */\n";
    s += "      man += inc;\n";
    s += "      /* Adjust Exp. */\n";
    s += "      exp += (0x1LL&(man>>"+cv->i2s(nbit_man)+"));\n";
    s += "      man &= "+cv->i2lmask(nbit_man)+";\n";
    s += "    }else{\n";
    s += "      LONG shift_00;\n";
    s += "      shift_00 = (0xffffffffffffffffLL^"+cv->i2lmask(nbit_man)+")&"
      + "(((LONG)fixabs)<<"+cv->i2s(nbit_man)+");\n";
    s += "      man = "+cv->i2lmask(nbit_man)+"&(shift_00>>exp);\n";
    s += "    }\n";
    s += "  }\n";
    s += "  *floatdata = (int)((sign<<"+cv->i2s(nbit_float-1)+") |"
      += "(exp<<"+cv->i2s(nbit_man)+")|man);\n";
    s += "  if(nonz == 0x0){*floatdata=0; *floatdata=(int)((sign<<"+cv->i2s(nbit_float-1)+")|"
      +"(0x1LL<<"+cv->i2s(nbit_float-2)+"));}  /* zero = sign,1,0...0 */ \n";
    s += "}\n";

    gfObj->generate(func_name,s);
  }
}
