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

        for (b = 0x80; b; b >>= 1)
            *o++ = ch & b ? '1' : '0';
    }

    (*out)[str_len] = '\0';

    return (str_len);
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

int32_t nrz(char* input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t rtn;

    if((rtn = mem_alloc(out, len, len)) == -1){
        return -1;
    }

    for(i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[i];

        *o++ = ch;
    }

    (*out)[len] = '\0';

    return (len);    
}

int32_t manchester(char* input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t j;
    uint32_t rtn;
    uint32_t str_len = len * 2;

    if((rtn = mem_alloc(out, len, str_len)) == -1){
        return -1;
    }

    for (i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[2 * i];

        for(j = 0; j < 2; j++) {
            *o++ = ch ^ j;
        }
    }

    (*out)[str_len] = '\0';

    return (str_len);
}

int32_t nrzi(char* input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t rtn;
    char current = '0';

    if((rtn = mem_alloc(out, len, len)) == -1){
        return -1;
    }
    
    for(i = 0; i < len; i++) {
        unsigned char ch = input[i];
        char *o = &(*out)[i];

        *o++ = ch == '0' ? current : (current = current == '0' ? '1' : '0');
    }

    (*out)[len] = '\0';

    return 0;
}

int32_t _4b5b(char* input, char **out, uint64_t len) {
    uint32_t i;
    uint32_t j;
    uint32_t rtn;
    uint32_t str_len = len + len/4;
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

    if((rtn = mem_alloc(out, len, len)) == -1){
        return -1;
    }

    for(i = 0; i < len/4; i++) {
        char *o = &(*out)[5 * i];
        char subbuff[5];
        memcpy(subbuff, &input[4 * i], 4);
        subbuff[4] = '\0';

        int index = bin2dec(atoi(subbuff));

        for(j = 0; j < 5; j++) {
            *o++ = encodings[5 * index + j];
        }
    }

    (*out)[str_len] = '\0';

    return 0;
}

int char_to_bits(char* input, char** output, int len){
	char* bits;
	char* cursor;
	int size=0;
	int i,j,c,jMax;

	bits = malloc(1);
	for(i=0;i<len;i+=8){
		size = size + sizeof(char);
		bits = realloc(bits,size);
		cursor = bits+i/8;
		memset(cursor,0,1);
		if(len-i < 8){
			jMax = len-i;
		}else
			jMax=8;
		//printf("bits: %x\ncursor: %x\n",bits,cursor);
		for(j=0;j<jMax;j++){
			//printf("%x\n",(char)*cursor);
			*cursor <<= 1;
			c = *(input+i+j)-48;
			printf("d: %d\n", c);
			*cursor |= (c & 0x01);
		}
		//printf("%x\n",(char)*bits);
		//printf("%x\n",(char)*cursor);
	}
	//printf("bits: %x\ncursor: %x\n",bits,cursor);
	//printf("%x\n",(char)*(bits));]
	*output = bits;
	return size;
}


int main(int argc, char *argv[]) {
    int32_t rtrn = 0;
    char *buffer = NULL;
    char *encoding = NULL;

    rtrn = ascii_to_binary(argv[2], &buffer, strlen(argv[2]));
    if(rtrn < 0) {
        printf("Can't convert string\n");
        return (-1);
    }

    printf("str: %s\n", buffer);

    if (strcmp(argv[1], "-n") == 0) {
        nrz(buffer, &encoding, strlen(buffer));
    } else if (strcmp(argv[1], "-m") == 0) {
        manchester(buffer, &encoding, strlen(buffer));
    } else if (strcmp(argv[1], "-i") == 0) {
        nrzi(buffer, &encoding, strlen(buffer));
    } else if (strcmp(argv[1], "-f") == 0) {
        _4b5b(buffer, &encoding, strlen(buffer));
    }

    printf("enc: %s\n", encoding);

    printf("%x",char_to_bits(encoding,&encoding,strlen(encoding));

    return (0);
}
