# Nish
Minimalistic command line interpreter, written in C, that mimics some of the functionalities of bash for UNIX-like operating systems

## Table of contents
- [Running the project](#running-the-project)
- [Features overview](#features-overview)
- [License](#license)
- [Contacts](#contacts)

## Running the project
By default the project is built using `clang`. If you want to use any other compiler, you need to change the [Makefile](./Makefile)

Building and running the project:
```shell
make
```

The `run` make script uses the `Address sanitizer` to check for memory leaks while in development. Make sure you have it set up or remove the `-fsanitize=address` flag from the script.

Cleaning the directory:
```shell
make clean
```

## Features overview
Nish is a command line iterpreter - it lets you execute any program that you can execute in a normal terminal like `bash`. This includes any commands that make system call to the kernel of the OS. Apart from executing a single command, Nish also supports:
- input/output redirection - use the '>' symbol at the end of the command and after it provide the name of the file to redirect the result of the command to that file, or use the '<' symbol to redirect the input of the command from the contents of the file;
- running in the background - use the '&' symbol at the end of the command to run the command in the background. The process id (pid) of the spawned process will be displayed, but you can still use the program without waiting for it to finish;
- piping multiple commands - to pipe the output from one command to the input of other command, you can separate the two commands by the '|' symbol. There is no limit to the number of commands that you can pipe this way.

You can combine the above features together, but if you want to run the command in the background, the '&' symbol must be the last symbol of the command. The `exit` command is used to end the running session and it overrides the `exit` system call in order to gracefully terminate the program. Keep in mind that when you run a command in the background, the result of that command will be displayed as soon as it completes (unless you redirect the output of course).

## License
The project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more information.

## Contacts
- GitHub: https://github.com/nigosto
- LinkedIn: https://www.linkedin.com/in/georgi-n-atanasov
- email: georgi.n.atanasov02@gmail.com