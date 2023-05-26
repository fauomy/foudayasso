#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h> 
#include <dirent.h>
#include <string.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <fts.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "main.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 2048
#endif

#define	FILEPERMS (S_IRUSR | S_IWUSR)

static char buf[BUF_SIZE];

// from vault to user
static void do_my_cp_get(char *src, char *dst)
{
    char   spath[MAXPATHLEN], dpath[MAXPATHLEN];
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;

    if (!my_realpath(src, spath, chroot_path)) {
	 my_errormsg(-20, "Error in path %s\n", spath); 
    }

    if (! my_realpath(dst, dpath, NULL)) {
	 my_errormsg(-20, "Error in path %s\n", dpath); 
    }

     // check if the destination is a directory
    {
	struct stat sb;
	int resultcode;
	int is_dir = 0;

	undo_setuid();
	resultcode = lstat(dpath, &sb);
	if (resultcode == 0) {
	    if (S_ISDIR(sb.st_mode)) {
		is_dir = 1;
	    }
	    else {
		my_errormsg(EXIT_FAILURE, "Error: Found a file with the same name: %s\n", dpath);
	    }
	}
	else {
	    if (errno != ENOENT) {
		my_errormsg(EXIT_FAILURE, "Error in path %s\n%s\n", dpath, strerror(errno));
	    }
	}

	// copy the base name to the destination path 
	if (is_dir) {
	    char *p = strrchr(spath, '/');
	    if (p == NULL) {
		my_errormsg(EXIT_FAILURE, "Error: Not an absolute path %s\n", spath);
	    }
	    strcat(dpath, "/");
	    strcat(dpath, p+1);

	    resultcode = lstat(dpath, &sb);
	    if (resultcode == 0) {
		my_errormsg(EXIT_FAILURE, "Error: Found a file with the same name: %s\n", dpath);
	    }
	    else {
		if (errno != ENOENT) {
		    my_errormsg(EXIT_FAILURE, "Error in path %s\n%s\n", dpath, strerror(errno));
		}
	    }
	}
    }

    do_setuid();
    inputFd = open(spath, O_RDONLY);
    if (inputFd == -1) { 
	my_errormsg(EXIT_FAILURE, "Cannot open the file %s\n%s\n", spath, strerror(errno));
    }

    undo_setuid();
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = FILEPERMS;
    outputFd = open(dpath, openFlags, filePerms);
    if (outputFd == -1) {
	my_errormsg(EXIT_FAILURE, "Error: Cannot create the destination file %s\n%s\n", dpath, strerror(errno));
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
	    my_errormsg(EXIT_FAILURE, "Error: File write error %s\n%s\n", dpath, strerror(errno));
	}
    }

    close(inputFd);
    close(outputFd);
}

static void do_my_cp_put(char *src, char *dst)
{
    char   spath[MAXPATHLEN], dpath[MAXPATHLEN];
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;

    if (!my_realpath(src, spath, NULL)) {
	 my_errormsg(-20, "Error in path %s\n", spath); 
    }

    if (! my_realpath(dst, dpath, chroot_path)) {
	 my_errormsg(-20, "Error in path %s\n", dpath); 
    }

     // check if the destination is a directory
    {
	struct stat sb;
	int resultcode;
	int is_dir = 0;

	do_setuid();
	resultcode = lstat(dpath, &sb);
	if (resultcode == 0) {
	    if (S_ISDIR(sb.st_mode)) {
		is_dir = 1;
	    }
	    else {
		my_errormsg(EXIT_FAILURE, "Error: Found a file with the same name: %s\n", dpath);
	    }
	}
	else {
	    if (errno != ENOENT) {
		my_errormsg(EXIT_FAILURE, "Error in path %s\n%s\n", dpath, strerror(errno));
	    }
	}

	// copy the base name to the destination path 
	if (is_dir) {
	    char *p = strrchr(spath, '/');
	    if (p == NULL) {
		my_errormsg(EXIT_FAILURE, "Error: Not an absolute path %s\n", spath);
	    }
	    strcat(dpath, "/");
	    strcat(dpath, p+1);

	    resultcode = lstat(dpath, &sb);
	    if (resultcode == 0) {
		my_errormsg(EXIT_FAILURE, "Error: Found a file with the same name: %s\n", dpath);
	    }
	    else {
		if (errno != ENOENT) {
		    my_errormsg(EXIT_FAILURE, "Error in path %s\n%s\n", dpath, strerror(errno));
		}
	    }
	}
    }

    undo_setuid();
    inputFd = open(spath, O_RDONLY);
    if (inputFd == -1) { 
	my_errormsg(EXIT_FAILURE, "Cannot open the file %s\n%s\n", spath, strerror(errno));
    }

    do_setuid();
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = FILEPERMS;
    outputFd = open(dpath, openFlags, filePerms);
    if (outputFd == -1) {
	my_errormsg(EXIT_FAILURE, "Error: Cannot create the destination file %s\n%s\n", dpath, strerror(errno));
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
	    my_errormsg(EXIT_FAILURE, "Error: File write error %s\n%s\n", dpath, strerror(errno));
	}
    }

    close(inputFd);
    close(outputFd);
}

int do_my_cp(char *src, char *dst, int is_get)
{
#if 0
     { char *p = get_current_dir_name(); puts(p); free(p);}
#endif 

    if (is_get) {
	do_my_cp_get(src, dst);
    }
    else {
	do_my_cp_put(src, dst);
    }
    return 0;
}
