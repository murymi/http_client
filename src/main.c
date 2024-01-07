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

    http_client_set_url("https://www.facebook.com", h);
    http_client_set_method(GET, h);

    if(http_client_send(h)){
        http_client_read_to_file(h, "readfile.html");
    }

    return 0;
}

// urls for testing
// https://www.google.com/url?sa=i&url=https%3A%2F%2Fencrypted-tbn2.gstatic.com%2Flicensed-image%3Fq%3Dtbn%3AANd9GcSO9Xd_NJYU1FU2u886CDMp-pX-nffkmg_h0yhAKgLWCltFmAbQnt_nGdpEPgQZMZzw1k_pGxWjlD3U_Yk&psig=AOvVaw28MNkxJJUKU938AdiPt-28&ust=1704401304592000&source=images&cd=vfe&ved=0CBIQjRxqFwoTCIiV0MKLwoMDFQAAAAAdAAAAABAE
// https://images.unsplash.com/photo-1693741014978-f8acfbb426c8?q=80&w=1288&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D

