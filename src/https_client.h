#pragma once


#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <stdbool.h>
#include "map.h"
#include <openssl/ssl.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <threads.h>
#include "stream.h"

typedef enum transferEncoding transferEncoding;
enum transferEncoding {
    chunked = 10,
    contentLength
};


typedef struct http_client http_client;
struct http_client
{
    SSL_CTX *context;
    SSL *handle;
    map_t *headers;
    map_t *response_headers;
    char * url;
    char * http_version;
    char * body;
    size_t file_size;
    char * method;
    char * address;
    char * port;
    size_t content_length;
    bool chunked_body;
    stream_t *stream;
};

enum files
{
    IMAGE = 10,
    JSON
};

enum codes
{
    OK = 200,
    NOT_FOUND = 404,
    BAD_REQ = 400
};


typedef enum methods methods;
enum methods
{
    POST = 100,
    GET,
    PUT,
    PATCH,
    DELETE,
    OPTIONS
};




ssize_t http_client_read(http_client *client, char *buff, size_t bytesToRead);

int http_client_create_socket(char *address_,char *port,struct sockaddr **host);

char * get_ip_as_string(struct sockaddr *address);

SSL * http_client_create_ssl(char *address_,SSL_CTX *ctx,int sock);

http_client *http_client_create();

bool http_client_set_url(char *url,http_client *client);

bool http_client_set_method(methods m,http_client *client);

bool http_client_set_method_str(char *m,http_client *client);

bool http_client_set_address(char *address,http_client *client);

bool http_client_set_port(char *port,http_client *client);

bool http_client_set_header(char *key,char *value,http_client *client);

bool http_client_append_file(char *path,http_client *client);

bool http_client_append_string(char *str,http_client *client);

bool http_client_connect(http_client * client);

void dbg_client(http_client *ct);

bool http_client_receive_response(SSL *sock,http_client *client);

char *http_client_write_header(http_client *ct);

SSL * http_client_create_bios();

bool http_client_set_host(struct sockaddr * host,http_client *client);

int http_client_get_service_port(char *service_name);

void http_client_destroy(http_client *client);

void http_client_send_request(http_client *client);

map_t * parse_http_response(char *req);

void join_headers(char *key, char *value, string_t *str);