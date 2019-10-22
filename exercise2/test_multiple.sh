#!/bin/bash

### CONFIG ###
PORT=1234
##############

set -e

#start server
test -x ./serverThreaded || ( echo "serverThreaded not found"; exit 1 )
test -x ./client || ( echo "client not found"; exit 1 )
./serverThreaded $PORT /tmp/logfile.txt &
sleep 5

#test with client
(sleep 6; cat test_in2.txt) | ./client 127.0.0.1 $PORT &
sleep 3
./client 127.0.0.1 $PORT < test_in1.txt
sleep 10

#stop all background processes
case $(jobs -rp) in
    "") echo Server has already died; exit 1;;
    *)
	kill -PIPE $(jobs -rp);;
esac

#check output
diff /tmp/logfile.txt test_out.txt
echo OK

