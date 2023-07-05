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

  void generate_c_pg_massoffset(vector<string> args, GenFile* gfObj)
  {
    Convert* conv = new Convert();

    // +------------------------------+
    // | Parse C-subroutine arguments |
    // +------------------------------+
    string sx = args[0];
    string sy = args[1];
    string sz = args[2];
    int nbit_fix = atoi(args[3].c_str());
    int nbit_log = atoi(args[4].c_str());
    int nbit_man = atoi(args[5].c_str());

    // +------------------------------+
    // | Function Name Generation     |
    // +------------------------------+
    string func_name;
    func_name = "pg_massoffset";
    func_name += "_fix" + conv->i2s(nbit_fix);
    func_name += "_log" + conv->i2s(nbit_log);
    func_name += "_man" + conv->i2s(nbit_man);

    //    cerr << "\tpgpgc_module generate  void ";
    //    cerr << func_name << "(double eps, int* eps2 , int* massoffset)" <<endl;

    // +------------------------------+
    // | Simulator Part               |
    // +------------------------------+
    string s;
    s += "#include<stdio.h>                                                  \n";
    s += "#include<math.h>                                                   \n";

    // ********************************************************************
    // * SHIGNED FIXED TO SIGNED LOGARITHMIC CONVERTER (LOCAL SUBROUTINE) *
    // ********************************************************************
    s += "/*******************************************************/          \n";
    s += "/* SHIGNED FIXED TO SIGNED LOGARITHMIC CONVERTER       */          \n";
    s += "/* nbit_fix   :   " + conv->i2s(nbit_fix) + "-bit      */          \n";
    s += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */          \n";
    s += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */          \n";
    s += "/*******************************************************/          \n";
    s += "static void local_pg_conv_ftol(int fixdata, int* logdata){         \n";
    s += "  int fixdata_msb=0;                                               \n";
    s += "  int logdata_sign=0;                                              \n";
    s += "  int fixdata_body=0;                                              \n";
    s += "  int abs=0;                                                       \n";
    s += "  int abs_decimal=0;                                               \n";
    s += "  int logdata_nonzero=0;                                           \n";
    s += "  int penc_out = 0; /* Output of penc */                           \n"; 
    s += "  int table_adr = 0;                                               \n"; 
    s += "  int table_overflow = 0;                                          \n"; 
    s += "  int logdata_mantissa=0;                                          \n";
    s += "  int logdata_exponent=0;                                          \n";

    s += "\n";
    s += "  /* SIGN BIT */                                                   \n";
    s += "  fixdata_msb = 0x1&(fixdata >>"+conv->i2s(nbit_fix-1)+");         \n";
    s += "  logdata_sign = fixdata_msb;                                      \n";

    s += "\n";
    s += "  /* ABSOLUTE */                                                   \n";
    s += "  fixdata_body    = "+conv->i2mask(nbit_fix-1)+" & fixdata;        \n";
    s += "  {                                                                \n";
    s += "    int inv_fixdata_body=0;                                        \n";
    s += "    inv_fixdata_body = "+conv->i2mask(nbit_fix-1)+" ^ fixdata_body;\n";
    s += "    if(fixdata_msb == 0x1){                                        \n";
    s += "      abs = "+conv->i2mask(nbit_fix-1)+" & (inv_fixdata_body + 1); \n";
    s += "    }else{                                                         \n";
    s += "      abs = fixdata_body;                                          \n";
    s += "    }                                                              \n";
    s += "  }                                                                \n";
    s += "  abs_decimal = "+conv->i2mask(nbit_fix-2)+"& abs;                 \n";

    s += "\n";
    s += "  /* GENELATE NON-ZERO BIT (ALL BIT OR) */                                 \n";
    s += "  if(abs != 0x0){ logdata_nonzero = 0x1; }else{ logdata_nonzero=0x0; }     \n";


    int penc_outwidth=0;
    if((nbit_fix <= 32)&&(nbit_fix > 16))      penc_outwidth=5;
    else if((nbit_fix <= 16)&&(nbit_fix > 8))  penc_outwidth=4;
    else if((nbit_fix <= 8)&&(nbit_fix > 4))   penc_outwidth=3;
    else if((nbit_fix <= 4)&&(nbit_fix > 2))   penc_outwidth=2;
    else if((nbit_fix <= 2)&&(nbit_fix > 1))   penc_outwidth=1;
    else{ cerr << "nbit_fix is out of range.\n"; exit(1); }
    s += "\n";
    s += "  { /* PRIORITY ENCODER */                                         \n";
    s += "    int i;                                                         \n";
    s += "    int count=0;                                                   \n";
    s += "    for(i=31;i >=0;i--){                                           \n";
    s += "        int buf;                                                   \n";
    s += "        buf = 0x1 & (abs >>i);                                     \n";
    s += "        if(buf == 0x1){ count = i; break;}                         \n";
    s += "        count = i;                                                 \n";
    s += "    }                                                              \n";
    s += "    penc_out=count;                                                \n";
    s += "  }                                                                \n";
    s += "  penc_out = "+conv->i2mask(penc_outwidth)+" & penc_out; /* "+conv->i2s(penc_outwidth)+"-bit */\n";
   
    s += "  \n";
    s += "  /* SHIFTER */                                                                      \n";
    s += "  {                                                                                  \n";
    s += "    int seed=0;                                                                      \n";
    s += "    if(penc_out>="+conv->i2s(nbit_man+2)+"){                                         \n";
    s += "      seed = abs_decimal;                                                            \n";
    s += "      table_adr = "+conv->i2mask(nbit_man+2)+" & (seed>>(penc_out-"+conv->i2s(nbit_man+2)+"));\n";
    s += "    }else{                                                                           \n";
    s += "      seed = abs_decimal << "+conv->i2s(nbit_man+2)+";                               \n";
    s += "      table_adr = "+conv->i2mask(nbit_man+2)+" & (seed>>penc_out);                         \n";
    s += "    }                                                                                \n";
    s += "  }                                                                                  \n";

    s += "  \n";
    s += "  /* TABLE */                                                                        \n";
    s += "  /* TABLE INPUT WIDTH  = "+conv->i2s(nbit_man+2)+"-bit */                           \n";
    s += "  /* TABLE OUTPUT WIDTH = "+conv->i2s(nbit_man)+"-bit */                             \n";
    s += "  {                                                                                  \n";
    s += "    int adr=0;                                                                       \n";
    s += "    double adr_double=0.0;                                                           \n";
    s += "    double data_double=0.0;                                                          \n";
    s += "    int data;                                                                        \n";
    if(nbit_man+2 > 31){cerr << "Error : table adr width at conv_ftol is out of range."; exit(1);}
    s += "    adr = "+conv->i2mask(nbit_man+2)+" & table_adr;                                  \n";
    s += "    adr_double =   ( ((double)adr) + 0.5 )/" +conv->i2ds(nbit_man+2)+ ";   \n";

    s += "    data_double = "+conv->i2ds(nbit_man)+"*(log(1.0 + adr_double))/log(2.0) + 0.5;\n";
    s += "    data = (int)data_double;                                                         \n";
    s += "    /* CHECK OVERFLOW */                                                             \n";
    s += "    if(0x1&(data >>"+conv->i2s(nbit_man)+")==0x1){                                   \n";
    s += "      data = 0;                                                                      \n";
    s += "      table_overflow = 1;   /* overflow flag */                                      \n";
    s += "    }else{                                                                           \n";
    s += "      table_overflow = 0;                                                            \n";
    s += "    }                                                                                \n";
    s += "    logdata_mantissa = "+conv->i2mask(nbit_man)+" & data;                            \n";
    s += "  }                                                                                  \n";

    s += "  \n";
    s += "  /* ADDER (GENERATE EXPONENT PART) */                                               \n";
    s += "  logdata_exponent = "+conv->i2mask(penc_outwidth)+" & (penc_out + table_overflow);  \n";

    s += "  \n";
    s += "  *logdata = logdata_sign << "+conv->i2s(nbit_log-1);
    s +=            "| logdata_nonzero << "+conv->i2s(nbit_log-2);
    s +=            "| logdata_exponent << "+conv->i2s(nbit_man);
    s +=            "| logdata_mantissa ;\n";
    s += "  \n";
    s += "  return;                                                          \n";
    s += "}                                                                \n\n";

    // **************************************************************
    // * SHIGNED LOGARITHMIC 1-BIT RIGHT SHIFTER (LOCAL SUBROUTINE) *
    // **************************************************************
    s += "/*******************************************************/\n";
    s += "/* LOGARITHMIC 1-bit RIGHT SHIFTER                     */\n";
    s += "/* UNSIGNED LOGARITHMIC SHIFTER( POWER 2)              */\n";
    s += "/*******************************************************/\n";
    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is different !\n";
    s += "   Input Width : "+conv->i2s(nbit_log)+"-bit. ["+conv->i2s(nbit_log)+"-bit Signed Logarithmic Format] \n";
    s += "   Onput Width : "+conv->i2s(nbit_log-1)+"-bit. ["+conv->i2s(nbit_log)+"-bit Unsigned Logarithmic Format without a sign bit.] */\n\n";
    s += "static void local_pg_log_shift_1(int logimage_x, int* logimage_z){\n";
    s += "  int x_sign=0;        /* Sign Flag:  1-bit (for debug)*/\n";
    s += "  int x_nonzero=0;     /* NonZero Flag: 1-bit */         \n";
    s += "  int x_body=0;        /* "+conv->i2s(nbit_log-2)+"-bit */\n";
    s += "  int z=0;             /* "+conv->i2s(nbit_log-1)+"-bit */\n";
    s += "\n";
    s += "  x_sign    = 0x1&(logimage_x>>"+conv->i2s(nbit_log-1)+");  /* for debug */\n";
    s += "  x_nonzero = 0x1&(logimage_x>>"+conv->i2s(nbit_log-2)+");\n";
    s += "  x_body    = "+conv->i2mask(nbit_log-2)+"&logimage_x;    \n";
    s += "\n";
    s += "  z = (x_nonzero <<"+conv->i2s(nbit_log-2)+")|("+conv->i2mask(nbit_log-2)+"&(x_body<<1));\n";
    s += "  z &= "+conv->i2mask(nbit_log-1)+";\n";
    s += "  *logimage_z = z;\n";
    s += "\n";
    s += "  return;\n";
    s += "}\n\n";

    // **************************************************************
    // * SHIGNED LOGARITHMIC 1-BIT LEFT SHIFTER (LOCAL SUBROUTINE)  *
    // **************************************************************
    s += "/*******************************************************/\n";
    s += "/* LOGARITHMIC 1-bit LEFT SHIFTER                      */\n";
    s += "/* UNSIGNED LOGARITHMIC SHIFTER( POWER 1/2)            */\n";
    s += "/*******************************************************/\n";
    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is different !\n";
    s += "   Input Width : "+conv->i2s(nbit_log)+"-bit. ["+conv->i2s(nbit_log)+"-bit Signed Logarithmic Format] \n";
    s += "   Onput Width : "+conv->i2s(nbit_log-1)+"-bit. ["+conv->i2s(nbit_log)+"-bit Unsigned Logarithmic Format without a sign bit.] */\n\n";
    s += "static void local_pg_log_shift_m1(int logimage_x, int* logimage_z){\n";
    s += "  int x_sign=0;        /* Sign Flag:  1-bit (for debug)*/\n";
    s += "  int x_nonzero=0;     /* NonZero Flag: 1-bit */         \n";
    s += "  int x_body=0;        /* "+conv->i2s(nbit_log-2)+"-bit */\n";
    s += "  int z=0;             /* "+conv->i2s(nbit_log-1)+"-bit */\n";
    s += "\n";
    s += "  x_sign    = 0x1&(logimage_x>>"+conv->i2s(nbit_log-1)+");  /* for debug */\n";
    s += "  x_nonzero = 0x1&(logimage_x>>"+conv->i2s(nbit_log-2)+");\n";
    s += "  x_body    = "+conv->i2mask(nbit_log-2)+"&logimage_x;    \n";
    s += "\n";
    s += "  z = (x_nonzero <<"+conv->i2s(nbit_log-2)+")|("+conv->i2mask(nbit_log-2)+"&(x_body>>1));\n";
    s += "  z &= "+conv->i2mask(nbit_log-1)+";\n";
    s += "  *logimage_z = z;\n";
    s += "\n";
    s += "  return;\n";
    s += "}\n\n";


    // ****************************************************
    // *   UNSIGHED LOGARITHMIC ADDER (LOCAL SUBROUTINE)  *
    // ****************************************************
    s += "/*******************************************************/\n";
    s += "/* UNSIGNED LOGARITHMIC ADDER                          */\n";
    s += "/*******************************************************/\n";
    s += "/* --- [NOTE!] ---\n";
    s += "   Input/Output width of this component is ";
    s += conv->i2s(nbit_log-1) + "-bit.      */\n\n";
    s += "static void local_pg_log_add(int logimage_x, int logimage_y, int* logimage_z){\n";
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
    s += "    table_adr = d0_low_part; /* (logimage X) - (logimage Y) */                          \n";
    s += "    double D = ((double)table_adr+0.25)/"+conv->i2ds(nbit_man)+";                       \n";
    s += "    double z_x = (log(1.0+pow(2.0,-1.0*D ))/log(2.0));   /* logimage[(Real)Z/X] */      \n";
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
    s += "    *logimage_z = add_out;                         \n";
    s += "  }                                                \n";

    s += "\n";
    s += "  return;  \n";
    s += "}        \n\n";

    // *********************************************************
    // *   UNSIGHED LOGARITHMIC MULTIPLIER (LOCAL SUBROUTINE)  *
    // *********************************************************
    string muldivflag = "MUL";
    s += "/*******************************************************/\n";
    s += "/* UNSIGNED LOGARITHMIC ADDER                          */\n";
    s += "/*******************************************************/\n";
    s += "/* nbit_log:   " + conv->i2s(nbit_log) + "-bit          */         \n";
    s += "/* MULDIV  :   " + muldivflag      + "                  */         \n";
    s += "static void local_pg_log_mul(int x,int y,int* z){                  \n";
    s += "  int _signbit_x=0;                                                \n";
    s += "  int _signbit_y=0;                                                \n";
    s += "  int _signbit_z=0;                                                \n";
    s += "  int _nonzbit_x=0;                                                \n";
    s += "  int _nonzbit_y=0;                                                \n";
    s += "  int _nonzbit_z=0;                                                \n";
    s += "  int _x=0;                                                        \n";
    s += "  int _y=0;                                                        \n";
    s += "  int _z=0;                                                        \n";
    s += "  _signbit_x = 0x1&(x >> " + conv->i2s(nbit_log-1) + ");           \n";
    s += "  _signbit_y = 0x1&(y >> " + conv->i2s(nbit_log-1) + ");           \n";
    s += "  _signbit_z = _signbit_x ^ _signbit_y;                            \n";
    s += "  _nonzbit_x = 0x1&(x >> " + conv->i2s(nbit_log-2) + ");           \n";
    s += "  _nonzbit_y = 0x1&(y >> " + conv->i2s(nbit_log-2) + ");           \n";
    s += "  _nonzbit_z = _nonzbit_x & _nonzbit_y;                            \n";
    s += "  _x = "+ conv->i2mask(nbit_log-2) + "&x;                          \n";
    s += "  _y = "+ conv->i2mask(nbit_log-2) + "&y;                          \n";
    if(muldivflag == "MUL"){
      s += "  _z = _x + _y;                                                  \n";
      s += "  _z = "+ conv->i2mask(nbit_log-2) + "&_z;                       \n";
    }else{
      s += "  _z = _x - _y;                                                  \n";
      s += "  _z = "+ conv->i2mask(nbit_log-2) + "&_z;                       \n";
    }
    s += "  *z = _signbit_z << " + conv->i2s(nbit_log-1)
         + " | _nonzbit_z << "   + conv->i2s(nbit_log-2)
         + " | _z;                                                           \n";
    s += "  *z = "+conv->i2mask(nbit_log) + "&(*z);                          \n";
    s += "  return;                                                          \n";
    s += "}                                                                 \n\n";

    // **************************************************************
    // * FLOATING-POINT TO FIXED-POINT CONVERTER (LOCAL SUBROUTINE) *
    // **************************************************************
    s += "/**************************************************************/\n";
    s += "/* FLOATING-POINT TO FIXED-POINT CONVERTER (LOCAL SUBROUTINE) */\n";
    s += "/**************************************************************/\n";
    s += "static void local_pg_conv_FLP_to_FXP(double flp, int* fxp){\n";
    s += "  double dx=0.0;                             \n";
    s += "  int ix=0;                                  \n";
    s += "\n";
    s += "  dx = "+conv->i2ds(nbit_fix-1)+" * flp;";
    s += " /*  [dx = pow(2.0,"+conv->i2s(nbit_fix-1)+".0)*flp]  */\n";
    s += "  ix = (int)dx;                           \n";
    s += "  ix &= "+conv->i2mask(nbit_fix)+";       \n";
    s += "  *fxp = ix;                              \n";
    s += "\n";
    s += "  return;                                   \n";
    s += "}                                         \n\n";

    // *********************************************************
    // *   MASSOFFSET (PUBLIC SUBROUTINE)                      *
    // *********************************************************
    s += "/*******************************************************/\n";
    s += "/* MASSOFFSET (PUBLIC SUBROUTINE)                      */\n";
    s += "/*******************************************************/\n";
    s += "/* nbit_fix  :   " + conv->i2s(nbit_fix) + "-bit       */\n";
    s += "/* nbit_log  :   " + conv->i2s(nbit_log) + "-bit       */\n";
    s += "/* nbit_man  :   " + conv->i2s(nbit_man) + "-bit       */\n";
    s += "void "+func_name+"(double eps, int* eps2, int* massoffset){\n";
    s += "  int fix_x_max="+conv->i2mask(nbit_fix-1)+";           \n";
    s += "  int x      = 0x0;   /* logarithmic x           */     \n";
    s += "  int e_fix  = 0x0;   /* eps                     */     \n";
    s += "  int e      = 0x0;   /* logarithmic e           */     \n";
    s += "  int e2     = 0x0;   /* eps2                    */     \n";
    s += "  int x2     = 0x0;   /* x^2                     */     \n";
    s += "  int x2x2   = 0x0;   /* x^2 + x^2               */     \n";
    s += "  int x2eps2 = 0x0;   /* x^2 + eps^2             */     \n";
    s += "  int r2     = 0x0;   /* (x^2+x^2) + (x^2+eps^2) */     \n";
    s += "  int r      = 0x0;   /* root r2                 */     \n";
    s += "  int r3     = 0x0;   /* r2 * r                  */     \n";
    s += "\n";
    s += "  local_pg_conv_FLP_to_FXP(eps,&e_fix);\n";
    s += "  local_pg_conv_ftol(e_fix,&e);\n";
    s += "  local_pg_log_shift_1(e,&e2);\n";
    s += "  *eps2 = e2;\n";

    s += "  local_pg_conv_ftol(fix_x_max,&x);\n";
    s += "  local_pg_log_shift_1(x,&x2);\n";
    s += "  local_pg_log_add(x2,x2,&x2x2);\n";
    s += "  local_pg_log_add(x2,e2,&x2eps2);\n";
    s += "  local_pg_log_add(x2x2,x2eps2,&r2);\n";
    s += "  local_pg_log_shift_m1(r2,&r);\n";
    s += "  local_pg_log_mul(r2,r,&r3);\n";
    s += "  *massoffset = "+conv->i2mask(nbit_log)+"&r3;\n";
    s += "  return; \n";
    s += "}\n";

    // +------------------------------+
    // | Test Driver Part             |
    // +------------------------------+
    string test;
    test += "/* nbit_fix   :   " + conv->i2s(nbit_fix) + "-bit      */       \n";
    test += "/* nbit_log   :   " + conv->i2s(nbit_log) + "-bit      */       \n";
    test += "/* nbit_man   :   " + conv->i2s(nbit_man) + "-bit      */       \n";
    test += "#include<stdio.h>                                               \n";
    test += "#include<math.h>                                                \n";
    test += "void "+func_name+"(double eps, int* eps2 , int* massoffset);    \n";
    test += "int main(){                                                     \n";
    test += "  double eps=0.999999;                                          \n";
    test += "  int eps2=0;                                                   \n";
    test += "  int massoffset=0;                                             \n";
    test += "  "+func_name+"(eps,&eps2,&massoffset);                         \n";
    test += "  printf(\"[double]eps  = %e\\n\",eps);                                 \n";
    test += "  printf(\"[ log  ]eps2 = 0x%X\\n\",eps2);                              \n";
    test += "  printf(\"massoffset = 0x%X\\n\",massoffset);                  \n";
    test += "}                                                               \n";

    // test driver compile shell script
    string shell;
    shell += "#!/bin/sh               \n";
    shell += "rm -rf test_"+func_name+".o "+func_name+".o test_"+func_name+"\n";
    shell += "gcc -c -DGNUPLOT test_"+func_name+".c\n";
    shell += "gcc -c "+func_name+".c\n";
    shell += "gcc test_"+func_name+".o "+func_name+".o -o test_"+func_name+" -lm\n";

    // gnuplot script
    string gnuplot;

    gfObj->generate(func_name,s);
    gfObj->generate("test_"+func_name,test);
    gfObj->generate("make_"+func_name,shell,".sh");
    //    gfObj->generate("test_"+func_name,gnuplot,".plot");

  }

}
