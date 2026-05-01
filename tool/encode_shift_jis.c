#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>

// Buffer size for file reading
#define MAX_FILE_SIZE 1024 * 512

void convert_and_escape(const char *input, char *output) {
    iconv_t cd = iconv_open("SHIFT_JIS", "UTF-8");
    if (cd == (iconv_t)-1) {
        perror("iconv_open failed");
        exit(1);
    }

    char *in_ptr = (char *)input;
    size_t in_bytes = strlen(input);
    
    char sjis_buf[1024];
    char *out_ptr = sjis_buf;
    size_t out_bytes = sizeof(sjis_buf);

    memset(sjis_buf, 0, sizeof(sjis_buf));

    if (iconv(cd, &in_ptr, &in_bytes, &out_ptr, &out_bytes) == (size_t)-1) {
        fprintf(stderr, "Iconv conversion failed at: %s\n", in_ptr);
    }

    size_t final_len = sizeof(sjis_buf) - out_bytes;
    char hex[5];
    output[0] = '\0';
    for (size_t i = 0; i < final_len; i++) {
        sprintf(hex, "\\x%02x", (unsigned char)sjis_buf[i]);
        strcat(output, hex);
    }

    iconv_close(cd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    FILE *out = fopen(argv[1], "wb");
    char *cursor = buffer;
    char *tag = "/* SJIS */";

    while (*cursor) {
        char *match = strstr(cursor, tag);
        if (match) {
            fwrite(cursor, 1, match - cursor, out);
            
            char *str_start = strchr(match, '\"');
            if (str_start) {
                char *str_end = strchr(str_start + 1, '\"');
                if (str_end) {
                    size_t len = str_end - (str_start + 1);
                    char content[512];
                    strncpy(content, str_start + 1, len);
                    content[len] = '\0';

                    char *nl;
                    while((nl = strstr(content, "\\n"))) {
                        *nl = '\n';
                        memmove(nl + 1, nl + 2, strlen(nl + 2) + 1);
                    }

                    char hex_out[2048];
                    convert_and_escape(content, hex_out);
                    
                    fprintf(out, "\"%s\"", hex_out);
                    cursor = str_end + 1;
                    continue;
                }
            }
        }
        fputc(*cursor, out);
        cursor++;
    }

    fclose(out);
    free(buffer);
    return 0;
}