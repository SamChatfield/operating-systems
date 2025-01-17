#!/bin/bash

set -e
./sort < in1.txt > /tmp/in1_sorted.txt
diff /tmp/in1_sorted.txt in1_sorted.txt
./sort < in2.txt > /tmp/in2_sorted.txt
diff /tmp/in2_sorted.txt in2_sorted.txt
./sort < in3.txt > /tmp/in3_sorted.txt
diff /tmp/in3_sorted.txt in3_sorted.txt
./sort < in4.txt > /tmp/in4_sorted.txt
diff /tmp/in4_sorted.txt in4_sorted.txt
./sort < faust.txt > /tmp/faust_sorted.txt
diff /tmp/faust_sorted.txt faust_sorted.txt
echo OK
