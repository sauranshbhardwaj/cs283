1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  
    
        We use fork() before execvp() for several reasons:
        
        execvp() replaces the current process entirely with the new program. If we called it directly in our shell, the shell would be replaced and terminate.

        fork() creates a copy of the current process, allowing the shell (parent) to continue running while the child process executes the command.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  
    
        When fork() fails, it returns -1 to the calling process. Our implementation handles this by:

        Checking the return value of fork() for negative values
        
        Printing an error message using perror() to show the system error
        
        Returning ERR_EXEC_CMD to the caller
        
        Continuing the shell loop rather than terminating

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  

        execvp() finds commands to execute through the following process:

        It uses the PATH environment variable, which contains a colon-separated list of directories

        For each directory in PATH, it:

            Concatenates the command name to the directory path
            
            Checks if the resulting path exists and is executable
            
            Uses the first matching executable found


        If the command includes a slash (/), it tries the exact path instead

        The 'p' in execvp stands for 'PATH', indicating it uses PATH searching
        
        If no executable is found in any PATH directory, it returns ENOENT error

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  

        Calling wait() in the parent process serves several essential purposes:

            Prevents zombie processes by properly cleaning up terminated child processes
            
            Allows the parent to receive the child's exit status
            
            Maintains proper process synchronization
            
            Enables sequential command execution

        If we didn't call wait():

            Child processes would become zombies, consuming system resources
            
            The shell wouldn't know if commands completed successfully
            
            We couldn't implement features requiring exit status
            
            System resources would eventually be exhausted by zombie processes

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  

        WEXITSTATUS() provides the following crucial information:

            Extracts the exit code (0-255) from the status value returned by wait()
            
            Tells us how the command terminated:

                0 typically indicates success
                
                Non-zero values indicate various types of failures


            Important because:

                Enables error handling and status checking

                Allows implementation of conditional execution
                
                Required for shell scripting features
                
                Helps maintain POSIX compatibility

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  
    
        Our build_cmd_buff() handles quoted arguments by:

            Using a state machine approach to track whether we're inside quotes
            
            Preserving spaces within quoted strings
            
            Treating quoted text as a single argument
            
            Properly handling nested quotes

        This is necessary because:

            Many commands need arguments with spaces (e.g., echo "hello world")
            
            File paths often contain spaces
            
            Preserves user intent in command arguments
            
            Maintains compatibility

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  

        The parsing logic underwent several significant changes:

            Changed from command_list_t to cmd_buff_t structure
            
            Removed pipe handling since we're focusing on single commands
            
            Added quoted string support with proper space preservation
            
            Implemented argv array building for execvp compatibility

        Unexpected challenges included:

            Managing memory differently for the new structure
            
            Ensuring proper NULL termination of argv array
            
            Handling edge cases with quotes and spaces
            
            Maintaining command argument count accuracy
            
            Converting from string-based args to array-based argv

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  

        Signals serve as a fundamental IPC mechanism in Linux with distinct characteristics:

            Purpose:

                Asynchronous notification between processes
                
                System-wide event handling
                
                Process control and state management
                
                Error and exception handling

        Differences from other IPC methods:

            Signals are interrupt-based rather than data-transfer focused
            
            They're lightweight compared to pipes or shared memory
            
            Limited to predefined signal types (can't send arbitrary data)
            
            Can be sent by both the kernel and other processes
            
            More suitable for control flow than data exchange

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  

        Three common signals and their use cases:

            SIGINT (Signal 2):

                Generated by Ctrl+C
                Requests graceful program termination
                Can be caught and handled
                Used for interactive program interruption

            SIGTERM (Signal 15):

                Default termination signal
                Allows cleanup before exit
                Can be caught and handled
                Standard way to request program shutdown

            SIGKILL (Signal 9):

                Immediate process termination
                Cannot be caught or ignored
                Used as last resort
                Bypasses normal cleanup procedures

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  

    When a process receives SIGSTOP:

        Process execution is immediately suspended
        
        Cannot be caught or ignored because:

            It's a fundamental process control signal
            
            Needed for debugger and job control
            
            System needs guaranteed way to stop processes

        Can only be resumed by SIGCONT
            
        Different from SIGTSTP (Ctrl+Z) which can be caught
            
        Essential for system-level process management
            
        Part of job control mechanism in shells

        Unlike SIGINT:

            SIGSTOP cannot be overridden
            
            No handler can be installed
            
            Process has no choice but to suspend
            
            This design ensures system administrators always have control