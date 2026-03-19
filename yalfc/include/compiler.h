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

#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>
#include "yalf.h"

/*
 * Statistics returned by compile_yalf() on success.
 * Printed to stdout when g_opts.verbose is set.
 */
typedef struct {
    int    entries;      /* number of word entries written       */
    int    pool_strings; /* unique strings in the string pool    */
    size_t file_bytes;   /* total output file size in bytes      */
} CompileStats;

/*
 * Compile a parsed Document into a binary .byalf file at out_path.
 *
 * On success returns 0 and (if stats != NULL) fills *stats.
 * On error returns -1; *stats is undefined.
 *
 * Output layout (all integers little-endian):
 *
 *   [Header 24 bytes]
 *     "YALF"  magic
 *     uint16  version = 1
 *     uint32  entry_count
 *     uint32  string_pool_offset  (= 24)
 *     uint32  index_offset
 *     uint32  data_offset
 *     00 00   reserved
 *
 *   [String Pool]
 *     uint16  pool_size
 *     for each: uint16 byte_len + UTF-8 bytes
 *
 *   [Index Table]  entry_count × uint32 absolute offsets, sorted by entry ID
 *
 *   [Data Block]   packed word entries in ID order
 *     Per entry:
 *       uint32  word_id
 *       uint32  parent_id         (0xFFFFFFFF = null)
 *       uint16  type_pool_id      (0xFFFF = null)
 *       uint8   word_len  + bytes
 *       uint8   trans_len + bytes
 *       uint8   ws_count
 *         for each ws: uint16 name_pool_id + uint8 text_len + bytes
 *       uint8   etymology_len  + bytes  (max 255; 0 = absent)
 *       uint8   root_word_len  + bytes  (max 255; 0 = absent)
 *       uint8   history_len    + bytes  (max 255; 0 = absent)
 *       uint8   def_count
 *         for each def:
 *           uint16 meaning_len  + bytes
 *           uint16 trans_ru_len + bytes
 *           uint16 trans_en_len + bytes
 *           uint8  ex_count
 *             for each ex: uint16 kk_len + bytes
 *                          uint16 ru_len + bytes
 *                          uint16 en_len + bytes
 *       uint8   syn_count
 *         for each syn:
 *           uint32 word_id       (0xFFFFFFFF = null)
 *           uint8  word_len + bytes
 *           uint16 note_pool_id  (0xFFFF = null)
 */
int compile_yalf(const Document *doc, const char *out_path, CompileStats *stats);

#endif /* COMPILER_H */
