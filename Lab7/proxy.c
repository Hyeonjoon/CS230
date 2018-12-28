#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_LINE 8192

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/* Function prototypes. */
void routine(int connfd, char *client_hostname);
int parse_uri(char *uri, char *parsed_uri, char *port);
void http_request(char *server_request, char *method, char *uri, char *version);
void http_header(char *host_name, char *header);
void forward_response(rio_t *server_rp, char *buf, int fd);
void sigchld_handler(int sig);

/*
 * main - Main function of proxy server.
 *        Get listening port from stdin and create listenfd.
 *        Intermediation is made by calling routine function.
 *        Concurrency is based on multiple processes formed by Fork().
 */
int main(int argc, char **argv)
{
        int listenfd, connfd;
        socklen_t clientlen;
        struct sockaddr_storage clientaddr;
        char client_hostname[MAX_LINE], client_port[MAXLINE];
	
	// Terminated child processes will be reaped by sigchld_handler.
	Signal(SIGCHLD, sigchld_handler);
        listenfd = Open_listenfd(argv[1]);
	printf("listening on port %s\n", argv[1]);
        while (1){
                clientlen = sizeof(struct sockaddr_storage);
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
                printf("Connected to (%s, %s)\n", client_hostname, client_port);
		// Child process work as intermediary server.
		if (Fork() == 0){
			Close(listenfd);
			routine(connfd, client_hostname);
			Close(connfd);
			exit(0);
                }
		// Parent process just listen for the next client.
		// Close connfd to prevent memory leak.
		Close(connfd);
        }
        return 0;
}

/*
 * routine - Main routine of proxy server.
 *           Read and parse the request line from clients
 *           and forward it to tiny server.
 *           Get response of tiny server
 *           and forward it to client.
 *           Child processes call this function.
 */
void routine(int connfd, char *client_hostname)
{
	char *server_port = (char *)(Malloc(MAXLINE));
	char *request_line = (char *)(Malloc(MAXLINE));
	char *method = (char *)(Malloc(MAXLINE));
	char *uri = (char *)(Malloc(MAXLINE));
	char *version = (char *)(Malloc(MAXLINE));
        char *parsed_uri = (char *)(Malloc(MAXLINE));
        char *fixed_version = (char *)(Malloc(MAXLINE));
	char *server_request = (char *)(Malloc(MAXLINE));
	char *server_header = (char *)(Malloc(MAXLINE));
	char *server_response = (char *)(Malloc(MAXLINE));
	
	rio_t server_rp, client_rp;
	int clientfd;
	
	// Read client's HTTP request.
	Rio_readinitb(&client_rp, connfd);
	if (!Rio_readlineb(&client_rp, request_line, MAXLINE)){
		return;
	}
	printf("request: %s\n", request_line);
	sscanf(request_line, "%s %s %s", method, uri, version);
	// Parse the uri.
	parse_uri(uri, parsed_uri, server_port);
	if (strcmp(version, "HTTP/1.0")){
		strcpy(fixed_version, "HTTP/1.0");
	}else{
		strcpy(fixed_version, version);
	}
	// Request line from proxy to server.
	http_request(server_request, method, parsed_uri, fixed_version);
	http_header(client_hostname, server_header);
	printf("request line: %s\n", server_request);
	// Now connect to tiny server.
	clientfd = Open_clientfd(client_hostname, server_port);
	// Write request line to tiny server.
	Rio_writen(clientfd, server_request, strlen(server_request));
	Rio_writen(clientfd, server_header, strlen(server_header));
	// Read response from tiny server and forward it to client.
	Rio_readinitb(&server_rp, clientfd);
	forward_response(&server_rp, server_response, connfd);
	
	// Free allocated spaces by Malloc.
	Free((void *)server_port);
	Free((void *)request_line);
	Free((void *)method);
	Free((void *)uri);
	Free((void *)version);
        Free((void *)parsed_uri);
        Free((void *)fixed_version);
	Free((void *)server_request);
	Free((void *)server_response);
	// Close clientfd.
	Close(clientfd);

	return;
}
	
/*
 * parse_uri - Parse the uri.
 *             Get server port number and file path.
 */
int parse_uri(char *uri, char *parsed_uri, char *port)
{
	char *buf = uri;
	char *p;
	char *q;
	char *r;
	
	if ((p = strstr(buf, "http://")) == NULL){
		app_error("Wrong URI\n");
	}
	q = strstr(p + 7, "/");
	strcpy(parsed_uri, q);
	*q = '\0';
	r = strstr(p + 7, ":");
	strcpy(port, r + 1);
	return 1;
}

/*
 * http_request - Make a single line http-request which would be sent to server
 *                using parsed uri and fixed version.
 */
void http_request(char *server_request, char *method, char *uri, char *version)
{
	char buf[MAXLINE] = "";
	strcpy(buf, method);
	buf[strlen(method)] = ' ';
	strcat(buf, uri);
	buf[strlen(method) + strlen(uri) + 1] = ' ';
	strcat(buf, version);
	strcat(buf, "\r\n");
	strcpy(server_request, buf);
	return;
}

/*
 * http_header - Make request header which would be sent to server.
 */
void http_header(char *host_name, char *header)
{
	sprintf(header, "Host: %s\r\n", host_name);
	sprintf(header, "%s%s", header, user_agent_hdr);
	sprintf(header, "%sConnection: close\r\n", header);
	sprintf(header, "%sProxy-Connection: close\r\n\r\n", header);
}

/*
 * forward)response - Read response of server and forward it to client.
 */
void forward_response(rio_t *server_rp, char *buf, int fd)
{
	ssize_t n;
	char *buffer = Malloc(MAXLINE);
	
	// Read response from server line by line.
	while ((n = Rio_readlineb(server_rp, buffer, MAXLINE)) != 0){
		printf("%s", buffer);
		// Contents may not be strings.
		// Using strlen(buffer) rather than n may not work.
		Rio_writen(fd, buffer, n);
	}
	Free((void *)buffer);
	return;
}

/*
 * sigchld_handler - Reap the terminated child processes.
 */
void sigchld_handler(int sig)
{
	while (waitpid(-1, 0, WNOHANG) > 0){
		;
	}
	return;
}
