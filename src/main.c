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
    http_client_set_url("https://localhost:3000/song.mp3", h);

    if(!http_client_connect(h)) {
        puts("failed to connect");
        //exit(1);
    }

    //assert(h->response_headers == NULL);

    //puts("========ssert=======");

    //exit(1);

    stream_t *s = stream_init(h->handle);

    while(true) {
        char buff[1000] = {0};
        //string_t *l = 
        if(stream_read(s, buff, 200) == -10){
            break;
        }

        puts(buff);

        //break;

        //printf("line len => %ld\n", l->size);

        //string_destroy(l);
    }



    //assert(l == 100);


    return 0;
}

