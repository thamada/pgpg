#CC = g++3 -g
#CC = g++ -O3 -march=i686
CC = g++
SRC = pgpg.o \
      pgpg_c_fix_addsub.o pgpg_c_log_muldiv.o pgpg_c_conv_ftol.o pgpg_c_conv_ltof.o \
      pgpg_c_log_unsigned_add.o pgpg_c_log_shift.o pgpg_c_pdelay.o pgpg_c_fix_accum.o \
      pgpg_c_log_add.o pgpg_c_log_unsigned_add_itp.o pgpg_c_conv_ftol_itp.o pgpg_c_conv_ltof_itp.o \
      pgpg_c_conv_FLP_to_FXP.o pgpg_c_conv_FLP_to_LNS.o pgpg_c_massoffset.o pgpg_c_function.o \
      pgpg_c_conv_fixtofloat.o pgpg_c_conv_floattofix.o  pgpg_c_float_mult.o \
      pgpg_c_float_unsigned_add.o \
      pgpg_c_bits_and.o pgpg_c_bits_or.o pgpg_c_bits_xor.o pgpg_c_bits_inv.o pgpg_c_bits_shift.o \
      pgpg_c_bits_rotate.o pgpg_c_bits_join.o pgpg_c_bits_part.o pgpg_c_bits_delay.o 


HEADER = pgpgc.h

#OPT_WALL = -O4 -Wall
OPT_WALL = -Wall

pgpgcm: $(SRC)  $(HEADER)
	$(CC) -static $(SRC) -o pgpgcm $(OPT_WALL)

pgpg.o : pgpg.cpp   $(HEADER)  pgpgc_help.h
	$(CC) -c pgpg.cpp $(OPT_WALL)

pgpg_c_fix_addsub.o : pgpg_c_fix_addsub.cpp  $(HEADER)  
	$(CC) -c pgpg_c_fix_addsub.cpp $(OPT_WALL)

pgpg_c_log_muldiv.o : pgpg_c_log_muldiv.cpp  $(HEADER)  
	$(CC) -c pgpg_c_log_muldiv.cpp $(OPT_WALL)

pgpg_c_conv_ftol.o : pgpg_c_conv_ftol.cpp  $(HEADER)  
	$(CC) -c pgpg_c_conv_ftol.cpp $(OPT_WALL)

pgpg_c_conv_ltof.o : pgpg_c_conv_ltof.cpp  $(HEADER)  
	$(CC) -c pgpg_c_conv_ltof.cpp $(OPT_WALL)

pgpg_c_log_unsigned_add.o : pgpg_c_log_unsigned_add.cpp  $(HEADER)  
	$(CC) -c pgpg_c_log_unsigned_add.cpp $(OPT_WALL)

pgpg_c_log_shift.o : pgpg_c_log_shift.cpp  $(HEADER)  
	$(CC) -c pgpg_c_log_shift.cpp $(OPT_WALL)

pgpg_c_pdelay.o : pgpg_c_pdelay.cpp  $(HEADER)  
	$(CC) -c pgpg_c_pdelay.cpp $(OPT_WALL)

pgpg_c_fix_accum.o : pgpg_c_fix_accum.cpp  $(HEADER)  
	$(CC) -c pgpg_c_fix_accum.cpp $(OPT_WALL)

pgpg_c_log_add.o : pgpg_c_log_add.cpp  $(HEADER)  
	$(CC) -c pgpg_c_log_add.cpp $(OPT_WALL)

pgpg_c_log_unsigned_add_itp.o : pgpg_c_log_unsigned_add_itp.cpp  $(HEADER)
	$(CC) -c pgpg_c_log_unsigned_add_itp.cpp $(OPT_WALL)

pgpg_c_conv_ftol_itp.o : pgpg_c_conv_ftol_itp.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_ftol_itp.cpp $(OPT_WALL)

pgpg_c_conv_ltof_itp.o : pgpg_c_conv_ltof_itp.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_ltof_itp.cpp $(OPT_WALL)
pgpg_c_conv_fixtofloat.o : pgpg_c_conv_fixtofloat.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_fixtofloat.cpp $(OPT_WALL)
pgpg_c_conv_floattofix.o : pgpg_c_conv_floattofix.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_floattofix.cpp $(OPT_WALL)
pgpg_c_float_mult.o : pgpg_c_float_mult.cpp  $(HEADER)
	$(CC) -c pgpg_c_float_mult.cpp $(OPT_WALL)
pgpg_c_float_unsigned_add.o : pgpg_c_float_unsigned_add.cpp  $(HEADER)
	$(CC) -c pgpg_c_float_unsigned_add.cpp $(OPT_WALL)

pgpg_c_bits_and.o : pgpg_c_bits_and.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_and.cpp $(OPT_WALL)
pgpg_c_bits_or.o : pgpg_c_bits_or.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_or.cpp $(OPT_WALL)
pgpg_c_bits_xor.o : pgpg_c_bits_xor.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_xor.cpp $(OPT_WALL)
pgpg_c_bits_inv.o : pgpg_c_bits_inv.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_inv.cpp $(OPT_WALL)
pgpg_c_bits_shift.o : pgpg_c_bits_shift.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_shift.cpp $(OPT_WALL)
pgpg_c_bits_rotate.o : pgpg_c_bits_rotate.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_rotate.cpp $(OPT_WALL)
pgpg_c_bits_join.o : pgpg_c_bits_join.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_join.cpp $(OPT_WALL)
pgpg_c_bits_part.o : pgpg_c_bits_part.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_part.cpp $(OPT_WALL)
pgpg_c_bits_delay.o : pgpg_c_bits_delay.cpp  $(HEADER)
	$(CC) -c pgpg_c_bits_delay.cpp $(OPT_WALL)


######################################
#  Software Emulator (NOT HARDWARE)  #
######################################
pgpg_c_conv_FLP_to_FXP.o : pgpg_c_conv_FLP_to_FXP.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_FLP_to_FXP.cpp $(OPT_WALL)

pgpg_c_conv_FLP_to_LNS.o : pgpg_c_conv_FLP_to_LNS.cpp  $(HEADER)
	$(CC) -c pgpg_c_conv_FLP_to_LNS.cpp $(OPT_WALL)

pgpg_c_massoffset.o : pgpg_c_massoffset.cpp  $(HEADER)
	$(CC) -c pgpg_c_massoffset.cpp $(OPT_WALL)

pgpg_c_function.o : pgpg_c_function.cpp  $(HEADER)
	$(CC) -c pgpg_c_function.cpp $(OPT_WALL)



clean:
	rm -f pgpgcm *.o *.vhd *.mif *~ .*~

softclean:
	rm -f pgpg pgpg.o EMU/*



depend:
	@makedepend -- $(CFLAGS) -- $(SRCS)

