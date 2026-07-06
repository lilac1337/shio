#include "file.h"

#include "input.h"
#include "output.h"
#include "row.h"
#include "syntax.h"
#include "terminal.h"

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: trailing \n dialog for EOF
char *rows2string(ssize_t *buflen) {
    size_t totlen = 0;
    size_t j;

    for (j = 0; j < c.nrows; ++j) {
        totlen += c.r[j].size + 1;
    }

    *buflen = (ssize_t)totlen;
    char *buf = malloc(totlen);

    if (buf == NULL)
        return NULL;
    
    char *p = buf;
    
    for (j = 0; j < c.nrows; ++j) {
        memcpy(p, c.r[j].chars, c.r[j].size);
        p += c.r[j].size;
        if (j != c.nrows)
            *p = '\n';
        ++p;
    }

    return buf;
}

void fileopen(const char *f) {
    free(c.fn);
    c.fn = strdup(f);

    selectsyntaxhl();
    
    FILE *fp = fopen(f, "r");

    if (!fp)
        die("fp from fopen is null");

    char *line = NULL;
    size_t cap = 0u;
    ssize_t len = 0u;

    while ((len = getline(&line, &cap, fp)) != -1) {
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            --len;

        insertrow(c.nrows, line, (size_t)len);
    }

    free(line);
    fclose(fp);

    c.dirty = 0u;
}

void save() {
    if (c.fn == NULL) {
        c.fn = prompt("save as: %s", NULL);
        
        if (c.fn == NULL) {
            setstatus("save cancelled");
            return;
        }

        selectsyntaxhl();
    }
        
    ssize_t len;
    int fd = 0;
    char *buf = rows2string(&len);

    if (buf == NULL)
        goto error;

    fd = open(c.fn, O_RDWR | O_CREAT, 0644);

    if (fd == -1)
        goto error;
    
    int ft = ftruncate(fd, len);

    if (ft == -1)
        goto error;

    ssize_t wr = write(fd, buf, (size_t)len);

    if (wr != len)
        goto error;

    setstatus("%zu bytes written to %s", len, c.fn);

    close(fd);
    free(buf);

    c.dirty = 0u;
    
    return;

error:
    setstatus("couldn't save file %s: %s", c.fn, strerror(errno));
    
    if (fd >= 0)
        close(fd);

    if (buf)
        free(buf);
}
