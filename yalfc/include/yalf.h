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

#ifndef YALF_H
#define YALF_H

#include <stdint.h>

/* Sentinel values */
#define YALF_NULL_ID    0xFFFFFFFFu
#define YALF_NULL_POOL  0xFFFFu

/* Array caps */
#define MAX_WS   16
#define MAX_DEFS 64
#define MAX_EX   32
#define MAX_SYNS 32

typedef struct {
    char *name;  /* writing system name, e.g. "latin_2017" — interned in string pool */
    char *text;  /* actual text, e.g. "adam" — stored inline with uint8 length */
} WritingSystem;

typedef struct {
    char *kk;
    char *ru;
    char *en;
} Example;

typedef struct {
    char      *meaning;
    char      *translation_ru;
    char      *translation_en;
    int        ex_count;
    Example    examples[MAX_EX];
} Definition;

typedef struct {
    uint32_t  word_id;  /* YALF_NULL_ID if null */
    char     *word;     /* stored inline with uint8 length */
    char     *note;     /* interned in string pool; NULL if absent */
} Synonym;

typedef struct {
    uint32_t      id;
    uint32_t      parent_id;     /* YALF_NULL_ID if null */
    char         *type;          /* interned in string pool */
    char         *word;
    char         *transcription;
    int           ws_count;
    WritingSystem writing_systems[MAX_WS];
    char         *etymology;     /* stored inline with uint8 length */
    char         *root_word;     /* stored inline with uint8 length */
    char         *history;       /* stored inline with uint8 length */
    int           def_count;
    Definition    definitions[MAX_DEFS];
    int           syn_count;
    Synonym       synonyms[MAX_SYNS];
} Entry;

typedef struct {
    Entry *entries;
    int    count;
    int    capacity;
} Document;

#endif /* YALF_H */
