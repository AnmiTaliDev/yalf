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

/*
 * Two-pass binary compiler:
 *
 *   Pass 1 — build the string pool (intern type, writing-system names,
 *             synonym notes) and serialize each entry into its own Buf so
 *             we know every entry's byte size before writing anything.
 *
 *   Pass 2 — compute section offsets, write the 24-byte header, then
 *             dump string-pool buf, index-table, and entry bufs in order.
 */

#include "compiler.h"
#include "buf.h"
#include "strpool.h"
#include "opts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmp_entry_id(const void *a, const void *b)
{
    const Entry *ea = (const Entry *)a;
    const Entry *eb = (const Entry *)b;
    if (ea->id < eb->id) return -1;
    if (ea->id > eb->id) return  1;
    return 0;
}

static void populate_pool(const Document *doc, StrPool *pool)
{
    for (int i = 0; i < doc->count; i++) {
        const Entry *e = &doc->entries[i];
        sp_intern(pool, e->type);
        for (int j = 0; j < e->ws_count; j++)
            sp_intern(pool, e->writing_systems[j].name);
        for (int j = 0; j < e->syn_count; j++)
            sp_intern(pool, e->synonyms[j].note);
    }
}

static void serialize_entry(const Entry *e, const StrPool *pool, Buf *b)
{
    /* 6.1 — word metadata */
    buf_u32(b, e->id);
    buf_u32(b, e->parent_id);
    buf_u16(b, sp_lookup(pool, e->type));
    buf_str8(b, e->word, "word");
    buf_str8(b, e->transcription, "transcription");

    /* 6.2 — writing systems */
    buf_u8(b, (uint8_t)e->ws_count);
    for (int i = 0; i < e->ws_count; i++) {
        buf_u16(b, sp_lookup(pool, e->writing_systems[i].name));
        buf_str8(b, e->writing_systems[i].text, "writing_system.text");
    }

    /* after 6.2 — etymology, root_word, history (uint8 length each) */
    buf_str8(b, e->etymology,  "etymology");
    buf_str8(b, e->root_word,  "root_word");
    buf_str8(b, e->history,    "history");

    /* 6.3 — definitions */
    buf_u8(b, (uint8_t)e->def_count);
    for (int i = 0; i < e->def_count; i++) {
        const Definition *d = &e->definitions[i];
        buf_str16(b, d->meaning);
        buf_str16(b, d->translation_ru);
        buf_str16(b, d->translation_en);

        buf_u8(b, (uint8_t)d->ex_count);
        for (int j = 0; j < d->ex_count; j++) {
            const Example *ex = &d->examples[j];
            buf_str16(b, ex->kk);
            buf_str16(b, ex->ru);
            buf_str16(b, ex->en);
        }
    }

    /* 6.4 — synonyms */
    buf_u8(b, (uint8_t)e->syn_count);
    for (int i = 0; i < e->syn_count; i++) {
        const Synonym *s = &e->synonyms[i];
        buf_u32(b, s->word_id);
        buf_str8(b, s->word, "synonym.word");
        buf_u16(b, sp_lookup(pool, s->note));
    }
}

int compile_yalf(const Document *doc, const char *out_path, CompileStats *stats)
{
    if (!doc || doc->count == 0) {
        fprintf(stderr, "yalfc: nothing to compile\n");
        return -1;
    }

    /* Sort entries by ID so the index table is in ascending order. */
    qsort(doc->entries, (size_t)doc->count, sizeof(Entry), cmp_entry_id);

    /* Pass 1a — build string pool. */
    StrPool pool;
    sp_init(&pool);
    populate_pool(doc, &pool);

    /* Pass 1b — serialize string pool into its own buffer. */
    Buf pool_buf;
    buf_init(&pool_buf);
    sp_serialize(&pool, &pool_buf);

    /* Pass 1c — serialize each entry into its own buffer. */
    Buf *entry_bufs = calloc((size_t)doc->count, sizeof(Buf));
    if (!entry_bufs) {
        fprintf(stderr, "yalfc: out of memory\n");
        return -1;
    }
    for (int i = 0; i < doc->count; i++) {
        buf_init(&entry_bufs[i]);
        serialize_entry(&doc->entries[i], &pool, &entry_bufs[i]);
    }

    /*
     * Pass 2 — compute offsets.
     *
     *   string_pool_offset = 24                      (header size)
     *   index_offset       = 24 + pool_buf.len
     *   data_offset        = index_offset + entry_count * 4
     */
    uint32_t string_pool_offset = 24;
    uint32_t index_offset       = string_pool_offset + (uint32_t)pool_buf.len;
    uint32_t data_offset        = index_offset + (uint32_t)doc->count * 4;

    /* Build index table (one uint32 per entry = absolute offset from file start). */
    Buf index_buf;
    buf_init(&index_buf);
    uint32_t running = data_offset;
    for (int i = 0; i < doc->count; i++) {
        buf_u32(&index_buf, running);
        running += (uint32_t)entry_bufs[i].len;
    }

    /* Assemble complete file in one buffer. */
    Buf file_buf;
    buf_init(&file_buf);

    /* Header (24 bytes) */
    buf_bytes(&file_buf, "YALF", 4);           /* magic            */
    buf_u16(&file_buf, 1);                     /* version          */
    buf_u32(&file_buf, (uint32_t)doc->count);  /* entry_count      */
    buf_u32(&file_buf, string_pool_offset);    /* string_pool_offset */
    buf_u32(&file_buf, index_offset);          /* index_offset     */
    buf_u32(&file_buf, data_offset);           /* data_offset      */
    buf_u16(&file_buf, 0);                     /* reserved         */

    /* String pool */
    buf_bytes(&file_buf, pool_buf.data, pool_buf.len);

    /* Index table */
    buf_bytes(&file_buf, index_buf.data, index_buf.len);

    /* Data block */
    for (int i = 0; i < doc->count; i++)
        buf_bytes(&file_buf, entry_bufs[i].data, entry_bufs[i].len);

    /* Capture stats before any cleanup (buffers still valid). */
    if (stats) {
        stats->entries      = doc->count;
        stats->pool_strings = pool.count;
        stats->file_bytes   = file_buf.len;
    }

    /* Write to disk. */
    int rc = buf_write_file(&file_buf, out_path);

    /* Cleanup. */
    buf_free(&file_buf);
    buf_free(&index_buf);
    buf_free(&pool_buf);
    for (int i = 0; i < doc->count; i++)
        buf_free(&entry_bufs[i]);
    free(entry_bufs);
    sp_free(&pool);

    return rc;
}
