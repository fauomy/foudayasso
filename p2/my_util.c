
int strncpy3(char *d, const char *s1, const char *s2, const char *s3, int n)
{
    const char *p;
    char *d_old = d;

    p = s1;
    while (*p) *d ++ = *p ++;
    *d = 0;
    if ((d - d_old) > n)  return -1;

    p = s2;
    while (*p) *d ++ = *p ++;
    *d = 0;
    if ((d - d_old) > n)  return -1;

    p = s3;
    while (*p) *d ++ = *p ++;
    *d = 0;
    if ((d - d_old) > n)  return -1;

    return d - d_old;
}

void str_chtr(char *s, int old, int new)
{
    if (!s)
	return;
    while (*s) {
	if (*s == old)
	    *s = new;
	s ++;
    }
}
