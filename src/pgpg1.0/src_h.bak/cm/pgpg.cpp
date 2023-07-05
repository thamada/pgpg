#include<iostream>
#include<string>
#include<cstdio>
#include<cstring>
//#include "pgpg.h"       // namespace PGPG
#include "pgpgc.h"      // namespace PGPG_PARSER
#include "pgpgc_help.h" // namespace HELP, REVISION

using namespace std;

int main(int argc,char *argv[])
{

  // GET FILENAME OF LIST
  string listfilename;
  string basedir;
  HELP::option* opt;
  opt = new HELP::option;
  opt->parse(argc,argv);

  listfilename = opt->get_listfilename();
  basedir = opt->get_basedir();

  GenFile *gfObj; // this object contains the "basedir" and generates C source files.
  gfObj = new GenFile(basedir);
  gfObj->set_revision_number(REVISION::SOFTWARE_REVISION); 
  gfObj->set_revision_last_modified(REVISION::LAST_MODIFIED);

  // ==========================================================
  // ======== PGPG SOFTWARE SIMULATOR GENERATING PART =========
  // ==========================================================
  {
    PGPG_PARSER::ListRead *cursor = new PGPG_PARSER::ListRead(listfilename);
    do {
      //      cout <<"[" << cursor->getIndex() << "]"<< cursor->getCurrentLine();
      if(cursor->isFind("pg_massoffset")) generate_c_pg_massoffset(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_FLP_to_FXP")) generate_c_pg_conv_FLP_to_FXP(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_FLP_to_LNS")) generate_c_pg_conv_FLP_to_LNS(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_fix_addsub")) generate_c_pg_fix_addsub(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_log_muldiv")) generate_c_pg_log_muldiv(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_log_shift"))  generate_c_pg_log_shift(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_pdelay"))     generate_c_pg_pdelay(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_fix_accum"))  generate_c_pg_fix_accum(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_log_add"))  generate_c_pg_log_add(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_log_unsigned_add")) generate_c_pg_log_unsigned_add(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_ftol")) generate_c_pg_conv_ftol(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_ltof")) generate_c_pg_conv_ltof(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_log_unsigned_add_itp")) generate_c_pg_log_unsigned_add_itp(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_ftol_itp")) generate_c_pg_conv_ftol_itp(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_ltof_itp")) generate_c_pg_conv_ltof_itp(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_fixtofloat")) generate_c_pg_conv_fixtofloat(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_conv_floattofix")) generate_c_pg_conv_floattofix(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_float_mult")) generate_c_pg_float_mult(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_float_unsigned_add")) generate_c_pg_float_unsigned_add(cursor->getArgs(),gfObj);

      if(cursor->isFind("pg_bits_and")) generate_c_pg_bits_and(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_or")) generate_c_pg_bits_or(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_xor")) generate_c_pg_bits_xor(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_inv")) generate_c_pg_bits_inv(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_shift")) generate_c_pg_bits_shift(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_rotate")) generate_c_pg_bits_rotate(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_join")) generate_c_pg_bits_join(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_part")) generate_c_pg_bits_part(cursor->getArgs(),gfObj);
      if(cursor->isFind("pg_bits_delay")) generate_c_pg_bits_delay(cursor->getArgs(),gfObj);

      //      if(cursor->isFind("pg_function"))  generate_c_pg_function(cursor->getArgs(),gfObj);
    }while(cursor->nextLine());
  } // --- PGPG SOFTWARE SIMULATOR PART END ---

}

