/*
 * waitproc - Wait for an arbitrary process
 *
 * Copyright (C) 2011 Mikko Värri (vmj@linuxbox.fi)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys/types.h>          /* for DIR */
#include <stdint.h>             /* for uint8_t */
#include <dirent.h>             /* for opendir()/closedir() */
#include <stdlib.h>             /* for malloc() */
#include <string.h>             /* for strlen() */
#include <stdio.h>              /* for sprintf() */
#include <unistd.h>             /* for sleep() */
#include <error.h>              /* for error() */
#include <errno.h>              /* for errno */
#include <argp.h>               /* for argp stuff */

/* See info libc -> Argp Global Variables */

/**
 * This is what '--version' shows (implemented by argp).
 */
const char *argp_program_version = "waitproc 0.1"
        "\nCopyright (C) 2011 Mikko Värri"
        "\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>"
        "\nThis is free software: you are free to change and redistribute it."
        "\nThere is NO WARRANTY, to the extent permitted by law.";

/**
 * This is what '--help' shows as bug reporing address (implemented by
 * argp).  We're abusing it to implement the GNU Coding Conventions for
 * '--help' option.
 */
const char *argp_program_bug_address = "vmj@linuxbox.fi"
        "\nwaitproc home page: <http://linuxbox.fi/~vmj/waitproc/>"
        "\nGeneral help using GNU software: <http://www.gnu.org/gethelp/>";

/* Local types */

#define TRUE 1        /**< Boolean true. */

/**
 * Configuration.  Filled during option parsing.
 *
 * @interval   Interval, in seconds, how often to check for status.
 */
typedef struct config_t config_t;
struct config_t {
        uint8_t interval;
        char *pid;
};

/**
 * Vector of command line options.
 *
 * @name    Long option.
 * @key     Short option (int).
 * @arg     NULL or name of the option argument.
 * @flags   Option flags.
 * @doc     Option documentation or group header.
 * @group   Group identity.
 */
static struct argp_option options[] = {
        {"interval", 'i', "SECONDS", 0,
         "Interval, in seconds, how often to check (default: 5)"},
        {0}
};

/**
 * Command line option handler (callback).
 *
 * @key     Short option or special key.
 * @arg     Option argument or non-option argument or NULL.
 * @state   Argp parsing state.
 */
static error_t
handle_option(int key, char *arg, struct argp_state *state)
{
        error_t err = 0;
        config_t *config = (config_t *) state->input;
        unsigned long int value = 0;
        char *end = NULL;

        switch (key)
        {
        case 'i':
                value = strtoul(arg, &end, 0);
                if (value == ULONG_MAX && errno == ERANGE)
                        error(EXIT_FAILURE, ERANGE, arg);
                if (*end != '\0' || arg == end)
                        error(EXIT_FAILURE, EINVAL, arg);
                if (value > UINT8_MAX)
                        error(EXIT_FAILURE, ERANGE, arg);
                config->interval = (uint8_t) value;
                break;
        case ARGP_KEY_ARG:
                config->pid = arg;
                break;
        default:
                err = ARGP_ERR_UNKNOWN;
                break;
        }

        return err;
}

/**
 * Argp parser.
 *
 * @options       NULL or pointer to a vector of argp_option structures.
 * @parser        NULL or pointer to a option handing function.
 * @args_doc      NULL or newline separated non-option names.
 * @doc           NULL or vertical tab separated documentation.
 * @childre       NULL or pointer to a vector of other parsers.
 * @help_filter   NULL or pointer to a help filter function.
 * @argp_domain   NULL or translation domain string.
 */
static const struct argp argp = { options, handle_option, "PID",
        "waitproc - Wait for an arbitrary process"
};

/**
 * Entry point.
 *
 * @argc   Number of command line options and arguments.
 * @argv   Vector of command line options and arguments.
 */
int
main(int argc, char **argv)
{
        error_t err = 0;
        config_t config = { 5 };
        char *procdirname = NULL;
        int printed = 0;
        DIR *procdir = NULL;

        /* Parse the command line options and arguments */
        err = argp_parse(&argp, argc, argv, 0, NULL, &config);
        if (err)
                error(EXIT_FAILURE, err, "argp_parse");

        if (!config.pid)
                error(EXIT_FAILURE, 0, "Too few arguments");

        /* Alloc space for "/proc/<PID>" */
        procdirname = (char *)malloc(6 + strlen(config.pid) + 1);
        if (!procdirname)
                error(EXIT_FAILURE, errno, "malloc");

        /* Format directory name */
        printed = sprintf(procdirname, "/proc/%s", config.pid);
        if (printed < 0)
                error(EXIT_FAILURE, -printed, "sprintf");

        while (1)
        {
                procdir = opendir(procdirname);
                if (procdir)
                {
                        /* The process is still there */
                        if (closedir(procdir))
                                error(EXIT_FAILURE, errno, "closedir");
                        procdir = NULL;
                        /* sleep can only fail if interrupted */
                        if (sleep(config.interval))
                                error(EXIT_FAILURE, EINTR, "sleep");
                }
                else
                {
                        /* The directory disappearing is success */
                        if (errno != ENOENT)
                                error(EXIT_FAILURE, errno, "opendir");
                        break;
                }
        }

        return 0;
}
