/* --- function list -----------------------------
   void std::generate_c_pg_float_mult(...);
   2003/11/21 coding start
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_float_mult(vector<string> args, GenFile* gfObj)
  {
    Convert* cv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    string sz = args[2];
    int nbit_float = atoi(args[3].c_str());
    int nbit_man = atoi(args[4].c_str());
    int nbit_exp = nbit_float-nbit_man-1;
    int nstage   = atoi(args[5].c_str());  nstage+=0; //not used
    int round_mode    = atoi(args[6].c_str());

    // round_mode 0=Truncation                ,inc=0
    //            1=Truncation to Zero        ,inc=F(1-(1-S)(1-G))
    //            2=Rounding to Plus infinity ,inc=S
    //            3=Rounding to Minus infinity,inc=SG
    //            4=Rounding to Infinity      ,inc=S(1-F(1-G))
    //            5=Rounding to Zero          ,inc=S(1-(1-F)(1-G))
    //            6=Rounding to Even          ,inc=S(1-(1-U)(1-G))
    //            7=Rounding to Odd           ,inc=S(1-U(1-G))
    if(round_mode>7){
      cerr << "Error at float_mult. \n";
      cerr << "Rounding Mode of ["+cv->i2s(round_mode)+"] is not supported. \n";
      exit(0);
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_float_mult";
    func_name += "_float" + cv->i2s(nbit_float);
    func_name += "_man" + cv->i2s(nbit_man);

    //    cerr << "\tpgpgc_module generate  void ";
    //    if(nbit_float>32){
    //      cerr << func_name << "(long long int opx, long long int opy, long long int* result)"<<endl;
    //    }else{
    //      cerr << func_name << "(int opx, int opy, int* result)"<<endl;
    //    }


    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "// PGPG Floating-Point Multiplier                    \n";
    s += "// by Tsuyoshi Hamada (2003/11)                      \n";
    s += "// nbit_float :   " + cv->i2s(nbit_float)+"-bit      \n";
    s += "// nbit_man   :   " + cv->i2s(nbit_man) + "-bit      \n";
    s += "// (1)This operation can't handle exceptions like    \n";
    s += "//     (+/-)inf, NaN and denormalized numbers.       \n";
    s += "// (2)Exponent is not biased.                        \n";
    s += "// (3)(+/-)Zero means: (sign<<"+cv->i2s(nbit_float-1)+
      ") |"+cv->i2slx(0x1<<(nbit_float-2))+"\n";
    s += "#include<stdio.h>                                   \n";
    s += "#include<math.h>   /* pow() */                      \n";
    s += "#ifndef LONG                                        \n";
    s += "#define LONG long long int                          \n";
    s += "#endif                                              \n";
    if(nbit_float>32){
      s += "void " + func_name + "(LONG opx, LONG opy, LONG* result){\n";
    }else{
      s += "void " + func_name + "(int opx, int opy, int* result){\n";
    }
    s += "  LONG x,y,z;\n";
    s += "  LONG nonzx,nonzy,nonzz;\n";
    s += "  LONG signx,signy,signz;\n";
    s += "  LONG expx,expy,expz;\n";
    s += "  LONG manx,many,manz;\n";
    s += "  LONG Ulp,Gbit,Rbit;\n";
    s += "  LONG exp_inc;\n";
    s += "  x = (LONG)opx;\n";
    s += "  y = (LONG)opx;\n";
    s += "  signx = 0x1LL&(x>>"+cv->i2s(nbit_float-1)+");\n";
    s += "  signy = 0x1LL&(x>>"+cv->i2s(nbit_float-1)+");\n";
    s += "  signz = 0x1LL&(signx ^ signy);\n";
    s += "  expx = "+cv->i2lmask(nbit_exp)+"&(x>>"+cv->i2s(nbit_man)+");\n";
    s += "  expy = "+cv->i2lmask(nbit_exp)+"&(y>>"+cv->i2s(nbit_man)+");\n";
    s += "  manx = (0x1LL<<"+cv->i2s(nbit_man)+")|("+cv->i2lmask(nbit_man)+"&x);\n";
    s += "  many = (0x1LL<<"+cv->i2s(nbit_man)+")|("+cv->i2lmask(nbit_man)+"&y);\n";
    s += "  /* nonz evaluation */\n";
    s += "  if(("+cv->i2lmask(nbit_float-1)+"&x)=="+cv->i2slx(0x1<<(nbit_float-2))
      +") nonzx=0; else nonzx=1;\n";
    s += "  if(("+cv->i2lmask(nbit_float-1)+"&y)=="+cv->i2slx(0x1<<(nbit_float-2))
      +") nonzy=0; else nonzy=1;\n";
    s += "  nonzz = nonzx * nonzy; // nonzz<= nonzx AND nonzy\n";
    s += "  /* Multiplier for mantissa */\n";
    s += "  manz= "+cv->i2lmask(2*(nbit_man+1))+" & (manx * many); /* "
      +cv->i2s(2*(nbit_man+1))+"-bit */\n";
    s += "  /* Adder for exponent */\n";
    s += "  expz = "+cv->i2lmask(nbit_exp)+"&(expx + expy); "
      +"/* Ignore overflow and biasing */\n";
    s += "  /* Store Ulp, Gbit, Rbit and exp_inc */\n";
    s += "  {\n";
    s += "    LONG muloh2b; /* Top 2-bit of multiplier's outdata */\n";
    s += "    LONG nman;\n";
    s += "    muloh2b = 0x3LL&(manz>>"+cv->i2s(2*(nbit_man+1)-2)+");\n";
    s += "    nman="+cv->i2s(nbit_man)+";\n";
    s += "    if(muloh2b==0x1){ /* If mult-result is \"01.X,,X\" */\n";
    s += "      exp_inc=0;\n";
    s += "      Ulp  = 0x1LL&(manz>>nman);\n";
    s += "      Gbit = 0x1LL&(manz>>(nman-1));\n";
    s += "      if(("+cv->i2lmask(nbit_man-1)+"&manz)==0) Rbit=0; else Rbit=1;\n";
    s += "    }else{            /* If mult-result is \"1X.X,,X\" */\n";
    s += "      exp_inc = 1;\n";
    s += "      Ulp  = 0x1LL&(manz>>(nman+1));\n";
    s += "      Gbit = 0x1LL&(manz>>nman);\n";
    s += "      if(("+cv->i2lmask(nbit_man)+"&manz)==0) Rbit=0; else Rbit=1;\n";
    s += "    }\n";
    s += "  }\n";
    s += "\n";
    s += "  /* manz Shift and Truncate */\n";
    s += "  if(exp_inc==0)\n";
    s += "    manz = "+cv->i2lmask(nbit_man)+"&(manz>>"+cv->i2s(nbit_man)+");\n";
    s += "  else\n";
    s += "    manz = "+cv->i2lmask(nbit_man)+"&(manz>>"+cv->i2s(nbit_man+1)+");\n";
    s += "\n";
    s += "  /* Rounding */\n";
    s += "  {\n";
    s += "    LONG man_inc;\n";
    s += "    int rmode = "+cv->i2s(round_mode)+";\n";
    s += "    if(rmode == 0)      man_inc = 0; /* Truncation */\n";
    s += "    else if(rmode == 1) man_inc = signz*(1-(1-Gbit)*(1-Rbit)); /* Truncation to Zero */\n";
    s += "    else if(rmode == 2) man_inc = Gbit; /* Rounding to Plus Infinity */\n";
    s += "    else if(rmode == 3) man_inc = Gbit*Rbit; /* Rounding to Minus Infinity */\n";
    s += "    else if(rmode == 4) man_inc = Gbit*(1-signz*(1-Rbit)); /* Rounding to Infinity */\n";
    s += "    else if(rmode == 5) man_inc = Gbit*(1-(1-signz)*(1-Rbit)); /* Rounding to Zero */\n";
    s += "    else if(rmode == 6) man_inc = Gbit*(1-(1-Ulp)*(1-Rbit)); /* Rounding to Even */\n";
    s += "    else if(rmode == 7) man_inc = Gbit*(1-Ulp*(1-Rbit)); /* Rounding to Odd */\n";
    s += "    else                man_inc = Gbit*(1-(1-Ulp)*(1-Rbit)); /* Rounding to Even */\n";
    s += "    /* Adder with overflow-flag */\n";
    s += "    manz = "+cv->i2lmask(nbit_man+1)+"&(manz+man_inc);\n";
    s += "    /* Check the overflow-flag */\n";
    s += "    if( (0x1LL&(manz>>"+cv->i2s(nbit_man)+")) == 0x1LL) exp_inc++;\n";
    s += "    manz &= "+cv->i2lmask(nbit_man)+";\n";
    s += "  }\n";
    s += "\n";
    s += "  /* Exp increase. Be sure that 00,01 and 10 are the candidate for exp_inc */\n";
    s += "  expz = "+cv->i2lmask(nbit_exp)+"&(expz + exp_inc); /* Ignore overflow */\n";
    s += "\n";
    s += "  /* set ZERO if nonz is active */\n";
    s += "  if(nonzz==0){\n";
    s += "    expz = 0x1LL<<"+cv->i2s(nbit_exp-1)+";\n";
    s += "    manz = 0x0LL;\n";
    s += "  }\n";
    s += "  z = (signz << "+cv->i2s(nbit_float-1)+") | (expz<<"+cv->i2s(nbit_man)+") | manz ;\n";
    if(nbit_float>32){
      s += "  (*result) = z;\n";
    }else{
      s += "  (*result) = (int)z;\n";
    }
    s += "}\n";
    //    cout << s;gfObj->generate(func_name,s);int nbit_fix=32;return;//debug-line
    gfObj->generate(func_name,s);
  }

}
