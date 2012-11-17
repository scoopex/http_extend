/*
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <pcre.h>
#include <getopt.h>
#include <unistd.h>

#include "globals.h"
#include "callback.h"

#define PACKAGE    "http_extend"
#define VERSION    "1.x"

void print_help(int exval) {

 if (isatty(0) == 1) {
	 printf("%s,%s fetch http urls and extract values\n", PACKAGE, VERSION); 
	 printf("%s [-?] [-V] [-v] [-u URL] [-r PCRE-REGEX]\n\n", PACKAGE);

	 printf("  -?              print this help and exit\n");
	 printf("  -V              print version and exit\n\n");
	 printf("  -v              set verbose flag\n");
	 printf("  -l              follow location redirects\n");
	 printf("  -i              ignore ssl certificat verification\n");
	 printf("  -f              fail request on curl errors (receive buffer of %i bytes exceded, http-errors, ..)\n",MAX_BUF);
	 printf("  -s              provide only the status of the request (zabbix values: 1 = OK, 0 = NOT OK)\n");
	 printf("  -u URL          Specify the url to fetch\n");
	 printf("  -t mseconds     Timeout of curl request in 1/1000 seconds (default: %i milliseconds)\n",TIMEOUT);
	 printf("  -r PCRE-REGEX   Specify the matching regex\n");
	 printf("  -h HOSTNAME     Specify the host header\n\n");
 }
 exit(exval);
}

void print_arguments(int argc, char *argv[]) {
   int i;
   if (isatty(0) != 1) {
	   fprintf(stderr,"ARGUMENTS: ");
	   for (i = 0; i < argc; i++){
		fprintf(stderr,"%s ",argv[i]);
	   }
	   fprintf(stderr,"\n");
	   }
}

int main(int argc, char *argv[]) {

	CURL *curl;
	CURLcode ret;
	char *url = NULL;
	char *host_header = NULL;
	char *host_name = NULL;
	char *regex = NULL;

   /* Commandline switches */
	int verbose = false;
	int status_only = false;
   int nossl_verify = false;
   int follow_location = false;
	int fail_on_curl_error = false;

	struct curl_slist *headers = NULL;
	int i;
	int curl_timeout = TIMEOUT;

	pcre *re;
	const char *error;
	int erroffset;
	int ovector[OVECCOUNT];
	int rc;
	int opt;

	wr_error = 0;
	wr_index = 0;

	/* First step, init curl */
	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "couldn't init curl\n");
		exit(EXIT_FAILURE);
	}

	/* 
	 * if no arguments are given
	 */
	 if(argc == 1) {
	  fprintf(stderr, "This program needs arguments....\n\n");
          print_arguments(argc, argv);
	  print_help(1);
	 }

	 while((opt = getopt(argc, argv, "?Vflsvt:u:h:r:i")) != -1) {
	  switch(opt) {
	   case 'V':
	    printf("%s %s\n\n", PACKAGE, VERSION); 
	    exit(0);
	    break;
	   case 'v':
	    verbose = true;
	    break;
	   case 'i':
	    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	    nossl_verify = true;
	    break;
	   case 's':
	    status_only = 1;
	    break;
	   case 'l':
       follow_location = true;
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	    break;
	   case 'f':
	    fail_on_curl_error = true;
	    break;
	   case 'u':
	    url = optarg;
	    break;
	   case 't':
	    curl_timeout = atoi(optarg);
	    break;
	   case 'h':
	    host_header = malloc(strlen("Host: ") + strlen(optarg) + 1);
	    host_name = optarg;
	    strcpy(host_header, "Host: ");
	    strcat(host_header, optarg);
	    headers = curl_slist_append(headers, host_header);
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	    break;
	   case 'r':
	    regex = optarg;
	    break;
	   case ':':
	    fprintf(stderr, "%s: Error - Option `%c' needs a value\n\n", PACKAGE, optopt);
            print_arguments(argc, argv);
	    print_help(1);
	    break;
	   case '?':
	    fprintf(stderr, "%s: Error - No such option: `%c'\n", PACKAGE, optopt);
            print_arguments(argc, argv);
	    print_help(1);
	   }
	}

	if (verbose == 1){
	    fprintf(stderr, "%-15s %s\n", "URL", url);
	    fprintf(stderr, "%-15s %s\n", "REGEX", regex);
	    fprintf(stderr, "%-15s %i\n", "TIMEOUT", curl_timeout);
	    fprintf(stderr, "%-15s %s\n",   "HOST HEADER", host_header);
	    fprintf(stderr, "%-15s %i\n\n", "STATUS ONLY", status_only);

	    fprintf(stderr, "%-15s %s[-t %i -u \"%s\" -r \"%s\"", "ZABBIX ITEM", PACKAGE, curl_timeout, url, regex);
	    if ( status_only == 1 ){
	    	fprintf(stderr, " -s");
	    }
       if ( nossl_verify == 1 ){
	    	fprintf(stderr, " -i");
	    }
       if ( follow_location == 1 ){
	    	fprintf(stderr, " -l");
	    }
       if ( fail_on_curl_error == 1 ){
	    	fprintf(stderr, " -f");
	    }

	    if ( host_name != NULL ){
	     	fprintf(stderr, " -h %s",host_name);
  	    }
	   fprintf(stderr, "]\n");

	}

	
	if ((url == NULL) || (regex == NULL)){
      print_arguments(argc, argv);
		print_help(EXIT_FAILURE);
	}

	/* Tell curl the URL of the file we're going to retrieve */
	curl_easy_setopt(curl, CURLOPT_URL, url);
   curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.2) Gecko/20090729 Firefox/3.5.2 GTB5");

	/* Tell curl that we'll receive data to the function write_data, and
	 * also provide it with a context pointer for our error return.
	 */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &wr_error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, curl_timeout);

	/* Allow curl to perform the action */
	ret = curl_easy_perform(curl);

	/* Stop execution here if only status is needed */
	if ((ret != 0) && (fail_on_curl_error == 1)){
		if (status_only == 1){
			printf("0");	
		}
		exit(EXIT_FAILURE);
	}

	re = pcre_compile(regex,	/* the pattern */
					  0,		/* default options */
					  &error,	/* for error message */
					  &erroffset,	/* for error offset */
					  NULL);	/* use default character tables */


	rc = pcre_exec(re,			/* the compiled pattern */
				   NULL,		/* no extra data - we didn't study the pattern */
				   wr_buf,		/* the subject string */
				   wr_index,	/* the length of the subject */
				   0,			/* start at offset 0 in the subject */
				   0,			/* default options */
				   ovector,		/* output vector for substring information */
				   OVECCOUNT);	/* number of elements in the output vector */

	if(verbose == 1) {
		fprintf(stderr, "out: %s\n", wr_buf);
	}

	/* Evaluate the match and output status */	
	if(rc < 0) {

		if (status_only == 1){
			printf("0");	
		}else{
			switch (rc) {
			case PCRE_ERROR_NOMATCH:
				printf("Damn, no match in http_extend\n");
				break;
				/*
				   Handle other special cases if you like
				 */
			default:
				printf("Matching error %d\n", rc);
				break;
			}
		}
		pcre_free(re);			/* Release memory used for the compiled pattern */
		exit(EXIT_FAILURE);
	}

	if(rc == 2) {
		if (status_only == 1){
			printf("1");	
		}else{
			char *substring_start = NULL;
			int substring_length = 0;
			i = 1;
			substring_start = wr_buf + ovector[2 * i];
			substring_length = ovector[2 * i + 1] - ovector[2 * i];
			printf("%.*s\n", substring_length, substring_start);
		}
	}

	curl_easy_cleanup(curl);

	exit(EXIT_SUCCESS);
}

/* vim:ai et ts=2 shiftwidth=2 expandtab tabstop=3 tw=120 */
