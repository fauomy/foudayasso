#include <stdlib.h> 
#include <stdio.h>

void print_help (char* progname)
{
    extern char chroot_path[64];
     printf(
"Usage: %s [command and options]\n"
"The valid commands are:\n"
"\tls\t[dir or file in the vault]\n"
"\tget\t[file in the vault] [local dir or file]\n"
"\tput\t[local file] [file or dir in the valut]\n"
"\tmkdir\t[path in the valut]\n"
"\trmdir\t[path in the valut]\n"
"\trm\t[file in the valut]\n"
"\nLocation of the vault: %s\n"
, progname, chroot_path);
     exit (-1);
}

void my_errormsg (char* s, int err)
{
    fprintf(stderr, "%s\n", s);
    exit (err);
}
