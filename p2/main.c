#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h> 
#include <dirent.h>
#include <string.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <fts.h>

#include	"main.h"

int main (int argc, char* argv[])
{
    if (argc < 2)
	print_help(argv[0]);

    undo_setuid();
#if 0
    do_setuid(); print_uids();
    undo_setuid(); print_uids();
    do_setuid(); print_uids();
#endif

    {
	int	i;
	for (i = 2; i < argc; i ++) {
	    int r = strlen(argv[i]);
	    if (r >> 8) {
		my_errormsg(EXIT_FAILURE, "Invalid argument: %s\n", argv[i]);
	    }
	}
    }

    if ((strcmp(argv[1], "ls"))==0) {
	// doing ls
	char * ppattern;
	if (argc == 2) {
		ppattern = "/"; //to use the chroot
		do_my_ls(ppattern);
	}
	else if (argc == 3) 
	{	
		do_my_ls(argv[2]);
	}
	else {
	    my_errormsg(EXIT_FAILURE, "Invalid arguments.\n");
	}
    }
    else if ((strcmp(argv[1], "get"))==0) {
	if (argc == 3) 
	    do_my_cp(argv[2], ".", 1);
	else if (argc == 4) 
	    do_my_cp(argv[2], argv[3], 1);
	else
	    my_errormsg(EXIT_FAILURE, "Invalid arguments.\n"); 
    }
    else if ((strcmp(argv[1], "put"))==0) {
	if (argc == 3) 
	    do_my_cp(argv[2], ".", 0);
	else if (argc == 4) 
	    do_my_cp(argv[2], argv[3], 0);
	else
	    my_errormsg(EXIT_FAILURE, "Invalid arguments.\n"); 
    }
    else if ((strcmp(argv[1], "mkdir"))==0){
	if (argc != 3) {
	    my_errormsg (-3, "Error: No directory is specified.\n");
        }
	do_my_mkdir(argv[2]);
    }
    else if ((strcmp(argv[1], "rmdir"))==0) {
        if (argc != 3) {
	    my_errormsg (-3, "Error: No directory is specified.\n");
	}
	do_my_rmdir(argv[2]);
    }
    else if ((strcmp(argv[1], "rm"))==0) {
        if (argc != 3) {
	    my_errormsg (-3, "Error: No file is specified.\n");
	}
	do_delete_my_file(argv[2]);
    }
    else {
	my_errormsg(-2, "Error: Invalid command.\n");
    }

    return 0;
}

