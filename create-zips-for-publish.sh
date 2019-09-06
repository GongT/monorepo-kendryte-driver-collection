#!/bin/bash

set -e

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
rm -rf build/*
mkdir -p build
TARGET_DIR=$(realpath "build")

ZIP_BIN=$(command -v zip)

function zip() {
	echo -e " > \e[2mzip $*\e[0m" >&2
	"$ZIP_BIN" "$@"
}
function ensure_dir() {
	mkdir -p "$(dirname "${1}")"
}

function create_driver_zip() {
	local PROJ="$1" TYPE="$2"
	local TARGET="$TARGET_DIR/driver/${PROJ}-${TYPE}-driver.zip"

	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ ($TYPE)"
		cd ..
		return
	fi
	ensure_dir "$TARGET"
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
	echo ""
done
cd ..

function create_demo_zip() {
	local PROJ="$1" TYPE="$2"

	local OIFS="$IFS"
	local IFS='_'
	local PROJ_PART=($(echo "$PROJ"))
	local IFS="$OIFS"

	if [[ -z "${PROJ_PART[1]}" ]]; then
		local TARGET="$TARGET_DIR/demo/${PROJ_PART[0]}-${TYPE}.zip"
	else
		local TARGET="$TARGET_DIR/demo/${PROJ_PART[0]}-${TYPE}-${PROJ_PART[1]}.zip"
	fi
	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ ($TYPE)"
		cd ..
		return
	fi

	ensure_dir "$TARGET"
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
	echo ""
done
cd ..

function create_library_zip_typed() {
	local PROJ="$1" TYPE="$2"
	local TARGET="$TARGET_DIR/library/${PROJ}-${TYPE}.zip"

	cd "$PROJ"
	if ! [[ -e "$TYPE/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ ($TYPE)"
		cd ..
		return
	fi

	ensure_dir "$TARGET"
	echo "Create zip file for library $PROJ ($TYPE)..."
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

function create_library_zip() {
	local PROJ="$1"

	local TARGET="$TARGET_DIR/library/${PROJ}.zip"
	if ! [[ -e "$PROJ/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ"
		return
	fi

	ensure_dir "$TARGET"
	echo "Create zip file for library $PROJ (common)..."
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
	if [[ -e "$N/kendryte-package.json" ]]; then
		create_library_zip "$N"
	else
		create_library_zip_typed "$N" "standalone"
		create_library_zip_typed "$N" "freertos"
	fi

	echo ""
done
cd ..

function create_board_zip() {
	local PROJ="$1"

	local TARGET="$TARGET_DIR/board/board-${PROJ}.zip"
	if ! [[ -e "$PROJ/kendryte-package.json" ]] ; then
		echo -e "\tskip $PROJ"
		return
	fi

	ensure_dir "$TARGET"
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

#cd boards
#for N in */ ; do
#	N=$(basename "$N")
#	create_board_zip "$N"
#	echo ""
#done
#cd ..

ND=$(date +%Y%m%d%H%M%S)
function create_sdk_zip() {
	local PROJ="$1"

	local TARGET="$TARGET_DIR/sdk/${PROJ}-sdk.zip"
	if ! [[ -e "kendryte-${PROJ}-sdk/kendryte-package.json" ]] ; then
		echo "SDK not clone: $PROJ">&2
		exit 1
	fi

	ensure_dir "$TARGET"
	echo "Create zip file for SDK $PROJ..."

	rm -rf "/tmp/kendryte-${PROJ}-sdk"
	cp -r "kendryte-${PROJ}-sdk" "/tmp/kendryte-${PROJ}-sdk"
	sed -Ei 's#"KENDRYTE_SDK_RELEASE_DATE:raw":[^,]+#"KENDRYTE_SDK_RELEASE_DATE:raw": "'$ND'"#g' "/tmp/kendryte-${PROJ}-sdk/kendryte-package.json"
	cd /tmp

	zip \
		"--exclude=.*" \
		"--exclude=./CMakeLists.txt" \
		"--exclude=.*/*" \
		"--exclude=build/*" \
		"--exclude=kendryte_libraries/*" \
		-ur "$TARGET" "kendryte-${PROJ}-sdk" >/dev/null

	cd -
}

create_sdk_zip freertos
create_sdk_zip standalone

echo "All done."

