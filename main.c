#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <bloom.h>

// include newline and null terminator
#define BUFFER_SIZE (2048+2)
#define VERSION_STRING "0.2.0"

#define file_close(f) \
    if ((f)) \
        fclose((f))

typedef struct bloom bloom_t;

int main(int argc, char *argv[]) {
    FILE *fp;
    int err = 0;
    char buf[BUFFER_SIZE];

    if (argc == 2) {
        if (!strcmp(argv[1], "-h")) {
            fprintf(stderr,
                    "bloniq v"VERSION_STRING
                    "\nUsage: %s [input-file]\n",
                    argv[0]);
            return 1;
        } else {
            if (!(fp = fopen(argv[1], "rb"))) {
                perror("fopen");
                err = errno;
                goto ret;
            }
        }
    } else {
        fp = stdin;
    }

    bloom_t bloom;
    if ( (err = bloom_init2(&bloom, (1 << 20), 0.01)))
        goto ret_fp_close;

    size_t buf_len;
    while (fgets(buf, BUFFER_SIZE, fp)) {
        buf_len = strlen(buf);
        // remove the newline
        buf[buf_len] = 0; buf_len--;
        if (!bloom_check(&bloom, buf, buf_len)) {
            bloom_add(&bloom, buf, buf_len);
            fputs(buf, stdout);
        }
    }

    bloom_free(&bloom);

ret_fp_close:
    file_close(fp);
ret:
    return err;
}
