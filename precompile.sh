#!/data/data/com.termux/files/usr/bin/bash

apt install -y git clang ffmpeg ffmpeg-dev make termux-elf-cleaner
if ! test $? == 0
then
	exit $?
fi
git clone https://github.com/MkfsSion/libarray libarray;
ln -sf `pwd`/libarray/include/libarray `pwd`/include/libarray;
cd libarray;
make clean&&make static-lib;
if ! test $? == 0
then
	exit $?
fi
if ! test -d "../prebuilt"
then
    rm -fr ../prebuilt
    mkdir ../prebuilt
fi
cp -f libarray.a ../prebuilt;
make clean;
cd ..;
echo "Precompile envirnoment has been set up.";
exit 0;
