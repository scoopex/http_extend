/*
 * globals.h
 *
 * vi:set ts=4:
 */
#include <sys/time.h>

#ifndef GLOBALS_H
#define GLOBALS_H

/* TODO: document these defines */
#define MAX_BUF   5242880
#define OVECCOUNT 6				/* should be a multiple of 3 */
#define TIMEOUT 5000

/* http://curl.haxx.se/libcurl/c/libcurl-tutorial.html */

/* TODO: document these global variables */
char wr_buf[MAX_BUF + 1];
int wr_index;
int wr_error;
void print_help(int exval);
void print_error_with_arguments(int argc, char *argv[],const char *msg);
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);


#endif							/* GLOBALS_H */
