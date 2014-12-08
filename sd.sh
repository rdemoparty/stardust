#!/bin/bash

function usage {
	echo -e "\nStardust utility script\n"
	echo -e "Usage:"
	echo -e "\t$0 clean"
	echo -e "\t$0 build"
	echo -e "\t$0 build cross"
	echo -e "\t$0 run"
	echo -e "\t$0 run cross"
	echo -e ""
}

function clean_project {
	echo -e "Cleaning up solution...\n"
	if [ -d build ]; then
		rm -rf build/*
	fi
}

function run_project {
	echo -e "Running project...\n"
	if [ ! -d bin ]; then
		echo -e "Error: bin/ folder does not exist\n"
		exit 1
	fi

	if [ "$1" == "cross" ]; then
		if [ ! -f bin/stardust.exe ]; then
			echo -e "Error: bin/stardust.exe file does not exist\n"
			exit 1
		fi

		cd bin && optirun ./stardust.exe && cd ..
	else
		if [ ! -f bin/stardust ]; then
			echo -e "Error: bin/stardust file does not exist\n"
			exit 1
		fi

		cd bin && ./stardust && cd ..
	fi
}

function build_project {
	CMAKE_EXTRA_ARGUMENTS=""

	if [ "$1" == "cross" ]; then
		echo -e "Building cross-compiled solution...\n"
		CMAKE_EXTRA_ARGUMENTS="-DCMAKE_TOOLCHAIN_FILE=../cmake/win32.cmake"
	else
		echo -e "Building solution...\n"
	fi

	mkdir -p build && cd build
	if [ ! -f Makefile ]; then
		cmake $CMAKE_EXTRA_ARGUMENTS ..
	fi

	make -j 4
	cd ..
}


if [ $# -eq 0 ]; then
	usage
	exit 1
fi

case $1 in
	clean)
		clean_project
		;;
	build)
		build_project $2
		;;
	run)
		run_project $2
		;;
	*)
		echo -e "Unknown option $1"
		usage
		exit 1
esac
