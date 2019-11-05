#!/bin/bash

#globals
ret=0
d=/dev/opsysmem
executable=charDeviceDriver

# --- helper function ---
function run(){
    echo -e "$1:"
    $1 #execute
    #check errors
    tmp=$?
    if [ $tmp -ne 0 ]; then
        ret=$tmp
    fi
    echo "" #newline
    return $tmp
}


# --- TESTCASES ---
function basic_testcase(){
	t="testcase 1"
	
	#cleanup
	rmmod $executable 2>/dev/null >/dev/null
	rm -f $d
	
	#load kernel driver
    echo -en " load kernel driver:\t"
	insmod ./$executable.ko
	if [ $? -ne 0 ]
	then
		echo -e "ERROR: insmod failed"
		return 1
    else
        echo "ok"
	fi

	#mknod
    echo -en " mknod:\t\t\t"
    major=`dmesg | tail -n 6 | sed -n "s/\[[0-9. ]*\] 'mknod \/dev\/opsysmem c \([0-9]*\) 0'\./\1/p"`
    case $major in
	"" )
	    echo -e "ERROR: cannot find major number in /var/log/syslog, probably the chardev-module isn't implemented properly";
	    return 1
    esac
	if [ $major -eq 0 ]
	then
	    echo -e "ERROR: major = 0, probably the module isn't implemented"
	    rmmod $executeable
	    return 1
	fi
	mknod $d c $major 0
	if [ $? -ne 0 ]
	then
		echo -e "ERROR: mknod command failed"
		rmmod $executeable
		return 1
    else
        echo "ok"
	fi
	
	#check file
    echo -en " ls $d:\t"
	line=`ls $d 2>/dev/null`
	if [ "$line" != "$d" ]
	then
		echo -e "ERROR: file $d does not exist after loading the kernel module"
		rmmod $executable
		return 1
    else
        echo "ok"
	fi
	
	#write test
    echo -en " write test:\t\t"
	echo "$t" > $d 2>/dev/null
	if [ $? -ne 0 ]
	then
		echo -e "ERROR: writing $d failed"
		rmmod $executable
		return 1
    else
        echo "ok"
	fi

	#read test
    echo -en " read test:\t\t"
	r=`head -n 1 < $d`
	if [ $? -ne 0 ]
	then
		echo -e "ERROR: reading $d failed"
		rmmod $executable
		return 1
	fi
	
	#check if same was read
	if [ "$r" != "$t" ]
	then
		echo -e "ERROR: $d: could not read what was written before"
		rmmod $executable
		return 1
    else
        echo "ok"
	fi

	#unload module
    echo -en " unload module:\t\t"
	rmmod $executable
	if [ $? -ne 0 ]
	then
		echo -e "ERROR: unloading kernel module failed"
		return 1
    else
        echo "ok"
	fi

	return 0
}

# --- execution ---
#reset
rmmod $executable 2>/dev/null

run basic_testcase
#cleanup
rm -f $d

exit $ret

