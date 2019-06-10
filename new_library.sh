#!/bin/bash

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

PROJ=$1

if [[ -z "$PROJ" ]] ; then
	echo -e "Usage: $0 \e[38;5;14m<ProjectName>\e[0m
Eg: $0 led to create new library 'led'" >&2
	exit 1
fi

PACKAGE="libraries/$PROJ/kendryte-package.json"

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
    "name": "kendryte/${PROJ}",
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
