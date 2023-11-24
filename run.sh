#!/bin/sh

cxx=${cxx:-clang++}

([ -z "`which $cxx`" ] && [ "$cxx" != "clang++" ]) && echo "$cxx not found, will try clang++" && cxx=clang++
[ -z "`which $cxx`" ] && echo "$cxx not found, will try g++" && cxx=g++

echo "build using $cxx"

$cxx -std=c++14 -g -o app main.cpp && time ./app
