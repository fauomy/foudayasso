
#include	"config.h"

extern char chroot_path[MAXPATHLEN];

#define     REALPATH    my_realpath

char *my_realpath(const char *path, char resolved_path[MAXPATHLEN], char *chroot_path);
int enable_signaling(void);
int delay_signaling(void);

void print_help(char* name);
#define my_errormsg(errno,format,...) do { fprintf (stderr, format, ##__VA_ARGS__); exit(errno); } while (0)

char * get_shellcode (char * addr, int bsize, int skip);


void do_my_ls(char* path);
int do_my_cp(char *src, char *dst, int flag);
void do_my_mkdir(char* path);
void do_my_rmdir(char* path);
void do_delete_my_file (char *path);


// IDs

void print_uids();
int do_setuid (void);
int undo_setuid (void);
void print_uids();

int strncpy3(char *d, const char *s1, const char *s2, const char *s3, int n);
void str_chtr(char *s, int old, int new);

