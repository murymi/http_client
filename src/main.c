#include "array_list.h"
#include "https_client.h"
#include "url_parser.h"
#include "strings.h"
#include <threads.h>
#include <poll.h>
#include "stream.h"


int main()
{
    char *st = "wacha&apos;umama";
    puts(replace_html_entity(st));
    return 0;
}
