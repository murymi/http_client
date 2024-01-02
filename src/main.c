#include "array_list.h"
#include "https_client.h"
#include "url_parser.h"
#include "strings.h"
#include <threads.h>
#include <poll.h>
#include "stream.h"

int main()
{
    http_client *h = http_client_create();
    http_client_set_method(GET, h);
    http_client_set_url("http://localhost/", h);

    char *headers = http_client_write_header(h);

     struct sockaddr *remote_host = NULL;

    int sock = http_client_create_socket("127.0.0.1", "3000", &remote_host);

    send(sock, headers, strlen(headers), 0);

    stream_t *s = stream_init(sock);

    while(true) {

        string_t *l = stream_read_line(s,"\r\n", false);

        puts(l->chars);

        if(l->size == 0){
            break;
        }

        string_destroy(l);
    }



    //assert(l == 100);


    return 0;
}

