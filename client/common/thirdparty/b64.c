/*********************************************************************\
MODULE NAME:    b64.c

AUTHOR:         Bob Trower 08/04/01

PROJECT:        Crypt Data Packaging

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001

NOTES:          This source code may be used as you wish, subject to
                the MIT license.  See the LICENCE section below.

                Canonical source should be at:
                    http://base64.sourceforge.net

DESCRIPTION:
                This little utility implements the Base64
                Content-Transfer-Encoding standard described in
                RFC1113 (http://www.faqs.org/rfcs/rfc1113.html).

				
LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

                Permission is hereby granted, free of charge, to any person
                obtaining a copy of this software and associated
                documentation files (the "Software"), to deal in the
                Software without restriction, including without limitation
                the rights to use, copy, modify, merge, publish, distribute,
                sublicense, and/or sell copies of the Software, and to
                permit persons to whom the Software is furnished to do so,
                subject to the following conditions:

                The above copyright notice and this permission notice shall
                be included in all copies or substantial portions of the
                Software.

                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
                KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
                WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
                PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
                OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
                OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
                OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
                SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

\******************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "b64.h"

//Translation Table to decode (created by original author)
const char cd64[] =
    "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

//Hex "A" character.  Set to lowercase or uppercase
const char HEX_A = 'a';

//decode 4 '6-bit' characters into 3 8-bit binary bytes
void decodeblock(unsigned char *in, unsigned char *out)
{
	out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
	out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
	out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

//decode a base64 encoded stream discarding padding, line breaks and noise
unsigned int b64decode(char **output, const char *intext)
{
	*output = (char *) malloc(sizeof(char) * strlen(intext));
	unsigned char in[4];
	unsigned char out[3];
	int v;
	int i, len;
	unsigned int b = 0;
	int a = 0;
	*in = (unsigned char) 0;
	*out = (unsigned char) 0;
	for (a = 0; intext[a] != '\0';) {
		for (len = 0, i = 0; i < 4 && intext[a] != '\0'; i++) {
			v = 0;
			while (intext[a] != '\0' && v == 0) {
				v = intext[a++];
				if (v != EOF) {
					v = ((v < 43
					      || v >
					      122) ? 0 : (int) cd64[v -
								    43]);
					if (v != 0) {
						v = ((v ==
						      (int) '$') ? 0 : v -
						     61);
					}
				}
			}
			if (intext[a] != '\0') {
				len++;
				if (v != 0) {
					in[i] = (unsigned char) (v - 1);
				}
			} else {
				in[i] = (unsigned char) 0;
			}
		}
		if (len > 0) {
			decodeblock(in, out);
			for (i = 0; i < len - 1; i++) {
				(*output)[b++] = out[i];
				(*output)[b] = out[i + 1];
			}
		}
	}

	//Return size of output
	return b;
}

//Encode in hex
char *b16encode(const char *intext, unsigned int length)
{
	char *output = (char *) malloc(sizeof(char) * (length * 2 + 1));

	unsigned int i;
	for (i = 0; i < length; i++) {
		//Check divisibility by 16
		output[2 * i] =
		    (unsigned char) intext[i] / 16 +
		    ((unsigned char) intext[i] / 16 >
		     9 ? HEX_A - 10 : '0');

		//Check mod 16
		output[2 * i + 1] =
		    (unsigned char) intext[i] % 16 +
		    ((unsigned char) intext[i] % 16 >
		     9 ? HEX_A - 10 : '0');

		//End string
		output[2 * i + 2] = '\0';
	}
	return output;
}

//Decode from hex
char *b16decode(const char *intext)
{
	char *output =
	    (char *) malloc(sizeof(char) * strlen(intext) / 2 + 1);

	//Merge every two bytes into one byte
	unsigned int i;
	for (i = 0; i < strlen(intext) / 2; i++)
		output[i] =
		    //Decode first hex char into first four bits
		    (intext[2 * i] -
		     (intext[2 * i] >= HEX_A ? HEX_A - 10 : '0')) * 16
		    //Decode last hex char into last four bits
		    + intext[2 * i + 1] - (intext[2 * i + 1] >=
					   HEX_A ? HEX_A - 10 : '0');

	//Add ending byte and return
	output[i + 1] = '\0';
	return output;
}
