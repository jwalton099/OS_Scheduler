#include <iostream>
#include <unistd.h>  // For pipe, fork, write, close
#include <cstdlib>   // For exit
#include <sys/wait.h> // For wait
#include <cstring>

using namespace std;

int main() {
    // Create a pipe for communication with the process manager
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        cerr << "Error creating pipe" << endl;
        exit(EXIT_FAILURE);
    }

    // Create a child process (process manager)
    pid_t processManagerPid = fork();

    if (processManagerPid < 0) {
        cerr << "Error forking process manager" << endl;
        exit(EXIT_FAILURE);
    }

    if (processManagerPid == 0) {  //Child Process
        close(pipefd[1]);

        // Redirect the read end of the pipe to stdin
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp("./prcs_mngr", "./prcs_mngr",  nullptr);

        cerr << "Error executing process manager" << endl;
        exit(EXIT_FAILURE);
    }
    else {  // Parent Process
        // Read commands from stdin (cin) and send them to the process manager through the pipe
        string line;
        while (getline(cin, line)) {
            // Send each line (command) to the process manager through the pipe
            ssize_t bytesWritten = write(pipefd[1], line.c_str(), line.size());
            if (bytesWritten == -1) {
                cerr << "Error writing to pipe" << endl;
                exit(EXIT_FAILURE);
            }
            write(pipefd[1], "\n", 1);

            sleep(2);
        }

        // Close the write end of the pipe to signal the end of input to the process manager
        close(pipefd[1]);

        int status;
        waitpid(processManagerPid, &status, 0);
    }
    return 0;
}
