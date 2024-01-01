#include "array_list.h"
#include "https_client.h"
#include "url_parser.h"
#include "strings.h"
#include <threads.h>



int main()
{

    http_client *h = http_client_create();

    http_client_set_url("https://github.com/nflvic/http_client/blob/main/makefile", h);
    http_client_set_method(GET, h);
    http_client_set_header("connection", "close", h);

    map_print(h->headers);

    if(http_client_connect(h)) {
        while(true) {
            char buff[1001] = {0};
            ssize_t a = http_client_read(h, buff, 1000);
            printf("%s", buff);

            if(a == 0) {
                puts("<!--==== END OF STREAM ==========-->");
                break;
            }

            if(a < 0) {
                puts("<!--=============== ERROR ====================-->");
                break;
            }
        }


    } else {
        puts("connection failed");
    }
    return 0;
}

