#!/bin/sh

# Usage: launch-router.sh router-FQDN port-no config-servers 

if [ $# -lt "3" ]; then
    echo "Invalid call to $0";
    exit 1;
fi

ROUTER=$1
PORTNO=$2
CONFIGSVRS=$3

ssh mongodb@$ROUTER 'test -f .puppetcomplete'
if [ $? -ne "0" ]; then
    exit 2;
fi

ssh mongodb@$ROUTER "mongos --fork --logpath /data/mongo/mongos.log --configdb $CONFIGSVRS --port $PORTNO"
if [ $? -ne "0" ]; then
    exit 3;
fi
