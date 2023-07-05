/* --- function list -----------------------------
   void std::generate_c_pg_float_unsigned_add(...);
   2003/11/24 coding end
   2003/11/24 coding start
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_float_unsigned_add(vector<string> args, GenFile* gfObj)
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
      cerr << "Error at float_unsigned_add. \n";
      cerr << "Rounding Mode of ["+cv->i2s(round_mode)+"] is not supported. \n";
      exit(0);
    }

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_float_unsigned_add";
    func_name += "_float" + cv->i2s(nbit_float);
    func_name += "_man" + cv->i2s(nbit_man);

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    string expmsk = cv->i2lmask(nbit_exp);
    string manmsk = cv->i2lmask(nbit_man);
    string expstr = cv->i2s(nbit_exp);
    string manstr = cv->i2s(nbit_man);

    s += "// PGPG Floating-Point Unsigned Adder                \n";
    s += "// by Tsuyoshi Hamada (2003/11)                      \n";
    s += "// nbit_float :   " + cv->i2s(nbit_float)+"-bit      \n";
    s += "// nbit_man   :   " + cv->i2s(nbit_man) + "-bit      \n";
    s += "// (1)This operation can't handle exceptions like    \n";
    s += "//     (+/-)inf, NaN and denormalized numbers.       \n";
    s += "// (2)Exponent is not biased.                        \n";
    s += "// (3)(+/-)Zero means: (sign<<"+cv->i2s(nbit_float-1)+
      ") |"+cv->i2slx(0x1<<(nbit_float-2))+"\n";
    s += "#include<stdio.h>                                   \n";
    s += "#include<stdlib.h>   /* exit() */                   \n";
    s += "#ifndef LONG                                        \n";
    s += "#define LONG long long int                          \n";
    s += "#endif                                              \n";
    if(nbit_float>32){
      s += "void " + func_name + "(LONG opx, LONG opy, LONG* result){\n";
    }else{
      s += "void " + func_name + "(int opx, int opy, int* result){\n";
    }
    s += "  LONG x,y,z;\n";
    s += "  LONG nonzx,nonzy;\n";
    s += "  LONG signx,signy,signz;\n";
    s += "  LONG expx,expy;\n";
    s += "  LONG expxr,expyr;\n";
    s += "  LONG manx,many;\n";
    s += "  LONG win_flag; /* 0: opx>=opy, 1: opx<opy */\n";
    s += "  LONG expdif; /* abs(expx-expy)    */\n";
    s += "  LONG mana; /* Winner(manx,many) */\n";
    s += "  LONG manb; /* Loser(manx,many)  */\n";
    s += "  LONG expz0=0;\n";
    s += "  LONG manz0=0;\n";
    s += "  LONG expz;\n";
    s += "  LONG manz=0;\n";
    s += "  LONG Ulp,Gbit,Rbit;\n";
    s += "  LONG manxy; /* added man ("+manstr+"-bit) */\n";
    s += "  LONG exp_inc;\n";
    s += "  LONG is_zeroadd;\n";
    s += "  x = (LONG)opx;\n";
    s += "  y = (LONG)opy;\n";
    s += "  signx = 0x1LL&(x>>"+cv->i2s(nbit_float-1)+");\n";
    s += "  signy = 0x1LL&(y>>"+cv->i2s(nbit_float-1)+");\n";
    s += "  signz = signx;\n";
    s += "  expx = "+expmsk+"&(x>>"+manstr+");\n";
    s += "  expy = "+expmsk+"&(y>>"+manstr+");\n";
    s += "  manx = "+manmsk+"&x;\n";
    s += "  many = "+manmsk+"&y;\n";
    s += "  /* nonz evaluation */\n";
    s += "  if(("+cv->i2lmask(nbit_float-1)+"&x)=="+cv->i2slx(0x1<<(nbit_float-2))
      +") nonzx=0; else nonzx=1;\n";
    s += "  if(("+cv->i2lmask(nbit_float-1)+"&y)=="+cv->i2slx(0x1<<(nbit_float-2))
      +") nonzy=0; else nonzy=1;\n";

    s += "  /* set z0 if one of nonzx,nonzy is negactive */\n";
    s += "  if((nonzx==0)&&(nonzy==0)){\n";
    s += "    is_zeroadd=1;\n";
    //    s += "    expz0 = 0x80LL;\n";
    s += "    expz0 = "+cv->i2slx(1<<(nbit_exp-1))+";\n";
    s += "    manz0 = 0x0LL;\n";
    s += "  }else if((nonzx==1)&&(nonzy==0)){\n";
    s += "    is_zeroadd=1;\n";
    s += "    expz0 = expx;\n";
    s += "    manz0 = manx;\n";
    s += "  }else if((nonzx==0)&&(nonzy==1)){\n";
    s += "    is_zeroadd=1;\n";
    s += "    expz0 = expy;\n";
    s += "    manz0 = many;\n";
    s += "  }else{\n";
    s += "    is_zeroadd=0;\n";
    s += "  }\n";
    s += "\n";

    s += "  { /* Biasing the exp (just inverce the MSB)*/\n";
    s += "    LONG msbx,msby;\n";
    s += "    msbx = 0x1LL&(expx>>"+cv->i2s(nbit_exp-1)+");\n";
    s += "    msby = 0x1LL&(expy>>"+cv->i2s(nbit_exp-1)+");\n";
    s += "    msbx = 0x1LL ^ msbx;\n";
    s += "    msby = 0x1LL ^ msby;\n";
    s += "    expxr = (msbx<<"+cv->i2s(nbit_exp-1)+")|("
      +cv->i2lmask(nbit_exp-1)+"&expx);\n";
    s += "    expyr = (msby<<"+cv->i2s(nbit_exp-1)+")|("
      +cv->i2lmask(nbit_exp-1)+"&expy);\n";
    s += "  }\n";
    s += "\n";

    s += "  { /* Compare and Swap (X , Y) */\n";
    s += "    LONG eflag,mflag,exy,mxy;\n";
    s += "    exy = "+cv->i2lmask(nbit_exp+1)+"&(expxr - expyr); /* "
      +cv->i2s(nbit_exp+1)+"-bit adder */\n";
    s += "    eflag = 0x1LL&(exy>>"+expstr+");\n";
    s += "    expdif = "+expmsk+"&exy;\n";
    s += "    if(eflag==1) expdif = "+expmsk+"&(("+expmsk+"^expdif)+0x1LL);"
      +" /* 2's complement */\n";
    s += "\n";

    s += "    mxy = "+cv->i2lmask(nbit_man+1)+"&(manx - many); "+
      "/* "+cv->i2s(nbit_man+1)+"-bit adder */\n";
    s += "    mflag = 0x1LL&(mxy>>"+manstr+");\n";
    s += "    mxy &= "+manmsk+"; /* not used */\n";
    s += "\n";

    s += "    if(exy==0) win_flag = mflag; else win_flag = eflag;\n";
    s += "\n";

    s += "    if(win_flag==0){\n";
    s += "      expz = expxr;\n";
    s += "      mana = manx;\n";
    s += "      manb = many;\n";
    s += "    }else{\n";
    s += "      expz = expyr;\n";
    s += "      mana = many;\n";
    s += "      manb = manx;\n";
    s += "    }\n";
    s += "  }\n";
    s += "\n";

    s += "  { /* Re-Biasing the exp (just inverce the MSB)*/\n";
    s += "    LONG msbz;\n";
    s += "    msbz = 0x1LL&(expz>>"+cv->i2s(nbit_exp-1)+");\n";
    s += "    msbz = 0x1LL ^ msbz;\n";
    s += "    expz = (msbz<<"+cv->i2s(nbit_exp-1)+")|("+cv->i2lmask(nbit_exp-1)+"&expz);\n";
    s += "  }\n";
    s += "\n";

    s += "  { /* Shift */\n";
    s += "    if(expdif>="+cv->i2s(nbit_man+2)+"){\n";
    s += "      Gbit = 0;\n";
    s += "      Rbit = 1;\n";
    s += "      manb = 0;\n";
    s += "    }else if (expdif=="+cv->i2s(nbit_man+1)+"){\n";
    s += "      Gbit = 1;\n";
    s += "      if(manb==0x0LL) Rbit=0; else Rbit=1;\n";
    s += "      manb = 0;\n";
    s += "    }else if (expdif=="+cv->i2s(nbit_man)+"){\n";
    s += "      Gbit = 0x1LL&(manb>>"+cv->i2s(nbit_man)+");\n";
    s += "      if(("+cv->i2lmask(nbit_man-1)+"&manb)==0x0LL) Rbit=0; else Rbit=1;\n";
    s += "      manb = 1;\n";
    s += "    }else if((expdif>=1)&&(expdif<="+cv->i2s(nbit_man-1)+")){\n";
    s += "      LONG val; /* 1+"+manstr+"+("+manstr+"-1) = "
      +cv->i2s(2*nbit_man)+"-bit  (MSB fixed to High) */\n";
    s += "      val = 0x1LL<<"+cv->i2s(2*nbit_man-1)
      +" | (manb<<"+cv->i2s(nbit_man-1)+"); /* set 1 to MSB */\n";
    s += "      val = val>>expdif;\n";
    s += "      Gbit = 0x1LL&(val>>"+cv->i2s(nbit_man-2)+");"
      +"                                 /* val("+cv->i2s(nbit_man-2)+") */\n";
    s += "      if(("+cv->i2lmask(nbit_man-2)+"&val)==0) Rbit=0; else Rbit=1;"
      +" /* val("+cv->i2s(nbit_man-3)+" downto 0) */\n";
    s += "      manb = "+manmsk+"&(val>>"+cv->i2s(nbit_man-1)+");"
      +"                 /* val("+cv->i2s(2*nbit_man-2)+" downto "+cv->i2s(nbit_man-1)+") */\n";
    s += "    }else if(expdif==0){\n";
    s += "      Gbit = 0;\n";
    s += "      Rbit = 0;\n";
    s += "      manb= (0x1LL<<"+manstr+")|manb; /* "+cv->i2s(nbit_man+1)+"-bit */\n";
    s += "    }else{\n";
    s += "      printf(\"system error on float_uadd.\\n\");exit(0);\n";
    s += "    }\n";
    s += "  }\n";
    s += "\n";

    s += "  { /* mana+manb : "+cv->i2s(nbit_man+1)+"-bit Adder with Carry-out */\n";
    s += "    LONG addo,addco;\n";
    s += "    mana =  (0x1LL<<"+manstr+")| mana; /* "+cv->i2s(nbit_man+1)+"-bit */\n";
    s += "    addo  = 0x1ffffffLL&(mana + manb);\n";
    s += "    addco = 0x1LL&(addo>>"+cv->i2s(nbit_man+1)+");\n";
    s += "    if(addco==0){\n";
    s += "      exp_inc = 0;                    /* note exp_inc is 2-bit */\n";
    s += "      manxy = "+manmsk+"&addo;        /* "+manstr+"-bit */\n";
    s += "    }else{\n";
    s += "      exp_inc = 1;                    /* note exp_inc is 2-bit */\n";
    s += "      manxy = "+manmsk+"&(addo>>1);   /* "+manstr+"-bit */\n";
    s += "    }\n";
    s += "  }\n";
    s += "\n";

    s += "  Ulp = 0x1LL&manxy;\n\n";

    s += "  { /* Rounding */\n";
    s += "    int rmode="+cv->i2s(round_mode)+";\n";
    s += "    LONG man_inc;\n";
    s += "    /* man_inc evaluation */\n";
    s += "    if(rmode == 0)      man_inc = 0;                           /* Truncation */\n";
    s += "    else if(rmode == 1) man_inc = signz*(1-(1-Gbit)*(1-Rbit)); /* Truncation to Zero */\n";
    s += "    else if(rmode == 2) man_inc = Gbit;      /* Rounding to Plus Infinity */\n";
    s += "    else if(rmode == 3) man_inc = Gbit*Rbit; /* Rounding to Minus Infinity */\n";
    s += "    else if(rmode == 4) man_inc = Gbit*(1-signz*(1-Rbit));     /* Rounding to Infinity */\n";
    s += "    else if(rmode == 5) man_inc = Gbit*(1-(1-signz)*(1-Rbit)); /* Rounding to Zero */\n";
    s += "    else if(rmode == 6) man_inc = Gbit*(1-(1-Ulp)*(1-Rbit));   /* Rounding to Even */\n";
    s += "    else if(rmode == 7) man_inc = Gbit*(1-Ulp*(1-Rbit));       /* Rounding to Odd */\n";
    s += "    else                man_inc = Gbit*(1-(1-Ulp)*(1-Rbit));   /* Rounding to Even */\n";
    s += "    /* "+manstr+"-bit ADDER with Carry-out */\n";
    s += "    manz = "+cv->i2lmask(nbit_man+1)+"&(manxy+man_inc);\n";
    s += "    /* Check Carry-out */\n";
    s += "    if( (0x1LL&(manz>>"+manstr+")) == 0x1LL){\n";
    s += "      exp_inc++;\n";
    s += "      if(manz != "+cv->i2slx((1<<nbit_man))+"){printf(\"Error at float_uadd\\n\");exit(0);}\n";
    s += "    }\n";
    s += "    manz &= "+manmsk+";\n";
    s += "  }\n";
    s += "\n";

    s += "  /* Exp Increase : 00,01 and 10 are the candidate for exp_inc !! */\n";
    s += "  expz = "+expmsk+"&(expz + exp_inc); /* Ignore overflow */\n";
    s += "\n";

    string tomsb = cv->i2s(nbit_float-1);
    s += "  if(is_zeroadd==1){\n";
    s += "    z = (signz << "+tomsb+") | (expz0<<"+manstr+") | manz0;\n";
    s += "  }else{\n";
    s += "    z = (signz << "+tomsb+") | (expz<<"+manstr+") | manz ;\n";
    s += "  }\n";

    if(nbit_float>32)
      s += "  (*result)=(LONG)z;\n";
    else
      s += "  (*result)=(int)z;\n";
    s += "}\n\n";
    //    cout << s;gfObj->generate(func_name,s);int nbit_fix=32;return;//debug-line
    gfObj->generate(func_name,s);
  }

}
