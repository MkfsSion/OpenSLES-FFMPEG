#!/data/data/com.termux/files/usr/bin/bash

if test -d "prebuilt" || test -d "libarray"
then
unlink include/libarray;
rm -rf prebuilt;
rm -rf libarray;
fi
echo "Precompile envirnoment has been removed.";
exit 0;
