#ifndef _GNU_SOURCE
int asprintf(char **path, const char *format, ...) {
    int n, size = 128;
    char *np;
    va_list ap;

    if (NULL == (*path = malloc(size))) {
        return -1;
    }
    while (1) {
        va_start(ap, format);
        n = vsnprintf(*path, size, format, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            return n;
        }
        if (n > -1) {
            size = n + 1;
        } else {
            size *= 2;
        }
        if (NULL == (np = realloc(*path, size))) {
            free(*path);
            return -1;
        } else {
            *path = np;
        }
    }
}
#endif /* _GNU_SOURCE */
