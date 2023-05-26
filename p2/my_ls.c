#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h> 
#include <dirent.h>
#include <string.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <fts.h>
#include <time.h>
#include <glob.h>

#include "main.h"

static void list_file(char *fn, char *name)
{
	struct stat     buf;
	char	tbuf[32];
	struct tm * timeinfo;
	int	status;

	status = lstat(fn, &buf);
	if (status < 0) return;

	if (!(S_ISDIR(buf.st_mode) || S_ISREG(buf.st_mode))) {
	    return;
	}

	timeinfo = localtime (&buf.st_ctime);

	strftime (tbuf,32,"%F %T",timeinfo);

	printf("%s ", tbuf);

	if (S_ISDIR(buf.st_mode)) {
	    printf("%10s", "dir");
	}
	else {
	    printf("%10lld", (long long) buf.st_size);
	}
	if (name == NULL) {
	    char *p = strrchr(fn, '/');

	    name = fn;
	    if (p) 
		name = p + 1;
	}
	printf(" %s\n", name);
}


static int use_glob (char * path)
{
    glob_t globbuf;
    int	 i;
    int	 r;

    globbuf.gl_offs = 0;
    r = glob(path, GLOB_DOOFFS, NULL, &globbuf); 
    if (r) {
	// printf("r=%d\n", r);
	globfree(&globbuf);
	return r;
    }

    for (i = 0; i < globbuf.gl_pathc; i ++) {
	list_file(globbuf.gl_pathv[i], NULL);
    }

    globfree(&globbuf);
    return 0;
//     execvp("ls", &globbuf.gl_pathv[0]);
}

void do_my_ls(char* path)
{
    char resolved_path[MAXPATHLEN + 128];
    int	is_dir = 0;
    int	rv;

    if (strcmp(path, ".")==0 || strcmp(path, "/")==0) {
	strcpy(resolved_path, chroot_path);
    } else {
	if (REALPATH(path, resolved_path, chroot_path) == NULL) {
	    // printf("path: %s \nresolved: %s \n", path, resolved_path);
	    my_errormsg(EXIT_FAILURE, "Error in path or no permission to access it: %s\n", path);
	}
    }
    // printf("resolved_path: %s\n", resolved_path);

    do_setuid();
    DIR * d;
    d = opendir(resolved_path);	    //To take the path from realpath function
    if (d) {
	closedir(d);
	strcat(resolved_path, "/*");
	is_dir = 1;
    }
    else if (EACCES == errno) {
	my_errormsg(EXIT_FAILURE, "ls %s %s\n", path, strerror(errno));
    }
    rv = use_glob(resolved_path);
    if (rv == GLOB_NOMATCH) {
	if (!is_dir) 
	    my_errormsg(EXIT_FAILURE, "No such file or directory. %s\n", path);
    }
    else if (rv) {
	my_errormsg(EXIT_FAILURE, "ls error code %d. %s\n", rv, path);
    }
}

#if 0
// #include <linux/limits.h>
// static char fullpath[PATH_MAX]; 
    DIR * d;
    d = opendir(resolved_path);	    //To take the path from realpath function
    if (! d) {
	if (use_glob(resolved_path) < 0) {
	    my_errormsg(EXIT_FAILURE, "Cannot open directory '%s': %s\n", path, strerror (errno));
	}
    }
    while (1) 
    {
        struct dirent * entry;

        entry = readdir (d);
        if (! entry) {
            break;
        }
        if (entry->d_name[0] == '.')
       	{   
	    if (entry->d_name[1] == 0 || (entry->d_name[1] == '.' && entry->d_name[2] == 0))
		continue;
	}

	strncpy3(fullpath, resolved_path, "/", entry->d_name, PATH_MAX - 1);
	list_file(fullpath, entry->d_name);
    }

    // Close the directory. 
    if (closedir (d)) {
	my_errormsg(EXIT_FAILURE, "Cannot close directory '%s': %s\n", path, strerror (errno));
    }
#endif

