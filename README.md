http\_extend
===========

"HTTP-Monitoring" Tool for Zabbix

This is a fork of https://github.com/digitalmediacenter/http_extend 

This tool can be used as an easy way to gather information about a webserver and/or application server. This information is generated by the application/webserver and must be available over HTTP.
It can be used as an external check for the zabbix monitoring system.
Http\_extend is written in c, so it creates very minimal overhead when Zabbix calls this tool as "External check".

Though Zabbix is already capable of monitoring via HTTP, the reasons for using this tool are:
(see also https://support.zabbix.com/browse/ZBXNEXT-989)

 * Zabbix web scenarios cannot be executed on proxies (Zabbix <= 2.0)
 * Zabbix cannot parse a value out of a website without using a agent (see also web.page.regexp[])
 * Zabbix cannot set HTTP headers
 * Zabbix cannot ignore SSL errors
 * Zabbix cannot monitor the remaining valid days of a  ssl-certificate
 * Zabbix cannot monitor two identicat jmx/beans on the same host but different tomcats/jvms (fetching the values by http/proxyservlet is a solution, because this prevents duplicate keys)


Compile and Install
-------------------

Install the tool on zabbix-proxies, zabbix-servers or zabbix-nodes

Prerequisites:
 * C-compiler
 * GNU Make
 * libpcre
 * libcurl
 * libssl

On Ubuntu just do
(This will install the binary 'http\_extend' to /etc/zabbix/externalscripts):
```
git clone git://github.com/digitalmediacenter/http_extend.git
cd http_extend
sudo apt-get install build-essential libpcre3-dev libcurl4-openssl-dev libssl-dev
make
# Install to /etc/zabbix/externalscripts
make install
# Install to /usr/local/bin
make INSTALLDIR=/usr/local/bin install
```

Usage
-----

Features:
```
$ ./http_extend 
This program needs arguments....

http_extend,1.x fetch http urls and extract values
http_extend [-?] [-V] [-v] [-u URL] [-r PCRE-REGEX]

  -?              print this help and exit
  -V              print version and exit

  -v              set verbose flag (repeat for more output)
  -a              use the entire http/https response for contect parsing (i.e. headers)
  -l              follow location redirects
  -i              ignore ssl certificate verification
  -f              fail request on curl errors (receive buffer of 5242880 bytes exceded, http-errors, ..)
  -s              provide only the status of the request (zabbix values: 1 = OK, 0 = NOT OK, )
  -m              provide the total delivery time of the request in seconds (zabbix values: >0.0 = OK (seconds), 0.0 = NOT OK)
  -c              check ssl certificate and return days until expire
  -u URL          Specify the url to fetch
  -t mseconds     Timeout of curl request in 1/1000 seconds (default: 5000 milliseconds)
  -r PCRE-REGEX   Specify the matching regex
  -h HOSTNAME     Specify the host header
```

Examples:
```
./http_extend -h
# Parse apache server status and gain "Total Accesses" value
./http_extend -v -u "https://www.foobar.org/server-status?auto" -r "Total Accesses: (\d+)"
# Check if string "Impressum" is in the response and return "1", otherwise return "0"
./http_extend -v -l -t 5000 -u 'http://10.1.1.1:80/men/shirts' -r '(Impressum)' -s -h www.foobarshop.de
# Check if header value "X-Cache-Lookup: HIT from" is set and return "1", 
# otherwise return "0" (show whole answer for debugging purposes)
./http_extend  -l -u http://foobar.com/ -r '(X-Cache-Lookup: HIT from)' -s  -t 100000 -vvv -a
# Get days until the SSL certificate expires
./http_extend -c -u https://goobar.org:443
# Gather java heap information data from a tomcat JMX proxyservlet
./http_extend -u "http://foouser:barpassword@10.218.51.21:9001/manager/jmxproxy?qry=java.lang:type=Memory" -r "HeapMemoryUsage: .*used=(\d+)" -vvvvv
```

Gather data from a apache-server-status page and add it as an measure to zabbix.
```
Description........: Apache Requests
Type...............: External check
Key Zabbix 1.8.....: http_extend[-t 5000 -u "https://{HOSTNAME}/server-status?auto" -r "Total Accesses: (\d+)"]
Key Zabbix 2.x.....: http_extend[-t,5000,-u,"https://{HOSTNAME}/server-status?auto",-r,"Total Accesses: (\d+)"]
Type of information: Numeric (unsigned)
Data type..........: Decimal
Update interval....: 300
Store value........: Delta (speed per second)
```

Check a website on a host behind a loadbalancer by setting the hostheader.
```
Description........: Check for "Impressum"
Type...............: External check
Key Zabbix 1.8.....: http_extend[-l -t {$TIMEOUT} -u 'http://{IPADDRESS}:80/men/shirts' -r '(Impressum)' -s -h www.foobarshop.de]
Key Zabbix 2.0.....: http_extend[-l,-t,{$TIMEOUT},-u,'http://{IPADDRESS}:80/men/shirts',-r,'(Impressum)',-s,-h,www.foobarshop.de]
Type of information: Numeric (unsigned)
Data type..........: Decimal
Update interval....: 300
Store value........: As is
Show value.........: Service state
```

The first matching regex group is printed to STDOUT. All other output is printed to STDERR.

See also subfolder "zabbix\_templates/".

Tips / Good to know
-------------------

Timeout:

You may have to increase timeout in Zabbix server configuration for external check:
```
Default:  3 seconds
Max:     30 seconds
```

More information can be found in documentation: https://www.zabbix.com/documentation/2.2/manual/appendix/config/zabbix_server

Missing features
----------------
- isupport for parsing values from json documents
  (i.e. by using https://stedolan.github.io/jq/)
- caching of request responses beetween different calls to reduce request overhead
  (i.e. use a shared memory segment and invalidate the cache after configurable amount of seconds)
- convert this functionality to zabbix module
  (see http://blog.zabbix.com/zabbix-2-2-features-part-10-support-of-loadable-modules/2379/ and
  https://www.zabbix.com/documentation/2.2/manual/config/items/loadablemodules)

Licence and Authors
-------------------

Additional authors are very welcome - just submit your patches as pull requests.

 * Marc Schoechlin <ms@256bit.org>
 * Marc Schoechlin <marc.schoechlin@dmc.de>
 * Marc Schoechlin <marc.schoechlin@breuninger.de>
 * Andreas Heil <andreas.heil@dmc.de>

License - see: LICENSE.txt
