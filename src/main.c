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

        if(strcmp(map_get(h->response_headers, "transfer-encoding"), "chunked") == 0){
            puts("transfer encoding is chunked");

            char decodedBuff[1000] = {0};

            chunkz *cz = chunkez_create_stream("",0);

            int offset = 0;

            while (true)
            {
                char buff[1000] = {0};
                int readb = SSL_read(h->handle, buff, 100);

                chunkz_feed(cz,buff,readb);

                ssize_t x = chunkzRead(cz, decodedBuff, 100);

                puts(decodedBuff);

                bzero(decodedBuff, sizeof decodedBuff);

                if(x < 0 || cz->finished) {
                    break;
                }

                if(readb < 1){
                    break;
                }

                offset += x;
            }

            puts(decodedBuff);
            
        }

    } else {
        puts("connection failed");
    }
    return 0;
}

/**
 * @param howmany number of bytes to parse
 * @param strln size of chunked string;
 * @param expectedSize previous reminder (-1 if prev ends with '\\r')
 * @param hangingLen reminder
 * @param nextstartindex where to start in next iteration
 */
int parse(char *str,
          char result[],
          int howMany,
          int strln,
          long expectedSize,
          long *hangingLen,
          long *nextStartIndex)
{

    // parsed size
    int resultIndex = 0;

    int index = 0;

    char currentChar;

    if (strln < 0)
    {
        return -1;
    }

    bool expectin_size = expectedSize > 0 ? false : true;
    bool expecting_chunk = expectedSize > 0 ? true : false;

    if (expectedSize == -1)
    {
        index = -1;
        currentChar = '\r';

        expecting_chunk = true;

        expectin_size = false;
    }

    char next_size[20] = {0};
    int size_index = 0;

    bool startProcess = expectedSize > 0 ? true : false;

    long chunkSize = 0;

    // number of `\\r\\n` encountered
    int numDelims = 0;

    while (true)
    {

        if (index >= strln)
        {
            break;
        }

        if (index >= 0)
            currentChar = str[index];

        if (startProcess && expecting_chunk)
            chunkSize++;

        if (currentChar == '\r' && str[index + 1] == '\n')
        {

            numDelims++;

            if (expectin_size)
            {
                expecting_chunk = true;
                expectin_size = false;

                chunkSize = 0;

                expectedSize = strtol(next_size, NULL, 10);

                if (expectedSize == 0)
                {
                    // puts(next_size);
                    break;
                }
                else
                {
                    startProcess = true;
                }

                bzero(next_size, sizeof next_size);

                size_index = 0;
            }
            else if (expecting_chunk)
            {

                if (startProcess && (chunkSize - 1 != expectedSize))
                {
                    printf("Error -> Wrong chunk size exp [%ld] found [%ld]\n", expectedSize, chunkSize - 1);
                    resultIndex = -1;
                    break;
                }
                expectin_size = true;
                expecting_chunk = false;
            }

            index += 1;
        }
        else
        {

            if (expectin_size)
            {
                next_size[size_index] = currentChar;
                size_index++;
            }
            else if (expecting_chunk)
            {
                result[resultIndex] = currentChar;
                resultIndex++;
            }
        }

        if (resultIndex == howMany)
        {
            break;
        }

        index++;
    }

    // -1 when no unread length;

    currentChar = str[index];

    if (numDelims % 2 == 0)
    {
        if (expectedSize == chunkSize)
        {
            // block read fully
            *hangingLen = -101;
        }
        else
        {

            // block not read fully
            *hangingLen = (expectedSize - chunkSize);
        }

        printf("@@ %ld @@\n", *hangingLen);
    }
    else
    {
        // what len to expect next
        *hangingLen = (expectedSize - chunkSize);

        printf("** %ld **\n", *hangingLen);
    }

    *nextStartIndex = index + 1;

    // printf("delims -> %d, hanging len -> %ld\n", numDelims, *hangingLen);
    return resultIndex;
}

int mainyo()
{

    char *str = "1\r\nA\r\n3\r\nBui\r\n2\r\nCD\r\n0\r\n\r\n";

    long hang = 0;

    char result[100] = {0};

    int offset = 0;
    int resOffset = 0;

    long nxt = 0;

    //////////////////////////////////////////////////////////////////////////////////////////
    // puts(str + offset);
    while (true)
    {

        int x = parse(str + offset, result + resOffset, 2, strlen(str + offset), hang, &hang, &nxt);

        if (x == 0)
        {
            puts("====================");
            break;
        }

        resOffset += x;
        offset += nxt;
    }

    puts(result);

    return 0;
}