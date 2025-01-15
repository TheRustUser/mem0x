#include <string.h>
#include <stdbool.h>

size_t strlen(const char* fmt) {
    if (!fmt) {
        return 0;
    }

    const char* s;
    for (s = fmt; *s; ++s);

    return (s - fmt);
}

char* itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /*  If %d is specified and D is minus, put ‘-’ in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;

  /*  Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /*  Terminate BUF. */
  *p = 0;
  
  /*  Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

// char* itoa(int value, char* str, int base) {
//     if (base < 2 || base > 36) {
//         *str = '\0';
//         return str;
//     }

//     char* ptr = str;
//     char* end = str;
//     int is_negative = 0;

//     if (value < 0 && base == 10) {
//         is_negative = 1;
//         value = -value;
//     }

//     do {
//         int digit = value % base;
//         *end++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
//         value /= base;
//     } while(value);

//     if (is_negative) {
//         *end++ = '-';
//     }

//     *end = '\0';

//     for (--end; ptr < end; ++ptr, --end) {
//         char temp = *ptr;
//         *ptr = *end;
//         *end = temp;
//     }

//     return str;
// }
