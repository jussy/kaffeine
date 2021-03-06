/**
 * File:        kaffeine.h
 * Author:      Justin Lewis Salmon
 * Student ID:  10000937
 * Created on:  28 February 2012
 * 
 * Defines constants, structs, function definitions and global variables 
 * for the kaffeine server.
 */

#ifndef KAFFEINE_H_
#define	KAFFEINE_H_

#define USR_PORT 	60000
#define MAX_Q_SIZE 	10
#define MAX_DATA_SIZE 	1024

#define HTCPCP_VERSION  "HTCPCP/1.0 "
#define METHOD_BREW     "BREW"
#define METHOD_POST     "POST"
#define METHOD_GET      "GET"
#define METHOD_POUR     "POUR"
#define METHOD_WHEN     "WHEN"
#define METHOD_PROPFIND "PROPFIND"
#define CONTENT_TYPE    "Content-Type: message/coffeepot\r\n\r\n"
#define SAFE_YES        "Safe: yes\r\n"
#define SAFE_NO         "Safe: no\r\n"        

/* Status codes */
#define C_200           "200 OK\r\n"
#define C_400           "400 Bad Request\r\n"
#define C_404           "404 Not Found\r\n"
#define C_406           "406 Not Acceptable\r\n"
#define C_418           "418 I'm A Teapot\r\n"
#define C_419           "419 Pot Turned Off\r\n"
#define C_420           "420 Pot In Use\r\n"
#define C_421           "421 Still Brewing\r\n"
#define C_422           "422 Still Pouring\r\n"
#define C_423           "423 Already Brewing\r\n"
#define C_424           "424 Already Pouring\r\n"
#define C_425           "425 Not Pouring\r\n"
#define C_426           "426 Cup Waiting\r\n"
#define C_427           "427 No Cup\r\n"
#define C_428           "428 Waiting For Additions\r\n"
#define C_503           "503 Service Unavailable\r\n"
#define C_504           "504 Cup Overflow\r\n"
#define C_505           "505 Cup Gone Cold\r\n"

/* Status messages */
#define M_200_BREW      "Your coffee is brewing. ETC (Estimated Time to Caffeination) = 20 seconds\r\n"
#define M_200_POUR      "Your additions are being poured. Say WHEN.\r\n"
#define M_200_WHEN      "Your additions were added successfully.\r\n"
#define M_400           "The request could not be understood by the server due to malformed syntax.\r\n"
#define M_404           "The requested pot does not exist.\r\n"
#define M_406           "The supplied additions were invalid.\r\n"
#define M_418           "The requested pot is not capable of brewing coffee. Please use a weaker protocol.\r\n"
#define M_419           "The requested pot is not turned on.\r\n"
#define M_420           "The requested pot is in use by another client.\r\n"
#define M_421           "The requested pot is still brewing. "
#define M_422           "The requested pot is still pouring. "
#define M_423           "The requested pot is already brewing.\r\n"
#define M_424           "The requested pot is already pouring.\r\n"
#define M_425           "The requested pot is not pouring.\r\n"
#define M_426           "The requested pot still has a cup waiting to be collected.\r\n"
#define M_427           "The requested pot has no cup waiting.\r\n"
#define M_428           "The requested pot is waiting for additions to be poured.\r\n"
#define M_503           "There are no pots available to serve your request. Please try again later.\r\n"
#define M_504           "Out of time: your cup has overflowed.\r\n"
#define M_505           "Out of time: your coffee has gone cold.\r\n"

#define TRUE 		1
#define FALSE		0

/* A single thread's properties */
typedef struct {
    pthread_t tid;
    int sock;
    int busy;
} thread_struct;

/* Max 5 pots => max 5 threads.*/
thread_struct threads[NUM_POTS];
char buf[MAX_DATA_SIZE];

static void *handle_request(void *ptr);
void parse_request(char*, char*);

/* VCP wrapper calls */
void propfind_request(pot_struct*, char*);
void brew_request(pot_struct*, char*, char*);
void get_request(pot_struct*, char*, char*);
void pour_request(pot_struct*, char*);
void when_request(pot_struct*, char*);

int valid_method(char*);
int extract_pot_id(char*);
void build_err_response(char*, pot_struct*, int);

/* Helper methods */
int create_tcp_endpoint();
void close_thread(thread_struct*);
void log(char*);

#endif	/* KAFFEINE_H */

