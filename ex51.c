#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#define PRINT_ERROR_AND_EXIT write(2, "Error in system call\n", strlen("Error in system call\n")); exit(1);
#define SIZE 150

/*
 * return the key that the user type
 */
char getCh() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

int game_key(char key) {
    if (key == 'a' || key == 's' || key == 'd' || key == 'w') {
        return 1;
    }
    return 0;
}

int main() {
    int fd[2];

    // creating a pipe
    if (pipe(fd) < 0) {
        PRINT_ERROR_AND_EXIT;
    }


    pid_t pid = fork();
    if (pid == -1) {
        PRINT_ERROR_AND_EXIT;

        // child process
    } else if (pid == 0) {
        // close the write fd
        close(fd[1]);
        char cwd[SIZE] = {0};

        // get the current path
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // create the path of the draw.out
            strcat(cwd, "/draw.out");
        } else {
            PRINT_ERROR_AND_EXIT;
        }

        char *arguments[2] = {"draw.out", NULL};

        // IO redirection
        if(dup2(fd[0], STDIN_FILENO) == -1) {
            PRINT_ERROR_AND_EXIT;
        }
        execvp(cwd, arguments);
        PRINT_ERROR_AND_EXIT;

        // father process
    } else {
        // close the read fd
        close(fd[0]);
        // read key from the user
        char user_input = getCh();

        // while the user not press q to quit the game
        while (user_input != 'q') {
            // if the user press not a key's game, do nothing
            if (!game_key(user_input)) {
                // read key from the user
                user_input = getCh();
                continue;
            }
            // writing to the pipe the user input key
            if(write(fd[1], &user_input, 1) < 0) {
                PRINT_ERROR_AND_EXIT;
            }
            // sends SIGUSR2 signal to child process
            kill(pid, SIGUSR2);
            // read key from the user
            user_input = getCh();
        }

        /* if the user press q - quit*/

        // writing to the pipe the user input key
        if(write(fd[1], &user_input, 2) < 0) {
            PRINT_ERROR_AND_EXIT;
        }
        // sends SIGUSR2 signal to child process
        kill(pid, SIGUSR2);
        // wait for child to finish
        waitpid(pid, NULL, 0);
    }

    return 0;
}