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

    http_client_set_url("https://internet.org/", h);
    http_client_set_method(GET, h);

    if(http_client_send(h)) {
       // map_print(h->response_headers);
    }

    return 0;
}
