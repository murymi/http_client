#include "array_list.h"
#include "https_client.h"
#include "url_parser.h"

typedef void (*thread_func_t)(void *arg);

void fun (char *key, char *value) {
    printf("%s: %s\n", key, value);
    return;
}

void fun2(char *key, char *value, string_t *str) {
    string_concat(str, key, strlen(key));
    string_append(str, ':');
    string_append(str, ' ');
    string_concat(str, value, strlen(value));
}

int main() {

    http_client *h = http_client_create();

    http_client_set_url("https://github.com/nflvic/http_client/blob/main/src/https_client.c", h);

    //http_client_set_header("content-length", "500", h);
    http_client_set_header("connection", "close", h);

    http_client_set_method(GET, h);

    if(http_client_connect(h)){
        puts("connection success");
    } else {
        puts("connection failed");
    }
    puts("====================================");
    puts(http_client_write_header(h));
    
    return 0;
}