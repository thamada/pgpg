#    Installation Program for PGPG
#         by Toshiyuki Fukushige
#
rm -rf ./bin
mkdir bin
echo "**********************************"
echo "  Installation Program for PGPG   "
echo "**********************************"
echo "Installation Type: "
echo "1. standard (HHHH)"
echo "2. vFH cFH        "
echo "3. vFF cFH        "
echo "10. skip        "
echo 0. exit
echo enter type number '(1-3 or others)':
set ans = $<
if ($ans == 0) then
  echo exit
  exit 0
else if ($ans == 1) then  
  set vttype = 0
  set vmtype = 0
  set cttype = 0
  set cmtype = 0  
else if ($ans == 2) then  
  set vttype = 1
  set vmtype = 0
  set cttype = 1
  set cmtype = 0  
else if ($ans == 3) then  
  set vttype = 1
  set vmtype = 1
  set cttype = 1
  set cmtype = 0  
else if ($ans == 10) then  
  set vttype = 10
  set vmtype = 10
  set cttype = 10
  set cmtype = 10
endif
echo "Hardware Type:"
echo "1. PROGRAPE-2"
echo "2. others"
echo enter type number '(1-2)':
set anshard = $<
set ansmem = -1
set ansdev = -1
set anslcell = -1
if($anshard == 1) then 
  echo "External Memory:"
  echo "1. with"
  echo "2. without"
  echo enter type number '(1-2)':
  set ansmem = $<
endif
if(($vttype == 0) || ($vmtype == 0)) then 
  if($anshard == 1) then
    set defdev = "-DAPEX20K -DJDATA_WIDTH=72"
  else if($anshard == 2) then
    echo "Device:"
    echo "1. Altera APEX20K (PG2)"
    echo "2. Altera APEX20KC"
    echo "3. Altera APEX20KE"
    echo "4. Altera Stratix (G6AXi)"
    echo "5. Altera Cyclone (G6AXp)"
    echo enter type number '(1-5)':
    set ansdev = $<
    if($ansdev == 1) then
      set defdev = -DAPEX20K
    endif
    if($ansdev == 2) then
      set defdev = -DAPEX20KC
    endif
    if($ansdev == 3) then
      set defdev = -DAPEX20KE
    endif
    if($ansdev == 4) then
      set defdev = -DSTRATIX
    endif
    if($ansdev == 5) then
      set defdev = -DCYCLONE
    endif
  endif 
  echo "LCELL ROM:"
  echo "1. off"
  echo "2. on"
  echo enter type number '(1-2)':
  set anslcell = $<
  if ($anslcell == 1) then 
    echo DFLAGS= $defdev > ./src_h/v/makefile.dev
  endif
  if ($anslcell == 2) then 
    echo DFLAGS= $defdev -DLCROM > ./src_h/v/makefile.dev
  endif
  cat ./src_h/v/Makefile >> ./src_h/v/makefile.dev       
endif
#
#  pgpgvt 
# 
if($vttype == 0) then
  cd ./src_h/v
  make clean
  make -f makefile.dev pgpgvt
  cp pgpgvt ../../bin
  cd ../../
endif
if($vttype == 1) then
  cd ./src_f/v
  make clean
  make pgpgvt
  cp pgpgvt ../../bin
  cd ../../
endif
#
#  pgpgvm 
# 
if($vmtype == 0) then
  cd ./src_h/v
  make clean
  make -f makefile.dev pgpgvm
  cp pgpgvm ../../bin
  cd ../../
endif
if($vmtype == 1) then
  cd ./src_f/v
  make clean
  make pgpgvm
  cp pgpgvm ../../bin
  cd ../../
endif
#
#  pgpgvi
# 
if(($anshard == 1)&&($ansmem == 1)) then
  cd ./src_f/vi
  make clean
  make pgpgvi
  cp pgpgvi ../../bin
  cd ../../
endif
#
#  pgpgv
# 
echo > ./bin/pgpgv
echo cpp \$1 tmp.cpp >> ./bin/pgpgv
if(($anshard == 1)&&($ansmem == 1)) then 
  echo $cwd/bin/pgpgvi tmp.cpp >> ./bin/pgpgv
  echo cp $cwd/src_f/vhw/pgfpga.vhd.pg2 pgfpga.vhd >> ./bin/pgpgv
  echo cp $cwd/src_f/vhw/pgfpga.csf.pg2 pgfpga.csf >> ./bin/pgpgv
  echo cp $cwd/src_f/vhw/pgfpga.quartus . >> ./bin/pgpgv
endif
echo $cwd/bin/pgpgvt tmp.cpp >> ./bin/pgpgv
echo $cwd/bin/pgpgvm tmp.cpp >> ./bin/pgpgv
if(($anshard == 1)&&($ansmem == 1)) then
  echo "cat pg_pipe.vhd >> pgfpga.vhd" >> ./bin/pgpgv
  echo "cat pg_module.vhd >> pgfpga.vhd" >> ./bin/pgpgv
endif
chmod +x ./bin/pgpgv
#
#  pgpgct
# 
if($cttype == 0) then
  cd ./src_h/ct
  make clean
  make pgpgct
  cp pgpgct ../../bin
  cd ../../
endif
if($cttype == 1) then
  cd ./src_f/ct
  make clean
  make pgpgct
  cp pgpgct ../../bin
  cd ../../
endif
#
#  pgpgcm
# 
if($cmtype == 0) then 
  cd ./src_h/cm
  make clean
  make pgpgcm
  cp pgpgcm ../../bin
  cd ../../
endif
#
#  pgpgc
# 
echo cpp \$1 \$1.cpp > ./bin/pgpgc
echo $cwd/bin/pgpgct \$1.cpp >> ./bin/pgpgc
echo $cwd/bin/pgpgcm \$1.cpp >> ./bin/pgpgc
chmod +x ./bin/pgpgc
#
#  log 
#
echo vttype = $vttype > ./bin/install.log
echo vmtype = $vmtype >> ./bin/install.log
echo cttype = $cttype >> ./bin/install.log
echo cmtype = $cmtype >> ./bin/install.log 
echo hardtype = $anshard >> ./bin/install.log
echo devicetype = $ansdev >> ./bin/install.log
echo lcell = $anslcell >> ./bin/install.log







