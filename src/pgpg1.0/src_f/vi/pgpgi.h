#define STRLEN 400

struct vhdl_description {
  int nvmp;
  int npipe;
  char sdfunc[10000][STRLEN];
  int ifunc;
  char sdpipe[10000][STRLEN];
  int ipipe;
  char sddef[10000][STRLEN];
  int idef;
  char sdjp[10000][STRLEN];
  int ijp;
  char sdip[10000][STRLEN];
  int iip;
  char sdfo[10000][STRLEN];
  int ifo;
  char sdco[10000][STRLEN];
  int ico;
  char filename[STRLEN];
  char sdjpd[100][4][STRLEN];
  int ijpd;
  char sdipd[100][2][STRLEN];
  int iipd;
  char sdfod[100][STRLEN];
  int ifod;
};

