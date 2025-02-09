#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

 // Helper function to trim leading and trailing whitespace
static char* trim(char* str) {
    if (!str) return NULL;
    
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

// Helper function to parse a single command and its arguments
static int parse_command(char* cmd_str, command_t* cmd) {
    if (!cmd_str || !cmd) return ERR_CMD_OR_ARGS_TOO_BIG;
    
    memset(cmd->exe, 0, EXE_MAX);
    memset(cmd->args, 0, ARG_MAX);
    
    cmd_str = trim(cmd_str);
    if (strlen(cmd_str) == 0) return OK;
    
    char* saveptr;
    char* token = strtok_r(cmd_str, " \t", &saveptr);
    if (!token) return OK;
    
    if (strlen(token) >= EXE_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
    strcpy(cmd->exe, token);
    
    char* remaining = strtok_r(NULL, "", &saveptr);
    if (remaining) {
        remaining = trim(remaining);
        if (strlen(remaining) >= ARG_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
        strcpy(cmd->args, remaining);
    }
    
    return OK;
}

int build_cmd_list(char* cmd_line, command_list_t* clist) {
    if (!cmd_line || !clist) return ERR_CMD_OR_ARGS_TOO_BIG;
    
    memset(clist, 0, sizeof(command_list_t));
    clist->num = 0;
    
    cmd_line = trim(cmd_line);
    if (strlen(cmd_line) == 0) return WARN_NO_CMDS;
    
    char* cmd_copy = strdup(cmd_line);
    if (!cmd_copy) return ERR_CMD_OR_ARGS_TOO_BIG;
    
    char* saveptr;
    char* cmd_str = strtok_r(cmd_copy, "|", &saveptr);
    
    while (cmd_str) {
        if (clist->num >= CMD_MAX) {
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;
        }
        
        int rc = parse_command(cmd_str, &clist->commands[clist->num]);
        if (rc != OK) {
            free(cmd_copy);
            return rc;
        }
        
        if (strlen(clist->commands[clist->num].exe) > 0) {
            clist->num++;
        }
        
        cmd_str = strtok_r(NULL, "|", &saveptr);
    }
    
    free(cmd_copy);
    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}