/*
 * yalfc - YALF Binary Compiler
 * Copyright (C) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "buf.h"
#include "opts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buf_init(Buf *b)
{
    b->data = NULL;
    b->len  = 0;
    b->cap  = 0;
}

void buf_free(Buf *b)
{
    free(b->data);
    buf_init(b);
}

static void buf_grow(Buf *b, size_t extra)
{
    if (b->len + extra <= b->cap)
        return;
    size_t new_cap = b->cap ? b->cap * 2 : 256;
    while (new_cap < b->len + extra)
        new_cap *= 2;
    b->data = realloc(b->data, new_cap);
    if (!b->data) {
        fprintf(stderr, "yalfc: out of memory\n");
        exit(1);
    }
    b->cap = new_cap;
}

void buf_u8(Buf *b, uint8_t v)
{
    buf_grow(b, 1);
    b->data[b->len++] = v;
}

void buf_u16(Buf *b, uint16_t v)
{
    buf_grow(b, 2);
    b->data[b->len++] = (uint8_t)(v & 0xFF);
    b->data[b->len++] = (uint8_t)(v >> 8);
}

void buf_u32(Buf *b, uint32_t v)
{
    buf_grow(b, 4);
    b->data[b->len++] = (uint8_t)(v & 0xFF);
    b->data[b->len++] = (uint8_t)((v >> 8)  & 0xFF);
    b->data[b->len++] = (uint8_t)((v >> 16) & 0xFF);
    b->data[b->len++] = (uint8_t)((v >> 24) & 0xFF);
}

void buf_bytes(Buf *b, const void *data, size_t n)
{
    if (!n) return;
    buf_grow(b, n);
    memcpy(b->data + b->len, data, n);
    b->len += n;
}

/* Safely truncate at a UTF-8 character boundary. */
static size_t utf8_trunc(const char *s, size_t max)
{
    size_t n = max;
    /* Back off while we land on a UTF-8 continuation byte (10xxxxxx). */
    while (n > 0 && ((unsigned char)s[n] & 0xC0u) == 0x80u)
        n--;
    return n;
}

void buf_str8(Buf *b, const char *s, const char *ctx)
{
    if (!s || !s[0]) {
        buf_u8(b, 0);
        return;
    }
    size_t len = strlen(s);
    if (len > 255) {
        len = utf8_trunc(s, 255);
        if (!g_opts.quiet)
            fprintf(stderr, "yalfc: warning: '%s' field truncated to 255 bytes\n",
                    ctx ? ctx : "field");
    }
    buf_u8(b, (uint8_t)len);
    buf_bytes(b, s, len);
}

void buf_str16(Buf *b, const char *s)
{
    if (!s || !s[0]) {
        buf_u16(b, 0);
        return;
    }
    size_t len = strlen(s);
    if (len > 65535) len = 65535;
    buf_u16(b, (uint16_t)len);
    buf_bytes(b, s, len);
}

int buf_write_file(const Buf *b, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror(path);
        return -1;
    }
    if (b->len && fwrite(b->data, 1, b->len, f) != b->len) {
        perror(path);
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}
