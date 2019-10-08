#!/bin/bash

set -e
./sort < in1.txt > /tmp/in1_sorted.txt
diff /tmp/in1_sorted.txt in1_sorted.txt
./sort < in2.txt > /tmp/in2_sorted.txt
diff /tmp/in2_sorted.txt in2_sorted.txt
./sort < in3.txt > /tmp/in3_sorted.txt
diff /tmp/in3_sorted.txt in3_sorted.txt
echo OK
