#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096

#define USAGE "USAGE: programme C équivalent au script shell suivant : ps eaux | grep \"^root \" > /dev/null && echo \"root est connecté\""
#define USAGE_SYNTAX "[OPTIONS] No parameters needed"
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

    // Business logic
    // Create a pipe to establish communication between parent and child processes
    int pipe_fd[2];

    // Check for errors while creating the pipe
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return EXIT_FAILURE;
    }

    // Fork a child process
    pid_t child_pid = fork();

    // Check for errors during fork
    if (child_pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child_pid == 0)
    {
        // Child process
        // Redirect stdout to the write end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Execute the ps command
        execlp("ps", "ps", "eaux", (char *)NULL);

        // execlp will only return if an error occurs
        perror("execlp");
        return EXIT_FAILURE;
    }
    else
    {
        // Parent process
        // Wait for the child process to complete
        wait(NULL);

        // Close the write end of the pipe
        close(pipe_fd[1]);

        // Redirect stdin to the read end of the pipe
        dup2(pipe_fd[0], STDIN_FILENO);

        // Close the read end of the pipe
        close(pipe_fd[0]);

        // Execute the grep command
        execlp("grep", "grep", "^root", (char *)NULL);

        // execlp will only return if an error occurs
        perror("execlp");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}