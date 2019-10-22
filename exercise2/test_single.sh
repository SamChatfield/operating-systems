#!/bin/bash

### CONFIG ###
PORT=1234
##############

set -e

#start server
test -x ./serverSingle || ( echo "serverSingle not found"; exit 1 )
test -x ./client || ( echo "client not found"; exit 1 )
rm -f /tmp/logfile.txt
./serverSingle $PORT /tmp/logfile.txt &
sleep 5

./client 127.0.0.1 $PORT < test_in1.txt
sleep 1
#stop all background processes
case $(jobs -rp) in
    "") echo Server has already died; exit 1;;
    *)
	kill -PIPE $(jobs -rp);;
esac
	

#check output
diff /tmp/logfile.txt test_outSingle.txt
echo OK

