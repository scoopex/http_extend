#!/bin/bash

FAILED_TESTS=""

assertSuccess(){
 CMD="$1"
 echo "+ $CMD";
 echo -n ">>>"
 eval "$CMD 2>&1"
 RET="$?"
 echo "<<<"
 if [ "$RET" != "0" ];then
   echo "=> ERROR: execution failed (returncode $RET)"
   FAILED_TESTS="$FAILED_TESTS#assertSuccess => $CMD"
   return 100
 fi
}

assertFail(){
 CMD="$1"
 echo "+ $CMD";
 echo -n ">>>"
 $CMD
 RET="$?"
 echo "<<<"
 if [ "$RET" == "0" ];then
   echo "=> ERROR: execution successful (returncode $RET)"
   FAILED_TESTS="$FAILED_TESTS#assertFail => $CMD"
   return 100
 fi
}

echo "*** TESTS"
assertSuccess './http_extend -l -u https://www.google.de -ci -vv'
assertSuccess './http_extend -v -l -u http://www.google.de -r "(.*body.*)"'
assertSuccess './http_extend -s -l -u http://www.google.de -r "(.*body.*)"'
assertSuccess './http_extend -l -m -u http://www.google.de -r "(.*body.*)"'
assertSuccess './http_extend -u http://www.google.de -r ".*(body).*"|grep "^body$"'

assertFail './http_extend -t 1 -m -u http://www.google.de -r "(.*body.*)"'
echo


echo "*** SUMMARY"
if [ -z "$FAILED_TESTS" ];then
   echo "ALL TESTS PASSED"
   exit 0
else
   echo "THE FOLLOWING TESTS FAILED"
   echo "$FAILED_TESTS"|tr '#' '\n'|sed '~s,^, ,'
   exit 1
fi
