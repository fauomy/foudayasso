#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h> 
#include <dirent.h>
#include <string.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <fts.h>
#include <unistd.h>

#include "main.h"

static char *get_chrooted_path(char * resolved, char *s)
{
    return my_realpath(s, resolved, chroot_path);
}

void do_my_mkdir(char* path)
{
    char return_path[MAXPATHLEN];
    int status;

    if (! get_chrooted_path(return_path, path)) 
        my_errormsg(-10, "Error in directory path %s.\n", path);

    // printf("%s\n", return_path);

    do_setuid();
//    status = mkdir(return_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    status = mkdir(return_path, S_IRWXU);
    if (status != 0) {
        my_errormsg(-10, "mkdir %s\n%s\n", path, strerror(errno));
    }
    undo_setuid();
}

void do_my_rmdir(char* path)
{
    int status;
    char return_path[MAXPATHLEN];

    if (! get_chrooted_path(return_path, path)) 
        my_errormsg(-11, "Error in directory path %s.\n", path);

    do_setuid();
    status = rmdir(return_path);
    if (status != 0) {
        my_errormsg(-11, "rmdir %s\n%s\n", path, strerror(errno));
    }
    undo_setuid();
}

void do_delete_my_file(char* path)
{
    int status;
    char return_path[MAXPATHLEN];

    int	len;

    len = strlen(path);
    if (!len || path[len-1] == '/') 
        my_errormsg(-11, "Error in file name %s.\n", path);
    if (! get_chrooted_path(return_path, path)) 
        my_errormsg(-11, "Error in file name %s.\n", path);

    do_setuid();
    status = remove(return_path);
    if (status != 0) {
        my_errormsg(-12, "rm %s\n%s\n",path,strerror(errno));
    }
    undo_setuid();
}
