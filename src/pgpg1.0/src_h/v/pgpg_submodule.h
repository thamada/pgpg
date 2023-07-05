/*
  Sub modules used in pgpg_modules.
  This header file must be included at "pgpg.c".
  Last modified at July 1, 2003.

  --- SUB MODULE LIST ---
  void itobit(int idata, char sdata[], int nbit); // hamada version
  void itobc(int idata, char sdata[], int nbit);  // fukushige version @ pgpg.c
  void generate_pg_table(vd);
  void generate_pg_reged_penc(vd,nbit_in,nbit_out,npipe);
  void generate_pg_reged_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_shift_ftol(vd,nbit_in,nbit_out,nbit_ctl);
  void generate_pg_penc(vd,nbit_in,nbit_out);
  void generate_pg_shift_ftol_round(vd,nbit_in,nbit_out,nbit_ctl);
  void generate_pg_unreg_table(vd);
  void generate_reged_add_sub(vd);
  void generate_unreg_add_sub(vd);
  void generate_reged_add_sub_cout(vd);
  void generate_unreg_add_sub_cout(vd);
  void generate_pg_reged_mult(vd);
  void generate_pg_unreg_mult(vd);
  void generate_pg_reged_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_unreg_shift_ltof(vd,nbit_in,nbit_out,nbit_ctl,npipe);
  void generate_pg_lcell_fulladder(vd,device)
  void generate_pg_lcell(struct vhdl_description *vd, char *device); // @ pgpg_lcell.c
  void generate_pg_adder_RCA(struct vhdl_description *vd,int nbit,int nstage,int is_sub); // @ pgpg_adder_RCA.c
  void generate_pg_shift_fixtofloat_withSbitGbit(struct vhdl_description *vd,int nbit_in,int nbit_out,int nbit_ctl,int nstage);

*/

/* this is STRLEN of Hamada version */
#define HSTRLEN 400


void i2bit(int idata, char sdata[], int nbit); // hamada version
void itobc(int idata, char sdata[], int nbit);  // fukushige version

// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_table(struct vhdl_description *vd);


// pgpg_conv_ftol.c
void generate_pg_reged_penc(struct vhdl_description *vd,
			    int nbit_in,
			    int nbit_out,
			    int npipe);

// pgpg_conv_ftol.c
void generate_pg_reged_shift_ftol(struct vhdl_description *vd,
				  int nbit_in,
				  int nbit_out,
				  int nbit_ctl,
				  int npipe);

// pgpg_conv_ftol.c
void generate_pg_shift_ftol(struct vhdl_description *vd,
			    int nbit_in,
			    int nbit_out,
			    int nbit_ctl);

// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_penc(struct vhdl_description *vd,
		      int nbit_in,
		      int nbit_out);

// 2003/07/01 not used anywhere
void generate_pg_shift_ftol_round(struct vhdl_description *vd,
				  int nbit_in,
				  int nbit_out,
				  int nbit_ctl);

// pgpg_conv_ftol.c
// pgpg_conv_ftol_itp.c
void generate_pg_unreg_table(struct vhdl_description *vd);

// pgpg_log_unsigned_add_itp.c
// pgpg_conv_ftol_itp.c
void generate_reged_add_sub(struct vhdl_description *vd); /* 2003/03/31 by T.Hamada */

// pgpg_conv_ltof_itp.c
void generate_unreg_add_sub(struct vhdl_description *vd);

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_reged_add_sub_cout(struct vhdl_description *vd); /* 2003/03/31 by T.Hamada */

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_unreg_add_sub_cout(struct vhdl_description *vd); /* 2003/03/31 by T.Hamada */

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_pg_reged_mult(struct vhdl_description *vd); /* 2003/03/31 by T.Hamada */

// pgpg_conv_ftol_itp.c
// pgpg_conv_ltof_itp.c
void generate_pg_unreg_mult(struct vhdl_description *vd); /* 2003/03/31 by T.Hamada */

// pg_conv_ltof_itp.c
void generate_pg_reged_shift_ltof(struct vhdl_description *vd,
				  int nbit_in,
				  int nbit_out,
				  int nbit_ctl,
				  int npipe);

// pg_conv_ltof_itp.c
void generate_pg_unreg_shift_ltof(struct vhdl_description *vd,
				  int nbit_in,
				  int nbit_out,
				  int nbit_ctl);


// pg_log_unsigned_add_itp.c
//void generate_pg_lcell_rom_old(struct vhdl_description *vd,
//			       int ilen,
//			       int olen,
//			       int depth,
//			       int nstage,
//			       char* name,
//			       long long int* long_type_rom);


// Logic Cell in normal and arithmetic mode
void generate_pg_lcell(struct vhdl_description *vd, char *device);

void generate_pg_adder_RCA(struct vhdl_description *vd,
			   int nbit,
			   int nstage,
			   int is_sub);

void generate_pg_shift_fixtofloat_withSbitGbit(
    struct vhdl_description *vd,
    int nbit_in,
    int nbit_out,
    int nbit_ctl,
    int nstage);


// debug line
//  printf("Err at %s ",__FILE__);printf("line %d.\n",__LINE__);exit(1);
//
