cd ./src_f/v
make clean
cd ../ct
make clean
cd ../vi
make clean
cd ../vhw
rm *~
cd ../../
tar cvf src_f.tar ./src_f
mv src_f.tar ~/pgpg_cont
cp install.csh ~/pgpg_cont

