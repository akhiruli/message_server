#!/bin/sh

#
# for this test in app.conf "TIME_INTERVAL_BTN_MSG" parameter need to set 10 and number of message to be send 6 from client
#

if [ "$#" -ne 1 ]; then
    echo "Please pass number of connection as argument"
    exit
fi

id=0
NUMCONN=$1
while [ $id -le $NUMCONN ]
do
    ../bin/msgclient $id ms1,msg2,msg3,msg4,msg5,msg6 >> /dev/null 2>&1 &
    id=`expr $id + 1`
    
    if [ `expr $id % 100` -eq 0 ];then
        echo "number of request sent $id"
    fi
done
