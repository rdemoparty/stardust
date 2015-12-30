#!/bin/bash

function usage {
	echo -e "\nStardust utility script\n"
	echo -e "Usage:"
	echo -e "\t$0 update-version"
	echo -e "\t$0 clean"
	echo -e "\t$0 build [debug]"
	echo -e "\t$0 build cross [debug]"
	echo -e "\t$0 run"
	echo -e "\t$0 run cross"
	echo -e "\t$0 package"
	echo -e "\t$0 editor"
	echo -e "\t$0 docker"
	echo -e ""
}

function update_version {
	echo -e "Updating version metadata files...\n"

	CURRENT_VERSION=$(git describe)
	CURRENT_TIMESTAMP=$(git show -s --format=%ct)

	echo -e "STARDUST_VERSION = '$CURRENT_VERSION';\nSTARDUST_TIMESTAMP = $CURRENT_TIMESTAMP;\n" > tools/editor/web/js/editor.version.js

	cp src/game/Version.h.sample src/game/Version.h
	sed -i.bak -e s/{version}/$CURRENT_VERSION/g src/game/Version.h
	sed -i.bak -e s/{timestamp}/$CURRENT_TIMESTAMP/g src/game/Version.h

	rm src/game/Version.h.bak
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
		shift 1
		if hash optirun 2>/dev/null; then
			cd bin && optirun ./stardust.exe $@ && cd ..
		else
			cd bin && ./stardust.exe $@ && cd ..
		fi

	else
		if [ ! -f bin/stardust ]; then
			echo -e "Error: bin/stardust file does not exist\n"
			exit 1
		fi
		cd bin && ./stardust $* && cd ..
	fi
}

function package_project {
	CURRENT_VERSION=$(git describe)
	ARCHIVE_FILENAME="stardust-$CURRENT_VERSION.zip"
	rm -rf $ARCHIVE_FILENAME
	zip $ARCHIVE_FILENAME bin/*.exe bin/*.dll data/ tools -r
}

function start_editor {
	if [ "$2" == "cross" ]; then
		run_project cross --width 1024 --height 768 --editor --fullscreen=false -v
	else
		run_project --width 1024 --height 768 --editor --fullscreen=false -v
	fi
}

function build_project {
	CMAKE_EXTRA_ARGUMENTS=""

	if [ "$1" == "cross" ]; then
		if [ "$2" == "debug" ]; then
			CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_BUILD_TYPE=Debug"
		else
			CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_BUILD_TYPE=Release"
		fi
		
		echo -e "Building cross-compiled solution...$CMAKE_EXTRA_ARGUMENTS\n"
		CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_TOOLCHAIN_FILE=../cmake/win64.cmake"
	else
		if [ "$1" == "debug" ]; then
			CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_BUILD_TYPE=Debug"
		else
			CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_BUILD_TYPE=Release"
		fi
		echo -e "Building solution...$CMAKE_EXTRA_ARGUMENTS\n"
	fi

	mkdir -p build && cd build
	if [ ! -f Makefile ]; then
		cmake $CMAKE_EXTRA_ARGUMENTS ..
	fi

	make -j 4
	cd ..
}

function compile_with_docker {
	docker build -t stardust/mingw .
	docker run --rm -it -v $(pwd):/stardust stardust/mingw sh -c "./sd.sh clean && ./sd.sh build cross"
}


if [ $# -eq 0 ]; then
	usage
	exit 1
fi

case $1 in
	clean)
		clean_project
		;;
	update-version)
		update_version
		;;
	build)
		build_project $2
		;;
	run)
		RUN_OPTION=$2
		shift 2
		run_project $RUN_OPTION $@
		;;
	package)
		package_project $2
		;;
	editor)
		start_editor $@
		;;
	docker)
		compile_with_docker
		;;
	*)
		echo -e "Unknown option $1"
		usage
		exit 1
esac

