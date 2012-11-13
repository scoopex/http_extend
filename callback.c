/*
 * callback.c
 *
 * vi:set ts=4:
 */
#include <string.h>

#include "globals.h"
#include "callback.h"

/*
 * Write data callback function (called within the context of 
 * curl_easy_perform.
 */
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	int segsize = size * nmemb;

	/* Check to see if this data exceeds the size of our buffer. If so, 
	 * set the user-defined context value and return 0 to indicate a
	 * problem to curl.
	 */
	if(wr_index + segsize > MAX_BUF) {
		*(int *) userp = 1;
		return 0;
	}

	/* Copy the data from the curl buffer into our buffer */
	memcpy((void *) &wr_buf[wr_index], buffer, (size_t) segsize);

	/* Update the write index */
	wr_index += segsize;

	/* Null terminate the buffer */
	wr_buf[wr_index] = 0;

	/* Return the number of bytes received, indicating to curl that all is okay */
	return segsize;
}
