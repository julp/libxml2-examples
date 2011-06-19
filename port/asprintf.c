#ifndef _GNU_SOURCE
int asprintf(char **ptr, const char *pat, ...) {
	va_list ap;
	int len = 0;

	va_start(ap, pat);
    len = vsprintf(NULL, pat, ap);
    va_end(ap);

	*ptr = calloc(1, len+1);
	if(!*ptr) {
		perror("calloc");
		exit(EXIT_FAILURE);
    }

    va_start(ap, pat);
    vsprintf(*ptr, pat, ap);
    va_end(ap);

    return len;
}
#endif /* _GNU_SOURCE */
