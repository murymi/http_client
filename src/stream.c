#include "stream.h"

stream_t *stream_init(int fd) {
    stream_t *s = malloc(sizeof(stream_t));
    s->pfd = malloc(sizeof(struct pollfd));
    s->pfd->fd = fd;
    s->pfd->events = POLLIN;
    s->mock = false;
    s->handle = fd;

    return s;
}

stream_t *stream_init_mock(char *buff, size_t buffSize){
    stream_t *s = malloc(sizeof(stream_t));
    s->mock = true;
    s->offset = 0;

    char *b = malloc(buffSize + 1);
    
    strncpy(b, buff, buffSize);

    s->buff  = b;

    s->buffSize = buffSize;

    return s;
}

size_t stream_peek(stream_t *stream, char *buffer ,size_t num) {
    if(stream->mock) {
        size_t r = 0;
        for(size_t i = stream->offset; i < stream->buffSize; i++){
            if(r >= num){
                break;
            }

            buffer[r] = stream->buff[stream->offset + r];
            r++;
        }

        //stream->offset += r;
        return r;

    } else {

        ssize_t r = 0;
        size_t rthist = num;
         while(true) {

            int p = poll(stream->pfd, 1, -1);

            if(p < 1) {


                if(p == 0){

                    break; /*time out*/
                }
                if(p < 0)
                    perror("poll");
            }

            ssize_t x = recv(stream->handle, buffer + r, rthist, MSG_PEEK);

            if(x < 1) {
                if(x < 0){
                    perror("stream recv ");
                } else { /** conn close */ }
                break;
            }

            r += x;

            rthist = num - x;

            if(rthist <= 0){
                break;
            }

        }

        return r;
    }
}

size_t stream_read(stream_t *stream, char *buffer, size_t num);

string_t *stream_read_line(stream_t *stream, char *delim, bool includeDelim) {
    size_t delim_len = strlen(delim);

    string_t *line = string_create();

    if(stream->mock) {
        //size_t seq = 0;
//
        //size_t tmpoffs = stream->offset;
//
        //for(size_t i = stream->offset; i < stream->buffSize; i++) {
//
        //    if(stream->buff[i] == delim[0]){
        //        stream->offset++;
        //        char peek_buff[50] = {0};
//
        //        ssize_t y = stream_peek(stream, peek_buff, delim_len - 1);
//
        //        if(strncmp(peek_buff, delim + 1, y) == 0) {
        //            
        //            if(includeDelim){
        //                for(size_t i = 0; i < delim_len; i++){
        //                    buffer[seq] = delim[i];
        //                    seq++;
        //                }
        //            }
//
        //            stream->offset += delim_len - 1;
//
        //            break;
        //        } else {
        //            stream->offset--;
        //        }
        //    }
//
        //    buffer[seq] = stream->buff[tmpoffs + seq];
//
        //    seq++;
        //    stream->offset++;
        //}
//
        return NULL;

    } else {

        size_t buffOffset = 0;


        while(true) {
        char recBuff[50] = {0};
            ssize_t x = stream_read(stream, recBuff, 1);

            if(recBuff[0] == delim[0]) {

                char peek_buff[50] = {0};

                size_t s = stream_peek(stream, peek_buff, delim_len - 1);

                if(strncmp(peek_buff, delim + 1, s) == 0) {
                    
                    ssize_t d = stream_read(stream, recBuff + 1, delim_len - 1);

                    if(includeDelim){

                        string_concat(line, recBuff, d + 1);

                        assert(d == (ssize_t)(delim_len - 1));
                    }

                    break;
                }

            }
            
            string_append(line, recBuff[0]);

            buffOffset += x;
        }

        return line;
    }
}


//void read_at_least(stream_t *stream, char *buffer, size_t num);

size_t stream_read(stream_t *stream, char *buffer, size_t num) {
    if(stream->mock) {
        size_t toR = 0;
        for(size_t i = stream->offset; i < stream->buffSize; i++) {
            if(toR == num) {
                break;
            }

            buffer[toR] = stream->buff[i];

            toR++;
        }

        stream->offset += toR;
        return toR;

    } else {

        size_t buffOffset = 0;
        size_t toReadNow = num;

        while(true) {
            int p = poll(stream->pfd, 1, -1);

            if(p < 1) {


                if(p == 0){

                    break; /*time out*/
                }
                if(p < 0)
                    perror("poll");
            }

            ssize_t r = recv(stream->handle, buffer + buffOffset, toReadNow, 0);

            if(r < 1) {
                if(r < 0){
                    perror("recv ");
                }

                break;
            }

            buffOffset += r;

            if(buffOffset == num){    
                break;
            }
            else if(toReadNow > num) {
                assert(num == toReadNow);
            } else {
                toReadNow = num - r;
            }
            
        }

        return buffOffset;
    }
}