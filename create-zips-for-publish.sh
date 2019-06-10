#!/bin/bash

set -e

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
rm -rf build/*
mkdir -p build
TARGET_DIR=$(realpath "build")


function create_driver_zip() {
	local PROJ="$1" TYPE="$2"
	local TARGET="$TARGET_DIR/${PROJ}-${TYPE}-driver.zip"
	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ ($TYPE)"
		cd ..
		return
	fi
	if ! [[ -e "README.md" ]] ; then
		touch README.md
	fi
	zip "$TARGET" README.md >/dev/null
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" "${TYPE}" >/dev/null

	cd ..
}

cd drivers
for N in */ ; do
	N=$(basename "$N")
	echo "Create zip file for driver $N..."
	create_driver_zip "$N" "standalone"
	create_driver_zip "$N" "freertos"
done
cd ..

function create_demo_zip() {
	local PROJ="$1" TYPE="$2"

	IFS='_' PROJ_PART=($(echo "$PROJ"))
	if [[ -z "${PROJ_PART[1]}" ]]; then
		local TARGET="$TARGET_DIR/${PROJ_PART[0]}-${TYPE}.zip"
	else
		local TARGET="$TARGET_DIR/${PROJ_PART[0]}-${TYPE}-${PROJ_PART[1]}.zip"
	fi
	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ ($TYPE)"
		cd ..
		return
	fi

	echo "Create zip file for demo $PROJ ($TYPE)..."
	if ! [[ -e "README.md" ]] ; then
		touch README.md
	fi
	zip "$TARGET" README.md >/dev/null
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" "${TYPE}" >/dev/null

	cd ..
}

cd demos
for N in */ ; do
	N=$(basename "$N")
	create_demo_zip "$N" "standalone"
	create_demo_zip "$N" "freertos"
done
cd ..

function create_library_zip() {
	local PROJ="$1"

	local TARGET="$TARGET_DIR/${PROJ}.zip"
	if ! [[ -e "$PROJ/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ"
		return
	fi

	echo "Create zip file for library $PROJ..."
	if ! [[ -e "$PROJ/README.md" ]] ; then
		touch "$PROJ/README.md"
	fi
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" "${PROJ}" >/dev/null
}

cd libraries
for N in */ ; do
	N=$(basename "$N")
	create_library_zip "$N"
done
cd ..

function create_board_zip() {
	local PROJ="$1"

	local TARGET="$TARGET_DIR/${PROJ}.zip"
	if ! [[ -e "$PROJ/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ"
		return
	fi

	echo "Create zip file for board $PROJ..."
	if ! [[ -e "$PROJ/README.md" ]] ; then
		touch "$PROJ/README.md"
	fi
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" "${PROJ}" >/dev/null
}

cd boards
for N in */ ; do
	N=$(basename "$N")
	create_board_zip "$N"
done
cd ..

echo "All done."

