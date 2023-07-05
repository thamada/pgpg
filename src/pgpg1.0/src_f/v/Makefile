SRC = pgpg.c pgpg_fix_addsub.c pgpg_log_muldiv.c pgpg_log_shift.c pgpg_pdelay.c pgpg_conv_ftol.c pgpg_conv_ltof.c pgpg_log_add.c pgpg_fix_accum.c pgpg_set_io.c pgpg_log_unsigned_add.c pgpg_wtable.c pgpg_dwtable.c pgpg_pipe_com.c

pgpgvt: $(SRC)
	cc -g $(SRC) -DPGPGVT -o pgpgvt -lm

pgpgvm: $(SRC)
	cc $(SRC) -DPGPGVM -o pgpgvm -lm

clean:
	rm -f *~ pgpgvt pgpgvm





