#!/bin/sh

# Usage: launch-dataserver.sh dataserver-FQDN port-no repl-set

if [ $# -lt "3" ]; then
    echo "Invalid call to $0";
    exit 1;
fi

DATASERVER=$1
PORTNO=$2
REPLSET=$3

ssh mongodb@$DATASERVER 'test -f .puppetcomplete'
if [ $? -ne "0" ]; then
    exit 2;
fi

ssh mongodb@$DATASERVER "mkdir -p /data/mongo/mongod-${REPLSET}"
ssh mongodb@$DATASERVER "mongod --fork --logpath /data/mongo/mongod-${REPLSET}.log --dbpath /data/mongo/mongod-${REPLSET} --replSet ${REPLSET} --port $PORTNO"
if [ $? -ne "0" ]; then
    exit 3;
fi
