#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096

#define USAGE "USAGE: Programme qui affiche qui affiche un message sur la sortie standard, dans ce message doit figurer le premier mot passé sur la ligne de commande après le nom du programme"
#define USAGE_SYNTAX "[OPTIONS] Any string after the binary name"
#define USAGE_PARAMS "OPTIONS:\n\
  -v, --verbose : enable *verbose* mode\n\
  -h, --help    : display this help\n\
"

/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char *bin_name)
{
    dprintf(1, "%s %s\n%s\n\n%s\n", bin_name, USAGE, USAGE_SYNTAX, USAGE_PARAMS);
}

/**
 * Procedure checks if variable must be free
 * (check: ptr != NULL)
 *
 * \param void* to_free pointer to an allocated mem
 * \see man 3 free
 * \return void
 */
void free_if_needed(void *to_free)
{
    if (to_free != NULL)
        free(to_free);
}

/**
 *
 * \see man 3 strndup
 * \see man 3 perror
 * \return
 */
char *dup_optarg_str()
{
    char *str = NULL;

    if (optarg != NULL)
    {
        str = strndup(optarg, MAX_PATH_LENGTH);

        // Checking if ERRNO is set
        if (str == NULL)
            perror(strerror(errno));
    }

    return str;
}

/**
 * Binary options declaration
 * (must end with {0,0,0,0})
 *
 * \see man 3 getopt_long or getopt
 * \see struct option definition
 */
static struct option binary_opts[] =
    {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}};

/**
 * Binary options string
 * (linked to optionn declaration)
 *
 * \see man 3 getopt_long or getopt
 */
const char *binary_optstr = "hvi:o:";

/**
 * Binary main loop
 *
 * \return 1 if it exit successfully
 */
int main(int argc, char **argv)
{
    /**
     * Binary variables
     * (could be defined in a structure)
     */
    short int is_verbose_mode = 0;

    // Parsing options
    int opt = -1;
    int opt_idx = -1;

    while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
    {
        switch (opt)
        {
        case 'v':
            // verbose mode
            is_verbose_mode = 1;
            break;
        case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }

    /**
     * Checking binary requirements
     * (could defined in a separate function)
     */
    if (argv[1] == NULL)
    {
        dprintf(STDERR, "Bad usage! See HELP [--help|-h]\n");
        // Exiting with a failure ERROR CODE (== 1)
        exit(EXIT_FAILURE);
    }

    // Concatenating all args in a single string
    char allArgs[4096] = "";
    for (int i = 1; i < argc; i++)
    {
        strcat(allArgs, argv[i]);
        strcat(allArgs, " ");
    }

    // Printing params
    dprintf(1, "** PARAMS **\n%-8s: %s\n%-8s: %d\n\n",
            "arguments", allArgs,
            "verbose", is_verbose_mode);

    // Printing the first word passed on the command line
    dprintf(STDOUT, "Message: %s\n\n", argv[1]);

    // Creating a child process
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0)
    {
        dprintf(STDOUT, "Child PID: %d\n", getpid());

        // Close STDOUT 
        close(STDOUT);

        // Create a temporary file in /tmp
        char temp_filename[] = "/tmp/proc-exerciseXXXXXX";
        int fd = mkstemp(temp_filename);
        if (fd == -1)
        {
            perror("mkstemp");
            exit(EXIT_FAILURE);
        }

        // Print the file descriptor of the temporary file
        dprintf(STDOUT, "File Descriptor of /tmp/proc-exercise: %d\n", fd);

        exit(EXIT_FAILURE);
    }
    else
    {
        // Print the PID of the parent process
        dprintf(STDOUT, "Parent PID: %d\n", getpid());

        // Wait for the child process to terminate
        int status;
        waitpid(child_pid, &status, 0);

        dprintf(STDOUT, "That’s All Folks !\n");
    }

    return EXIT_SUCCESS;
}