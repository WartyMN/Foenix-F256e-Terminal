#!/bin/zsh

function errorexit () {
    echo "Error: $1"
    exit 1
}

if [ "$#" -lt 1 ]; then
    errorexit "Need to specify argument file"
fi

if [ ! -f ${1} ]; then
    errorexit "File ${1} not found, aborting."
fi

## ftp it to the linux box
sftp micahbly@192.168.0.19 <<EOF
cd Documents/fterm
put ${1}
exit
EOF