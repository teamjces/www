
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char input[] = "aiuäöüあいう𐀀𐀁𐀂\naiuäöüあいう𐀀𐀁𐀂\naiuäöüあいう𐀀𐀁𐀂\n";

/*
 * u8buf
 */

typedef struct {
    union {
        unsigned char str[4];
        unsigned long data;
    } result, work;
    unsigned char *cp;
    unsigned char n;
} u8buf;

u8buf*
new_u8buf ()
{
    u8buf *buf;
    if ((buf = malloc (sizeof (u8buf))) == NULL) {
        printf ("memory exhausted.");
        exit (-1);
    }

    buf->n = 4;
    buf->cp = &buf->work.str[0];

    return buf;
}

void
u8buf_fill (u8buf *buf)
{
    int i;
    static int p=0;

    for (i=0; i<buf->n; i++) {
        buf->cp[i] = input[p++];
    }
    if (buf->work.str[0] == 0) {
        exit (0);
    }
}

void
u8buf_getc (u8buf *buf)
{
    unsigned char tmp;

    buf->result.data = buf->work.data;
    tmp = buf->work.str[0];

    if ((tmp & 0x80) == 0) {    // 0b0xxxxxxx
        buf->n = 1;
        buf->work.str[0] = buf->work.str[1];
        buf->work.str[1] = buf->work.str[2];
        buf->work.str[2] = buf->work.str[3];
        buf->cp = &buf->work.str[3];
        return;
    }

    if ((tmp & 0x40) == 0) {    // 0b10xxxxxx
        // error continuas byte as 1st char
        buf->n = 0;
        return;
    }

    if (tmp == 0xC0 || tmp == 0xC1 || tmp >= 0xF5) {
        // C0 C1 F5 F6 F7 F8 F9 FA FB FC FD FE FF
        // error illegal byte
        buf->n = 0;
        return;
    }

    if ((tmp & 0x20) == 0) {    // 0b110xxxxx: 1st byte of 2-byte sequence
        tmp = buf->work.str[1]; // 2nd byte
        if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuas byte
            buf->n = 2;
            buf->work.str[0] = buf->work.str[2];
            buf->work.str[1] = buf->work.str[3];
            buf->cp = &buf->work.str[2];
            return;
        }

        // error non-continuas byte as 2nd byte
        buf->n = 0;
        return;
    }

    if ((tmp & 0x10) == 0) {    // 0b1110xxxx: 1st byte of 3-byte sequence
        if ((tmp & 0x0F) == 0) {    // 0b11100000: 0xE0
            tmp = buf->work.str[1];     // 2nd byte
            if (tmp < 0xA0) {
                // over encoding
                buf->n = 0;
                return;
            }
        } else {
            tmp = buf->work.str[1]; // 2nd byte
        }

        if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuad byte
            tmp = buf->work.str[2]; // 3rd byte
            if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuas byte
                buf->n = 3;
                buf->work.str[0] = buf->work.str[3];
                buf->cp = &buf->work.str[1];
                return;
            }
        }

        // error non-continuas byte
        buf->n = 0;
        return;
    }
    if ((tmp & 0x08) == 0) {    // 0b11110xxx: 1st byte of 4-byte sequence
        if ((tmp & 0x0F) == 0) {    // 0b11110000: 0xF0
            tmp = buf->work.str[1]; // 2nd byte
            if (tmp < 0x90) {
                // over encoding
                buf->n = 0;
                return;
            }
        } else {
            tmp = buf->work.str[1]; // 2nd byte
        }

        if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuad byte
            tmp = buf->work.str[2]; // 3rd byte
            if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuas byte
                tmp = buf->work.str[3]; // 4th byte
                if ((tmp & 0xC0) == 0x80) { // 0b10xxxxxx: continuas byte
                    buf->n = 4;
                    buf->cp = &buf->work.str[0];
                    return;
                }
            }
        }
        // error non-continuas byte
        buf->n = 0;
        return;
    }
    printf ("UTF-8 PANIC!!\n");
    exit(0);
}

int
main ()
{
    u8buf *buf;
    int i, j;

    buf = new_u8buf();
    while (1) {
        u8buf_fill (buf);
        u8buf_getc (buf);
        printf ("[");
        for (i=0; i<buf->n; i++) {
            printf ("%c", buf->result.str[i]);
        }
        printf ("]");
    }
}
