
#include "md5.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

int sysmd5 (char *result, char *input) {
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[33];
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)input, strlen(input));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(result + di * 2, "%02x", digest[di]);
   return 1;
}
