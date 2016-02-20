# ashell
The ashell can execute applications, can setup pipes between executed applications, redirect standard input/output from/to files for the executed applications, and perform simple commands internally. 
The shell can handle commands cd, ls, pwd, history, and exit internally. Below are descriptions of the commands:

cd [directory] – Changes the current working directory to specified directory. If the optional directory is not specified, then the current working directory is changed to the directory specified by the HOME environmental variable.

ls [directory] – Lists the files/directories in the directory specified, if no directory is specified, it lists the contents of the current working directory. The order of files listed does not matter, but the type and permissions must precede the file/directory name with one entry per line. Below is an example listing
drwxr-xr-x .
drwxr-xr-x ..
-rwxr-xr-x ashell
-rwxr-xr-x main.cpp

pwd – Prints the current working directory name.

history – Prints the history of the last 10 commands entered into the shell. Below is an example where the commands entered were ls, cd .., pwd, and then history:
0 ls
1 cd ..
2 pwd
3 history


exit – Exits the shell.

The shell can handle up and down arrows to select through the history of
commands. Only the most recent 10 commands need to be stored in the history. The up arrow can toggle to the previous command, down arrow to the next command. If the user attempts to navigate beyond the end of the list the audible bell character '\a' (ASCII 0x07) will be output. In addition, if a user enters a backspace or delete and no characters exist on the command to delete, the audible bell will be output.
