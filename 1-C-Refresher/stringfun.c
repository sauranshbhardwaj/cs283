#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Student Name: Sauransh Bhardwaj
// Student ID: sb4564

#define BUFFER_SZ 50

// Prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
int reverse_string(char *, int, int);
int print_words(char *, int, int);
int replace_words(char *, int, int, char *, char *);

/**
 * Sets up the internal buffer with the user string, handling whitespace and padding
 * @param buff The internal buffer to populate
 * @param user_str The user provided input string
 * @param len The size of the internal buffer
 * @return Length of processed string or error code (-1 if too large, -2 other errors)
 */
int setup_buff(char *buff, char *user_str, int len) {
    //TODO: #4:  Implement the setup buff as per the directions
    int str_len = 0;
    int in_space = 0;
    char *ptr = user_str;
    char *buff_ptr = buff;
    
    while (*ptr == ' ' || *ptr == '\t') ptr++;
    
    while (*ptr != '\0') {
        if (str_len >= len) {
            return -1; 
        }
        
        if (*ptr == ' ' || *ptr == '\t') {
            if (!in_space && str_len > 0) { 
                *buff_ptr++ = ' ';
                str_len++;
                in_space = 1;
            }
        } else {
            *buff_ptr++ = *ptr;
            str_len++;
            in_space = 0;
        }
        ptr++;
    }
    
    if (str_len > 0 && *(buff_ptr-1) == ' ') {
        buff_ptr--;
        str_len--;
    }
    
    while (str_len < len) {
        *buff_ptr++ = '.';
        str_len++;
    }
    
    return str_len;
}

void print_buff(char *buff, int len) {
    printf("Buffer:  [");
    for (int i=0; i<len; i++) {
        putchar(*(buff+i));
    }
    printf("]\n");
}

void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

/**
 * Counts the number of words in the buffer
 * @return Number of words or error code
 */
int count_words(char *buff, int len, int str_len) {
    if (str_len > len || str_len <= 0) return -1;
    
    int count = 0;
    int in_word = 0;
    char *ptr = buff;
    
    while (ptr < buff + str_len) {
        if (*ptr != ' ' && !in_word) {
            count++;
            in_word = 1;
        } else if (*ptr == ' ') {
            in_word = 0;
        }
        ptr++;
    }
    
    return count;
}

/**
 * Reverses the characters in the string portion of the buffer
 * @return 0 on success, negative on error
 */
int reverse_string(char *buff, int len, int str_len) {
    if (str_len > len || str_len <= 0) return -1;
    
    char *start = buff;
    char *end = buff + str_len - 1;
    
    while (*end == '.') end--;
    
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    
    return 0;
}

/**
 * Prints each word and its length
 * @return Number of words processed or error code
 */
int print_words(char *buff, int len, int str_len) {
    if (str_len > len || str_len <= 0) return -1;
    
    printf("Word Print\n----------\n");
    
    int word_count = 0;
    int char_count = 0;
    int in_word = 0;
    char *word_start = buff;
    char *ptr = buff;
    
    while (ptr < buff + str_len && *ptr != '.') {
        if (*ptr != ' ' && !in_word) {
            word_start = ptr;
            in_word = 1;
            char_count = 1;
        } else if (*ptr != ' ' && in_word) {
            char_count++;
        } else if (*ptr == ' ' && in_word) {
            word_count++;
            printf("%d. ", word_count);
            char *temp = word_start;
            while (temp < ptr) putchar(*temp++);
            printf("(%d)\n", char_count);
            in_word = 0;
        }
        ptr++;
    }
    
    if (in_word) {
        word_count++;
        printf("%d. ", word_count);
        char *temp = word_start;
        while (temp < ptr && *temp != '.') putchar(*temp++);
        printf("(%d)\n", char_count);
    }
    
    printf("\nNumber of words returned: %d\n", word_count);
    return word_count;
}

/**
 * Replaces first occurrence of search string with replace string
 * @return 0 on success, negative on error
 */
int replace_words(char *buff, int len, int str_len, char *search, char *replace) {
    if (str_len > len || str_len <= 0) return -1;
    
    char *ptr = buff;
    char *end = buff + str_len;
    char *search_start = search;
    int search_len = 0;
    int replace_len = 0;
    
    while (*search) {
        search_len++;
        search++;
    }
    
    while (*(replace + replace_len)) {
        replace_len++;
    }
    
    while (ptr < end - search_len) {
        int matched = 1;
        char *s = search_start;
        char *p = ptr;
        
        while (*s != '\0') {
            if (*p != *s) {
                matched = 0;
                break;
            }
            p++;
            s++;
        }
        
        if (matched) {
            int new_str_len = str_len - search_len + replace_len;
            
            if (new_str_len > len) {
                new_str_len = len;
            }
            
            if (replace_len != search_len) {
                char *src = ptr + search_len;
                char *dst = ptr + replace_len;
                int to_move = end - (ptr + search_len);
                
                if (replace_len < search_len) {
                    while (to_move > 0) {
                        *dst = *src;
                        dst++;
                        src++;
                        to_move--;
                    }
                } else {
                    src += to_move;
                    dst += to_move;
                    while (to_move >= 0) {
                        if (dst < buff + len) {
                            *dst = *src;
                        }
                        dst--;
                        src--;
                        to_move--;
                    }
                }
            }
            
            char *r = replace;
            char *p = ptr;
            while (*r && p < buff + len) {
                *p++ = *r++;
            }
            
            ptr = buff + new_str_len;
            while (ptr < buff + len) {
                *ptr++ = '.';
            }
            
            return 0;
        }
        ptr++;
    }
    
    return -1; 
}

int main(int argc, char *argv[]) {
    char *buff;             // Placeholder for the internal buffer
    char *input_string;     // Holds the string provided by the user on cmd line
    char opt;               // Used to capture user option from cmd line
    int  rc;                // Used for return codes
    int  user_str_len;      // Length of user supplied string

    // TODO #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /* This is safe because the if condition checks both:
     * 1. If argc < 2 (ensuring argv[1] exists)
     * 2. If first char of argv[1] is '-'
     * Only if both conditions are false do we proceed to access argv[1]
     */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   // Get the option flag

    // Handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    // TODO #2 Document the purpose of the if statement below 
    /* This if statement ensures that for all operations except
     * help (-h), we have at least one additional argument (the input string).
     * Without this check, we could segfault when trying to access argv[2]
     * for operations that require the input string.
     */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; // Capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL) {
        printf("Error: Failed to allocate buffer\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
            
        case 'r':
            rc = reverse_string(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error reversing string, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
            
        case 'w':
            rc = print_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
            
        case 'x':
            if (argc < 5) {
                printf("Error: -x option requires search and replace strings\n");
                free(buff);
                exit(1);
            }
            rc = replace_words(buff, BUFFER_SZ, user_str_len, argv[3], argv[4]);
            if (rc < 0) {
                printf("Error: Search string not found or buffer overflow\n");
                free(buff);
                exit(2);
            }
            break;
            
        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    print_buff(buff, BUFFER_SZ);
    
    //TODO:  #6 Dont forget to free your buffer before exiting
    free(buff);
    
//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//   
    /* Providing both pointer and length is good practice because:
     * 1. It enables bounds checking to prevent buffer overflows
     * 2. It makes the functions more reusable with different buffer sizes
     * 3. It makes the code more maintainable if BUFFER_SZ changes
     * 4. It follows the principle of least privilege by explicitly
     *    declaring how much memory the function can access
     * 5. It helps prevent subtle bugs where functions might
     *    accidentally access memory beyond the buffer
     */
    
    exit(0);
}