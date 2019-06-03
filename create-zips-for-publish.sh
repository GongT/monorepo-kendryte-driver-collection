#!/bin/bash

set -e

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
rm -rf build/*
mkdir -p build
TARGET_DIR=$(realpath "build")


function create_driver_zip() {
	local PROJ="$1" TYPE="$2"
	local TARGET="$TARGET_DIR/${PROJ}-${TYPE}.zip"
	cd "$PROJ"
	if ! [[ -e "README.md" ]] ; then
		touch README.md
	fi
	zip "$TARGET" README.md >/dev/null
	cd "$TYPE"
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" * >/dev/null

	cd ../..
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
	local TARGET="$TARGET_DIR/${PROJ_PART[0]}-${TYPE}-${PROJ_PART[1]}.zip"
	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo "skip $PROJ ($TYPE)"
		cd ..
		return
	fi

	echo "Create zip file for demo $PROJ ($TYPE)..."
	if ! [[ -e "README.md" ]] ; then
		touch README.md
	fi
	zip "$TARGET" README.md >/dev/null
	cd "$TYPE"
	zip \
		"--exclude=.*" \
		"--exclude=.*/*" \
	       	"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" * >/dev/null

	cd ../..
}

cd demos
for N in */ ; do
	N=$(basename "$N")
	create_demo_zip "$N" "standalone"
	create_demo_zip "$N" "freertos"
done
cd ..


echo "All done."
