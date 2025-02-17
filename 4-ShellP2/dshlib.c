#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */


static int last_return_code = 0;

// Helper function to trim whitespace
static char* trim(char* str) {
    if (!str) return NULL;
    
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return ERR_MEMORY;
    
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;
    
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return ERR_MEMORY;
    
    free(cmd_buff->_cmd_buffer);
    cmd_buff->_cmd_buffer = NULL;
    cmd_buff->argc = 0;
    
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff || !cmd_buff->_cmd_buffer) return ERR_MEMORY;
    
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) return ERR_MEMORY;
    
    clear_cmd_buff(cmd_buff);
    strcpy(cmd_buff->_cmd_buffer, cmd_line);
    
    char *str = cmd_buff->_cmd_buffer;
    bool in_quotes = false;
    char *start = str;
    int i;
    
    for (i = 0; str[i]; i++) {
        if (str[i] == '"') {
            if (!in_quotes) start = str + i + 1;
            in_quotes = !in_quotes;
            continue;
        }
        
        if (!in_quotes && isspace(str[i])) {
            str[i] = '\0';
            if (start < str + i) {
                cmd_buff->argv[cmd_buff->argc++] = start;
                if (cmd_buff->argc >= CMD_ARGV_MAX) break;
            }
            start = str + i + 1;
        }
    }
    
    // Handle last argument
    if (start < str + i && *start) {
        cmd_buff->argv[cmd_buff->argc++] = start;
    }
    
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (!input) return BI_NOT_BI;
    
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "dragon") == 0) return BI_CMD_DRAGON;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    if (strcmp(input, "rc") == 0) return BI_CMD_RC;
    
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (!cmd || !cmd->argv[0]) return BI_NOT_BI;
    
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    
    switch(cmd_type) {
        case BI_CMD_EXIT:
            exit(EXIT_SUCCESS);
            
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd failed");
                }
            }
            return BI_EXECUTED;
            
        case BI_CMD_RC:
            printf("%d\n", last_return_code);
            return BI_EXECUTED;
            
        case BI_CMD_DRAGON:
            return BI_EXECUTED;
            
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    if (!cmd || !cmd->argv[0]) return ERR_EXEC_CMD;
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        return ERR_EXEC_CMD;
    }
    
    if (pid == 0) {  // Child process
        execvp(cmd->argv[0], cmd->argv);
        int err = errno;
        if (err == ENOENT) {
            fprintf(stderr, "Command not found in PATH\n");
        } else if (err == EACCES) {
            fprintf(stderr, "Permission denied\n");
        } else {
            perror("Command execution failed");
        }
        exit(err);
    } else {  // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            last_return_code = WEXITSTATUS(status);
            return WEXITSTATUS(status);
        }
        return ERR_EXEC_CMD;
    }
}

int exec_local_cmd_loop() {
    cmd_buff_t cmd;
    int rc = alloc_cmd_buff(&cmd);
    if (rc != OK) return rc;
    
    char input_buffer[SH_CMD_MAX];
    
    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(input_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
        
        if (strlen(trim(input_buffer)) == 0) {
            continue;
        }
        
        rc = build_cmd_buff(input_buffer, &cmd);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }
        
        if (exec_built_in_cmd(&cmd) == BI_NOT_BI) {
            rc = exec_cmd(&cmd);
        }
    }
    
    free_cmd_buff(&cmd);
    return OK;
}