/* +------------------------------------------------------------------+
   | calculate the table data and return nbit_tin for upper VHDL code |
   +------------------------------------------------------------------+
   You MUST call this function firstly.
*/
void create_table_lcell_log_unsigned_add(int nbit_man,
					 int nstage,
					 int* nbit_tin,
					 char* mname);


/* +------------------------------------------------------------------+
   | generate the lcell table                                         |
   +------------------------------------------------------------------+ */
void generate_pg_table_lcell_of_log_unsigned_add(struct vhdl_description *sd);


/* --- SAMPLE CODE ---
   int nbit_man,nstage,nbit_tin;
   struct vhdl_description *sd;
   char lcname[256];
   nbit_man=8;
   nstage=1;

   //--- API(1) ---
   create_table_lcell_log_unsigned_add(nbit_man,nstage,&nbit_tin,lcname);
   //--- API(2) ---
   generate_pg_table_lcell_of_log_unsigned_add(&sd);
*/








