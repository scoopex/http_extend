/*
 * callback.h
 *
 * vi:set ts=4:
 */
#ifndef CALLBACK_H
#define CALLBACK_H

/*
 * Write data callback function (called within the context of 
 * curl_easy_perform.
 * TODO: document parameters and return value
 */
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

#endif							/* CALLBACK_H */
