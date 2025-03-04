1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    My implementation ensures all child processes complete before the shell continues by tracking child process IDs in an array (`child_pids`) and systematically calling `waitpid()` for each process after setting up the pipe. In the `execute_pipeline()` function, after creating all the child processes and setting up the pipes, the parent process iterates through the array of child PIDs and waits for each one to complete using `waitpid(child_pids[i], &status, 0)`.

    If I forgot to call `waitpid()` on all child processes, several problems would occur:
        
        1. Zombie processes would accumulate in the system - these are terminated processes that still have entries in the process table, consuming system resources.
        
        2. The shell would continue accepting new input before the current command completes, causing potential race conditions and unexpected behavior.
        
        3. The standard output might get corrupted or interleaved if multiple processes try writing to it simultaneously.
        
        4. Child processes might be orphaned if the shell terminates before they complete, potentially causing resource leaks.


2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    After calling `dup2()`, it's necessary to close unused pipe ends because:

        1. Resource Leaks: Each open file descriptor consumes kernel resources. If pipes aren't closed properly, the system might eventually run out of available file descriptors.

        2. Blocking on Read: A pipe remains open as long as at least one write end is open. If a process is reading from a pipe and there are unused open write ends that aren't being written to, the reading process will hang indefinitely waiting for EOF that never comes.

        3. Data Flow Issues: In a pipeline like `A | B | C`, if process B doesn't close its inherited read end from the A-B pipe, process C might receive incomplete data from B because B hasn't fully processed input from A.

        4. Performance Impact: Keeping unnecessary file descriptors open can degrade system performance due to additional context switching and memory usage.

    If pipes are left open, a common symptom is that commands might hang or not terminate properly, or the shell might eventually stop functioning due to file descriptor exhaustion.


3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    The `cd` command is implemented as a built-in command rather than an external command for fundamental reasons related to process behavior:

        1. Process Environment Isolation: Each process has its own working directory. If `cd` were implemented as an external command, it would change the working directory only for itself, not for the parent shell process. After the external `cd` process terminated, the shell would still be in the original directory.

        2. Shell State Modification: The purpose of `cd` is to change the shell's working directory for all subsequent commands. Only built-in commands can directly modify the shell's state.

    Challenges if `cd` were implemented as an external process:

        1. No Persistent Directory Change: The directory change would only affect the `cd` process, not the shell itself. After `cd` exits, the shell would remain in the original directory.

        2. Context Loss: Any environment or working directory changes made by the external process would be lost when it exits.
        
        3. Inefficiency: Spawning a new process just to change a directory is less efficient than making the change directly within the shell.
        
        4. User Confusion: Users expect `cd` to change the shell's working directory for subsequent commands, which wouldn't happen if it were external.


4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    To modify my implementation to support an arbitrary number of piped commands while maintaining efficient memory management, I would:

        1. Use Dynamic Data Structures: Replace the fixed-size arrays with dynamically allocated structures like linked lists or resizable arrays (similar to C++ vectors) for both the commands and pipes.

            
            typedef struct command_node {
                cmd_buff_t cmd;
                struct command_node* next;
            } command_node_t;

            typedef struct command_list {
                int num;
                command_node_t* head;
                command_node_t* tail;
            } command_list_t;
            

        2. On-demand Pipe Creation: Instead of creating all pipes at once, create pipes as needed during command processing.

        3. Memory Pool: Implement a memory pool for frequently allocated/deallocated structures to reduce fragmentation.

        4. Circular Buffers: For very long pipelines, implement a sliding window of active pipes to limit the maximum number of simultaneously open file descriptors.

    Trade-offs to consider:

        1. Complexity vs. Flexibility: Dynamic allocation adds code complexity but provides unlimited pipeline length.

        2. Performance vs. Resource Usage: Dynamic allocation has higher runtime overhead but prevents wasting memory on unused command slots.

        3. Memory Fragmentation: Frequent allocation/deallocation can lead to memory fragmentation. Using memory pools can mitigate this but adds complexity.

        4. File Descriptor Limits: Even with dynamic memory, the system has a limit on open file descriptors. Very long pipelines might still fail due to system limits.

        5. Error Handling: Dynamic allocation introduces more failure points requiring robust error handling.

        6. Security Considerations: Unbounded pipeline length could potentially be exploited in denial-of-service attacks. Implementing a reasonable upper limit would still be prudent.

    The best approach would be to use a linked list for command storage with on-demand pipe creation and careful resource cleanup, but still implement a configurable (rather than hard-coded) maximum length to prevent resource exhaustion attacks.