#!/data/data/com.termux/files/usr/bin/bash

git clone https://github.com/MkfsSion/libarray libarray;
ln -sf `pwd`/libarray/include/libarray `pwd`/include/libarray;
cd libarray;
make clean&&make static-lib;
cp -f libarray.a ../prebuilt;
make clean;
cd ..;
echo "Precompile envirnoment has been set up.";
exit 0;
