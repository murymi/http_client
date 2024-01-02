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
    http_client_set_url("https://0.freebasics.com/check_cookies/?encrypted_next=aaszoz0mh-mn0t5bn3apz_zpi8ipqk-t9mivupmmi0inkqoanfu7ywplngxusgtw_9xma5xqqyn_ydn1gulj4_gc0c9ttof4ksh5kh7rsprp1hv1lirxc3ia46aq50kinfrplwxay_wvelqgr423y-epg3e3snr7g-zx8y0qhmv9f9gmvibj96izphlmatk8jhtozzp2", h);

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


