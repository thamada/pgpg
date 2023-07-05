/* --- function list -----------------------------
       void std::generate_c_pg_log_shift(...);
   -----------------------------------------------*/ 
#include<iostream>
#include "pgpgc.h"

namespace std{

  void generate_c_pg_log_shift(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    int nshift = atoi(args[0].c_str());
    string sx = args[1];
    string sz = args[2];
    int nbit = atoi(args[3].c_str());
    // args[4] : mantissa-length -> no need in this program

    string snshift;
    {
      if(nshift<0){
	snshift = "m";
	snshift += conv->i2s(-nshift);
      }else{
	snshift += conv->i2s(nshift);
      }	  
      if((nshift>2)||(nshift<-2)){
	cerr << "pg_log_shift: nshift is out of range: " <<nshift<<endl; 
	exit(1);
      }
      if(nshift==0){
	cerr << "pg_log_shift: nshift is ZERO. Why will you use this component?" <<endl; 
	exit(1);
      }
    }

    if(nbit < 3) {cerr << "pg_log_shift: length is too small:" << nbit <<endl;exit(1);}

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_log_shift";
    func_name += "_log" + conv->i2s(nbit);
    func_name += "_" + snshift;

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(int logx, int* logz)"<<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* UNSIGNED logarithmic shifter(power 2^[nshift]) */\n";
    s += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */ \n";
    s += "/* nshift :   " + snshift + "-bit      */         \n";

    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is different !\n";
    s += "     Input Width : "+conv->i2s(nbit)+"-bit. ["+conv->i2s(nbit)+"-bit Signed Logarithmic Format] \n";
    s += "     Onput Width : "+conv->i2s(nbit-1)+"-bit. ["+conv->i2s(nbit)+"-bit Unsigned Logarithmic Format without a sign bit.] */\n\n";
    s += "/* --- [NOTE!] ---\n";
    s += "   nshift must be 2, 1, -1 or -2. */\n\n";

    s += "#include<stdio.h>                                        \n";
    s += "void " + func_name + "(int logimage_x, int* logimage_z){ \n";
    s += "  int x_sign=0;        /* Sign Flag:  1-bit (for debug)*/\n";
    s += "  int x_nonzero=0;     /* NonZero Flag: 1-bit */         \n";
    s += "  int x_body=0;        /* "+conv->i2s(nbit-2)+"-bit */   \n";
    s += "  int z=0;             /* "+conv->i2s(nbit-1)+"-bit */   \n";

    s += "\n";
    s += "  x_sign    = 0x1&(logimage_x>>"+conv->i2s(nbit-1)+");  /* for debug */\n";
    s += "  x_nonzero = 0x1&(logimage_x>>"+conv->i2s(nbit-2)+");\n";
    s += "  x_body    = "+conv->i2mask(nbit-2)+"&logimage_x;    \n";

    s += "\n";
    if(nshift>0){
      s += "  z = (x_nonzero <<"+conv->i2s(nbit-2)+")|("+conv->i2mask(nbit-2)+"&(x_body<<"+conv->i2s(nshift)+"));\n";
    }else{
      s += "  z = (x_nonzero <<"+conv->i2s(nbit-2)+")|("+conv->i2mask(nbit-2)+"&(x_body>>"+conv->i2s(-nshift)+"));\n";
    }

    s += "  z &= "+conv->i2mask(nbit-1)+";\n";
    s += "  *logimage_z = z;\n";


    s += "\n";
    s += "  return;  \n";
    s += "}          \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */ \n";
    test += "/* nshift :   " + snshift + "-bit      */         \n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(int logimage_x,int* logimage_z);             \n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "  int nbit_man;                                                 \n";
    test += "  int nbit_fxp;                                                 \n";
    test += "  printf(\"Please set fraction(mantissa) length [must be integer]:\");\n";
    test += "  scanf(\"%d\",&nbit_man);                                      \n";
    test += "  printf(\"Please set fixpoint length [must be integer]:\");    \n";
    test += "  scanf(\"%d\",&nbit_fxp);                                      \n";
    test += "\n\n";
    test += "#ifndef GNUPLOT\n";
    test += "  printf(\"nbit = "+ conv->i2s(nbit) +"\\n\");\n";
    test += "  printf(\"nshift = "+ snshift +"\\n\");      \n";
    test += "#endif\n\n";
    test += "  for(i=(int)(-1.0*pow(2.0,(double)(nbit_fxp-1)));i<(int)pow(2.0,(double)(nbit_fxp-1));i+=32){\n";
    test += "    int logimage_x=0;                                           \n";
    test += "    int logimage_z=0;                                           \n";
    test += "    double real_x =(double)i;                                   \n";
    test += "    double real_z = 0.0;                                        \n";
    test += "    double real_z_bylogshift = 0.0;                             \n";
    test += "\n";

    if(nshift>0){
      test += "    real_z = pow(real_x,pow(2.0,"+conv->i2s(nshift)+".0));     \n";
    }else{
      test += "    real_z = pow(real_x,1.0/pow(2.0,"+conv->i2s(-nshift)+".0));\n";
    }

    test += "    if(real_x > 0.0){                                                         \n";
    test += "      logimage_x = (int)(pow(2.0,(double)nbit_man)*log(real_x)/log(2.0)+0.5); \n";
    test += "      logimage_x &= "+conv->i2mask(nbit-2)+";                                 \n";
    test += "      logimage_x = 0x1<<"+conv->i2s(nbit-2)+"|logimage_x;   /* Non-Zero bit */\n";
    test += "    }else if(real_x < 0.0){                                                   \n";
    test += "      logimage_x = (int)(pow(2.0,(double)nbit_man)*log(-1.0*real_x)/log(2.0)+0.5); \n";
    test += "      logimage_x &= "+conv->i2mask(nbit-2)+";                                 \n";
    test += "      logimage_x = 0x1<<"+conv->i2s(nbit-2)+"|logimage_x;   /* Non-Zero bit */\n";
    test += "    }else{                                                                    \n";
    test += "      logimage_x = 0;                                                         \n";
    test += "    }                                                                         \n";

    test += "\n";
    test += "    "+func_name+"(logimage_x, &logimage_z);                     \n";
    test += "\n";

    test += "    {                                                               \n";
    test += "      int logimage_z_nonzero=0;                                     \n";
    test += "      logimage_z_nonzero=0x1&(logimage_z>>"+conv->i2s(nbit-2)+");   \n";
    test += "      int logimage_z_body="+conv->i2mask(nbit-2)+"&logimage_z;      \n";
    test += "      real_z_bylogshift = pow(2.0, ((double)logimage_z_body)/pow(2.0,(double)nbit_man) );\n";
    test += "      if(logimage_z_nonzero ==0){ real_z_bylogshift = 0.0;}             \n";
    test += "    }                                                                   \n";

    test += "#ifdef GNUPLOT\n";
    test += "    if(real_z>0.0){\n";
    test += "      printf(\"%f\\t\",real_x); \n";
    test += "      printf(\"%f\\n\",sqrt(pow((real_z-real_z_bylogshift)/real_z,2.0)) );\n";
    test += "    }\n";
    test += "#else\n";
    test += "    printf(\"(LOG)x=0x%X,(LOG)z=0x%X\\t\",logimage_x,logimage_z); \n";
    test += "    printf(\"(Re)z=%f,(Re')z=%f\\t\",real_z,real_z_bylogshift);\n";
    test += "    printf(\"Error(Abs)%f\\t\",(real_z-real_z_bylogshift));\n";
    test += "    printf(\"Error(Rel)%f\\n\",(real_z-real_z_bylogshift)/real_z);\n";
    test += "#endif\n";

    test += "  }                                                             \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

    test = "\n\n"; // CLEAR and Simple Version.
    test += " /* ===================================================== */\n";
    test += " /* =   This is preliminary version of test driver.     = */\n";
    test += " /* ===================================================== */\n";
    test += "/* nbit   :   " + conv->i2s(nbit) + "-bit      */ \n";
    test += "/* nshift :   " + snshift + "-bit      */         \n";
    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(int logimage_x,int* logimage_z);             \n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "  for(i=0;i<="+conv->i2mask(nbit)+";i++){ \n";
    test += "    int logimage_x=i;                     \n";
    test += "    int logimage_z=0;                     \n";
    test += "    "+func_name+"(logimage_x,&logimage_z);\n";
    test += "    printf(\"(LOG)x=0x%X, (LOG)z=0x%X\\n\",logimage_x,logimage_z);\n";
    test += "  }                                       \n";
    test += "  return 1;\n";
    test += "}\n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c -DGNUPLOT test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";
    shell += "./test_"+func_name+">./test_"+func_name+".log\n";
    shell += "gnuplot ./test_"+func_name+".plot > ./test_"+func_name+".ps\n"; // --- generate psfile by "gnuplot"
    shell += "gv -scale -3 -landscape ./test_"+func_name+".ps\n";             // --- open psfile by "gv"

    shell = "\n\n"; // CLEAR and Simply version.
    shell += " echo ' ===================================================== '\n";
    shell += " echo '      This is preliminary version of test driver.     '\n";
    shell += " echo ' ===================================================== '\n";
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c -DGNUPLOT test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";
    shell += "./test_"+func_name+"|less\n";

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

    gnuplot = "\n\n"; // CLEAR
    gnuplot += "# ====================================== \n";
    gnuplot += "#      SORRY, UNDER CONSTRUCTION         \n";
    gnuplot += "# ====================================== \n";

    gfObj->generate(func_name,s);
    //    gfObj->generate("test_"+func_name,test);            // for debug no problem
    //    gfObj->generate("make_"+func_name,shell,".sh");     // for debug no problem
    //    gfObj->generate("test_"+func_name,gnuplot,".plot"); // for debug no problem

  }




}
