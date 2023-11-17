#include <stdio.h>
#include <string.h>

#define FILE_LINE_BUFFER_SIZE 256
#define FILE_BUFFER_SIZE 4096

#define UTIL_SUCCESS 1
#define UTIL_FAILURE 0

int read_file_source(const char* file_path, char *dest_buffer) {
    FILE *fp;
    int status = UTIL_FAILURE;

    dest_buffer[0] = '\0';
    fp = fopen(file_path, "r");

    if (fp) {
        char line[FILE_LINE_BUFFER_SIZE];
        unsigned int pos = 0;
        while (fgets(line, FILE_LINE_BUFFER_SIZE, fp) != NULL) {
            unsigned int line_len = strlen(line);
            strcpy(dest_buffer + pos, line);
            pos += line_len;
        }
        fgets(dest_buffer, FILE_BUFFER_SIZE, fp);
        status = UTIL_SUCCESS;
        fclose(fp);
    }

    return status;
}

