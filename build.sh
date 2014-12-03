#!/bin/bash

mkdir -p build && cd build
if [-f "Makefile"]
then
	make
else
	cmake ..
	make
fi
cd ..
mkdir -p bin
cp build/stardust bin/

