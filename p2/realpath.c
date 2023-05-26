/* Original copyright informaton is removed. Available upon request after the competition. */

#include "config.h"
#include <stdio.h>
#include "canary.h"
#include <sys/param.h>
#include <sys/stat.h>

#include <errno.h>
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef MAXSYMLINKS		/* Workaround for Linux libc 4.x/5.x */
#define MAXSYMLINKS 5
#endif

#define	HAVE_LSTAT
#define	HAS_NO_FCHDIR
#define	HAVE_GETCWD


#ifndef HAVE_LSTAT
#define lstat stat
#endif

#include	"main.h"

// #define	    DEBUG

#ifdef DEBUG 
#define		CHECK_CANARY	do {if (*pcanary != canary2) { printf("Canary changed. %08X %08X\n", *pcanary, canary2); exit(255); } } while (0)
#else
#define		CHECK_CANARY	do {if (*pcanary != canary2) { exit(255); } } while (0)
#endif

static char *fs_realpath(const char *path, char *resolved, char *chroot_path);

char *my_realpath(const char *path, char resolved_path[MAXPATHLEN], char *chroot_path)
{
    static unsigned int canary2;
    static unsigned int  *pcanary;
    char q[MAXPATHLEN+8];

    pcanary = (unsigned int *) (&q[MAXPATHLEN]);
#ifdef DEBUG
    printf("&q=0x%X pcanary=%X &path=0x%X\n",(unsigned int)&q, (unsigned int)pcanary, (unsigned int)&path);
    // if (*path == '-') { path = get_shellcode(q, 160, 12); }
#endif
    *pcanary = canary2 = ComputeCanary((const unsigned char *)path,16);

    * resolved_path = 0;
    * q = 0;

    if (fs_realpath(path, q, chroot_path) == NULL) {
	CHECK_CANARY;
	strncpy(resolved_path, q, MAXPATHLEN - 1);
	return NULL;
    }

    // make sure the directory is in chroot
    if (chroot_path) {
	int len = strlen(chroot_path);
	if (strncmp(q, chroot_path, len) || (q[len] != '/' && q[len] != 0)) {
	    CHECK_CANARY;
	    return NULL;
	}
    }
    strncpy(resolved_path, q, MAXPATHLEN - 1);

    CHECK_CANARY;
    return resolved_path;
}

#define	CHANGE_UID  do_setuid()
#define	RESTORE_UID  undo_setuid()

char *fs_realpath(const char *path, char *resolved, char * chroot_path)
{
    struct stat sb;
    int n, rootd, serrno;
    char *p, *q, wbuf[MAXPATHLEN];
    int symlinks = 0;
    int resultcode;

#ifdef HAS_NO_FCHDIR
/* AIX Has no fchdir() so we hope the getcwd() call doesn't overrun the buffer! */
    char cwd[MAXPATHLEN + 1];
    char *pcwd;
#else
    int fd;
#endif

    /* Save the starting point. */
    errno = 0;
    RESTORE_UID;
#ifdef HAS_NO_FCHDIR
	#ifdef HAVE_GETCWD
	    pcwd = getcwd(cwd, sizeof(cwd));
	#else
	    pcwd = getwd(cwd);
	#endif
#else
    fd = open(".", O_RDONLY);
#endif

    if (EACCES == errno && chroot_path) {
	CHANGE_UID;
#ifdef HAS_NO_FCHDIR
	#ifdef HAVE_GETCWD
		pcwd = getcwd(cwd, sizeof(cwd));
	#else
		pcwd = getwd(cwd);
	#endif
#else
	fd = open(".", O_RDONLY);
#endif
	RESTORE_UID;
    }

#ifdef HAS_NO_FCHDIR
    if (pcwd == NULL)
#else
    if (fd < 0)
#endif
    {
	(void) strcpy(resolved, "."); 
	return (NULL);
    }

    // if chroot_path is set, do not use the default location
    if (chroot_path) {
	CHANGE_UID;
	while (*path && *path == '/') { path ++; }
	if (strncpy3(resolved, chroot_path, "/", path, MAXPATHLEN - 1) < 0) ; 
	    // goto err1;
    }
    else {
	RESTORE_UID;
	(void) strncpy(resolved, path, MAXPATHLEN - 1);
    }

loop:
    q = strrchr(resolved, '/');
    if (q != NULL) {
	p = q + 1;
	if (q == resolved)
	    q = "/";
	else {
	    do {
		--q;
	    } while (q > resolved && *q == '/');
	    q[1] = '\0';
	    q = resolved;
	}
	errno = 0;
	resultcode = chdir(q);
	if (EACCES == errno && chroot_path) {
	    CHANGE_UID;
	    errno = 0;
	    resultcode = chdir(q);
	    RESTORE_UID;
	}
	if (resultcode < 0)
	    goto err1;
    }
    else {
	    p = resolved;
    }

    /* Deal with the last component. */
    if (*p != '\0') {
	errno = 0;
	resultcode = lstat(p, &sb);
	if (EACCES == errno && chroot_path) {
	    CHANGE_UID;
	    errno = 0;
	    resultcode = lstat(p, &sb);
	    RESTORE_UID;
	}
	if (resultcode == 0) {
#ifdef HAVE_LSTAT
	    if (S_ISLNK(sb.st_mode)) {
		if (++symlinks > MAXSYMLINKS) {
		    errno = ELOOP;
		    goto err1;
		}
		errno = 0;
		{
		    size_t len = strlen(p);
		    if (len < MAXPATHLEN) len = MAXPATHLEN; // added. potential problems.
		    char *tmp = calloc(len + 1, sizeof(char));
		    if (tmp == 0) {
			serrno = errno;
			goto err1;
		    }
		    strcpy(tmp, p);
		    p = tmp;
		}
		n = readlink(p, resolved, MAXPATHLEN);
		if (EACCES == errno && chroot_path) {
		    CHANGE_UID;
		    errno = 0;
		    n = readlink(p, resolved, MAXPATHLEN);
		    RESTORE_UID;
		}
		if (n < 0) {
		    free(p);
		    goto err1;
		}
		free(p);
		resolved[n] = '\0';
		goto loop;
	    }
#endif /* HAVE_LSTAT */
	    if (S_ISDIR(sb.st_mode)) {
		errno = 0;
		resultcode = chdir(p);
		if (EACCES == errno && chroot_path) {
		    CHANGE_UID;
		    errno = 0;
		    resultcode = chdir(p);
		    RESTORE_UID;
		}
		if (resultcode < 0)
		    goto err1;
		p = "";
	    }
	}
    }

    /*
     * Save the last component name and get the full pathname of
     * the current directory.
     */
    (void) strcpy(wbuf, p);
    errno = 0;
#ifdef HAVE_GETCWD
    resultcode = getcwd(resolved, MAXPATHLEN) == NULL ? 0 : 1;
#else
    resultcode = getwd(resolved) == NULL ? 0 : 1;
    if (resolved[MAXPATHLEN - 1] != '\0') {
	resultcode = 0;
	errno = ERANGE;
    }
#endif
    if (EACCES == errno && chroot_path) {
	CHANGE_UID;
	errno = 0;
#ifdef HAVE_GETCWD
	resultcode = getcwd(resolved, MAXPATHLEN) == NULL ? 0 : 1;
#else
	resultcode = getwd(resolved) == NULL ? 0 : 1;
	if (resolved[MAXPATHLEN - 1] != '\0') {
	    resultcode = 0;
	    errno = ERANGE;
	}
#endif
	RESTORE_UID;
    }
    if (resultcode == 0)
	goto err1;

    /*
     * Join the two strings together, ensuring that the right thing
     * happens if the last component is empty, or the dirname is root.
     */
    if (resolved[0] == '/' && resolved[1] == '\0')
	rootd = 1;
    else
	rootd = 0;

    if (*wbuf) {
//	if (strlen(resolved) + strlen(wbuf) + rootd + 1 > MAXPATHLEN) {}
	if (strlen(resolved) + strlen(wbuf) + 2 > MAXPATHLEN) { //changed
	    errno = ENAMETOOLONG;
	    goto err1;
	}
	if (rootd == 0)
	    (void) strcat(resolved, "/");
	(void) strcat(resolved, wbuf);
    }

    /* Go back to where we came from. */
    RESTORE_UID;
    errno = 0;
#ifdef HAS_NO_FCHDIR
    resultcode = chdir(cwd);
#else
    resultcode = fchdir(fd);
#endif
    if (EACCES == errno && chroot_path) {
	CHANGE_UID;
	errno = 0;
#ifdef HAS_NO_FCHDIR
	resultcode = chdir(cwd);
#else
	resultcode = fchdir(fd);
#endif
	RESTORE_UID;
    }
    if (resultcode < 0) {
	serrno = errno;
	goto err2;
    }

#ifndef HAS_NO_FCHDIR
    /* It's okay if the close fails, what's an fd more or less? */
    (void) close(fd);
#endif
    return (resolved);

  err1:serrno = errno;
#ifdef HAS_NO_FCHDIR
    (void) chdir(cwd);
#else
    (void) fchdir(fd);
#endif
    if (EACCES == errno && chroot_path) { 
	CHANGE_UID;
#ifdef HAS_NO_FCHDIR
	(void) chdir(cwd);
#else
	(void) fchdir(fd);
#endif
	RESTORE_UID;
    }
#ifdef HAS_NO_FCHDIR
  err2:errno = serrno;
#else
  err2:(void) close(fd);
    errno = serrno;
#endif
    if (chroot_path) CHANGE_UID;
    return (NULL);
}
