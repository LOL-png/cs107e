/* File: printf.c
 * --------------
 * ***** This function implements printf *****
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/* Prototypes for internal helpers.
 * Typically we would qualify these functions as static (private to module)
 * but in order to call them from the test program, must declare externally
 */
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);

// max number of digits in long + space for negative sign and null-terminator
#define MAX_DIGITS 25


/* Convenience functions `hex_string` and `decimal_string` are provided
 * to you.  You should use the functions as-is, do not change the code!
 *
 * A key implementation detail to note is these functions declare
 * a buffer to hold the output string and return the address of buffer
 * to the caller. If that buffer memory were located on stack, it would be
 * incorrect to use pointer after function exit because local variables
 * are deallocated. To ensure the buffer memory is accessible after
 * the function exists, the declaration is qualified `static`. Memory
 * for a static variable is not stored on stack, but instead in the global data
 * section, which exists outside of any function call. Additionally static
 * makes it so there is a single copy of the variable, which is shared by all
 * calls to the function. Each time you call the function, it overwrites/reuses
 * the same variable/memory.
 *
 * Adding static qualifier to a variable declared inside a function is a
 * highly atypical practice and appropriate only in very specific situations.
 * You will likely never need to do this yourself.
 * Come talk to us if you want to know more!
 */

const char *hex_string(unsigned long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    num_to_string(val, 16, buf); // num_to_string does the hard work
    return buf;
}

const char *decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        num_to_string(-val, 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        num_to_string(val, 10, buf);
    }
    return buf;
}

//converts a number to string (in only base 10 or 16)
void num_to_string(unsigned long num, int base, char *outstr) {
    if(num == 0){
        outstr[0] = '0';
        outstr[1] = '\0';
        return;
    }

    size_t n = 0; //digit number

    int divideBy = 10; //divideBy number
    if(base == 16){
        divideBy = 16;
    }

    char arr[65];
    char *temp = arr; //temporary buffer

    //register each invidual digit
    while(num > 0){
        int digit = num % divideBy;
        num = num/divideBy;

        if(digit < 10){ //if less than 10
            *(temp+n) = digit + '0';
        }

        else{ //if a letter
            *(temp+n) = (digit-10) + 'a';
        }

        n++;
    }

    *(temp+n) = '\0'; //add null char

    //reverse order of temp and add it to outstr
    for(int i = n-1;i >=0;i--){
        *(outstr+(n-1-i)) = *(temp + i);
    }

    *(outstr+n) = '\0'; //add null char
}

//adds padding
char* padding( char * temp, long whitespace, int hexMaybe, int pointMaybe){
   if((strlen(temp) < whitespace)){ //add padding for hex/other characters
       long remainingSpace = (whitespace - strlen(temp));
       char arr[remainingSpace+strlen(temp) + 1];
       char * padding = arr;
       if(hexMaybe | pointMaybe){ //hex and pointer padding
           memset(padding,0x30,remainingSpace);
       }
       else{ //normal character padding
           memset(padding,0x20,remainingSpace);
       }
       *(padding+remainingSpace) = '\0';
       strlcat(padding,temp,strlen(padding)+strlen(temp) + 1);
       return padding;
       
   }
   return temp;
}


int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args) {
    size_t output = 0; //track output
    unsigned long a = (unsigned long) format; //track char location 
    char * tracker = (char *) a;

    char arrF[bufsize]; //makes another array that will be added to buf
    char * luf = arrF;
    *luf = '\0';

    // const char nullCharArr[] = {'\0'};
    // const char * nullchar = nullCharArr;

    while (*(tracker) != '\0'){
        char arrG[bufsize*4]; //will never be used, but needs to be set
        char *temp = arrG;

        if(*(tracker) == '%'){ //special formatting
            tracker++;

            long whitespace = 0; //find number for space padding
            int whiteChange = 0; //determines if we defined padding
            int hexMaybe = 0; //determines what version of space padding
            int pointMaybe = 0; //determines what version of space padding

            if( (*(tracker) >= 0x30) & (*(tracker) <= 0x39)) { //add space padding
                const char *rest = NULL; //record beginning of string after whitespace
                whitespace  = strtonum(tracker,&rest);
                unsigned long b = (unsigned long) rest;
                tracker = (char *) b;
                whiteChange = 1;
            } 

            if(*(tracker) == '%'){ //percent
                char arr[] = { 0x25,'\0'};
                temp = arr;
            }
            if(*(tracker) == 'c'){ //single character
                char arr[] = { va_arg(args,int),'\0'};
                temp = arr;
            }
            if(*(tracker) == 's'){ //string
                temp = va_arg(args,char *);
            }
            if(*(tracker) == 'd'){ //decimal int
                const char* var = decimal_string(va_arg(args, int));
                memcpy(temp,var,strlen(var)+1);
            }
            if(*(tracker) == 'x'){ //hexidecimal int
                const char * var = hex_string(va_arg(args,unsigned int));
                memcpy(temp,var,strlen(var)+1);
                hexMaybe = 1;
            }
            if(*(tracker) == 'l'){ //long
               tracker++;;
               if(*(tracker) == 'd'){ //decimal long
                    const char * var = decimal_string(va_arg(args,long));
                    memcpy(temp,var,strlen(var)+1);
               }
               if(*(tracker) == 'x'){ //hexidecimal long
                    const char * var = hex_string(va_arg(args,unsigned long));
                    memcpy(temp,var,strlen(var)+1);
                    hexMaybe = 1;
               }
            }
            if(*(tracker) == 'p'){ //pointer - think about later
                pointMaybe = 1;  //mark that we need to be wary of pointer changes
                output+=strlen("0x");
                strlcat(luf,"0x",bufsize); //add "0x" prefix into buf
                const char * var = hex_string(va_arg(args,long));
                memcpy(temp,var,strlen(var)+1);
                if(!whiteChange){ //specifically creates default 8 width IF width has not been specified
                    whitespace = 8;
                }
            }

            //adds padding
            temp = padding(temp,whitespace,hexMaybe,pointMaybe);
            tracker++;
        }
        else{ //normal character
            char arr[] = { *(tracker),'\0'};
            temp = arr;
            tracker++;
        }

        output += strlen(temp);
        strlcat(luf,temp,bufsize);
    }   

    //if bufsize not zero, add luf to buf
    if(bufsize != 0){
        *buf = '\0';
        strlcat(buf,luf,bufsize);
    }

    return output;
}


int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    va_list ap; //initalize va_list
    va_start(ap,format); 
    int output = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);
    return output;
}

// ok to assume printf output is never longer that MAX_OUTPUT_LEN
#define MAX_OUTPUT_LEN 1024

int printf(const char *format, ...) {
    char array[MAX_OUTPUT_LEN]; //declare array
    va_list ap; //initalize va_list
    va_start(ap,format); 
    int output = vsnprintf(array,sizeof(array),format,ap);
    va_end(ap);
    uart_putstring(array);
    return output;
}