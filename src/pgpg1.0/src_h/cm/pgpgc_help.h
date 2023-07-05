#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cmath>

namespace REVISION{
  std::string SOFTWARE_REVISION="0.55";
  std::string SOFTWARE_NAME="pgpgc_module";
  std::string EMAIL_HAMADA="hamada@provence.c.u-tokyo.ac.jp";
  std::string LAST_MODIFIED="0.55: Mon Dec 24 17:090:00 JST 2003 (output all sources to stdout)";
//  std::string LAST_MODIFIED="0.54: Fri Sep 12 17:090:00 JST 2003 (bugfix pg_fix_addsub)";
//  std::string LAST_MODIFIED="0.53: Fri Sep 12 13:00:00 JST 2003 (bugfix pg_fix_addsub,pg_log_unsigned_add)";
//  std::string LAST_MODIFIED="0.52: Tue Sep 11 19:57:58 JST 2003 (add SDIV in pg_log_muldiv )";
//  std::string LAST_MODIFIED="0.51: Tue Jun 19 22:53:58 JST 2003 (bug fixed at conv_ltof_itp shifter same as 2003/4/23)";
//  std::string LAST_MODIFIED="0.50: Tue Jun 19 21:26:00 JST 2003 (change conv_ftol_itp of shifter to no-rounding version)";
//  std::string LAST_MODIFIED="0.49beta3: Wed Jun 16 12:00:00 JST 2003 (int->long long int :pg_fix_addsub)";
//  std::string LAST_MODIFIED="0.49beta2: Wed Apr 23 23:00:00 JST 2003 (bug fixed at conv_ltof shifter)";
//  std::string LAST_MODIFIED="0.49beta: Fri Mar 31 10:41:30 JST 2003 (debug check pg_conv_ltof_itp)";
//  std::string LAST_MODIFIED="0.48: Fri Mar 30 22:41:30 JST 2003 (a part check pg_conv_ftol_itp)";
//  std::string LAST_MODIFIED="0.48beta: Fri Mar 28 17:16:46 JST 2003 (debug pg_conv_ftol_itp)";
//  std::string LAST_MODIFIED="0.47: Wed Mar 14 17:00:00 JST 2003 (check pg_log_unsigned_add_itp)";
//  std::string LAST_MODIFIED="0.47beta: Wed Mar 13 17:50:00 JST 2003 (add pg_log_unsigned_add_itp)";
//  std::string LAST_MODIFIED="0.46: Fri Oct 18 16:43:48 JST 2002 (remove pg_function)";
//  std::string LAST_MODIFIED="    : Wed Oct 16 17:54:02 JST 2002 (add pg_function)";
//  std::string LAST_MODIFIED="    : Wed Oct 16 11:33:02 JST 2002 (add basedir option, cut help options)";
//  std::string LAST_MODIFIED="0.40: Tue Aug 20 17:32:49 JST 2002 (refine pg_log_add minus table)";

}


// +--------------------------------------------------------------+
// | THIS NAMESPACE IS FOR OPTION PARSER FOR PGPG.                | 
// | ALL OF THE MANUAL IS INCLUDED AT THIS NAME SPACE.            |
// | BY T.HAMADA(2002/05/27)                                      |
// +--------------------------------------------------------------+
namespace HELP{
  using namespace std;
  class option{
    string listfilename;
    string basedir;
  public:
    string get_listfilename(){ return this->listfilename; }
    string get_basedir(){ return this->basedir; }

    void parse(int argc,char *argv[]){
      if(argc<2){
	printf("pgpgc filename\n");
	exit(0);
      }else if(argc == 2){
	this->listfilename = argv[1];
	this->basedir = "./";
	//	cout << "/* PGPGC_MODULE Revision " + REVISION::SOFTWARE_REVISION + " */\n";
	//	cout << "/* " + REVISION::LAST_MODIFIED + " */\n";
	//	cout << "/* ============================================ */\n";
	//	cout << "/* = Copyright 2003 by Toshiyuki Fukushige.   = */\n";
	//	cout << "/* = Copyright 2003 by Tsuyoshi Hamada.       = */\n";
	//	cout << "/* =                                 |      / = */\n";
	//	cout << "/* = Pipeline                        |   @@@@ = */\n";
	//	cout << "/* =     Generator for               |  @@@@  = */\n";
	//	cout << "/* =         Programmable Grape      |  @@@   = */\n";
	//	cout << "/* ============================================ */\n\n";
	//	exit(0);
      }else if(argc >= 3){
	this->listfilename = argv[1];
	this->basedir= argv[2];
      }
    }

  };
}





