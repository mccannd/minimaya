#!/bin/sh

if [ -e /usr/local/Qt5.4.0/5.4/gcc_64/bin/qmake ] ; then
    QMAKE=/usr/local/Qt5.4.0/5.4/gcc_64/bin/qmake
else
    QMAKE=qmake-qt5
fi

git submodule update --init
mkdir -p build-asan
cd build-asan
$QMAKE -Wall -spec linux-clang CONFIG+=address_sanitizer CONFIG+=silent ..
make
