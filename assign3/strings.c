/* File: strings.c
 * ---------------
 * ***** This code is for a string module.
 */
#include "strings.h"

void *memcpy(void *dst, const void *src, size_t n) {
    /* Copy contents from src to dst one byte at a time */
    char *d = dst;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    val = val & 0b11111111; //filters to only LSByte (rightmost byte)
    char *d = dst;
    
    while (n--){ //copies each byte to destination
        *d++ = val;
    }

    return dst; //return destination
}

size_t strlen(const char *str) {
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

//compares two strings and returns which is less or greater lexographically 
int strcmp(const char *s1, const char *s2) {
    size_t n = 0;
    int counter = 0; //track the result

    //compares only while both have not reached null char
    while ((s1[n] != '\0') & (s2[n] != '\0')) {
        if( s1[n] < s2[n]){ //decrement if the ascii character s1 is less
            counter --;
            break;
        }

        else if( s1[n] > s2[n]){//increment if the ascii character in s1 is greater
            counter ++;
            break;
        }
        n++;
    }

    //if counter has not changed, this means we have reached null char on one of the strings without any difference in characters
    //this implies we need to continue checking by seeing which one has greater length (and if equal, return 0) 
    if(counter == 0){
        if( (s1[n] == '\0') & (s2[n] != '\0' ) ){
            counter --;
        }

        if( (s1[n] != '\0') & (s2[n] == '\0' ) ){
            counter ++;
        }

        return counter;
    } 

    else{
        return counter;
    }
}

//concatnates two strings based on the specification of the header file
size_t strlcat(char *dst, const char *src, size_t dstsize) {
    size_t n = 0;

    //track size of src
    size_t size_src = strlen(src); //don't include null char
    size_t size_dst = strlen(dst); //don't include null char

    //obtain the index of null char of dst
    for(; n < dstsize;n++){
        if(dst[n] == '\0' ){
            break;
        }
    }

    //if no index is found, return
    if(n == dstsize){
        return dstsize + size_src;
    }

    //ready return value;
    size_t returnVal = size_dst + size_src;

    //filter size of accomodation for src data
    if( size_src > (dstsize - size_dst-1)){
        size_src = dstsize - size_dst-1;
    } 

    //copy to the memory address described by n
    memcpy((dst+n),src,size_src); //size_src does not null char

    //add the null char character 
    *(dst+n+size_src) = '\0';

    return returnVal;
}

//converts string to number, only working in base 10 or base 16
unsigned long strtonum(const char *str, const char **endptr) {
    //counting
    unsigned long count = 0;
    
    //base 16
    if( (*str == '0') & (*(str+1) == 'x')){
        size_t n = 2;

        //loop to find length of number
        while ( ((str[n] >= 0x30) & (str[n] <= 0x39)) | ((str[n] >= 0x41) & (str[n] <= 0x46)) | ((str[n] >= 0x61) & (str[n] <= 0x66))) {
            n++;
        }

        //add to endptr
        if(endptr != NULL){
            *endptr = (str+n);
        }

        //count number of base exponent (16 ^ basecount)
        long baseCount = 1;

        while (n > 2) {
            n--;
            if((str[n] >= 0x30) & (str[n] <= 0x39)){ //if number
                count += (str[n] % 0x10) * baseCount; //convert ascii to number
            }

            else{ //if letter
                count += ((str[n] % 0x10)+9) * baseCount; //convert ascii to number
            }
            baseCount = baseCount * 16;
        }

    }

    //base 10
    else{
        size_t n = 0;

        //loop for each byte
        while ((str[n] >= 0x30) & (str[n] <= 0x39)) {
            n++;
        }

        //add to endptr
        if(endptr != NULL){
            *endptr = (str+n);
        }

        //count number of base exponent (10 ^ basecount)
        long baseCount = 1;

        while (n > 0) {
            n--;
            count += (str[n] % 0x10)*baseCount; //convert to number
            baseCount = baseCount * 10;
        }
    }
    return count;
}
