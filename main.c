#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <bloom.h>

// include newline and null terminator
#define BUFFER_SIZE (2048+2)
#define VERSION_STRING "0.3.0"

typedef struct bloom bloom_t;

void handle_file(bloom_t *bloom, FILE *fp);

int main(int argc, char *argv[]) {
    FILE *fp;
    int err = 0;
    bloom_t bloom;

    if ( (err = bloom_init2(&bloom, (1 << 20), 0.01)))
        goto ret;

    if (argc >= 2) {
        if (!strcmp(argv[1], "-h")) {
            fprintf(stderr,
                    "bloniq v"VERSION_STRING
                    "\nUsage: %s [input-file]\n",
                    argv[0]);
            goto ret_bloom_free;
        }
        argc--;
        argv++;
        while (argc--) {
            fp = fopen(*argv++, "rb");
            if (!fp) {
                perror("fopen");
                continue;
            }
            handle_file(&bloom, fp);
            fclose(fp);
        }
    } else {
        fp = stdin;
        handle_file(&bloom, fp);
    }

ret_bloom_free:
    bloom_free(&bloom);
ret:
    return err;
}

void handle_file(bloom_t *bloom, FILE *fp) {
    size_t buf_len;
    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, fp)) {
        buf_len = strlen(buf);
        // remove the newline
        buf[buf_len] = 0; buf_len--;
        if (!bloom_check(bloom, buf, buf_len)) {
            bloom_add(bloom, buf, buf_len);
            fputs(buf, stdout);
        }
    }
}
