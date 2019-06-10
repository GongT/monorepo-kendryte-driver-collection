#!/bin/bash

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

PROJ=$1

if [[ -z "$PROJ" ]] ; then
	echo -e "Usage: $0 \e[38;5;14m<BoardName>\e[0m
Eg: $0 KD233 to create new board 'KD233'" >&2
	exit 1
fi

PACKAGE="boards/$PROJ/kendryte-package.json"

echo "Will create '$PACKAGE' file"
if [[ -e "$PACKAGE" ]]; then
	echo "Error: it exists."
	exit 1
fi

mkdir -p "$(dirname "$PACKAGE")"
cat > "$PACKAGE" <<PACKAGE_FILE 
{
    "name": "kendryte/${PROJ}",
    "version": "develop",
    "type": "define",
    "dependency": {
    },
    "homepage": "https://github.com/GongT/monorepo-kendryte-driver-collection"
}
PACKAGE_FILE
echo "Done. (Tip: run update-workspace-file.sh)"
