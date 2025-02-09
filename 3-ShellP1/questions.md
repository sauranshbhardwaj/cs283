1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is an excellent choice for this shell application because It reads input line by line, which matches how users naturally enter shell commands (one command per line) and it has built-in buffer overflow protection since it takes a maximum size parameter.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() for cmd_buff instead of a fixed-size array is beneficial because:
    1. Dynamic memory allocation allows us to handle varying command sizes more efficiently
    2. Stack memory (used by fixed arrays) is typically more limited than heap memory (used by malloc)
    3. It prevents stack overflow issues that could occur with large fixed arrays


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  
    
    Trimming spaces is very important because:
    1. Command execution: Many operating systems and programs won't recognize commands with leading/trailing spaces
    2. Consistency: Commands like "ls" and "  ls  " should be treated the same way
    3. Parsing accuracy: Spaces can interfere with proper argument parsing

    Without trimming, we could face issues like:
    1. Failed command execution because the system can't find the command with spaces
    2. Incorrect argument parsing where spaces become part of argument values
    3. Inconsistent behavior where identical commands with different spacing behave differently

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  

    Example 1:
    Output redirection (>): command > file.txt
    1. Challenge: Need to handle file creation/overwriting
    2. Must manage file permissions and errors
    3. Need to restore original stdout after redirection

    Example 2:
    Input redirection (<): command < input.txt
    1. Challenge: Must verify file exists and is readable
    2. Need to handle file reading errors gracefully
    3. Must properly restore stdin after completion

    Example 3: 
    Error redirection (2>): command 2> error.log
    1. Challenge: Requires separate handling of stderr
    2. Must maintain proper ordering of output streams
    3. Need to handle both stderr and stdout simultaneously

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Key differences between redirection and piping:

    Redirection (>, <, 2>) connects a command with files:
    1. Sends output to or gets input from files
    2. Works with one command at a time
    3. Permanent storage of input/output
    
    Piping (|) connects commands with each other:
    1. Transfers data between running processes
    2. Links multiple commands together
    3. Data exists only during command execution

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  

    Keeping STDERR separate from STDOUT is important because:
    1. Error messages need to be distinguished from normal output
    2. Allows errors to be visible even when output is redirected
    3. Enables different handling of errors vs. normal output
    4. Helps scripts distinguish between successful output and errors

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  

    1. Error Output Management:

    (a) By default, error messages should go to STDERR while normal output goes to STDOUT

    (b) Support standard error redirection using 2> for separating error messages
    
    (c) Implement 2>&1 syntax to merge STDERR into STDOUT when requested
    
    2. Error Handling in Pipes:
    
    (a) When commands are piped, errors from each command should be handled separately

    (b) Error messages should not be passed through pipes unless explicitly requested
    
    (c) Each command in a pipe chain should maintain its own error stream
    
    3. Implementation Strategy:
    
    (a) Use file descriptor duplication (dup2) to handle redirections
    
    (b) Implement proper cleanup of file descriptors after command execution
    
    (c) Maintain proper ordering of messages when streams are merged
    
    4. User Interface:

    (a) Provide clear and formatted error messages that include: Command name that failed, Specific error description, and Any relevant system error messages
    
    (b) Support verbose mode for detailed error information
    
    (c) Allow customization of error output format
    
    5. Stream Merging:
    
    (a) Yes, we should provide a way to merge STDOUT and STDERR using: 
        
        2>&1 notation for merging STDERR into STDOUT
        
        &> notation as a shorthand for redirecting both streams
        
    (b) Implement proper buffering to maintain message order
    
    (c) Ensure atomic writes when streams are merged
    
    6. Error Recovery:
    
    (a) Implement proper cleanup after command failures
    
    (b) Restore original file descriptors after redirection
    
    (c) Handle pipe chain failures gracefully