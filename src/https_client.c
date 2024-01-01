#include "https_client.h"
#include "url_parser.h"
#include <stdarg.h>

char *get_ip_as_string(struct sockaddr *address)
{
  char *ip_string = malloc(INET6_ADDRSTRLEN);
  if (address->sa_family == AF_INET)
  {
    inet_ntop(AF_INET6, (struct in6_addr *)address, ip_string, INET6_ADDRSTRLEN);
    return ip_string;
  }

  inet_ntop(AF_INET, (struct in_addr *)address, ip_string, INET_ADDRSTRLEN);
  return ip_string;
}

int http_client_create_socket(char *address_, char *port,struct sockaddr **host)
{
  int status, sock;
  struct addrinfo hints;
  struct addrinfo *res, *p;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME | AF_INET;

  status = getaddrinfo(address_, port, &hints, &res);

  if (status != 0)
  {
    fprintf(stderr, "%s\n", gai_strerror(status));
    return -1;
  }

  for (p = res; p; p = p->ai_next)
  {
    get_ip_as_string(p->ai_addr);

    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (sock < 0)
      continue;

    if (!connect(sock, p->ai_addr, p->ai_addrlen))
    {
      *host = malloc(sizeof p->ai_addr);
      memcpy(*host,p->ai_addr,p->ai_addrlen);
      break;
    }

    perror("connect");
  }

  if (p == NULL)
  {
    return -1;
  }

  freeaddrinfo(res);
  return sock;
}



SSL *http_client_create_ssl(char *address_, SSL_CTX *ctx, int sock)
{
  SSL *ssl = NULL;
  (void)SSL_library_init();
  SSL_load_error_strings();
  // OPENSSL_config(NULL);

#if defined(OPENSSL_THREADS)
// to do
#endif

  int res;

  ssl = SSL_new(ctx);
  if (ssl == NULL)
  {
    return NULL;
  }

  const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
  SSL_CTX_set_options(ctx, flags);

  res = SSL_set_tlsext_host_name(ssl, address_);
  if (res != 1)
  {
    return NULL;
  }

  const char *preffered_ciphers = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";

  res = SSL_set_cipher_list(ssl, preffered_ciphers);

  if (res != 1)
  {
    return NULL;
  }

  res = SSL_set_fd(ssl, sock);
  if (res != 1)
  {
    return NULL;
  }

  if(ssl == NULL)
  {
    exit(1);
  }

  res = SSL_connect(ssl);
  if (res != 1)
  {
    return NULL;
  }

  res = SSL_do_handshake(ssl);
  if (res != 1)
  {
    return NULL;
  }

  return ssl;
}

http_client *http_client_create()
{

  http_client *client = malloc(sizeof(http_client));

  client->headers = NULL;

  client->url = NULL;

  client->http_version = malloc(10 * sizeof(char));

  client->port = NULL;

  client->address = NULL;


  client->body = NULL;
  client->response_headers = NULL;

  strcpy(client->http_version, "HTTP/1.1");

  client->file_size = 0;

  return client;
}

bool http_client_set_url(char *url, http_client *client)
{
  if (!url || !client)
    return false;

  
  url_t *c_url = url_parser_parse(url);

  if(c_url == NULL) return false;

  client->url = string_create_copy(c_url->path);
  
  http_client_set_address(c_url->domain,client);
  http_client_set_header("Host",c_url->domain,client);

  if(c_url->port){
    http_client_set_port(c_url->port, client);
  } else {
    http_client_set_port("443", client);
  }

  url_free(c_url);

  return true;
}

bool http_client_set_method(methods m, http_client *client)
{
  switch (m)
  {
  case GET:
    return http_client_set_method_str("GET", client);
    break;
  case POST:
    return http_client_set_method_str("POST", client);
    break;
  case PATCH:
    return http_client_set_method_str("PATCH", client);
    break;
  case PUT:
    return http_client_set_method_str("PUT", client);
    break;
  case DELETE:
    return http_client_set_method_str("DELETE", client);
    break;
  case OPTIONS:
    return http_client_set_method_str("OPTIONS", client);
    break;
  default:
    return http_client_set_method_str("GET", client);
  }

  return false;
}

bool http_client_set_method_str(char *m, http_client *client)
{
  if (!client || !m)
    return false;
  int l = strlen(m);
  client->method = malloc(l + 1);
  strcpy(client->method, m);
  return true;
}

bool http_client_set_address(char *address, http_client *client)
{
  if (!client || !address)
    return false;
  int l = strlen(address);
  client->address = malloc(l + 1);
  strcpy(client->address, address);
  return true;
}

bool http_client_set_port(char *port, http_client *client)
{
  if (!client || !port)
    return false;
  int l = strlen(port);
  client->port = malloc(l + 1);
  strcpy(client->port, port);
  return true;
}

bool http_client_set_header(char *key, char *value, http_client *client)
{
  if (!key || !value || !client)
    return false;

  if (client->headers == NULL)
    client->headers = map_create();

  map_put(client->headers, key, value);
  return true;
}

bool http_client_append_file(char *path, http_client *client)
{
  if (!client || client->body)
    return false;

  FILE *fptr;
  size_t file_size;

  if (!(fptr = fopen(path, "rb")))
    return false;

  fseek(fptr, 0, SEEK_END);

  file_size = ftell(fptr);
  client->file_size = file_size;

  client->body = malloc(file_size + 1);
  bzero(client->body, file_size + 1);

  rewind(fptr);
  size_t read_bytes = 0;
  bool out = true;
  size_t offset = 0;

  while ((read_bytes = fread(client->body + offset, 1, 100, fptr)))
  {
    if (read_bytes != 100)
    {
      if (ferror(fptr))
      {
        free(client->body);
        out = false;
        break;
      }
    }
    offset += 100;
  }

  return out;
}

bool http_client_append_string(char *str, http_client *client)
{
  if (!str || !client || client->body)
    return false;

  int len = strlen(str);

  client->body = malloc(len + 1);

  if (!client->body)
    return false;

  client->file_size = len;

  strcpy(client->body, str);

  return true;
}

bool http_client_connect(http_client *client)
{
  if(client == NULL) return false;

  if (!client->url || !client->method)
  {
    return false;
  }

  
  //if(!client->body)
  //{
  //  http_client_set_header("Content-length","0",client);
  //}


  SSL_CTX *ctx = NULL;
  (void)SSL_library_init();
  SSL_load_error_strings();

  const SSL_METHOD *method = SSLv23_method();

  if (method == NULL)
  {
    // handle error
    return false;
  }

  ctx = SSL_CTX_new(method);

  if (ctx == NULL)
  {
    return false;
  }

  int sock;
  struct sockaddr *remote_host = NULL;

  if ((sock = http_client_create_socket(client->address, client->port,&remote_host)) == -1)
  {
    return false;
  }

  SSL *ssl = NULL;
 char *header = http_client_write_header(client);


  if ((ssl = http_client_create_ssl(client->address, ctx, sock)) == NULL)
  {
    puts("SSl failed");
    return false;
  }

  if ((SSL_write(ssl, header, strlen(header))) == -1)
  {
    return false;
  }

  bool out = true;

  if (client->body)
  {
    int size;
    int offset = 0;
    int rem = 100;
    size_t total_sent = 0;

    while (true)
    {
      int b_sent;

      if (client->file_size <= 100)
      {
        size = client->file_size;
      }else{ size = rem; }

      b_sent = SSL_write(ssl, client->body + offset, size);

      total_sent += b_sent;

      if(b_sent < 1)
      {
        if(b_sent == -1) out = false;
        break;
      }

      if(total_sent >= client->file_size)
        break;

      if (client->file_size <= 100)
        break;

      rem = client->file_size - total_sent;

      if (rem < 100)
      {
        size = rem;
      }
      else
      {
        size = 100;
      }

      offset += 100;
    }
  }

  out = http_client_receive_response(ssl, client);
  client->context = ctx;
  client->handle = ssl;

  return out;
}

bool http_client_receive_response(SSL *sock, http_client *client)
{
  char recv_buf[1] = {0}, end_of_header[] = "\r\n\r\n";
  int bytes_received, header_size = 0, marker = 0;
  string_t *b = string_create();

  map_t *http_res = NULL;

  bool out = true;

  while (true)
  {

    bytes_received = SSL_read(sock, recv_buf, 1);

    if (bytes_received <= 0)
    {
      out = false;
      break;
    }
    
    string_append(b, recv_buf[0]);

    if (recv_buf[0] == end_of_header[marker])
      marker++;
    else
      marker = 0;

    if (bytes_received <= 0 /* && file_reached*/)
    {
      break;
    }

    if (marker == 4)
    {

      if ((http_res = parse_http_response(b->chars)) == NULL){
        out = false;
        break;
      }

      char *clen = map_get(http_res, "content-length");
      char *trenc = map_get(http_res, "transfer-encoding");

      if(clen){
        client->content_length = strtol(clen, NULL, 10);
      } else if(trenc){
        client->chunked_body = true;
        client->chunker = chunkez_create_stream("",0);
      } else {
        out = false;
      }

      client->response_headers = http_res;

      break;
    }

    header_size++;
  }

  return out;
}

void dbg_client(http_client *ct)
{
  puts(ct->address);
  puts(ct->body);
  puts(ct->http_version);
  puts(ct->method);
  puts(ct->port);
}

void join_headers(char *key, char *value, string_t *str)
{
    string_concat(str, key, strlen(key));
    string_append(str, ':');
    string_append(str, ' ');
    string_concat(str, value, strlen(value));
    string_concat(str, "\r\n", 2);
}

char *http_client_write_header(http_client *ct)
{
  string_t *head = string_create();
  char onst[500];

  sprintf(onst, "%s %s %s\r\n", ct->method, ct->url, ct->http_version);

  string_concat(head, onst, strlen(onst));

  if (ct->headers == NULL)
  {
    char *chd = head->chars;
    free(head);
    return chd;
  }

  char *rawheaders = for_each_map(ct->headers, join_headers);

  string_concat(head, rawheaders, strlen(rawheaders));

  string_concat(head, "\r\n", 2);

  char *chd = head->chars;
  free(head);
  return chd;
}

bool http_client_set_host(struct sockaddr * host,http_client *client)
{
  char host_name[1024];
  char service[50];
  int status;

  if((status = getnameinfo(host,sizeof(struct sockaddr_in),host_name,sizeof host_name,service,sizeof service,0)) != 0)
  {
    return false;
  }

  free(client);

  //int port = http_client_get_service_port(service);

  //char host_port[1074];

  //sprintf(host_port,"%s:%d",host_name,port);
  return true;
}

int http_client_get_service_port(char *service_name)
{
  struct servent *sv = NULL;

  char proto[4] = "tcp";

  sv =  getservbyname(service_name,proto);

  if(sv == NULL) return -1;
  //getservbyname(service_name,"TCP");
  return ntohs(sv->s_port);
}


void http_client_destroy(http_client *client)
{
  if(client == NULL) return;

  if(client->address) free(client->address);
  if(client->body) free(client->body);
  if(client->url) free(client->url);
  if(client->headers) map_destroy(&(client->headers));
  if(client->response_headers) map_destroy(&(client->response_headers));
  if(client->port) free(client->port);
  if(client->method) free(client->method);
  if(client->http_version) free(client->http_version);

  if(client->context){
      SSL_free(client->handle);
      SSL_CTX_free(client->context);
  }

  free(client);
}


map_t * parse_http_response(char *req)
{
    int len = strlen(req);
    req = string_removechar('\r', req, len);
    strAL *lines = split('\n', req, strlen(req));


    string_array_list_delete(lines, lines->size-1);
    string_array_list_delete(lines, lines->size-1);

  strAL *vl = lines;

  char *firstLine = string_array_list_get(vl, 0);

  strAL *vc = split_lim(' ', firstLine, strlen(firstLine),3);

  if (vc->size < 3)
  {
        fprintf(stderr, "Invalid response\n");
        return NULL;
  }

  map_t *map = map_create();

  map_put(
          map,
        "http-version",
        string_array_list_get(vc, 0));

  map_put(
          map,
        "status-code",
        string_array_list_get(vc, 1));

  map_put(
        map,
        "code-name",
        string_array_list_get(vc, 2));

  string_arraylist_destroy(&vc);


    for(size_t i = 1; i < vl->size; i++)
    {
        char *thisLine = string_array_list_get(vl, i);
        vc = split_lim(':', thisLine, strlen(thisLine), 2);
        if (vc-> size!= 2)
        {
            string_arraylist_destroy(&vc);
            continue;
        }

        char *ss = trim(string_array_list_get(vc, 1));
        char *sk = trim(string_array_list_get(vc, 0));

        char *sklow = string_to_lower(sk);

        char *sslow = string_to_lower(ss);

        if(!strcmp(sklow,"sec-websocket-key")){
            map_put(map, sklow, ss);
        }else{

            map_put(map, sklow, sslow);
        }


        free(sslow);
        free(sklow);
        free(ss);
        free(sk);

        string_arraylist_destroy(&vc);
    }

    free(req);
    string_arraylist_destroy(&lines);
    return map;
}

/**
 * buff must start with first length initially
 */
chunkz *chunkez_create_stream(char *buff, size_t bufferSize)
{
    chunkz *c = malloc(sizeof(chunkz));

    c->bufferSize = bufferSize;

    c->encodedBuffer = malloc(bufferSize + 1);

    strcpy(c->encodedBuffer, buff);

    mtx_init(&(c->chunkMutex), 0);

    c->bytesActuallyRead = 0;

    c->bytesToExpectOnCurrentRead = 0;

    c->finished = false;

    c->offset = 0;

    c->isReadingChunk = false;

    c->sizeBuffer = malloc(20);

    c->sizeBufferIndex = 0;

    c->started = false;

    c->decodedIndex = 0;

    c->remainingUnprocessedBytes = bufferSize;

    return c;
};

ssize_t chunkzRead(chunkz *chk, char *buff, size_t amountOfBytesToRead)
{
    
    if(chk == NULL) {
        __assert("Is NULL",__FILE__, __LINE__);
    }

    if(chk->encodedBuffer == NULL) {
        return -1;
    }
    mtx_lock(&(chk->chunkMutex));

    char currentChar;


    chk->decodedIndex = 0;

    while (true)
    {

        if (chk->offset >= chk->bufferSize)
        {
            break;
        }

        currentChar = chk->encodedBuffer[chk->offset];

        if (currentChar == '\r' && chk->encodedBuffer[chk->offset + 1] == '\n')
        {

            if (!chk->isReadingChunk)
            {
                chk->isReadingChunk = true;

                chk->bytesToExpectOnCurrentRead = 0;

                chk->bytesToExpectOnCurrentRead = strtol(chk->sizeBuffer, NULL, 16);

                if (chk->bytesToExpectOnCurrentRead == 0)
                {
                    // puts(next_size);
                    chk->finished = true;
                    break;
                }

                bzero(chk->sizeBuffer, 20);
                chk->sizeBufferIndex = 0;
            }
            else
            {

                if (chk->bytesActuallyRead != chk->bytesToExpectOnCurrentRead)
                {
                    printf("Error -> Wrong chunk size exp [%ld] found [%ld]\n", chk->bytesToExpectOnCurrentRead, chk->bytesActuallyRead);
                    // resultIndex = -1;
                    chk->decodedIndex = -1;
                    break;

                }
                chk->isReadingChunk = false;
                chk->bytesActuallyRead = 0;
            }

            chk->offset++;
        }
        else
        {

            if (!chk->isReadingChunk)
            {
                chk->sizeBuffer[chk->sizeBufferIndex] = currentChar;
                chk->sizeBufferIndex++;
                //printf("> %c\n", currentChar);
            }
            else
            {

                if(currentChar == '\r' && chk->offset == chk->bufferSize-1){
                    puts("NULL found");
                    break;
                }

                buff[chk->decodedIndex] = currentChar;
                chk->decodedIndex++;
                chk->bytesActuallyRead++;

                //printf("%ld> %c\n", chk->decodedIndex, currentChar);
            }
        }

        chk->offset++;

        if (chk->decodedIndex == amountOfBytesToRead)
        {
            break;
        }

    }

    // -1 when no unread length;

    currentChar = chk->encodedBuffer[chk->offset];

    if(chk->finished){
        chk->remainingUnprocessedBytes = 0;
        free(chk->encodedBuffer);
        chk->encodedBuffer = NULL;
    } else {
        chk->remainingUnprocessedBytes = chk->bufferSize - chk->offset;
        char *temp = malloc(chk->remainingUnprocessedBytes + 1);
        memcpy(temp, chk->encodedBuffer + chk->offset, chk->remainingUnprocessedBytes);

        free(chk->encodedBuffer);

        chk->encodedBuffer = temp;
        chk->bufferSize = chk->remainingUnprocessedBytes;
        chk->offset = 0;
    }

    int res = chk->decodedIndex;
    mtx_unlock(&(chk->chunkMutex));

    return res;
}


/**
 * to feed a continuation of the chk buffer. not for adding new buffer
*/
void chunkz_feed(chunkz *chk, char *buff, size_t bsize) {
    mtx_lock(&(chk->chunkMutex));

    size_t remainingUnprocessedBytes = chk->bufferSize - chk->offset;
    char *tmp = malloc(remainingUnprocessedBytes + bsize + 1);
    size_t indx = chk->offset;

    for(size_t i = 0; i < remainingUnprocessedBytes; i++){
        tmp[i] = chk->encodedBuffer[indx];
        indx++;
    }

    for(size_t i = 0; i < bsize; i++){
        tmp[indx] = buff[i];
        indx++;
    }

    free(chk->encodedBuffer);

    chk->encodedBuffer = tmp;
    chk->bufferSize = chk->remainingUnprocessedBytes + bsize;
    chk->offset = 0;
    chk->remainingUnprocessedBytes = remainingUnprocessedBytes + bsize;

    mtx_unlock(&(chk->chunkMutex));
}

ssize_t http_client_read(http_client *client, char *_buff, size_t bytesToRead){

          if(client->chunked_body){

            if(client->chunker->finished)
              return 0;

            ssize_t bytesRead = 0;

            ssize_t whatToReadThisRound = bytesToRead;

            while (true)
            {
                char buff[10] = {0};
                int readb = SSL_read(client->handle, buff, 10);

                if(readb < 1){
                    bytesRead = readb;
                    break;
                }

                chunkz_feed(client->chunker, buff, readb);

                ssize_t x = chunkzRead(client->chunker, _buff + bytesRead, whatToReadThisRound);


                if(x < 0) {
                  bytesRead = x;
                    break;
                }

                bytesRead += x;

                if(x < whatToReadThisRound){
                  whatToReadThisRound = bytesToRead - bytesRead;
                } else {
                  break;
                }

                if(client->chunker->finished){
                  break;
                }

            }

            return bytesRead;
            
        } else {
          return SSL_read(client->handle, _buff, bytesToRead);
        }
}