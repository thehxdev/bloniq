#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <bloom.h>

// include newline and null terminator
#define BUFFER_SIZE (2048+2)
#define VERSION_STRING "0.1.1"

#define file_close(f) \
    if ((f)) \
        fclose((f))

typedef struct bloom bloom_t;

int main(int argc, char *argv[]) {
    int err = 0;
    if (argc != 3) {
        fprintf(stderr,
                "bloniq v"VERSION_STRING
                "\nUsage: %s (<input-file>|stdin) (<output-file>|stdout)\n",
                argv[0]);
        return 1;
    }

    char buf[BUFFER_SIZE];
    const char *file_path = argv[1];
    const char *out_path = argv[2];

    bloom_t bloom;
    if ( (err = bloom_init2(&bloom, (1 << 20), 0.01)))
        goto ret;

    FILE *fp, *op;
    if (!strcmp(file_path, "stdin"))
        fp = stdin;
    else
        if (!(fp = fopen(file_path, "rb"))) {
            perror("fopen");
            err = errno;
            goto ret_bloom_free;
        }

    if (!strcmp(out_path, "stdout"))
        op = stdout;
    else
        if (!(op = fopen(out_path, "wb"))) {
            perror("fopen");
            err = errno;
            goto ret_close_fp;
        }

    size_t buf_len;
    while (fgets(buf, BUFFER_SIZE, fp)) {
        buf_len = strlen(buf);
        // remove the newline
        buf[buf_len] = 0; buf_len--;
        if (!bloom_check(&bloom, buf, buf_len)) {
            bloom_add(&bloom, buf, buf_len);
            fputs(buf, op);
        }
    }

    file_close(op);
ret_close_fp:
    file_close(fp);
ret_bloom_free:
    bloom_free(&bloom);
ret:
    return err;
}
