#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
// Union to store 4 bytes as an array of bytes, an unsigned, signed, or float number
union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};
//function declaration
char hexDigit(char c);
// reads 8 hex characters from string input and stores it in the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input){
    if(strlen(input) != 8){
        return -1;
    }

    for(int i = 0; i < 4; i++){
        char high = hexDigit(input[2*i]);
        char low = hexDigit(input[2*i + 1]);

        if(high == -1 || low == -1){
            return -1;
        }

        v->bytes[3-i] = (high << 4) | low;
    }

    return 0;
}

// converts the ASCII hex character c to binary
// returns the hex value of c if c is a valid hex digit, -1 otherwise
char hexDigit(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    if(c >= 'a' && c <= 'f'){
        return c - 'a' + 10;
    }
    if(c >= 'A' && c <= 'F'){
        return c - 'A' + 10;
    }
    return -1;
}

// returns true if x has any even bit equal to 1, 0 otherwise
int any_even_one(unsigned x){
    unsigned ref = 0x55555555;
    return (x & ref) != 0;
}
// returns a mask indicating the position of the left most one in x
int leftmost_one(unsigned x){
    unsigned mask = 0x80000000;
    while(x != 0 && (x & mask) == 0){
        mask >>= 1;
    }
    return mask;
}
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n){

    unsigned int mask = x >> (32 - n);
    x <<= n;
    return x | mask;
}
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n){

    unsigned int mask = x << (32 - n);
    x >>= n;
    return x | mask;
}
// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y){
    if ((x > 0 && y > 0 && x > INT_MAX - y)){
        return INT_MAX;
    }
    if ((x < 0 && y < 0 && x < INT_MIN - y)){
        return INT_MIN;
    }
    return x + y;
}
// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f){
    unsigned sign = f & 0x80000000;
    unsigned exp = f & 0x7F800000;
    unsigned frac = f & 0x007FFFFF;

    // NaN or infinity
    if(exp == 0x7F800000){
        return f;
    }

    if(exp == 0){
        frac <<= 1;
        return sign | frac;
    }

    exp += 0x00800000;

    if(exp == 0x7F800000){
        frac = 0;
    }

    return sign | exp | frac;
}
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f){
    unsigned sign = f & 0x80000000;
    unsigned exp = f & 0x7F800000;
    unsigned frac = f & 0x007FFFFF;

    // NaN or infinity
    if(exp == 0x7F800000){
        return f;
    }

    if(exp == 0){
        frac >>= 1;
        return sign | frac;
    }

    exp -= 0x00800000;

    if(exp == 0){
        frac = frac | 0x00800000;
        frac >>= 1;
    }

    return sign | exp | frac;
}

int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        printf("Invalid number of arguments");
        exit(0);
    }

    union value v;
    union value u;
    if(read_hex(&v, argv[2]) == -1){
        printf("Invalid hex value\n");
        exit(0);
    }

    switch (argv[1][0]) {
        case 'e':
            // even
            if(any_even_one(v.uval)){
                printf("True\n");
            }
            else {
                printf("False\n");
            }
            // printf("%d\n", any_even_one(v.uval));
            break;

        case 'l':
            // left OR lrotate
            if(argv[1][1] == 'r'){
                if(atoi(argv[3]) > 32){
                    printf("Invalid number of shift positions\n");
                    exit(0);
                }
                printf("%08x\n", rotate_left(v.uval, atoi(argv[3])));
            } else {
                printf("%08x\n", leftmost_one(v.uval));
            }
            break;

        case 'r':
            // rrotate
            if(atoi(argv[3]) > 32){
                printf("Invalid number of shift positions\n");
                exit(0);
            }
            printf("%08x\n", rotate_right(v.uval, atoi(argv[3])));
            break;

        case 's':
            // saturate
            if(read_hex(&u, argv[3]) == -1){
                printf("Invalid hexadecimal u number\n");
                exit(0);
            }
            int results = saturating_add(v.sval, u.sval);
            printf("%08x %d\n", results, results);
            break;

        case 't':{
            // twice
            unsigned resultt = float_twice(v.uval);
            union value tempt;
            tempt.uval = resultt;
            printf("%08x %e\n", resultt, tempt.fval);
            break;
        }

        case 'h':{
            // half

            unsigned resulth = float_half(v.uval);
            union value temph;
            temph.uval = resulth;
            printf("%08x %e\n", resulth, temph.fval);
            break;
        }

        default:
            // invalid operation
            printf("Invalid operation\n");
            break;
    }
}
