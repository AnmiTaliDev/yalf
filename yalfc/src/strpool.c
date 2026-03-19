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

#include "strpool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sp_init(StrPool *p)
{
    p->strings = NULL;
    p->count   = 0;
    p->cap     = 0;
}

void sp_free(StrPool *p)
{
    for (int i = 0; i < p->count; i++)
        free(p->strings[i]);
    free(p->strings);
    sp_init(p);
}

uint16_t sp_lookup(const StrPool *p, const char *s)
{
    if (!s || !s[0])
        return YALF_NULL_POOL;
    for (int i = 0; i < p->count; i++)
        if (strcmp(p->strings[i], s) == 0)
            return (uint16_t)i;
    return YALF_NULL_POOL;
}

uint16_t sp_intern(StrPool *p, const char *s)
{
    if (!s || !s[0])
        return YALF_NULL_POOL;

    /* Return existing ID if already present. */
    for (int i = 0; i < p->count; i++)
        if (strcmp(p->strings[i], s) == 0)
            return (uint16_t)i;

    /* Pool is limited to 65534 entries (0xFFFF reserved). */
    if (p->count >= 0xFFFE) {
        fprintf(stderr, "yalfc: string pool overflow\n");
        exit(1);
    }

    if (p->count >= p->cap) {
        p->cap = p->cap ? p->cap * 2 : 16;
        p->strings = realloc(p->strings, (size_t)p->cap * sizeof(char *));
        if (!p->strings) {
            fprintf(stderr, "yalfc: out of memory\n");
            exit(1);
        }
    }

    p->strings[p->count] = strdup(s);
    return (uint16_t)p->count++;
}

void sp_serialize(const StrPool *p, Buf *b)
{
    buf_u16(b, (uint16_t)p->count);
    for (int i = 0; i < p->count; i++) {
        size_t len = strlen(p->strings[i]);
        buf_u16(b, (uint16_t)len);
        buf_bytes(b, p->strings[i], len);
    }
}
