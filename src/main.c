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
    http_client_set_url("https://internet.org/homey", h);

    if(!http_client_connect(h)) {
        puts("failed to connect");
        //exit(1);
    }

    map_print(h->response_headers);


    while(true) {
        char buff[1000] = {0};
        //string_t *l = 
        if(http_client_read(h, buff, 200) == -10){
            break;
        }

        printf("%s", buff);

        //break;

        //printf("line len => %ld\n", l->size);

        //string_destroy(l);
    }



    //assert(l == 100);


    return 0;
}

