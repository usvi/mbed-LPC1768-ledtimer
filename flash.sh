#/bin/bash

MOUNT_BASE=$1
BIN=$2

if [ -z "$MOUNT_BASE" ];
then
    echo "No mount point base given!"
    exit 1
fi

if [ -z $BIN ];
then
    echo "No flashing binary given!"
    exit 1
fi

MEDIA_BASE=`find $MOUNT_BASE | grep MBED | head -n 1 | sed s/\\\/MBED.*//;`

if [ ! -d "$MEDIA_BASE"  -o ! "$MEDIA_BASE/MBED" ];
then
    echo "Failed to figure out MBED location!"
    exit 1
fi

cp $BIN $MEDIA_BASE/MBED
RESULT=$?
sync

if [ $RESULT -eq 0 ];
then
    echo "Flashed successfully $BIN to $MEDIA_BASE/MBED !"
    echo "Press reset button on mbed to run the binary."
    exit 0
else
    echo "Failed to flash $BIN to $MEDIA_BASE/MBED !"
    exit 1
fi

