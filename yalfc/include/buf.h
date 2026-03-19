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

#ifndef BUF_H
#define BUF_H

#include <stddef.h>
#include <stdint.h>

/*
 * Growable byte buffer. All binary output is written here first,
 * then flushed to disk — this lets us compute section offsets before writing.
 */
typedef struct {
    uint8_t *data;
    size_t   len;
    size_t   cap;
} Buf;

/* Initialize / release */
void buf_init(Buf *b);
void buf_free(Buf *b);

/* Append primitives (little-endian) */
void buf_u8(Buf *b, uint8_t v);
void buf_u16(Buf *b, uint16_t v);
void buf_u32(Buf *b, uint32_t v);
void buf_bytes(Buf *b, const void *data, size_t n);

/*
 * Append length-prefixed strings.
 *   buf_str8  — uint8 length prefix  (max 255 bytes; truncates safely at UTF-8 boundary)
 *   buf_str16 — uint16 length prefix (max 65535 bytes)
 * NULL or empty string writes length = 0 only.
 * ctx is used in the truncation warning message (may be NULL).
 */
void buf_str8(Buf *b, const char *s, const char *ctx);
void buf_str16(Buf *b, const char *s);

/* Write buffer contents to an open FILE. Returns 0 on success, -1 on error. */
int buf_write_file(const Buf *b, const char *path);

#endif /* BUF_H */
