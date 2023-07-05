#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cmath>
#include <typeinfo>
#include <fstream>

// +-------------------------------------------------+
// | THIS NAMESPACE IS FOR A PGPG                    | 
// |  PARSING A LIST FILE,                           | 
// |  AND HANDING GENERATOR ROUTINES                 |
// |  EACH MODULE INFORMATION OF ARGUMENTS.          |
// |                                                 |
// | BY T.HAMADA(2002/05/8)                          |
// +-------------------------------------------------+
namespace PGPG_PARSER{
  using namespace std;
  // LINE PARSER  =============================================================
  class Line{
  public:
    void stringTokenize(string str,string delim,vector<string>& args) {
      args.clear(); 
      string::size_type from;
      string::size_type to = 0;
      while ( to != string::npos ) {
	from = str.find_first_not_of(delim,to);
	if ( from == string::npos ) break;
	to = str.find_first_of(delim, from);
	string token = str.substr(from, to-from);
	args.push_back(token);
      }
      return;
    }

    string getStringField(string str,string start_delim, string end_delim)
    {
      string::size_type pos_start = str.find_first_of("(");
      string::size_type pos_end = str.find_first_of(")");
      string str_part = str.substr(pos_start+1,pos_end-pos_start-1);
      return str_part;
    }
    string getModuleArgField(string line){  return getStringField(line,"(",")"); }
  };


  // LIST FILE READER  =============================================================
  class ListRead{
  private:
    int index;
    int max_index;
    string listfilename;
    string current_line;
    vector<string> current_args;
    vector<string> lines;
  public:
    // CONSTRUCTOR
    ListRead(string fname="list"){
      this->listfilename = fname;
      this->index=0;
      this->max_index=0;
      this->readList();  // read the whole file and get the whole lines. 
      this->nextLine();  // set cursor at the first line field.
    };

    // FIND THE INDICATED MODULE AND RETURN TRUE(FIND)/FALSE(NOT FIND) FLAG 
    //   IF TRUE(FIND), SET ARGMENTS TO THE OBJECT PROPERTY. 
    bool isFind(string module_name){
      Line* lineObj = new Line();      // line parse argorithm tool-kit (this has no properties)
      string line  = this->current_line;
      bool is_find=0;
      {
	char *p;
	char stmp[1024];
	//	stmp = new char[line.size()];
	strcpy(stmp,line.c_str());
	p = (char* )strtok(stmp,"( ");
	string line_mname(p);
	if(module_name == line_mname) is_find=1; else is_find=0;
      }
      if(is_find){
	string argfield = lineObj->getModuleArgField(line);    // get "modulename(XXX)" of XXX field.
	lineObj->stringTokenize(argfield,",",this->current_args); // delimita is a ','.
      }
      return is_find;
    }

    // RETURN ARGUMENTS
    vector<string> getArgs(){  return this->current_args; }

    // RETURN NEXT INDEX
    int getNextIndex(){  return this->index; }

    // RETURN CURRENT INDEX
    int getIndex(){  return (this->index - 1); }

    // RETURN CURRENT LINE
    string getCurrentLine(){  return this->current_line; }

    // RETURN MAXIMUM INDEX (THE NUMBER OF LINES)
    int getMaxIndex(){  return this->max_index; }

    // RETURN NEXT LINE
    bool nextLine(){
      if( (this->index) > (this->max_index) ) return false;
      this->current_line = this->lines[this->index];
      (this->index)++;
      return true;
    }

    // OPEN FILE AND READ THE WHOLE FILE INTO OBJECT
    void readList(){
      ifstream fin;
      fin.unsetf(ios::skipws);

      // Open File
      fin.open((this->listfilename).c_str(),ios::in|ios::binary);
      if(!fin.is_open()){
	cerr << "Cannot open file."<<endl;
	exit(1);
      }

      while(!fin.eof()){
	int _Nchar=200;
	char _cline[_Nchar];
	string _line;
	fin.getline(_cline,(_Nchar-1),'\n');
	_line = _cline;
	_line += "\n";
	this->lines.push_back(_line);
      }
      fin.close();
      this->max_index = (this->lines).size() -1;
      return;
    }

    string operator[](int i){
      if(i>= (int)(this->lines.size())){
	cerr << "ListRead: Miss indexing. No more lines.\n";exit(1);
      }
      return this->lines[i];
    }

    // DUMP THE CURRENT ARGUMENT TO STDOUT. THIS IS FOR DEBUGING. 
    void dumpCurrentArgInfo(){
      for(int i=0; i<(int)(this->current_args).size();i++)
	cout << this->current_args[i] <<endl;
    }

    // DUMP ALL OF THE LINES
    void dump(){
      for(int i=0;i<(int)this->lines.size();i++) cout << this->lines[i];
      return; 
    }
  };

}

// +--------------------------------------------------------------+
// | THIS NAMESPACE IS FOR PGPG MODULE GENERATION.                | 
// | NOW THE NAMESPACE IS "STD",                                  |
// | BUT I WILL CHANGE THE NAME TO "PGPG_TOOLKIT" SOMEDAY.        |
// |                                                              |
// | BY T.HAMADA(2002/04/30)                                      |
// +--------------------------------------------------------------+
namespace std{
  // INTEGER CONVERTER =============================================================
  class Convert{
  public:
    string i2slx(int i){ // int to string as long long hexel-format
      char strbuf[20];   
      string s;
      sprintf(strbuf ,"0x%llxULL",(long long int)i);
      s = strbuf;
      return s;
    }

    string i2sx(int i){ // int to string as hexel-format
      char strbuf[20];
      string s;
      sprintf(strbuf ,"0x%x",i);
      s = strbuf;
      return s;
    }

    string i2s(int i){
      char strbuf[11];
      string s;
      sprintf(strbuf ,"%d",i);
      s = strbuf;
      return s;
    }

    string i2mask(int i){
      string s;
      if(i<33){
	unsigned int mask;
	mask = (unsigned int)(pow(2.0,(double)i)-1.0);
	char strbuf[8];
	sprintf(strbuf ,"0x%X",mask);
	s = strbuf;
      }else if(i<65){
	/* FIND BUG! (if i=54,55,56,57,58,59,60,61,....) */
	/*
	  unsigned long long int mask;
	  mask = (unsigned long long int)(pow(2.0,(double)i)-1.0);
	  char strbuf[16];
	  //    sprintf(strbuf ,"0x%016llXull",mask);
	  sprintf(strbuf ,"0x%016LXull",mask);
	  s = strbuf;
	*/

	/* AD HOCK VERSION! */ 
	switch (i){
	case 33 :{ s = "0x00000001FFFFFFFFULL"; break;}
	case 34 :{ s = "0x00000003FFFFFFFFULL"; break;}
	case 35 :{ s = "0x00000007FFFFFFFFULL"; break;}
	case 36 :{ s = "0x0000000FFFFFFFFFULL"; break;}
	case 37 :{ s = "0x0000001FFFFFFFFFULL"; break;}
	case 38 :{ s = "0x0000003FFFFFFFFFULL"; break;}
	case 39 :{ s = "0x0000007FFFFFFFFFULL"; break;}
	case 40 :{ s = "0x000000FFFFFFFFFFULL"; break;}
	case 41 :{ s = "0x000001FFFFFFFFFFULL"; break;}
	case 42 :{ s = "0x000003FFFFFFFFFFULL"; break;}
	case 43 :{ s = "0x000007FFFFFFFFFFULL"; break;}
	case 44 :{ s = "0x00000FFFFFFFFFFFULL"; break;}
	case 45 :{ s = "0x00001FFFFFFFFFFFULL"; break;}
	case 46 :{ s = "0x00003FFFFFFFFFFFULL"; break;}
	case 47 :{ s = "0x00007FFFFFFFFFFFULL"; break;}
	case 48 :{ s = "0x0000FFFFFFFFFFFFULL"; break;}
	case 49 :{ s = "0x0001FFFFFFFFFFFFULL"; break;}
	case 50 :{ s = "0x0003FFFFFFFFFFFFULL"; break;}
	case 51 :{ s = "0x0007FFFFFFFFFFFFULL"; break;}
	case 52 :{ s = "0x000FFFFFFFFFFFFFULL"; break;}
	case 53 :{ s = "0x001FFFFFFFFFFFFFULL"; break;}
	case 54 :{ s = "0x003FFFFFFFFFFFFFULL"; break;}
	case 55 :{ s = "0x007FFFFFFFFFFFFFULL"; break;}
	case 56 :{ s = "0x00FFFFFFFFFFFFFFULL"; break;}
	case 57 :{ s = "0x01FFFFFFFFFFFFFFULL"; break;}
	case 58 :{ s = "0x03FFFFFFFFFFFFFFULL"; break;}
	case 59 :{ s = "0x07FFFFFFFFFFFFFFULL"; break;}
	case 60 :{ s = "0x0FFFFFFFFFFFFFFFULL"; break;}
	case 61 :{ s = "0x1FFFFFFFFFFFFFFFULL"; break;}
	case 62 :{ s = "0x3FFFFFFFFFFFFFFFULL"; break;}
	case 63 :{ s = "0x7FFFFFFFFFFFFFFFULL"; break;}
	case 64 :{ s = "0xFFFFFFFFFFFFFFFFULL"; break;}
	default :{ cerr << "Convert i2mask: input is out of range." <<endl; exit(1);}
	}
	/* AD HOCK VERSION! -- END*/ 

      }else{
	cerr << "Convert i2lmask: input is out of range." <<endl; exit(1);
      }

      return s;
    }

    // integer to double string
    string i2ds(int i){ 
      double d;
      d = pow(2.0,(double)i);
      char strbuf[4];
      string s;
      sprintf(strbuf ,"%f",d);
      s = strbuf;
      return s;
    }

    string i2lmask(int i){
      string s;
      switch (i){
      case  0 :{ s = "0x0000000000000000ULL"; break;}
      case  1 :{ s = "0x0000000000000001ULL"; break;}
      case  2 :{ s = "0x0000000000000003ULL"; break;}
      case  3 :{ s = "0x0000000000000007ULL"; break;}
      case  4 :{ s = "0x000000000000000FULL"; break;}
      case  5 :{ s = "0x000000000000001FULL"; break;}
      case  6 :{ s = "0x000000000000003FULL"; break;}
      case  7 :{ s = "0x000000000000007FULL"; break;}
      case  8 :{ s = "0x00000000000000FFULL"; break;}
      case  9 :{ s = "0x00000000000001FFULL"; break;}
      case 10 :{ s = "0x00000000000003FFULL"; break;}
      case 11 :{ s = "0x00000000000007FFULL"; break;}
      case 12 :{ s = "0x0000000000000FFFULL"; break;}
      case 13 :{ s = "0x0000000000001FFFULL"; break;}
      case 14 :{ s = "0x0000000000003FFFULL"; break;}
      case 15 :{ s = "0x0000000000007FFFULL"; break;}
      case 16 :{ s = "0x000000000000FFFFULL"; break;}
      case 17 :{ s = "0x000000000001FFFFULL"; break;}
      case 18 :{ s = "0x000000000003FFFFULL"; break;}
      case 19 :{ s = "0x000000000007FFFFULL"; break;}
      case 20 :{ s = "0x00000000000FFFFFULL"; break;}
      case 21 :{ s = "0x00000000001FFFFFULL"; break;}
      case 22 :{ s = "0x00000000003FFFFFULL"; break;}
      case 23 :{ s = "0x00000000007FFFFFULL"; break;}
      case 24 :{ s = "0x0000000000FFFFFFULL"; break;}
      case 25 :{ s = "0x0000000001FFFFFFULL"; break;}
      case 26 :{ s = "0x0000000003FFFFFFULL"; break;}
      case 27 :{ s = "0x0000000007FFFFFFULL"; break;}
      case 28 :{ s = "0x000000000FFFFFFFULL"; break;}
      case 29 :{ s = "0x000000001FFFFFFFULL"; break;}
      case 30 :{ s = "0x000000003FFFFFFFULL"; break;}
      case 31 :{ s = "0x000000007FFFFFFFULL"; break;}
      case 32 :{ s = "0x00000000FFFFFFFFULL"; break;}
      case 33 :{ s = "0x00000001FFFFFFFFULL"; break;}
      case 34 :{ s = "0x00000003FFFFFFFFULL"; break;}
      case 35 :{ s = "0x00000007FFFFFFFFULL"; break;}
      case 36 :{ s = "0x0000000FFFFFFFFFULL"; break;}
      case 37 :{ s = "0x0000001FFFFFFFFFULL"; break;}
      case 38 :{ s = "0x0000003FFFFFFFFFULL"; break;}
      case 39 :{ s = "0x0000007FFFFFFFFFULL"; break;}
      case 40 :{ s = "0x000000FFFFFFFFFFULL"; break;}
      case 41 :{ s = "0x000001FFFFFFFFFFULL"; break;}
      case 42 :{ s = "0x000003FFFFFFFFFFULL"; break;}
      case 43 :{ s = "0x000007FFFFFFFFFFULL"; break;}
      case 44 :{ s = "0x00000FFFFFFFFFFFULL"; break;}
      case 45 :{ s = "0x00001FFFFFFFFFFFULL"; break;}
      case 46 :{ s = "0x00003FFFFFFFFFFFULL"; break;}
      case 47 :{ s = "0x00007FFFFFFFFFFFULL"; break;}
      case 48 :{ s = "0x0000FFFFFFFFFFFFULL"; break;}
      case 49 :{ s = "0x0001FFFFFFFFFFFFULL"; break;}
      case 50 :{ s = "0x0003FFFFFFFFFFFFULL"; break;}
      case 51 :{ s = "0x0007FFFFFFFFFFFFULL"; break;}
      case 52 :{ s = "0x000FFFFFFFFFFFFFULL"; break;}
      case 53 :{ s = "0x001FFFFFFFFFFFFFULL"; break;}
      case 54 :{ s = "0x003FFFFFFFFFFFFFULL"; break;}
      case 55 :{ s = "0x007FFFFFFFFFFFFFULL"; break;}
      case 56 :{ s = "0x00FFFFFFFFFFFFFFULL"; break;}
      case 57 :{ s = "0x01FFFFFFFFFFFFFFULL"; break;}
      case 58 :{ s = "0x03FFFFFFFFFFFFFFULL"; break;}
      case 59 :{ s = "0x07FFFFFFFFFFFFFFULL"; break;}
      case 60 :{ s = "0x0FFFFFFFFFFFFFFFULL"; break;}
      case 61 :{ s = "0x1FFFFFFFFFFFFFFFULL"; break;}
      case 62 :{ s = "0x3FFFFFFFFFFFFFFFULL"; break;}
      case 63 :{ s = "0x7FFFFFFFFFFFFFFFULL"; break;}
      case 64 :{ s = "0xFFFFFFFFFFFFFFFFULL"; break;}
      default :{ cerr << "Convert i2mask: input is out of range." <<endl; exit(1);}
      }
      return s;
    }



  };

  //=====================================================================
  class GenFile{
    string* cfunclist;
    int cfi; /* index of cfunclist */ 
    string basedir;
    string revision_number;
    string revision_last_modified;
  public :
    GenFile(string _bdir="./") /* Constructor */
    {
      cfi=0;
      cfunclist = new string[2048];
      this->basedir= _bdir;
    }

    void set_revision_number(string s="none"){ this->revision_number=s; }
    void set_revision_last_modified(string s="none"){ this->revision_last_modified=s; }

    void generate(string func_name, string body, string extension=".c"){
      string fstring = this->basedir + func_name + extension; 
      const char* fname;
      // string to char* convert
      fname = new char[fstring.size()];
      fname = fstring.c_str();

      /* Check whether the Module had already been generated or not         * */
      int is_gened=0;                                                      /* */
      for(int j=0;j<(this->cfi);j++){                                      /* */
	string::size_type pos;                                             /* */
	pos = fstring.find(cfunclist[j]);                                  /* */
	if(pos != string::npos) is_gened=1;         /* be already generated * */
      }                                                                    /* */
      /* If Module had already been generated, break out from this function.* */
      if(is_gened==1) return;                                              /* */
      else{                                                                /* */
	cerr << "Hamada kaku: "+fstring+"\n";                              /* */
	this->cfunclist[(this->cfi)]=fstring;                              /* */
	this->cfi=this->cfi+1;                                             /* */
      }                                                                    /* */

      //------------------------------------------------------------ changed 2003/12/27
      //      ofstream fout(fname);                               // changed 2003/12/27
      static int is_overwrite=0;                                  // changed 2003/12/27
      if(is_overwrite==0){                                        // changed 2003/12/27
	ofstream ferase("pg_module.c",ios::trunc);                // changed 2003/12/27
	ferase << "";                                             // changed 2003/12/27
	ferase.close();                                           // changed 2003/12/27
	is_overwrite=1;                                           // changed 2003/12/27
      }                                                           // changed 2003/12/27
      ofstream fout("pg_module.c",ios::app);                      // changed 2003/12/27
      //------------------------------------------------------------ changed 2003/12/27

      if(!fout){
	cerr << "Error:\n";
	cerr << "Cannot open output file.\n";
	cerr << "Did you make \"vhdl\" directory at current directory?\n";
	cerr << "[hamada@provence.c.u-tokyo.ac.jp]\n";
	exit(1) ;
      }

      if(extension == ".c"){
	string body_author;
	body_author += "/* Generated by PGPG module generateor       */\n";
	body_author += "/* Copyright(c) 2001-2004 by Tsuyoshi Hamada */\n";
	body = body_author + body;
      }else if(extension == ".sh"){
	string body_author;
	body_author += "#  ============================================\n";
	body = body_author + body;
      }

      fout << body;
      //      cout << body;
      fout.close();
      //      cout << fname <<endl;
    }

  };

  void generate_c_pg_fix_addsub(vector<string> args,      GenFile* gfObj);
  void generate_c_pg_log_muldiv(vector<string> args,      GenFile* gfObj);
  void generate_c_pg_conv_ftol(vector<string> args,       GenFile* gfObj);
  void generate_c_pg_conv_ltof(vector<string> args,       GenFile* gfObj);
  void generate_c_pg_log_unsigned_add(vector<string> args,GenFile* gfObj);
  void generate_c_pg_log_shift(vector<string> args,       GenFile* gfObj);
  void generate_c_pg_pdelay(vector<string> args,          GenFile* gfObj);
  void generate_c_pg_fix_accum(vector<string> args,       GenFile* gfObj);
  void generate_c_pg_log_add(vector<string> args,         GenFile* gfObj);
  void generate_c_pg_conv_FLP_to_FXP(vector<string> args, GenFile* gfObj);
  void generate_c_pg_conv_FLP_to_LNS(vector<string> args, GenFile* gfObj);
  void generate_c_pg_massoffset(vector<string> args,      GenFile* gfObj);
  void generate_c_pg_function(vector<string> args,        GenFile* gfObj);
  void generate_c_pg_log_unsigned_add_itp(vector<string> args,GenFile* gfObj);
  void generate_c_pg_conv_ftol_itp(vector<string> args,   GenFile* gfObj);
  void generate_c_pg_conv_ltof_itp(vector<string> args,   GenFile* gfObj);
  void generate_c_pg_conv_fixtofloat(vector<string> args, GenFile* gfObj);
  void generate_c_pg_conv_floattofix(vector<string> args, GenFile* gfObj);
  void generate_c_pg_float_mult(vector<string> args, GenFile* gfObj);
  void generate_c_pg_float_unsigned_add(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_and(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_or(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_xor(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_inv(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_shift(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_rotate(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_join(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_part(vector<string> args, GenFile* gfObj);
  void generate_c_pg_bits_delay(vector<string> args, GenFile* gfObj);
}
