#!/bin/sh

./test_list > /tmp/test_list.txt
diff /tmp/test_list.txt test_list_output.txt && echo "OK"
