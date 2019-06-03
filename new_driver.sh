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
esac

if [[ -z "$PROJ" ]] || [[ -z "$TYPE" ]] ; then
	echo -e "Usage: $0 \e[38;5;14m<ProjectName>\e[0m \e[38;5;9m<'s' or 'r'>\e[0m
Eg: $0 pwm s to create new driver 'pwm' with standalone sdk" >&2
	exit 1
fi

PACKAGE="drivers/$PROJ/$TYPE/kendryte-package.json"

echo "Will create '$PACKAGE' file"
if [[ -e "$PACKAGE" ]]; then
	echo "Error: it exists."
	exit 1
fi

mkdir -p "drivers/$PROJ/$TYPE/include"
mkdir -p "drivers/$PROJ/$TYPE/src"

cat > "$PACKAGE" <<PACKAGE_FILE 
{
    "name": "kendryte_${PROJ}-${TYPE}-driver",
    "version": "develop",
    "type": "library",
    "dependency": {
        "kendryte-${TYPE}-sdk": "develop"
    },
    "source": [
        "src/${PROJ}.c"
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
