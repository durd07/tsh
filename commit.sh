#########################################################################
## File Name   : ChangeLog.sh
## Author      : xhyz
## E-maill     : xhyz008@163.com
## Created Time: Fri Oct 25 15:42:04 2013
#########################################################################
#!/bin/bash
git diff > diff.txt
echo "=====================================================" >> diff.txt
git log -1 >> diff.txt
echo "" >> diff.txt
cat ChangeLog >> diff.txt
mv diff.txt ChangeLog
git commit -a
