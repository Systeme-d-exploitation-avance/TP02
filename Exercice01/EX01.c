#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_usage(char *bin_name) {
    dprintf(1, "Usage: %s\n", bin_name);
}

int main(int argc, char **argv) {
    pid_t child_pid, wpid;
    int status;

    int opt;

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    child_pid = fork();

    if (child_pid < 0) {
        perror("Error with fork()");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        printf("> Processus fils... <\n");
        printf("PID FILS = %d\nPID PERE = %d\n", getpid(), getppid());
        int last_pid_digit = getpid() % 10;
        exit(last_pid_digit);
    } else {
        printf("> Processus pere... <\n");
        printf("PID FILS = %d\n", child_pid);

        wpid = wait(&status);
        if (WIFEXITED(status)) {
            printf("Code retour du fils (PID:%d) = %d\n", wpid, WEXITSTATUS(status));
        } else {
            perror("Le fils n'a pas terminé normalement");
        }
    }

    return 0;
}
