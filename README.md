http_extend
===========

This repository contains a tool to be used as a zabbix external script to monitor http(s)-based services ## Installation

Usage
=====

   $ ./http_extend 
   This program needs arguments....

   http_extend,0.0.1 fetch http urls and extract values
   http_extend [-?] [-V] [-v] [-u URL] [-r PCRE-REGEX]

     -?              print this help and exit
     -V              print version and exit

     -v              set verbose flag
     -l              follow location redirects
     -i              ignore ssl certificat verification
     -f              fail request on curl errors (receive buffer of 5242880 bytes exceded, http-errors, ..)
     -s              provide only the status of the request (zabbix values: 1 = OK, 0 = NOT OK)
     -u URL          Specify the url to fetch
     -t mseconds     Timeout of curl request in 1/1000 seconds (default: 5000 milliseconds)
     -r PCRE-REGEX   Specify the matching regex
     -h HOSTNAME     Specify the host header

Examples
========


Compile
========
* Ubuntu
  apt-get install libpcre3 libpcre3-dev gcc-4.4
  make 
  make install
