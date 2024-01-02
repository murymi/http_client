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
    http_client_set_url("https://0.freebasics.com/unsupported/?r=desktop_client", h);

    if(!http_client_connect(h)) {
        puts("failed to connect");
        //exit(1);
    }

    map_print(h->response_headers);



    char *buff;

    while(true) {
        //string_t *l = 
        ssize_t a = http_client_read_chunks(h, &buff);
        printf("[%s] [%ld]", buff, a); 

        if(h->stream->finished){
        //printf("%s", buff);
            break;
        }

        free(buff);
        //break;    
        //printf("line len => %ld\n", l->size); 
        //string_destroy(l);
    }



    //assert(l == 100);


    return 0;
}

