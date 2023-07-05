/*
  PGPGV library: Lcell-Table generation routines for pg_conv_ftol
  Author: Tsuyoshi Hamada 
  Last modified at Sep 19, 2003.
*/

/* +------------------------------------------------------------------+
   | calculate the table data and return nbit_tin for upper VHDL code |
   +------------------------------------------------------------------+
   You MUST call this function firstly.
 */
void create_table_lcell_conv_ftol(int nfix,     // fixed-point format length (this is only used in module name)
				  int nbit_man, // mantissa length (= table output length)
				  int nstage,   // pipeline stages
				  int* nadd,    // table overflow counts
				  char* mname); // module name


/* +------------------------------------------------------------------+
   | generate the lcell table                                         |
   +------------------------------------------------------------------+ */
void generate_pg_table_lcell_of_conv_ftol(struct vhdl_description *sd);
