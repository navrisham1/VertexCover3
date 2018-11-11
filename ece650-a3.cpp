#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

class A3Driver
{
private:
    const int NOPIPE=0;
    std::string myname;
    int s_val;
    int n_val;
    int l_val;
    int c_val;
    int is_debug;

    int rgen_to_a1[2];
    int a1_to_a2[2];
    std::vector<pid_t> proc_ids;

    void debug(std::string msg);
    void closePipes();
    int runProc(const char *command, const char *param, int stdin_pipe, int stdout_pipe, char **argv);

public:
    A3Driver(bool is_debug);
    void parseOptions(int argc, char **argv);
    void killProcs();
    int runRgen(int argc, char **argv);
    int readStdin();
    int runA1();
    int runA2();
};

A3Driver::A3Driver(bool is_debug=false)
{
    this->myname = "Parent";
    this->is_debug = is_debug;
    pipe(this->rgen_to_a1);
    pipe(this->a1_to_a2);
    std::cout.sync_with_stdio(false);

    if (this->is_debug) {
        std::cerr << "rgen_to_a1 = " << rgen_to_a1[0] << ", " << rgen_to_a1[1] << std::endl;
        std::cerr << "a1_to_a2 = " << a1_to_a2[0] << ", " << a1_to_a2[1] << std::endl;
    }
}

void A3Driver::debug(std::string msg)
{
    if(this->is_debug) {
        std::cerr << "Error: " << this->myname << ": " << msg << std::endl;
    }
}
void A3Driver::parseOptions(int argc, char **argv)
{
    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "v")) != -1)
    {
        if(c == 'v') {
            this->is_debug = true;
            break;
        }
    }
}

void A3Driver::closePipes()
{
    std::string exception;

    if( close(this->rgen_to_a1[0]) !=0 ||
        close(this->rgen_to_a1[1]) != 0 ||
        close(this->a1_to_a2[0]) != 0 ||
        close(this->a1_to_a2[1]) )
    {
        exception = "Could not close pipes for " + this->myname;
        throw exception;
    }

}

int A3Driver::readStdin()
{
    std::string exception;
    this->myname = "Stdin";

    this->debug("Ready to start " + this->myname);
    if (dup2(this->a1_to_a2[1], STDOUT_FILENO) != 1)
    {
        exception = "Could not setup STDOUT pipe for " + this->myname;
        throw exception;
    }
    this->closePipes();

    while (!std::cin.eof())
    {
        std::string line;
        std::getline(std::cin, line);
        std::cout << line << std::endl << std::flush;
    }
    return 0;
}

void A3Driver::killProcs()
{
    for (pid_t k : this->proc_ids) {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }
}

int A3Driver::runProc(const char *command, const char *param, int stdin_pipe, int stdout_pipe, char **argv=nullptr)
{
    std::string exception;
    pid_t child_pid;

    this->debug("Ready to fork " + this->myname);
    child_pid = fork ();
    if (child_pid < 0) { //exception
        exception = "Could not fork " + this->myname;
        throw exception;
    }

    else if (child_pid == 0) { // child proc
        if (stdin_pipe > this->NOPIPE && dup2(stdin_pipe, STDIN_FILENO) != 0) {
            exception = "Could not setup STDIN pipe for " + this->myname;
            throw exception;
        }
        std::cout.sync_with_stdio(false);
        if (stdout_pipe > this->NOPIPE && dup2(stdout_pipe, STDOUT_FILENO) != 1) {
            exception = "Could not setup STDOUT pipe for " + this->myname;
            throw exception;
        }
        this->closePipes();
        if (argv == nullptr) {
            execl (command, command, param, nullptr);
        } else {
            execv (command, argv);
        }

    }else{ // parent process savs child's pid
        this->proc_ids.push_back(child_pid);
    }

    return child_pid;
}

int A3Driver::runRgen(int argc, char **argv)
{
    argv[0] = (char *) "./rgen";
    this->myname = "Rgen";
    return this->runProc("./rgen", "", this->NOPIPE, this->rgen_to_a1[1], argv);
}

int A3Driver::runA1()
{
    this->myname = "A1";
    return this->runProc("/usr/bin/python", "ece650-a1.py", this->rgen_to_a1[0], this->a1_to_a2[1]);
}

int A3Driver::runA2()
{
    this->myname = "A2";
    return this->runProc("./ece650-a2", "", this->a1_to_a2[0], this->NOPIPE);
}


/*****************************
 *             MAIN
 *****************************/
int main (int argc, char **argv)
{
    bool debug = false;
    A3Driver a3_drv(debug);

    try {
        //a3_drv.parseOptions(argc, argv);

        if (a3_drv.runA2() == 0)    exit(0); //child proc
        if (a3_drv.runA1() == 0)    exit(0); //child proc
        if (a3_drv.runRgen(argc, argv) == 0)  exit(0); //child proc
        a3_drv.readStdin();

    }//try
    catch(std::string error) {
        std::cerr << "Error: " << error << std::endl;
    }//catch
    catch(const char* error) {
        std::cerr << "Error: " << error << std::endl;
    }//catch

    a3_drv.killProcs();
}

/*
1)  run the driver program
2)  parse command line inputs (-s.-l,-n,-e) and save them
    Try to figure
3)  generate random street data using program rgen
4)  pipe random street data into a1 program
5)  pipe output to a2 input
6)  pipe user inputs from std::in to a2

*/
