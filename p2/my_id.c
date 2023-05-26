#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <errno.h> 
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include "main.h"

// #define	PRINTMSG

/* Restore the effective UID to its original value. */

static uid_t euid, ruid; // tuid;
static gid_t egid, rgid;
static int init_done = 0;

void print_uids()
{
    uid_t  ruid_t, euid_t;
    ruid_t = getuid ();
    euid_t = geteuid ();
    printf("ruid=%d, euid=%d\n", ruid_t, euid_t);
}

/*
int getuid_byname(const char *name)
{
    if (name) {
        struct passwd *pwd = getpwnam(name); 
        if (pwd) { tuid = pwd->pw_uid; return 0; } 
    }
    return  -1;
}
*/

static void get_ids()
{
    ruid = getuid ();
    euid = geteuid ();

    rgid = getgid ();
    egid = getegid ();
/*
    if (getuid_byname("one") < 0) {
	my_errormsg(EXIT_FAILURE, "Cannot find user %s\n", "one");
    }
*/
}

int do_setuid (void)
{
    int status;

    if (init_done == 0) { get_ids(); init_done = 1;}

    delay_signaling();

#ifdef _POSIX_SAVED_IDS
  status = seteuid (euid);
#else
  status = setreuid (ruid, euid);
#endif
#ifdef PRINTMSG
    if (status < 0) {
    my_errormsg(EXIT_FAILURE, "Couldn't set uid. r=%d, e=%d,  %s\n", ruid, euid, strerror(errno));
    }
#endif
    return status;
}

int undo_setuid (void)
{
    int status;

    if (init_done == 0) { get_ids(); init_done = 1;}

#ifdef _POSIX_SAVED_IDS
    status = seteuid (ruid);
#else
    status = setreuid (euid, ruid);
#endif

    enable_signaling();

#ifdef PRINTMSG
    if (status < 0) {
	my_errormsg(EXIT_FAILURE, "Couldn't set uid. r=%d, e=%d,  %s\n", ruid, euid, strerror(errno));
	exit (status);
    }
#endif
  return status;
}
