/*
 * globals.h
 *
 * vi:set ts=4:
 */
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
void print_arguments(int argc, char *argv[]);

#endif							/* GLOBALS_H */
