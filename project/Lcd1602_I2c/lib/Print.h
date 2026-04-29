#ifndef Print_h
#define Print_h

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

class Print {
public:
    virtual size_t write(uint8_t) = 0;
    
    size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    }
    
    size_t write(const uint8_t *buffer, size_t size) {
        size_t n = 0;
        while (size--) {
            if (write(*buffer++)) n++;
            else break;
        }
        return n;
    }
    
    // print() functions
    size_t print(const char str[]) {
        return write(str);
    }
    
    size_t print(char c) {
        return write((uint8_t)c);
    }
    
    size_t print(unsigned char b, int base = 10) {
        return printNumber(b, base);
    }
    
    size_t print(int n, int base = 10) {
        if (base == 0) {
            return write(n);
        } else if (base == 10) {
            if (n < 0) {
                int t = print('-');
                return printNumber(-n, 10) + t;
            }
            return printNumber(n, 10);
        } else {
            return printNumber(n, base);
        }
    }
    
    size_t print(unsigned int n, int base = 10) {
        return printNumber(n, base);
    }
    
    size_t print(long n, int base = 10) {
        if (base == 0) {
            return write(n);
        } else if (base == 10) {
            if (n < 0) {
                int t = print('-');
                return printNumber(-n, 10) + t;
            }
            return printNumber(n, 10);
        } else {
            return printNumber(n, base);
        }
    }
    
    size_t print(unsigned long n, int base = 10) {
        return printNumber(n, base);
    }
    
    size_t println(const char str[]) {
        size_t n = print(str);
        n += println();
        return n;
    }
    
    size_t println(void) {
        return write("\r\n");
    }
    
    size_t println(char c) {
        size_t n = print(c);
        n += println();
        return n;
    }
    
    size_t println(unsigned char b, int base = 10) {
        size_t n = print(b, base);
        n += println();
        return n;
    }
    
    size_t println(int num, int base = 10) {
        size_t n = print(num, base);
        n += println();
        return n;
    }
    
    size_t println(unsigned int num, int base = 10) {
        size_t n = print(num, base);
        n += println();
        return n;
    }
    
    size_t println(long num, int base = 10) {
        size_t n = print(num, base);
        n += println();
        return n;
    }
    
    size_t println(unsigned long num, int base = 10) {
        size_t n = print(num, base);
        n += println();
        return n;
    }

protected:
    size_t printNumber(unsigned long n, uint8_t base) {
        char buf[33];
        char *str = &buf[32];
        *str = '\0';
        
        if (n == 0) {
            return write('0');
        }
        
        unsigned long i = n;
        while (i > 0) {
            unsigned long m = i % base;
            char c = (m < 10) ? ('0' + m) : ('A' + m - 10);
            *--str = c;
            i /= base;
        }
        return write(str);
    }
};

#endif

