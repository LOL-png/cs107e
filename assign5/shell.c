/* File: shell.c
 * -------------
 * ***** Shell commands *****
 */
#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h"
#include "mango.h"

#define LINE_LEN 80

// Module-level global variables for shell
static struct {
    input_fn_t shell_read;
    formatted_fn_t shell_printf;
} module;


// NOTE TO STUDENTS:
// Your shell commands output various information and respond to user
// error with helpful messages. The specific wording and format of
// these messages would not generally be of great importance, but
// in order to streamline grading, we ask that you aim to match the
// output of the reference version.
//
// The behavior of the shell commands is documented in "shell_commands.h"
// https://cs107e.github.io/header#shell_commands
// The header file gives example output and error messages for all
// commands of the reference shell. Please match this wording and format.
//
// Your graders thank you in advance for taking this care!


static const command_t commands[] = {
    {"help",  "help [cmd]       ",  "print command usage and description", cmd_help},
    {"echo",  "echo [args]      ",  "print arguments", cmd_echo},
    {"clear", "clear            ",  "clear screen (if your terminal supports it)", cmd_clear},
    {"reboot","reboot           ",  "reboot the Mango Pi", cmd_reboot},
    {"peek",  "peek [addr]      ",  "print contents of memory at address", cmd_peek},
    {"poke",  "poke [addr] [val]",  "store value into memory at address", cmd_poke},
};


int cmd_help(int argc, const char *argv[]) {
    if(argc == 1){ //no arguments 
        for(int i = 0; i < 6;i++){
            module.shell_printf("%s %s\n",commands[i].usage,commands[i].description);
        }
        return 0;
    }
    else{ //1 argument
        int index = 0;
        for(; index < 6;index++){
            if(strcmp(commands[index].name,argv[1])==0){
                break;
            }
        }

        if(index == 6){ //could not find argument
            module.shell_printf("error: no such command '%s'\n",argv[1]);
            return 1;
        }
        else{
            module.shell_printf("%s %s\n",commands[index].usage,commands[index].description);
            return 0;
        }
    }

    return 1;
}

int cmd_echo(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i)
        module.shell_printf("%s ", argv[i]);
    module.shell_printf("\n");
    return 0;
}

int cmd_clear(int argc, const char* argv[]) {
    module.shell_printf("\f");   // formfeed character
    return 0;
}

int cmd_reboot(int argc, const char* argv[]) {
    module.shell_printf("Rebooting..."); //print rebooting
    mango_reboot();
    return 0;
}

int cmd_peek(int argc, const char* argv[]) {
    if(argc==1) { //no arguments
        module.shell_printf("error: peek expects 1 argument [addr]\n");
        return 1;
    } 
    else{
        const char * num = argv[1];
        
        if((num[0]=='0') & ((num[1] == 'x'))){ //verify if hex number
            for(int n = 2; n < strlen(num);n++){
                if( ((num[n] >= 0x30) & (num[n] <= 0x39)) | ((num[n] >= 0x41) & (num[n] <= 0x46)) | ((num[n] >= 0x61) & (num[n] <= 0x66))){

                }
                else{
                    module.shell_printf("error: peek cannot convert '%s'\n",num);
                    return 1;
                }
            }
        }
        else{ //verify if decimal number
            for(int n = 0; n < strlen(num);n++){
                if( ((num[n] >= 0x30) & (num[n] <= 0x39))){

                }
                else{
                    module.shell_printf("error: peek cannot convert '%s'\n",num);
                    return 1;
                }
            }
        }

        if((strtonum(num,NULL)%4) != 0){ //verify if 4-byte aligned
            module.shell_printf("error: peek address must be 4-byte aligned\n");
            return 1;
        }

        uint32_t * ptr = (uint32_t *) strtonum(num,NULL);
        unsigned int value =  *(ptr);

        module.shell_printf("%p: %8x\n",(uint32_t*) strtonum(num,NULL),value); //print shell
        return 0;

    }
    return 1;
}

int cmd_poke(int argc, const char* argv[]) {
    if(argc<=2) { //not enough
        module.shell_printf("error: poke expects 2 arguments [addr] and [val]\n");
        return 1;
    } 
    else{
        for(int i = 1; i < 3;i++){ //check both address and value as valid number
            const char * num = argv[i];
            if((num[0]=='0') & ((num[1] == 'x'))){ //verify if hex number
                for(int n = 2; n < strlen(num);n++){
                    if( ((num[n] >= 0x30) & (num[n] <= 0x39)) | ((num[n] >= 0x41) & (num[n] <= 0x46)) | ((num[n] >= 0x61) & (num[n] <= 0x66))){
    
                    }
                    else{
                        module.shell_printf("error: poke cannot convert '%s'\n",num);
                        return 1;
                    }
                }
            }
            else{ //verify if decimal number
                for(int n = 0; n < strlen(num);n++){
                    if( ((num[n] >= 0x30) & (num[n] <= 0x39))){
    
                    }
                    else{
                        module.shell_printf("error: poke cannot convert '%s'\n",num);
                        return 1;
                    }
                }
            }
        }

        if((strtonum(argv[1],NULL)%4) != 0){ //verify if 4-byte aligned
            module.shell_printf("error: poke address must be 4-byte aligned\n");
            return 1;
        }

        unsigned int * ptr = (unsigned int *) ( strtonum(argv[1],NULL) );
        *(ptr) = (unsigned int) strtonum(argv[2],NULL);

        return 0;

    }
    return 1;
}

void shell_init(input_fn_t read_fn, formatted_fn_t print_fn) {
    module.shell_read = read_fn;
    module.shell_printf = print_fn;
}

void shell_bell(void) {
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize) {
    unsigned long countSize = 1; //if it becomes 0, we know we fell under allowed size

    while( (countSize <= (bufsize)) & (countSize > 0)){ //must be greater than 0, or equal to bufsize (last \n is ignored for null char)
        char ch = module.shell_read();
        if(ch > 0x7f){ //discard invalid values
            continue;
        }
    
        else{
            if(ch == '\b'){ //delete space and ready next position to overfill
                if((countSize-1)==0){
                    shell_bell();
                    continue;
                }
               
                module.shell_printf("%c",'\b');
                module.shell_printf("%c",' ');
                module.shell_printf("%c",'\b');
            
                countSize--;
                buf[countSize-1] = '\0';
            }

            else if(ch == '\n'){ //new line adds last null character and returns
                module.shell_printf("%c",'\n');
                buf[countSize-1] = '\0';
                return;
            }

            else{ //regular character
                if((countSize+1)>bufsize){
                    shell_bell();
                    continue;
                }
                module.shell_printf("%c",ch);
                buf[countSize-1] = ch;
                countSize++;
            }
        }    
    }

}   

static char *strndup(const char *src, size_t n) {
    char *first = (char*)malloc(n+1);
    memcpy(first,src,n);
    *(first+n) = '\0';
    return first;
}

static bool isspace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, char *array[],  int max) {
    int ntokens = 0;
    const char *cur = line;

    while (ntokens < max) {
        while (isspace(*cur)) cur++;    // skip spaces (stop non-space/null)
        if (*cur == '\0') break;        // no more non-space chars
        const char *start = cur;
        while (*cur != '\0' && !isspace(*cur)) cur++; // advance to end (stop space/null)
        array[ntokens++] = strndup(start, cur - start);   // make heap-copy, add to array
    }
    return ntokens;
}

void callfree(const char * arr[],int numberTokens){
    for(int i = 0; i < numberTokens;i++){
        free( (void *) arr[i]);
    }
}

int shell_evaluate(const char *line) {
    char *arrCopy[LINE_LEN];
    int numberTokens = tokenize(line,arrCopy,LINE_LEN);
    const char * arr[numberTokens];
    for(int i = 0; i < numberTokens;i++){
        arr[i]=arrCopy[i];
    }

    if(numberTokens == 0){
        callfree(arr,numberTokens);
        return 1;
    }

    else if(strcmp(commands[0].name,arr[0])==0){ //help
        int m =  cmd_help(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else if((strcmp(commands[1].name,arr[0])==0)){ //echo
        int m = cmd_echo(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else if(strcmp(commands[2].name,arr[0])==0){ //clear
        int m = cmd_clear(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else if(strcmp(commands[3].name,arr[0])==0){ //reboot
        int m = cmd_reboot(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else if(strcmp(commands[4].name,arr[0])==0){ //peek
        int m = cmd_peek(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else if(strcmp(commands[5].name,arr[0])==0){ //poke
        int m = cmd_poke(numberTokens,arr);
        callfree(arr,numberTokens);
        return m;
    }

    else{
        module.shell_printf("error: no such command '%s'\n",arr[0]);
        callfree(arr,numberTokens);
        return 1;
    }

    return 1;
}

void shell_run(void) {
    module.shell_printf("Welcome to the CS107E shell.\nRemember to type on your PS/2 keyboard!\n");
    while (1) {
        char line[LINE_LEN];

        module.shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
