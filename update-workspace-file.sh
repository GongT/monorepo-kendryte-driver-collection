#!/bin/bash

#if [[ "${BASH_VERSION:0:1}${BASH_VERSION:2:1}" -lt "42" ]]; then
#	echo "OMG, your bash is so damn old, update it now!"
#fi

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

MONO_FILE_STANDALONE="mono-workspace-standalone.code-workspace"
MONO_FILE_FREERTOS="mono-workspace-freertos.code-workspace"

echo '{"folders":[' > $MONO_FILE_STANDALONE
echo '{"folders":[' > $MONO_FILE_FREERTOS

function output_driver() {
	local P="$1"
	local TYPE="$2"
	if ! [[ -e "$P/$TYPE/kendryte-package.json" ]]; then
		mkdir -p "$P/$TYPE"
		return
	fi
	echo -n '{"name": "驱动: '
	echo -n "$(basename "$P")"
	echo -n '", "path": "'
	echo -n "$P/$TYPE"
	echo '"},'
	mkdir -p "$P/$TYPE"
}
function output_demo() {
	local P="$1"
	local TYPE="$2"
	if ! [[ -e "$P/$TYPE/kendryte-package.json" ]]; then
		mkdir -p "$P/$TYPE"
		return
	fi
	echo -n '{"name": "示例: '
	echo -n "$(basename "$P")"
	echo -n '", "path": "'
	echo -n "$P/$TYPE"
	echo '"},'
}
function output_library() {
	local P="$1"
	local TYPE="$2"
	if [[ -e "$P/kendryte-package.json" ]]; then
		echo -n '{"name": "库: '
		echo -n "$(basename "$P")"
		echo -n '", "path": "'
		echo -n "$P"
		echo '"},'
		return
	fi
	mkdir -p "$P"

	if ! [[ -e "$P/$TYPE/kendryte-package.json" ]]; then
		mkdir -p "$P/$TYPE"
		return
	fi
	echo -n '{"name": "库: '
	echo -n "$(basename "$P")"
	echo -n '", "path": "'
	echo -n "$P/$TYPE"
	echo '"},'
}
function output_board() {
	local P="$1"
	if ! [[ -e "$P/kendryte-package.json" ]]; then
		mkdir -p "$P"
		return
	fi
	echo -n '{"name": "开发板: '
	echo -n "$(basename "$P")"
	echo -n '", "path": "'
	echo -n "$P"
	echo '"},'
}
export -f output_board
export -f output_library
export -f output_driver
export -f output_demo

ls -d demos/*/ | xargs -IF bash -c "output_demo 'F' standalone" >> $MONO_FILE_STANDALONE
ls -d drivers/*/ | xargs -IF bash -c "output_driver 'F' standalone" >> $MONO_FILE_STANDALONE
ls -d libraries/*/ | xargs -IF bash -c "output_library 'F' standalone" >> $MONO_FILE_STANDALONE
ls -d boards/*/ | xargs -IF bash -c "output_board 'F'" >> $MONO_FILE_STANDALONE
echo '{"name": "Standalone SDK", "path": "kendryte-standalone-sdk"}' >> $MONO_FILE_STANDALONE

ls -d demos/*/ | xargs -IF bash -c "output_demo 'F' freertos" >> $MONO_FILE_FREERTOS
ls -d drivers/*/ | xargs -IF bash -c "output_driver 'F' freertos" >> $MONO_FILE_FREERTOS
ls -d libraries/*/ | xargs -IF bash -c "output_library 'F' freertos" >> $MONO_FILE_FREERTOS
ls -d boards/*/ | xargs -IF bash -c "output_board 'F'" >> $MONO_FILE_FREERTOS
echo '{"name": "Freertos SDK", "path": "kendryte-freertos-sdk"}' >> $MONO_FILE_FREERTOS

echo "]}" >> $MONO_FILE_STANDALONE
echo "]}" >> $MONO_FILE_FREERTOS



