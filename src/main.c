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
    http_client_set_url("https://0.freebasics.com/check_cookies/?encrypted_next=aav_teeo1g6bs3vdz4aklj3b_reuxyu_j4cekprtvvzsafj5zgqtrnjtreqjvyagpa8wjswhyweldhbvy8ivvamuy7ck_rwtk_xi--h3achb-masqs1vsxjylazjaxzz9dmu907laxurlrmdhp5y-6bwp9_vcufuaktglbttx3avssg8eqnvj37zdv6d2a5ca24qknza", h);

    if(!http_client_connect(h)) {
        puts("failed to connect");
        //exit(1);
    }

    //map_print(h->response_headers);


    while(true) {
        char *buff;
        //string_t *l = 
        if(http_client_read_chunks(h, &buff) == -10){
        //printf("%s", buff);
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

