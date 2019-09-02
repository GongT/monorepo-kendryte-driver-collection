#!/bin/bash

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

PROJ=$1
case "$2" in
s)
	TYPE="standalone"
	;;
r)
	TYPE="freertos"
	;;
c)
	TYPE="common"
	;;
esac

if [[ -z "$PROJ" ]] || [[ -z "$TYPE" ]] ; then
	echo -e "Usage: $0 \e[38;5;14m<ProjectName> \e[38;5;9m<'s' or 'r' or 'c'>\e[0m
Eg: '$0 image-process c' to create new common library 'image-process'" >&2
	exit 1
fi

if [[ "$TYPE" == "common" ]]; then
    TYPE=''
    MTYPE=''
else
    MTYPE="-${TYPE}"
fi

PACKAGE="libraries/$PROJ/$TYPE/kendryte-package.json"

echo "Will create '$PACKAGE' file"
if [[ -e "$PACKAGE" ]]; then
	echo "Error: it exists."
	exit 1
fi

mkdir -p "$(dirname "$PACKAGE")"
mkdir -p "$(dirname "$PACKAGE")/include"
mkdir -p "$(dirname "$PACKAGE")/src"
cat > "$PACKAGE" <<PACKAGE_FILE
{
    "name": "kendryte/${PROJ}${MTYPE}",
    "version": "develop",
    "type": "library",
    "dependency": {
    },
    "source": [
        "src/*.c"
    ],
    "include": [
        "include"
    ],
    "header": [
        "src"
    ],
    "homepage": "https://github.com/GongT/monorepo-kendryte-driver-collection"
}
PACKAGE_FILE
echo "Done. (Tip: run update-workspace-file.sh)"
