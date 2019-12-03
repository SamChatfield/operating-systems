#!/bin/sh

# load firewall module
insmod firewallExtension.ko || { echo "Cannot insert firewallExtension module" ; exit 1; }

./Setup/firewallSetup W Setup/rules.txt
wget -O /dev/null http://www.cs.bham.ac.uk/ || { echo "Wget test failed"; exit 1 ;} 
echo "Wget test passed"
curl -o /dev/null http://www.cs.bham.ac.uk/ && { echo "Curl test failed"; exit 1 ;}
echo "Curl test passed"
curl -o /dev/null https://www.cs.bham.ac.uk/ || { echo "Curl ssl test failed"; exit 1 ;}
echo "Curl ssl test passed"
rmmod firewallExtension || { echo "cannot remove firewall Extension module"; exit 1 ;}
echo "OK"
exit 0
