/* --- function list -----------------------------
   void std::generate_c_pg_log_unsigned_add_taylor(...);

   Thu Aug  1 15:27:32 JST 2002

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

  // Get "LNSimage[(Real Z)/(Real X)]" from "(Logimage X) - (Logimage Y)"
  int get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(
						      int logimage_XY,  // (LNSimage)X - (LNSimage)Y
						      int nbit_man)
  {
    double D = ((double)logimage_XY+0.25)/pow(2.0,(double)nbit_man );
    double z_x_double = (log(1.0+pow(2.0,-1.0*D ))/log(2.0)); // (Real)X - (Real)Y
    z_x_double *= pow(2.0,(double)nbit_man );
    return (  (int)(z_x_double+0.5)  );
  }

  // calculate the width of table input
  int get_ladd_table_input_width(
			   int nbit_log,
			   int nbit_man)
  {
    int table_width=0;
    int i=0;
    int index=0;
    int lastflag=0;

    int imax = (int)pow(2.0,(double)(nbit_log-1));
    while(lastflag==0||i==imax){
      int LNS_z_x = get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(i,nbit_man);
      if(LNS_z_x == 0){
	index = i;
	lastflag=1;
      }
      i++;
    }

    table_width = 1 + priority_encoder(index);

    return table_width;
  }

  // calculate the width of table output
  int get_ladd_table_output_width(int nbit_man)
  {
    int LNS_z_x = get_LNSimage_Z_div_X_from_LNSimage_X_sub_LNSimage_Y(0x0,nbit_man);
    return (  1 + priority_encoder(LNS_z_x)  );
  }

}

namespace std{

  void generate_c_pg_log_unsigned_add(vector<string> args,Inter& inter)
  {
    Convert* conv;

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    string sz = args[2];
    int nbit_log = atoi(args[3].c_str());
    int nbit_man = atoi(args[4].c_str());
    int nstage   = atoi(args[5].c_str());

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_log_unsigned_add";
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);
    inter.push_function(func_name);
    {
      string function_arg = sx + "," + sy +", &"+sz;
      inter.push_function_arg(func_name+"("+function_arg+")");
    }

    // +--------------------------------+
    // | Function Prototype Declaration |
    // +--------------------------------+
    inter.push_function_prototype(func_name+"(int logx, int logy, int* logz)");

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "/* logarithmic UNSIGNED adder */                                   \n";
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */          \n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */          \n";

    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is ";
    s += conv->i2s(nbit_log-1) + "-bit.      */\n\n";

    s += "#include<stdio.h>                                                  \n";
    s += "#include<math.h>                                                   \n";
    s += "void " + func_name + "(int logimage_x, int logimage_y, int* logimage_z){\n";
    s += "  int signz=0;  /* 1-bit */                                        \n";
    s += "  int x1=0;     /* "+conv->i2s(nbit_log)+"-bit */                  \n";
    s += "  int y1=0;     /* "+conv->i2s(nbit_log)+"-bit */                  \n";
    s += "  int xy=0;     /* "+conv->i2s(nbit_log)+"-bit (x1 - y1) */        \n";
    s += "  int yx=0;     /* "+conv->i2s(nbit_log)+"-bit (y1 - x1) */        \n";
    s += "  int yx_msb=0; /*  1-bit SIGN of yx */                            \n";
    s += "  int x2=0; /* "+conv->i2s(nbit_log-1)+"-bit output of Left-MUX  */\n";
    s += "  int d0=0; /* "+conv->i2s(nbit_log-1)+"-bit output of Right-MUX */\n";
    s += "  int d0_low_part=0;    /* TABLE INPUT */                          \n";
    s += "  int d0_high_part=0;   /* MUX Middle INPUT */                     \n";
    s += "  int df=0;             /* MUX Middle OUTPUT */                    \n";
    s += "  int table_data=0;     /* TABLE OUTPUT */                         \n";
    s += "  int logimage_ZperX=0; /* Logimage of (Real)Z/X */                \n";
    s += "\n";

    s += "  /* SIGN EVALUATION                      \n";
    s += "          ----------------------------    \n";
    s += "          nonz(X) | nonz(Y) -> sign(Z)    \n";
    s += "          --------+-------------------    \n";
    s += "             0    |   0     ->   0        \n";
    s += "             0    |   1     -> sign(Y)    \n";
    s += "             1    |   0     -> sign(X)    \n";
    s += "             1    |   1     -> sign(X)    \n";
    s += "          ----------------------------  */\n";
    s += "  {\n";
    s += "    int nonzx,nonzy,signx,signy;\n";
    s += "    signx = (logimage_x>>"+conv->i2s(nbit_log-1)+")&0x1;\n";
    s += "    signy = (logimage_y>>"+conv->i2s(nbit_log-1)+")&0x1;\n";
    s += "    nonzx = (logimage_x>>"+conv->i2s(nbit_log-2)+")&0x1;\n";
    s += "    nonzy = (logimage_y>>"+conv->i2s(nbit_log-2)+")&0x1;\n";
    s += "    if(      (nonzx==0x0)&&(nonzy=0x0)){ signz = 0x0;   }\n";
    s += "    else if((nonzx==0x0)&&(nonzy=0x1)){  signz = signy; }\n";
    s += "    else if((nonzx==0x1)&&(nonzy=0x0)){  signz = signx; }\n";
    s += "    else{ /*(nonzx==0x1)&&(nonzy=0x1) */ signz = signx; }\n";
    s += "  }\n";
    s += "\n";

    s += "  /* RESET FUNCTION */ \n";
    s += "  {\n";
    s += "    int nonzx;\n";
    s += "    int nonzy;\n";
    s += "    nonzx = 0x1&(logimage_x>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    nonzy = 0x1&(logimage_y>>"+conv->i2s(nbit_log-2)+");\n";
    s += "    if(nonzx == 0x0) logimage_x &= 0x0;\n";
    s += "    if(nonzy == 0x0) logimage_y &= 0x0;\n";
    s += "  }\n";

    s += "\n";
    s += "  x1 = "+conv->i2mask(nbit_log-1)+"&logimage_x; /* The MSB("+conv->i2s(nbit_log)+"th-bit) is zero. */\n";
    s += "  y1 = "+conv->i2mask(nbit_log-1)+"&logimage_y; /* The MSB("+conv->i2s(nbit_log)+"th-bit) is zero. */\n";

    s += "\n";
    s += "  /* SUB (Y-X),(X-Y) */                                            \n";
    s += "  yx = "+conv->i2mask(nbit_log)+"&(y1 - x1);                       \n";
    s += "  xy = "+conv->i2mask(nbit_log)+"&(x1 - y1);                       \n";

    s += "\n";
    s += "  yx_msb = 0x1 & (yx >>"+conv->i2s(nbit_log-1)+");                 \n";

    s += "\n";
    s += "  /* MUX Left */\n";
    s += "  if(yx_msb==0x1){x2="+conv->i2mask(nbit_log-1)+"&logimage_x;}else{x2="+conv->i2mask(nbit_log-1)+"&logimage_y;}\n";

    s += "\n";
    s += "  /* MUX Right */\n";
    s += "  if(yx_msb==0x1){d0="+conv->i2mask(nbit_log-1)+"&xy;}else{d0="+conv->i2mask(nbit_log-1)+"&yx;}\n";

    s += "\n";
    int nbit_table_in  = get_ladd_table_input_width(nbit_log,nbit_man); // Width of Table Input
    int nbit_table_out = get_ladd_table_output_width(nbit_man);         // Width of Table Output
    s += "  /* ===========================\n";
    s += "     table input  = "+conv->i2s(nbit_table_in)+" bits                   \n";
    s += "     table output = "+conv->i2s(nbit_table_out)+" bits                  \n";
    s += "     =========================== */\n";
    s += "  d0_low_part  = "+conv->i2mask(nbit_table_in)+"&d0;      /* "+conv->i2s(nbit_table_in)+"-bit */\n";
    if(nbit_log-1-nbit_table_in <= 0){
      cerr << "\n\nPGPG error at generation of "<<func_name<< ".c:\n\t d0_high_part becomes under 0-bit.\n\n";
      exit(1);
    }
    s += "  d0_high_part = "+conv->i2mask(nbit_log-1-nbit_table_in)+"&(d0>>"+conv->i2s(nbit_table_in)+"); \n";

    s += "\n";
    s += "  /* MUX Middle */\n";
    s += "  if(d0_high_part == 0x0){ df=0x1; }else{ df=0x0; }                \n";

    s += "\n";
    s += "  /* TABLE LOG_ADD */\n";
    s += "  {                                                                                     \n";
    s += "    int table_adr = 0;                                                                  \n";
    s += "    double D = 0.0;                                                                     \n";
    s += "    double z_x = 0.0;                                                                   \n";
    s += "    table_adr = d0_low_part; /* (logimage X) - (logimage Y) */                          \n";
    s += "    D = ((double)table_adr+0.25)/"+conv->i2ds(nbit_man)+";                              \n";
    s += "    z_x = (log(1.0+pow(2.0,-1.0*D ))/log(2.0));   /* logimage[(Real)Z/X] */             \n";
    s += "    z_x *= "+conv->i2ds(nbit_man)+";                                                    \n";
    s += "    table_data = (int)(z_x+0.5);                                                        \n";
    s += "    table_data &= "+conv->i2mask(nbit_table_out)+";  ";
    s += "/* TABLE OUTPUT : "+conv->i2s(nbit_table_out)+"-bit */\n";
    s += "  }                                                                                     \n";

    s += "\n";
    s += "  /* MUX Last */\n";
    s += "  if(df == 0x1){ logimage_ZperX = table_data; }else{ logimage_ZperX = 0;}\n";

    s += "\n";
    s += "  /* ADDER :"+conv->i2s(nbit_log-1)+"-bit width */ \n";
    s += "  {                                                \n";
    s += "    int add_out=0;                                 \n";
    s += "    add_out = x2 + logimage_ZperX;                 \n";
    s += "    add_out &= "+conv->i2mask(nbit_log-1)+";       \n";
    s += "                   /* WITH SIGN-BIT (2002/07/26) */\n";
    s += "    *logimage_z = (signz<<"+conv->i2s(nbit_log-1)+") | add_out;\n";
    s += "  }                                                \n";


    s += "\n";
    s += "  return;  \n";
    s += "}          \n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */       \n";
    test += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */       \n";

    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(int logimage_x,int logimage_y,int* logimage_z);\n";
    test += "int main(){                                                     \n";
    test += "  int i;                                                        \n";
    test += "\n\n";
    test += "#ifndef GNUPLOT\n";
    test += "  printf(\"nbit_log = "+ conv->i2s(nbit_log) +"\\n\");          \n";
    test += "  printf(\"nbit_man = "+ conv->i2s(nbit_man) +"\\n\");          \n";
    test += "#endif\n\n";
    test += "  for(i=0;i<="+conv->i2mask(/* must be nbit_fix */ 19)+";i+=32){\n";
    test += "    int logimage_x=0;                                           \n";
    test += "    int logimage_y=0;                                           \n";
    test += "    int logimage_z=0;                                           \n";
    test += "    double real_x =(double)i;                                   \n";
    test += "    double real_y =(double)i;                                   \n";
    test += "    double real_z = 0.0;                                        \n";
    test += "    double real_z_byladd = 0.0;                                 \n";
    test += "\n\n";
    test += "    real_z = real_x + real_y;                                   \n";
    test += "    logimage_x = (int)("+conv->i2ds(nbit_man)+"*log(real_x)/log(2.0)+0.5); \n";
    test += "    logimage_x &= "+conv->i2mask(nbit_log-2)+";                 \n";
    test += "    if(i>0){  /* Non-Zero bit addition */                       \n";
    test += "      logimage_x = 0x1<<"+conv->i2s(nbit_log-2)+"|logimage_x;   \n";
    test += "    }                                                           \n";
    test += "    logimage_y = logimage_x;                                    \n";

    test += "    "+func_name+"(logimage_x,logimage_y,&logimage_z);           \n";
    test += "    {                                                                   \n";
    test += "      int logimage_z_nonzero=0;                                         \n";
    test += "      int logimage_z_body="+conv->i2mask(nbit_log-2)+"&logimage_z;      \n";
    test += "      logimage_z_nonzero=0x1&(logimage_z>>"+conv->i2s(nbit_log-2)+");   \n";
    test += "      real_z_byladd= pow(2.0, logimage_z_body/"+conv->i2ds(nbit_man)+");\n";
    test += "      if(logimage_z_nonzero ==0){ real_z_byladd = 0.0;}                 \n";
    test += "    }                                                                   \n";

    test += "#ifdef GNUPLOT\n";
    test += "    if(real_z>0.0){\n";
    test += "      printf(\"%f\\t\",real_x); \n";
    test += "      printf(\"%f\\n\",sqrt(pow((real_z-real_z_byladd)/real_z,2.0)) );\n";
    test += "    }\n";
    test += "#else\n";
    test += "    printf(\"(LOG)x=0x%X,(LOG)y=0x%X,(LOG)z=0x%X\\t\",logimage_x,logimage_y,logimage_z); \n";
    test += "    printf(\"(Re)z=%f,(Re')z=%f\\t\",real_z,real_z_byladd);\n";
    test += "    printf(\"Error(Abs)%f\\t\",(real_z-real_z_byladd));\n";
    test += "    printf(\"Error(Rel)%f\\n\",(real_z-real_z_byladd)/real_z);\n";
    test += "#endif\n";

    test += "  }                                                             \n";
    test += "  return 1;\n                                                   \n";
    test += "}                                                               \n";

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

    GenFile *file;
    file = new GenFile();
    file->generate(func_name,s);
    //    file->generate("test_"+func_name,test);            // for debug no problem
    //    file->generate("make_"+func_name,shell,".sh");     // for debug no problem
    //    file->generate("test_"+func_name,gnuplot,".plot"); // for debug no problem

  }




}
