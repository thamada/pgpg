#define STRLEN 200
#define SDESIZE 100000

struct vhdl_description {
  char sdc[10000][STRLEN];
  int ic;
  char sds[10000][STRLEN];
  int is;
  char sdm[10000][STRLEN];
  int im;
  char sde[SDESIZE][STRLEN];
  int ie;
  int ucnt;
  int nvmp;
  int npipe;
  int alteraesbflag;  
  char sdjp[100][10][STRLEN];
  int ijp;
  char sdip[100][10][STRLEN];
  int iip;
  char sdfo[100][10][STRLEN];
  int ifo;
  char sdda[100][10][STRLEN];
  int ida;
};

