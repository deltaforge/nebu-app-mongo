#!/bin/sh

# Usage: launch-configsvr.sh configsvr-FQDN port-no

if [ $# -lt "2" ]; then
    echo "Invalid call to $0";
    exit 1;
fi

CONFIGSVR=$1
PORTNO=$2

ssh mongodb@$CONFIGSVR 'test -f .puppetcomplete'
if [ $? -ne "0" ]; then
    exit 2;
fi

ssh mongodb@$CONFIGSVR "mkdir -p /data/mongo/configsvr"
ssh mongodb@$CONFIGSVR "mongod --fork --logpath /data/mongo/configsvr.log --configsvr --dbpath /data/mongo/configsvr --port $PORTNO"
if [ $? -ne "0" ]; then
    exit 3;
fi
