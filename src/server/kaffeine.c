/*
 * kaffeine.c
 *
 *  Created on: Feb 27, 2012
 *      Author: jl2-salmon
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "vcp.h"
#include "kaffeine.h"

int main(void) {

    int sock, tmpsock, curr_thread;
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    char cli_addr[20], buf[MAX_DATA_SIZE];

    /* Create an endpoint to listen on */
    if ((sock = create_tcp_endpoint(USR_PORT)) < 0) {
        fprintf(stderr, "Cannot create endpoint\n");
        exit(1);
    } else {
        fprintf(stderr, "TCP endpoint created.\n");
    }

    fprintf(stderr, "Initialising virtual coffee pots...\n");
    for (int i = 0; i < NUM_POTS; ++i) {
        init_pot(&pots[i], i);
    }
    fprintf(stderr, "Pots initialised.\n");
    fprintf(stderr, "Accepting connections on port %d.\n", USR_PORT);

    /* main accept() loop */
    while (1) {
        curr_thread = 0;

        while (threads[curr_thread].busy == TRUE && curr_thread < NUM_POTS) {
            curr_thread++;
        }

        sin_size = sizeof (struct sockaddr_in);

        if (curr_thread == NUM_POTS) {
            fprintf(stderr, "Connection limit reached\n");
            tmpsock = accept(sock, (struct sockaddr *) &client_addr, &sin_size);

            strcpy(buf, HTCPCP_VERSION);
            strcat(buf, C_503);
            strcat(buf, CONTENT_TYPE);
            strcat(buf, M_503);

            if (send(tmpsock, buf, strlen((char*) buf), 0) <= 0) {
                perror("send");
                exit(-1);
            }
        } else {

            if ((threads[curr_thread].sock = accept(
                    sock, (struct sockaddr *) &client_addr, &sin_size)) < 0) {
                perror("accept");
                exit(-1);
            }

            strcpy(cli_addr, inet_ntoa(client_addr.sin_addr));
            printf("\nConnection established with %s\n", cli_addr);

            threads[curr_thread].busy = TRUE;
            if (pthread_create(&threads[curr_thread].tid,
                    NULL,
                    &handle_request,
                    (void *) &threads[curr_thread])
                    != 0) {
                perror("pthread_create");
                exit(-2);
            }
        }
    }
}

/**
 * 
 * @param tptr
 * @return 
 */
static void *handle_request(void *tptr) {
    thread_struct *thread;
    thread = (thread_struct *) tptr;
    char request[MAX_DATA_SIZE];
    char response[MAX_DATA_SIZE];
    int numbytes;

    printf("Created thread %d\n", (int) thread->tid);

    memset(&request, 0, sizeof (request));

    /* receive initial message */
    if ((numbytes = recv(thread->sock, request, MAX_DATA_SIZE - 1, 0)) == -1) {
        perror("recv");
        close_thread(thread);
    }

    while (strcmp(request, "quit") != 0) {

        fprintf(stderr, "Message received: \n%s\n", request);

        if (strcmp(request, "\0") != 0) {
            parse_request(request, response);
        }

        if (send(thread->sock, response, strlen(response), MSG_NOSIGNAL) == -1) {
            perror("send");
            close_thread(thread);
        }

        memset(&request, 0, sizeof (request));

        if ((numbytes = recv(thread->sock, request, MAX_DATA_SIZE - 1, 0)) == -1) {
            perror("recv");
            close_thread(thread);
        }
    }

    if (send(thread->sock, QUIT_MSG, strlen(QUIT_MSG), MSG_NOSIGNAL) == -1) {
        perror("send");
        close_thread(thread);
    }

    fprintf(stderr, "Thread %d exiting.\n", (int) pthread_self());
    close_thread(thread);
    return 0;
}

void parse_request(char* request, char* response) {
    const char delimiters[] = " :/?";
    char *method, *scheme, *host, *pot_no, *start_line, *header;
    char *rqcpy;
    int pot_id;

    rqcpy = strdup(request);
    method = strtok(rqcpy, delimiters);
    scheme = strtok(NULL, delimiters);
    host = strtok(NULL, delimiters);

    pot_no = strtok(NULL, delimiters);
    pot_id = extract_pot_id(pot_no);

    strcpy(response, HTCPCP_VERSION);

    if (pot_id > NUM_POTS || pot_id < 0) {
        strcat(response, C_404);
        strcat(response, CONTENT_TYPE);
        strcat(response, M_404);
        return;
    } else if (pot_id == TEAPOT) {
        strcat(response, C_418);
        strcat(response, CONTENT_TYPE);
        strcat(response, M_418);
        return;
    }

    fprintf(stderr, "Method: %s, Scheme: %s, Host: %s, Pot: %d\n"
            , method, scheme, host, pot_id);

    if (pots[pot_id].current_thread == 0) {
        pots[pot_id].current_thread = pthread_self();
    } else if (!pthread_equal(pots[pot_id].current_thread, pthread_self())) {
        strcat(response, C_420);
        strcat(response, CONTENT_TYPE);
        strcat(response, M_420);
        return;
    }

    if (strcmp(method, METHOD_PROPFIND) == 0) {
        propfind_request(&pots[pot_id], response);

    } else if (strcmp(method, METHOD_BREW) == 0) {
        start_line = strtok(request, "\r\n");
        header = strtok(NULL, "\r\n");
        brew_request(&pots[pot_id], header, response);

    } else if (strcmp(method, METHOD_GET) == 0) {
        get_request(&pots[pot_id], request, response);

    } else if (strcmp(method, METHOD_WHEN) == 0) {
        when_request(&pots[pot_id], response);

    }

    return;
}

void propfind_request(pot_struct * pot, char* response) {
    strcat(response, C_200);
    strcat(response, CONTENT_TYPE);
    propfind(pot, response);
}

void brew_request(pot_struct* pot, char* header, char* response) {
    int err = brew(pot, header);
    build_err_response(response, err);
}

void get_request(pot_struct* pot, char* adds, char* response) {
    strcat(response, C_200);
    strcat(response, CONTENT_TYPE);

    int err = get(pot, adds, response);
    if (err) {
        build_err_response(response, err);
    }
}

void when_request(pot_struct* pot, char* response) {

    strcpy(response, HTCPCP_VERSION);
    strcat(response, C_200);
}

void build_err_response(char* response, int err) {
    strcpy(response, HTCPCP_VERSION);

    switch (err) {
        case E_OFF:
            strcat(response, C_419);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_419);
            break;
        case E_BUSY:
            strcat(response, C_420);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_420);
            break;
        case E_STILL_BREWING:
            strcat(response, C_421);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_421);
            break;
        case E_STILL_POURING:
            strcat(response, C_422);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_422);
            break;
        case E_ALRDY_BREWING:
            strcat(response, C_423);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_423);
            break;
        case E_ALRDY_POURING:
            strcat(response, C_424);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_424);
            break;
        case E_NOT_POURING:
            strcat(response, C_425);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_425);
            break;
        case E_CUP_WAITING:
            strcat(response, C_426);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_426);
            break;
        case E_NO_CUP:
            strcat(response, C_427);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_427);
            break;
        case E_OVERFLOW:
            strcat(response, C_504);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_504);
            break;
        case E_CUP_COLD:
            strcat(response, C_505);
            strcat(response, CONTENT_TYPE);
            strcat(response, M_505);
            break;
        default:
            break;
    }
}

int extract_pot_id(char* pot_no) {
    const char delimiters[] = "-";
    char *head, *s_id;

    head = strtok(pot_no, delimiters);
    s_id = strtok(NULL, delimiters);
    return atoi(s_id);
}

/* Useful function to create server endpoint */
int create_tcp_endpoint(int port) {
    int sock, yes = 1;
    struct sockaddr_in server;

    /* make socket with TCP streams. Kernel choose a suitable protocol */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Server socket");
        return -1;
    }

    /* Set Unix socket level to allow address reuse */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        perror("Server setsockopt");
        return -2;
    }

    /* zero the struct */
    memset(&server, 0, sizeof (server));
    server.sin_family = AF_INET;
    /* any server's IP addr */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
        perror("Server bind");
        return -3;
    }

    /* Listen for client connections */
    if (listen(sock, MAX_Q_SIZE) == -1) {
        perror("Server listen");
        return -4;
    }

    return sock;
}

void close_thread(thread_struct* thread) {
    for (int i = 0; i < NUM_POTS; ++i) {
        if (pthread_equal(pots[i].current_thread, pthread_self())) {
            init_pot(&pots[i], i);
        }
    }
    thread->busy = FALSE;
    close((int) thread->sock);
    pthread_exit((void *) 1);
}