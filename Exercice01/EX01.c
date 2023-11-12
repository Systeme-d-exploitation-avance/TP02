#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>

// Usage information
#define USAGE "USAGE: Programme générant un processus fils avec la primitive système fork"
#define USAGE_PARAMS "PARAMETERS : No parameters needed\n"

/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char *binary_name)
{
    dprintf(1, "%s\n%s\n%s\n", binary_name, USAGE, USAGE_PARAMS);
    dprintf(1, "Options:\n");
    dprintf(1, "  -h, --help     Display this help and exit\n");
    dprintf(1, "  -v, --verbose  Print more detailed information\n");
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
    pid_t child_pid;
    int status;

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

    // Printing params
    dprintf(1, "** PARAMS **\n%-8s: %d\n\n",
            "verbose", is_verbose_mode);

    // Business logic must be implemented at this point
    // Create a child process
    child_pid = fork();

    if (child_pid == -1)
    {
        // Error handling if fork fails
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0)
    {
        // Code for the child process
        printf("Child Process Info:\n");
        printf("  PID: %d\n", getpid());
        printf("  Parent PID: %d\n", getppid());

        if (is_verbose_mode)
        {
            printf("  [Verbose] Child process created successfully.\n");
        }

        // Exit with return code equal to the last digit of PID
        exit(getpid() % 10);
    }
    else
    {
        // Code for the parent process
        printf("Parent Process Info:\n");
        printf("  PID: %d\n", getpid());
        printf("  Child PID: %d\n", child_pid);

        if (is_verbose_mode)
        {
            printf("  [Verbose] Parent process created a child with PID: %d.\n", child_pid);
        }

        // Wait for the child to terminate and get its return code
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status))
        {
            printf("Child process exited with return code: %d\n", WEXITSTATUS(status));
            if (is_verbose_mode)
            {
                printf("  [Verbose] Child process terminated successfully.\n");
            }
        }
        else
        {
            printf("Child process did not exit normally.\n");
        }
    }

    return EXIT_SUCCESS;
}
