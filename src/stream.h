#pragma once

#include <sys/socket.h>
#include <stdbool.h>
#include "_string.h"
#include <poll.h>


typedef struct stream_t stream_t;

struct stream_t {
    int handle;

    struct pollfd *pfd;
    /**
     * if mock buff
    */
    bool mock;
    char *buff;

    size_t offset;
    size_t buffSize;
};


stream_t *stream_init(int fd);

stream_t *stream_init_mock(char *buff, size_t buffSize);

size_t stream_peek(stream_t *stream, char *buffer ,size_t num);

size_t stream_read(stream_t *stream, char *buffer, size_t num);

string_t *stream_read_line(stream_t *stream, char *delim, bool includeDelim);