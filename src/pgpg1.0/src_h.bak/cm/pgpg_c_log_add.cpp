// PROGRAPE SIGNED LOGARITHMIC ADDER  MODULE */
// Copyright(c) 2002 by T.Hamada 
// hamada@provence.c.u-tokyo.ac.jp
/* --- function list -----------------------------
       void std::generate_c_pg_log_add(...);
   -----------------------------------------------*/ 
#include<iostream>
#include<cmath>
#include "pgpgc.h"

// static private members
namespace{
  using namespace std;

  // PRIORITY ENCODER
  int priority_encoder(int input)
  { 
    int out=0;
    for(int i=31;i >=0;i--){
        int buf = 0x1 & (input >>i);
        if(buf == 0x1){ out = i; break;}
        out = i;
    }
    if(out == 0){
      cerr << "error"<<endl;
      exit(0);
    }
    return out;
  }


  // +-------------------+
  // | ABOUT PLUS TABLE  |
  // +-------------------+

  int get_log_diff_tab_plus(int indata, int nbit_man)
  {
    double ddiv = pow(2.0,(double)nbit_man);
    double d = ((double)indata+0.25)/ddiv;
    double x = 1.0 + pow(2.0, -1.0*d);
    int iy = (int) (0.5 + ddiv*log(x)/log(2.0));
    return (iy);
  }

  int get_plustable_inwidth(
			    int nbit_log,
			    int nbit_man)
  {
    int imax = (int)pow(2.0,(double)(nbit_log-2));
    int i=1;
    int index=0;
    while(i < imax){
      int log_diff = get_log_diff_tab_plus(i,nbit_man);
      if(log_diff == 0){index = --i;i=imax;}
      i++;
    }
    int table_width=0;
    table_width = 1 + priority_encoder(index);
    return table_width;
  }

  int get_plustable_outwidth(
			    int nbit_log,
			    int nbit_man)
  {
    return(nbit_man);
  }

  // +--------------------+
  // | ABOUT MINUS TABLE  |
  // +--------------------+

  int get_log_diff_tab_minus(int indata, int nbit_man)
  {
    double ddiv = pow(2.0,(double)nbit_man);
    double d = ((double)indata+0.25)/ddiv;
    double x = 1.0 - pow(2.0, -1.0*d);
    int iy = (int) (0.5 + -1.0*ddiv*log(x)/log(2.0));
    return (iy);
  }

  int get_minustable_inwidth(
			    int nbit_log,
			    int nbit_man)
  {
    int imax = (int)pow(2.0,(double)(nbit_log-2));
    int i=1;
    int index=0;
    while(i < imax){
      int log_diff = get_log_diff_tab_minus(i,nbit_man);
      if(log_diff == 0){index = --i;i=imax;}
      i++;
    }
    int table_width=0;
    table_width = 1 + priority_encoder(index);
    return table_width;
  }

  int get_minustable_outwidth(
			    int nbit_log,
			    int nbit_man)
  {
    int log_diff = get_log_diff_tab_minus(1,nbit_man);
    int outwidth = 1 + priority_encoder(log_diff);

    if((0x10000000&log_diff) != 0 || log_diff == 0){
      cerr << "output width of minus table is out of range:" <<  outwidth <<endl;
      exit(1);
    }
    return outwidth;
  }

}

namespace std{

  void generate_c_pg_log_add(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();
    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    string sz = args[2];
    int nbit_log = atoi(args[3].c_str());
    int nbit_man = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str()); nstage += 0;

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_log_add";
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int logx, int logy, int* logz)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    int WinA = get_plustable_inwidth(nbit_log,nbit_man);   // input width of table (+)
    int WoutA = get_plustable_outwidth(nbit_log,nbit_man); // output width of table (+)
    int WinB = get_minustable_inwidth(nbit_log,nbit_man);  // input width of table (-)
    int WoutB = get_minustable_outwidth(nbit_log,nbit_man);// output width of table (-) 

    string s;
    s += "/* SIGNED logarithmic adder */\n";
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */\n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */\n";
    s += "/* input width of table(+)   :   " + conv->i2s(WinA) + "-bit    */\n";
    s += "/* output width of table(+)  :   " + conv->i2s(WoutA) + "-bit    */\n";
    s += "/* input width of table(-)   :   " + conv->i2s(WinB) + "-bit    */\n";
    s += "/* output width of table(-)  :   " + conv->i2s(WoutB) + "-bit    */\n";
    s += "/* Estimation of the table size: ";
    s += conv->i2s((int)(pow(2.0,(double)WinA)*(double)WoutA+pow(2.0,(double)WinB)*(double)WoutB));
    s += " bits */\n";

    s += "/*\n";
    s += "   logimage["+ conv->i2s(nbit_log-1) +"] (MSB): Signed Frag\n";
    s += "   logimage["+ conv->i2s(nbit_log-2) +"]      : Non-Zero Frag\n";
    s += "   logimage["+ conv->i2s(nbit_log-3) +" downto "+ conv->i2s(nbit_man) + "] : log part of exponent\n";
    s += "   logimage["+ conv->i2s(nbit_man-1) +" downto 0]  : log part of mantissa\n";
    s += "*/\n\n";

    s += "#include<stdio.h>\n";
    s += "#include<math.h>\n";
    s += "void " + func_name + "(int logimage_x, int logimage_y, int* logimage_z){\n";
    s += "  int signz=0;\n"; 
    s += "  int zlogpart=0;\n";
    s += "  int signx=0;     /* signed-frag of logimage_x */\n";
    s += "  int signy=0;     /* signed-frag of logimage_y */\n";
    s += "  int x=0;         /* logpart of logimage_x */\n";
    s += "  int y=0;         /* logpart of logimage_y */\n";
    s += "  int xy=0;     /* "+conv->i2s(nbit_log-1)+"-bit (x - y) */\n";
    s += "  int yx=0;     /* "+conv->i2s(nbit_log-1)+"-bit (y - x) */\n";
    s += "  int signxy=0; /*  1-bit SIGN of xy */\n";
    s += "  int d=0;      /*  X-Y(X>=Y) or Y-X(X<Y) */\n";
    s += "  int add_operand=0;\n";
    s += "  int func_plus=0;   /* output of the function plus part */\n";
    s += "  int func_minus=0;  /* output of the function plus part */\n";
    s += "\n\n";

    s += "  signx = (logimage_x>>"+conv->i2s(nbit_log-1)+")&1;\n";
    s += "  signy = (logimage_y>>"+conv->i2s(nbit_log-1)+")&1;\n";

    s += "  x= "+conv->i2mask(nbit_log-1)+"&logimage_x;\n";
    s += "  y= "+conv->i2mask(nbit_log-1)+"&logimage_y;\n";
    s += "\n";

    s += "  /* X RESET FUNCTION */\n";
    s += "  {\n";
    s += "    int _nonzero = 0x1&(x>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    if(_nonzero == 0x0){ x = 0;} /* RESET */\n";
    s += "  }\n";

    s += "  /* Y RESET FUNCTION */\n";
    s += "  {\n";
    s += "    int _nonzero = 0x1&(y>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    if(_nonzero == 0x0){ y = 0;} /* RESET */\n";
    s += "  }\n";

    s += "  /* SUBTRACTORS */\n";
    s += "  /* X - Y */\n";
    s += "  xy = x + ("+conv->i2mask(nbit_log)+"&(("+conv->i2mask(nbit_log)+"^y)+1));";
    s += "  xy &= "+conv->i2mask(nbit_log)+"; /* "+conv->i2s(nbit_log)+"-bit arithmetic (the MSB of each operands is 0.)*/\n";
    s += "  /* Y - X */\n";
    s += "  yx = y + ("+conv->i2mask(nbit_log-1)+"&(("+conv->i2mask(nbit_log-1)+"^x)+1));";
    s += "  yx &= "+conv->i2mask(nbit_log-1)+"; /* "+conv->i2s(nbit_log-1)+"-bit arithmetic */\n";

    s += "\n";
    s += "  signxy = (xy>>"+conv->i2s(nbit_log-1)+")&0x1;\n";
    s += "  xy &= "+conv->i2mask(nbit_log-1)+"; /* sharpen the MSB-bit */\n";
    s += "\n";

    s += "  /* MULTIPLEXOR (1) */\n";
    s += "  if(signxy == 0){\n";
    s += "    d = xy;\n";
    s += "  }else{\n";
    s += "    d = yx;\n";
    s += "  }\n";
    s += "\n";

    s += "  /* MULTIPLEXOR (2) */\n";
    s += "  if(signxy == 0){\n";
    s += "    add_operand= x;\n";
    s += "  }else{\n";
    s += "    add_operand = y;\n";
    s += "  }\n";
    s += "\n";


    s += "  /* PLUS FUNCTION PART */\n";
    s += "  {\n";
    s += "    int d_low_part=0;    /* TABLE INPUT : d["+conv->i2s(WinA-1)+" downto 0] */\n";
    s += "    int d_high_part=0;   /* MUX Middle INPUT : d["+conv->i2s(nbit_log-2)+" downto "+conv->i2s(WinA)+"] */\n";
    s += "    int is_zero=0;       /* ALL OR */\n";
    s += "    int one=0;           /* MSB of this function's output */\n";
    s += "    int table_data = 0;  /* "+conv->i2s(WoutA)+"-bit (only mantissa) */\n";
    s += "    int logdiff = 0;     /* "+conv->i2s(WoutA+1)+"-bit (one-frag + table_data(mantissa)) */\n";
    s += "    d_low_part = "+conv->i2mask(WinA)+"&d;\n";
    s += "    d_high_part = "+conv->i2mask(nbit_log-WinA-1)+"&(d>>"+conv->i2s(WinA)+");\n";
    s += "    /* ALL OR */\n";
    s += "    if(d_high_part == 0x0){is_zero = 0x0;}else{is_zero=0x1;}\n";
    s += "    /* GEN MSB-BIT (ALL OR -> NOT) */\n";
    s += "    if(d_low_part == 0x0){one = 0x1;}else{one=0x0;}\n";
    s += "    /* TABLE LOG_ADD */\n";
    s += "    {\n";
    s += "      int table_adr = 0;\n";
    s += "      double D = 0.0;\n";
    s += "      double z_x = 0.0;\n";
    s += "      table_adr = d_low_part; /* (logimage X) - (logimage Y) */\n";
    s += "      D = ((double)table_adr+0.25)/"+conv->i2ds(nbit_man)+";\n";
    s += "      z_x = (log(1.0+pow(2.0,-1.0*D))/log(2.0)); /* logimage[(Real)Z/X] */\n";
    s += "      z_x *= "+conv->i2ds(nbit_man)+";\n";
    s += "      z_x += 0.5;\n";
    s += "      table_data = (int)(z_x);\n";
    s += "      table_data &= "+conv->i2mask(WoutA)+";\n";
    s += "    }\n";
    s += "    logdiff = one<<"+conv->i2s(WoutA)+"|table_data;\n";
    s += "    /* MULTIPLEXOR */\n";
    s += "    if(is_zero == 0x0){ func_plus=logdiff; }else{ func_plus=0x0;}\n";
    s += "  }\n";
    s += "\n";


    s += "  /* MINUS FUNCTION PART */\n";
    s += "  {\n";
    s += "    int d_low_part=0;    /* TABLE INPUT : d["+conv->i2s(WinB-1)+" downto 0] */\n";
    s += "    int d_high_part=0;   /* MUX Middle INPUT : d["+conv->i2s(nbit_log-2)+" downto "+conv->i2s(WinB)+"] */\n";
    s += "    int is_zero=0;       /* ALL OR */\n";
    s += "    int table_data = 0;  /* "+conv->i2s(WoutB)+"-bit  */\n";
    s += "    int minusval = 0;    /* "+conv->i2s(WoutB)+"-bit  */\n";
    s += "    int func_minus_high_part = 0;  /* "+conv->i2s(nbit_log-WoutB-1)+"-bit  */\n";
    s += "    d_low_part = "+conv->i2mask(WinB)+"&d;\n";
    s += "    d_high_part = "+conv->i2mask(nbit_log-WinB-1)+"&(d>>"+conv->i2s(WinB)+");\n";
    s += "    /* ALL OR */\n";
    s += "    if(d_high_part == 0x0){is_zero = 0x0;}else{is_zero=0x1;}\n";
    s += "    /* TABLE LOG_SUB */\n";
    s += "    {\n";
    s += "      int table_adr = 0;\n";
    s += "      double D = 0.0;\n";
    s += "      double z_x = 0.0;\n";
    s += "      table_adr = d_low_part; /* (logimage X) - (logimage Y) */\n";
    s += "      D = ((double)table_adr+0.25)/"+conv->i2ds(nbit_man)+";\n";
    s += "      z_x = (log(1.0-pow(2.0,-1.0*D))/log(2.0)); /* logimage[(Real)Z/X] */\n";
    s += "      z_x *= "+conv->i2ds(nbit_man)+";\n";
    s += "      z_x *= -1.0;  /*-- bug fixed 2002/08/20 --*/\n";
    s += "      z_x += 0.5;   /*-- bug fixed 2002/08/20 --*/\n";
    s += "      table_data = (int)(z_x);\n";
    s += "      if(table_adr==0x0){table_data=0x0;} /* ATTENTION: Input=0x0, Output=don't care. */\n";
    s += "      table_data &= "+conv->i2mask(WoutB)+";\n";
    s += "    }\n";
    s += "    /* MULTIPLEXOR */\n";
    s += "    if(is_zero == 0x1){ table_data=0x0; }\n";
    s += "    /* 2's completition */\n";
    s += "    table_data = "+conv->i2mask(WoutB)+"&("+conv->i2mask(WoutB)+"^table_data);\n";
    s += "    minusval = "+conv->i2mask(WoutB)+"&(table_data + 1);\n";
    s += "    /* MULTIPLEXOR */\n";
    s += "    if(minusval == 0x0){\n";
    s += "      func_minus_high_part=0x0;\n";
    s += "    }else{\n";
    s += "      func_minus_high_part="+conv->i2mask(nbit_log-WoutB-1)+";\n";
    s += "    }\n";
    s += "    func_minus = (func_minus_high_part<<"+conv->i2s(WoutB)+")|minusval;\n";
    s += "  }\n";
    s += "\n";

    s += "  /* LAST ADDITION */ \n";
    s += "  {\n";
    s += "    int inc=0;\n";
    s += "    int addout=0;\n";
    s += "    /* MULTIPLEXOR */\n";
    s += "    if(signx == signy){\n";
    s += "      inc = func_plus;\n";
    s += "    }else{\n";
    s += "      inc = func_minus;\n";
    s += "    }\n";
    s += "    /* ADDER */\n";
    s += "    addout = add_operand + inc;\n";
    s += "    addout &= "+conv->i2mask(nbit_log-1)+";\n";
    s += "    zlogpart = addout;\n";
    s += "  }\n";
    s += "\n";

    s += "  /* SIGN Z */ \n";
    s += "  if(signx == signy){\n";
    s += "    signz = signx;\n";
    s += "  }else{\n";
    s += "    signz = signx ^ signxy;\n";
    s += "  }\n";
    s += "\n";

    s += "  /* NON-ZERO EVALUATION WHEN (X<>Y AND |X|=|Y| -> Z=ZERO */ \n";
    s += "  {\n";
    s += "    int nonzero_mask;\n";
    s += "    int nonzero_z;\n";
    s += "    if((signx != signy)&&(d==0x0)){nonzero_mask=0x0;}else{nonzero_mask=0x1;}\n";
    s += "    nonzero_z =  (zlogpart >>"+conv->i2s(nbit_log-2)+")&nonzero_mask;\n";
    s += "    *logimage_z = (signz<<"+conv->i2s(nbit_log-1)+")|(nonzero_z<<"+conv->i2s(nbit_log-2)+")|("+conv->i2mask(nbit_log-2)+"&zlogpart);\n";
    s += "  }\n";
    s += "\n";
    s += "  return;\n";
    s += "}\n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */       \n";
    test += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */       \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";

    // gnuplot script
    string gnuplot;
    gnuplot +="set terminal postscript\n";
    gnuplot +="set title '" +func_name+ "'\n";
    gnuplot +="set key title \"Relative error: ({(exact(z) - practical(z))/exact(z)}^2)^0.5,\\n z = x + x\"\n";
    gnuplot +="set logscale x\n";
    gnuplot +="set grid\n";
    gnuplot +="set xlabel 'real x'\n";
    gnuplot +="set ylabel 'Re'\n";
    gnuplot +="plot \"./test_"+func_name+".log\"  u 1:2 t \"\"  with lines\n";
    gnuplot +="save \"./test_"+func_name+".gp\" \n";  // save gpfile by "gnuplot"

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);            // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");     // for debug no problem
    //    gfObj->generate("test_"+func_name,gnuplot,".plot"); // for debug no problem

  }




}
