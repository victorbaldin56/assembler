#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include "codector.h"
#include "parse.h"

LineArray *ParseBuf(char *buf, LineArray *text) {
    assert(buf);
    assert(text);

    text->size = strcount_(buf, '\n') + 1;

    text->lines = (char **)calloc(text->size, sizeof(void *));

    if (!text->lines) return NULL;

    for (size_t lp = 0; lp < text->size; lp++) {
        text->lines[lp] = buf;
        char *newbuf = strchr(buf, '\n');
        // there are no more '\n'
        if (!newbuf) break;
        *newbuf = '\0';

        char *comment = strchr(buf, ';');

        if (comment) *comment = '\0'; // commenting code in asm

        buf = newbuf + 1;
    }

    return text;
}

char *ReadFile(const char *filename) {
    assert(filename);

    size_t bufsiz = (size_t)(fsize(filename) + 1);

    if (!bufsiz) return NULL;

    char *buf = (char *)calloc(bufsiz, 1);

    if (!buf) return NULL;

    int fd = open(filename, O_RDONLY, 0);
    read(fd, buf, bufsiz);

    close(fd);
    return buf;
}

ssize_t fsize(const char *filename) {
    assert(filename);

    struct stat st = {};

    if (stat(filename, &st) == -1) {
        return -1;
    }

    return st.st_size;
}

size_t strcount_(const char *str, const char chr) {
    assert(str);

    size_t count = 0;

    for (count = 0; (str = strchr(str, chr)); count++) {
        str++;
    }

    return count;
}
