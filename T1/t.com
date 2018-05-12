#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

/**
* Converts a sequence of ascii characters into its correpondents binary
* 
* @param input string to be converted
* @param out string in binary
* @param len length of the string to be converted
*
* @return the length of the new binary string or -1 in case of error
*/
int32_t ascii_to_binary(char *input, char **out, uint64_t len) {
    uint32_t i;
    int32_t rtn;
    uint32_t str_len = len * 8;

    if((rtn = mem_alloc(out, len, str_len)) == -1){
        return -1;
    }

    for(i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[8 * i];
        unsigned char b;

        // for (b = 7; b >= 0; b--)
        //     *o++ = (ch & (1 << b)) ? '1' : '0';

        for (b = 0x80; b; b >>= 1)
            *o++ = ch & b ? '1' : '0';
    }

    (*out)[str_len] = '\0';

    return (str_len);
}

int32_t nrzi(char* input, char **out, uint64_t len) {
    printf("str: %s\n", input);

    uint32_t i;
    uint32_t rtn;
    char current = '0';

    if((rtn = mem_alloc(out, len, len)) == -1){
        return -1;
    }

    printf("cor: %s\n", "0011010110000011");
    printf("enc: %c", current);
    for (i = 1; i < len; i++) {
        // printf("Iteration %d\n", i);
            // printf("%c\n", message[i]);
        if(input[i] == '0') {
            // printf("aqui\n");
            printf("%c", current);
            // *out[i] = current;
        } else {
            // printf("nope\n");
            current = (current == '0') ? '1' : '0';
            // *out[i] = current;
            printf("%c", current);
        }

    }
    
    for(i = 1; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[i];
        current = ch != '0' ? !current : current;
        *o++ = current;
        printf("%c", *o);
    }


    printf("\nLeaving\n");

    (*out)[len] = '\0';

    return 0;
}

int main(int argc, char *argv[]) {
    int32_t rtrn = 0;
    char *buffer = NULL;
    char *encoding = NULL;

    rtrn = ascii_to_binary(argv[1], &buffer, strlen(argv[1]));
    if(rtrn < 0) {
        printf("Can't convert string\n");
        return (-1);
    }

    printf("str: %s\n", buffer);

    nrzi(buffer, &encoding, strlen(buffer));

    printf("enc: %s\n", encoding);

    return (0);
}
