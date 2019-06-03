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
	echo -n '{"name": "Driver: '
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
	echo -n '{"name": "Demo: '
	echo -n "$(basename "$P")"
	echo -n '", "path": "'
	echo -n "$P/$TYPE"
	echo '"},'
}
export -f output_driver
export -f output_demo

ls -d demos/*/ | xargs -IF bash -c "output_demo 'F' standalone" >> $MONO_FILE_STANDALONE
ls -d drivers/*/ | xargs -IF bash -c "output_driver 'F' standalone" >> $MONO_FILE_STANDALONE
echo '{"name": "Standalone SDK", "path": "kendryte-standalone-sdk"}' >> $MONO_FILE_STANDALONE

ls -d demos/*/ | xargs -IF bash -c "output_demo 'F' freertos" >> $MONO_FILE_FREERTOS
ls -d drivers/*/ | xargs -IF bash -c "output_driver 'F' freertos" >> $MONO_FILE_FREERTOS
echo '{"name": "Freertos SDK", "path": "kendryte-freertos-sdk"}' >> $MONO_FILE_FREERTOS

echo "]}" >> $MONO_FILE_STANDALONE
echo "]}" >> $MONO_FILE_FREERTOS



