#ifndef _GNU_SOURCE
size_t strnlen(const char *string, size_t maxlen)
{
  const char *end = memchr (string, '\0', maxlen);
  return end ? (size_t) (end - string) : maxlen;
}

char *strndup(const char *s, size_t n) {
    size_t len = strnlen(s, n);
    char *copy = malloc(len + 1);

    if (copy == NULL) {
        return NULL;
    }
    copy[len] = '\0';
    return (char *) memcpy (copy, s, len);
}
#endif /* _GNU_SOURCE */
