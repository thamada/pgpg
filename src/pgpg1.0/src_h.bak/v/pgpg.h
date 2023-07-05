/* 
   last updated at 2004/05/27
*/
#define STRLEN 150
#define SDE_MAX 400000

struct vhdl_description {
  char sdc[10000][STRLEN];
  int ic;
  char sds[10000][STRLEN];
  int is;
  char sdm[10000][STRLEN];
  int im;
  //  char sde[10000][STRLEN]; // 2003/05/27
  //  char sde[1262144][STRLEN];
  char sde[SDE_MAX][STRLEN];
  int ie;
  int ucnt;
  int nvmp;
  int npipe;
  char sdjp[100][10][STRLEN];
  int ijp;
  char sdip[100][10][STRLEN];
  int iip;
  char sdfo[100][10][STRLEN];
  int ifo;
  char sdda[100][10][STRLEN];
  int ida;
  int jdata_width;
  int alteraesbflag;  
  int nboost_fixaccum;
};

