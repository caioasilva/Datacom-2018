#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

/**
* Allocates memory for the output variables
* 
* @param out string in binary
* @param len length of the string to be converted
* @param the length of the binary string
*
* @return 0 if succeeds or -1 in case of error
*/
int32_t mem_alloc(char **out, uint64_t len, uint32_t str_len) {
    if(len == 0) {
        printf("Length argument is zero\n");
        return (-1);
    }

    (*out) = malloc(str_len + 1);
    if((*out) == NULL) {
        printf("Can't allocate binary string: %s\n", strerror(errno));
        return (-1);
    }

    if(memset((*out), 0, (str_len)) == NULL) {
        printf("Can't initialize memory to zero: %s\n", strerror(errno));
        return (-1);
    }

    return 0;
}

int bin2dec (int num) {
    int  decimal_val = 0, base = 1, rem;

    while (num > 0) {
        rem = num % 10;
        decimal_val = decimal_val + rem * base;
        num = num / 10 ;
        base = base * 2;
    }

    return decimal_val;
}

/**
* Converts a sequence of ascii characters into its correpondents binary
* 
* @param input string to be converted
* @param out string in binary
* @param len length of the string to be converted
*
* @return the length of the new binary string or -1 in case of error
*/
int32_t ascii_to_binary(char *input, char **out, uint64_t len, uint32_t size) {
    uint32_t i;
    int32_t rtn;
    uint32_t str_len = len * size;

    if((rtn = mem_alloc(out, len, str_len)) == -1){
        return -1;
    }

    for(i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[size * i];
        unsigned char b;

        for (b = pow(2,size-1); b; b >>= 1)
            *o++ = ch & b ? '1' : '0';
    }

    (*out)[str_len] = '\0';

    return (str_len);
}

int32_t binary_to_ascii(char *input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t j;
    int32_t rtn;
    uint32_t str_len = len / 8;

    for(i = 0; i < str_len; i++) {
        char *o = &(*out)[i];
        char subbuff[9];
        memcpy(subbuff, &input[8 * i], 8);
        subbuff[8] = '\0';
        char character = (char) bin2dec(atoi(subbuff));

        *o++ = character;
    }

    (*out)[str_len] = '\0';

    return (str_len);
}

int32_t nrz(char* input, char **out, uint64_t len) {
    int32_t rtrn = 0;
    char *buffer = NULL;
    printf("%s\n", input);

    if((rtrn = mem_alloc(out, len, len)) == -1){
        return -1;
    }

    if(rtrn < 0) {
        printf("Can't convert string\n");
        return (-1);
    }

    rtrn = binary_to_ascii(input, out, len);

    return len;
}

int32_t manchester(char* input, char **out, uint64_t len) {
    int32_t rtrn = 0;
    uint32_t i;
    uint32_t j;
    uint32_t str_len = len / 2;

    if((rtrn = mem_alloc(out, len, str_len)) == -1){
        return -1;
    }

    if(rtrn < 0) {
        printf("Can't convert string\n");
        return (-1);
    }

    for (i = 0; i < len; i+=2) {
        unsigned char ch = input[i];
        char *o = &(*out)[i/2];

        *o++ = ch ^ (i % 2);
    }

    rtrn = binary_to_ascii(*out, out, str_len);

    return str_len;
}

int32_t nrzi(char* input, char **out, uint64_t len) {
    int32_t rtrn = 0;
    uint32_t i;
    char current = '0';

    if((rtrn = mem_alloc(out, len, len)) == -1){
        return -1;
    }

    if(rtrn < 0) {
        printf("Can't convert string\n");
        return (-1);
    }

    for (i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[i];

        if (ch == current) {
            *o++ = '0';
        } else {
            current = current == '0' ? '1' : '0';
            *o++ = '1';
        }
    }

    rtrn = binary_to_ascii(*out, out, len);

    return len;
}



int32_t _4b5b(char* input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t j;
    uint32_t rtn;
    uint32_t str_len = len - len/5;
    char encodings[80] = { '1', '1', '1', '1', '0', 
                            '0', '1', '0', '0', '1', 
                            '1', '0', '1', '0', '0', 
                            '1', '0', '1', '0', '1', 
                            '0', '1', '0', '1', '0', 
                            '0', '1', '0', '1', '1', 
                            '0', '1', '1', '1', '0', 
                            '0', '1', '1', '1', '1', 
                            '1', '0', '0', '1', '0', 
                            '1', '0', '0', '1', '1', 
                            '1', '0', '1', '1', '0', 
                            '1', '0', '1', '1', '1', 
                            '1', '1', '0', '1', '0', 
                            '1', '1', '0', '1', '1', 
                            '1', '1', '1', '0', '0', 
                            '1', '1', '1', '0', '1'};

    if((rtn = mem_alloc(out, len, str_len)) == -1){
        return -1;
    }
    char* process = *out;
    strcpy(process, input);
    for (i=0;i<16;i++){
        char n[1]={i};
        char* b;
        ascii_to_binary(n,&b,1,4);
        for (j=0; j<len/5;j++){
            if (strncmp(encodings+i*5,input+j*5,5)==0){
                memcpy(process+j*4,b,4);
            }
        }
    }

    process[str_len]='\0';
    rtn = binary_to_ascii(process, out, str_len);

    return str_len;
}

int main(int argc, char *argv[]) {
    int32_t rtrn = 0;
    char *buffer = NULL;
    char *decoding = NULL;

    if (strcmp(argv[1], "-n") == 0) {
        nrz(argv[2], &decoding, strlen(argv[2]));
    } else if (strcmp(argv[1], "-m") == 0) {
        manchester(argv[2], &decoding, strlen(argv[2]));
    } else if (strcmp(argv[1], "-i") == 0) {
        nrzi(argv[2], &decoding, strlen(argv[2]));
    } else if (strcmp(argv[1], "-f") == 0) {
        _4b5b(argv[2], &decoding, strlen(argv[2]));
    }

    printf("dec: %s\n", decoding);

    return (0);
}
