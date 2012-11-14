http_extend
============

"HTTP-Monitoring" Tool for Zabbix

This tool can be utilized to gather behavioral measurement data or configuration data of webservers/appplication servers.
It can be used as an external check for the zabbix monitoring system.

Zabbix is already capable to monitor via the http protocol - the reasons for using this tool are:
 * Zabbix web scenarios cannot be executed on proxies
 * Zabbix is not capable to parse a value out of a website
 * Zabbix is not capable to set a host header
 * Zabbix is not capable to ignore ssl errors


Compile and Install
-------------------

Install the tool on zabbix-proxies, zabbix-servers or zabbix-nodes

Prerequisites:
 * C-compiler
 * GNU Make
 * Libpcre and Libpcre-Dev
 * Libcurl and Libcurl-Dev

On Ubuntu just do
(This will install the binary 'mysql_extend' to /etc/zabbix/externalscripts):
```
git clone git://github.com/digitalmediacenter/http_extend.git http_extend
cd http_extend
sudo apt-get install libpcre3 libpcre3-dev gcc-4.4 libcurl3 libcurl4-openssl-dev
make
make install
```

Usage
-----

Examples:
```
./http_extend -h
./http_extend -v -u "https://www.foobar.org/server-status?auto" -r "Total Accesses: (\d+)"
./http_extend -v -l -t 5000 -u 'http://10.1.1.1:80/men/shirts' -r '(Impressum)' -s -h www.foobarshop.de
```

Gather data from a apache-server-status page and add it as an measure to zabbix.
```
Description........: Apache Requests
Type...............: External check
Key................: http_extend[-t 5000 -u "https://{HOSTNAME}/server-status?auto" -r "Total Accesses: (\d+)"]
Type of information: Numeric (unsigned)
Data type..........: Decimal
Update interval....: 300
Store value........: Delta (simple change)
```

Check a website on a host behind a loadbalancer by setting the hostheader.
```
Description........: Check for "Impressum"
Type...............: External check
Key................: http_extend[-l -t {$TIMEOUT} -u 'http://{IPADDRESS}:80/men/shirts' -r '(Impressum)' -s -h www.foobarshop.de]
Type of information: Numeric (unsigned)
Data type..........: Decimal
Update interval....: 300
Store value........: As is
Show value.........: Service state
```

The first matching regex group is printed to STDOUT. All other output is printed to STDERR.

Missing features
----------------
 * Check ssl certificates for the remaining days of validness

Licence and Authors
-------------------

Additional authors are very welcome - just submit you patches as pull requests.

 * Marc Schoechlin <marc.schoechlin@dmc.de>
 * Andreas Heil <andreas.heil@dmc.de>

License - see: LICENSE.txt
