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

#ifndef STRPOOL_H
#define STRPOOL_H

#include <stdint.h>
#include "yalf.h"   /* YALF_NULL_POOL */
#include "buf.h"

/*
 * String pool: deduplicates repeated strings (word types, writing system
 * names, synonym notes). Each unique string gets a uint16 ID.
 *
 * Strings are stored in insertion order so IDs are stable once assigned.
 * Pool capacity tops out at 65534 entries (0xFFFF is reserved for null).
 */
typedef struct {
    char   **strings;
    int      count;
    int      cap;
} StrPool;

/* Initialize / release */
void sp_init(StrPool *p);
void sp_free(StrPool *p);

/*
 * Intern a string. Returns its uint16 pool ID.
 * Returns YALF_NULL_POOL (0xFFFF) for NULL or empty string.
 * If the string is already present, returns its existing ID without adding.
 */
uint16_t sp_intern(StrPool *p, const char *s);

/*
 * Look up a string without adding it. Returns YALF_NULL_POOL if not found
 * or if s is NULL / empty.
 */
uint16_t sp_lookup(const StrPool *p, const char *s);

/*
 * Serialize the pool into buf (ready to embed in the .byalf file):
 *   uint16  pool_size
 *   for each string: uint16 byte_length + UTF-8 bytes
 */
void sp_serialize(const StrPool *p, Buf *b);

#endif /* STRPOOL_H */
