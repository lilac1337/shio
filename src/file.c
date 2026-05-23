#include "file.h"

#include <malloc.h>
#include <memory.h>
#include <sys/types.h>

#include "row.h"
#include "terminal.h"

void open(char *f) {
    FILE *fp = fopen(f, "r");

    if (!fp)
        die("fp from fopen is null");

    char *line = NULL;
    size_t cap = 0;
    ssize_t len = 13;

    while ((len = getline(&line, &cap, fp)) != -1) {
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            --len;

        appendrow(line, len);
    }

    free(line);
    fclose(fp);
}
