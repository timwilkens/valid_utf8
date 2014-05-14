#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool is_valid_utf8(char *c, int size);
bool invalid_trailing_byte(char c);
int number_of_bytes(char c);

int
main (void) {

  char *strings[] = { "\xc3\xb1",                  // Valid.
                      "\xe2\x28\xa1",              // Invalid.
                      "Hello, World!",             // Valid.
                      "\xf0\x28\x8c\x28",          // Invalid.
                      "\xE2\x98\x82\xE2\x98\x83",  // Valid.
                      "\xf0\x28\x8c\xbc",          // Invalid.
                      "\xe2\x28\x28",              // Invalid.
                      "\xc3\x28",                  // Invalid.
                      "",                          // Valid.
                      "   ",                       // Valid.
                      "\x80",                      // Invalid.
                    };

  int end = sizeof(strings) / sizeof(strings[0]);
  for (int i = 0; i < end; i++) {

    char *string = strings[i];
    int size = strlen(string);

    char *answer = is_valid_utf8(string, size) ? "Valid" : "Invalid";
    printf("%s\n", string);
    printf("\t\t%s\n", answer);
  }

  return 0;
}

bool
is_valid_utf8(char *c, int size) {

  for (int i = 0; i < size; i++) {
    switch(number_of_bytes(c[i])) {
      case 1 :
        break;
      case 2 :
        if (i + 1 > size) { return false; }
        if (invalid_trailing_byte(c[++i])) { return false; }
        break;
      case 3 :
        for (int temp = 0; temp < 2; temp++) {
          if (i + 1 > size) { return false; }
          if (invalid_trailing_byte(c[++i])) { return false; }
        }
        break;
      case 4 :
        for (int temp = 0; temp < 3; temp++) {
          if (i + 1 > size) { return false; }
          if (invalid_trailing_byte(c[++i])) { return false; }
        }
        break;
      default :
        return false;
    }
  }
  return true;
}

bool
invalid_trailing_byte(char c) {
// After the leading byte, we have n-1 occurrences of 10xxxxxx bytes
  return ((c & 0xC0) == 0x80) ? false : true;
}

int
number_of_bytes(char c) {
  // High bit set to 0 only on single byte sequences (same as ASCII).
  if ((c & 0x80) == 0) { return 1; }
  // All others have the first n bits set to 1 and the next 0.
  // e.g. 2 byte sequences have the first byte start with 110.
  else if ((c & 0xE0) == 0xC0) { return 2; }
  else if ((c & 0xF0) == 0xE0) { return 3; }
  else if ((c & 0xF8) == 0xF0) { return 4; }
  // Not a valid UTF-8 sequence.
  else { return 100; }
}
