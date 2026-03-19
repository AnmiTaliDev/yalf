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
 * Line-by-line YAML-subset parser for .yalf files.
 *
 * Indentation convention used by .yalf:
 *   0  "- id: N"              → new top-level entry
 *   2  "  key: value"         → entry field / section start
 *   4  "    key: value"       → writing-system field
 *   4  "    - key: value"     → new definition or synonym (list item)
 *   6  "      key: value"     → definition/synonym field, or "examples:"
 *   8  "        - key: value" → new example (list item)
 *  10  "          key: value" → example field (kk / ru / en)
 *
 * Only kk/ru/en are captured for examples; other languages are skipped.
 * Only translation_ru/translation_en are captured for definitions.
 * Unknown keys are silently ignored.
 */

#include "parser.h"
#include "opts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF 8192

static void rtrim(char *s)
{
    size_t n = strlen(s);
    while (n && ((unsigned char)s[n - 1] <= ' '))
        s[--n] = '\0';
}

static int line_indent(const char *s)
{
    int n = 0;
    while (s[n] == ' ') n++;
    return n;
}

/* Is there a "- " list marker right after the indent? */
static int line_is_list(const char *s)
{
    int i = line_indent(s);
    return s[i] == '-' && (s[i + 1] == ' ' || s[i + 1] == '\0');
}

/*
 * Unquote a value string.
 * "null"  → NULL
 * ""      → NULL
 * '"..."' → content between quotes (first and last '"')
 * other   → strdup as-is
 */
static char *unquote(const char *s)
{
    while (*s == ' ') s++;
    if (!*s || strcmp(s, "null") == 0)
        return NULL;
    if (*s == '"') {
        s++;
        size_t len = strlen(s);
        if (len > 0 && s[len - 1] == '"')
            len--;
        char *r = malloc(len + 1);
        memcpy(r, s, len);
        r[len] = '\0';
        return r;
    }
    return strdup(s);
}

/*
 * Parse "  [- ]key: value" from a line.
 * key_buf receives the key (NUL-terminated, trailing spaces stripped).
 * *val receives a heap-allocated value string (may be NULL for null/empty).
 * Returns 1 on success, 0 if the line has no colon.
 */
static int parse_kv(const char *line, char *key_buf, size_t kbsz, char **val)
{
    const char *p = line;
    while (*p == ' ') p++;
    if (*p == '-' && p[1] == ' ') p += 2;

    const char *colon = strchr(p, ':');
    if (!colon) return 0;

    size_t klen = (size_t)(colon - p);
    if (klen == 0 || klen >= kbsz) return 0;
    memcpy(key_buf, p, klen);
    key_buf[klen] = '\0';
    /* strip trailing spaces from key */
    while (klen > 0 && key_buf[klen - 1] == ' ')
        key_buf[--klen] = '\0';

    const char *v = colon + 1;
    if (*v == ' ') v++;
    *val = unquote(v);
    return 1;
}

static void entry_init(Entry *e)
{
    memset(e, 0, sizeof(*e));
    e->parent_id = YALF_NULL_ID;
    for (int i = 0; i < MAX_SYNS; i++)
        e->synonyms[i].word_id = YALF_NULL_ID;
}

static Entry *doc_add_entry(Document *doc)
{
    if (doc->count >= doc->capacity) {
        doc->capacity = doc->capacity ? doc->capacity * 2 : 64;
        doc->entries  = realloc(doc->entries,
                                (size_t)doc->capacity * sizeof(Entry));
        if (!doc->entries) {
            fprintf(stderr, "yalfc: out of memory\n");
            exit(1);
        }
    }
    Entry *e = &doc->entries[doc->count++];
    entry_init(e);
    return e;
}

static void free_entry(Entry *e)
{
    free(e->type);
    free(e->word);
    free(e->transcription);
    free(e->etymology);
    free(e->root_word);
    free(e->history);
    for (int i = 0; i < e->ws_count; i++) {
        free(e->writing_systems[i].name);
        free(e->writing_systems[i].text);
    }
    for (int i = 0; i < e->def_count; i++) {
        Definition *d = &e->definitions[i];
        free(d->meaning);
        free(d->translation_ru);
        free(d->translation_en);
        for (int j = 0; j < d->ex_count; j++) {
            free(d->examples[j].kk);
            free(d->examples[j].ru);
            free(d->examples[j].en);
        }
    }
    for (int i = 0; i < e->syn_count; i++) {
        free(e->synonyms[i].word);
        free(e->synonyms[i].note);
    }
}

void doc_free(Document *doc)
{
    if (!doc) return;
    for (int i = 0; i < doc->count; i++)
        free_entry(&doc->entries[i]);
    free(doc->entries);
    free(doc);
}

static char *dup(const char *s) { return s ? strdup(s) : NULL; }

static Entry entry_copy(const Entry *src)
{
    Entry dst;
    dst.id          = src->id;
    dst.parent_id   = src->parent_id;
    dst.type        = dup(src->type);
    dst.word        = dup(src->word);
    dst.transcription = dup(src->transcription);
    dst.etymology   = dup(src->etymology);
    dst.root_word   = dup(src->root_word);
    dst.history     = dup(src->history);

    dst.ws_count = src->ws_count;
    for (int i = 0; i < src->ws_count; i++) {
        dst.writing_systems[i].name = dup(src->writing_systems[i].name);
        dst.writing_systems[i].text = dup(src->writing_systems[i].text);
    }

    dst.def_count = src->def_count;
    for (int i = 0; i < src->def_count; i++) {
        const Definition *sd = &src->definitions[i];
        Definition       *dd = &dst.definitions[i];
        dd->meaning        = dup(sd->meaning);
        dd->translation_ru = dup(sd->translation_ru);
        dd->translation_en = dup(sd->translation_en);
        dd->ex_count       = sd->ex_count;
        for (int j = 0; j < sd->ex_count; j++) {
            dd->examples[j].kk = dup(sd->examples[j].kk);
            dd->examples[j].ru = dup(sd->examples[j].ru);
            dd->examples[j].en = dup(sd->examples[j].en);
        }
    }

    dst.syn_count = src->syn_count;
    for (int i = 0; i < src->syn_count; i++) {
        dst.synonyms[i].word_id = src->synonyms[i].word_id;
        dst.synonyms[i].word    = dup(src->synonyms[i].word);
        dst.synonyms[i].note    = dup(src->synonyms[i].note);
    }

    return dst;
}

/*
 * Look up new_id for old_id in the remap table.
 * Returns YALF_NULL_ID if old_id is not found (external or null reference).
 */
static uint32_t remap_id(const uint32_t *old_ids, const uint32_t *new_ids,
                          int n, uint32_t old_id)
{
    if (old_id == YALF_NULL_ID) return YALF_NULL_ID;
    for (int i = 0; i < n; i++)
        if (old_ids[i] == old_id) return new_ids[i];
    return old_id; /* not in this file — leave unchanged */
}

void doc_merge(Document *dst, const Document *src)
{
    if (src->count == 0) return;

    /* Find the highest ID currently in dst to know where to start. */
    uint32_t next_id = 0;
    for (int i = 0; i < dst->count; i++)
        if (dst->entries[i].id != YALF_NULL_ID && dst->entries[i].id > next_id)
            next_id = dst->entries[i].id;
    next_id++;  /* first new ID = max_existing + 1, or 1 if dst is empty */

    /* Build old→new ID remap table for this src batch. */
    uint32_t *old_ids = malloc((size_t)src->count * sizeof(uint32_t));
    uint32_t *new_ids = malloc((size_t)src->count * sizeof(uint32_t));
    if (!old_ids || !new_ids) { fprintf(stderr, "yalfc: out of memory\n"); exit(1); }

    for (int i = 0; i < src->count; i++) {
        old_ids[i] = src->entries[i].id;
        new_ids[i] = next_id++;
    }

    /* Grow dst if needed. */
    while (dst->count + src->count > dst->capacity) {
        dst->capacity = dst->capacity ? dst->capacity * 2 : 64;
        dst->entries  = realloc(dst->entries,
                                (size_t)dst->capacity * sizeof(Entry));
        if (!dst->entries) { fprintf(stderr, "yalfc: out of memory\n"); exit(1); }
    }

    /* Deep-copy each entry and apply the ID remap. */
    for (int i = 0; i < src->count; i++) {
        Entry e = entry_copy(&src->entries[i]);

        e.id        = new_ids[i];
        e.parent_id = remap_id(old_ids, new_ids, src->count, e.parent_id);

        for (int j = 0; j < e.syn_count; j++)
            e.synonyms[j].word_id = remap_id(old_ids, new_ids, src->count,
                                             e.synonyms[j].word_id);

        dst->entries[dst->count++] = e;
    }

    free(old_ids);
    free(new_ids);
}

/*
 * Active section inside an entry (set when we encounter a section-header key).
 */
typedef enum { SEC_NONE, SEC_WS, SEC_DEFS, SEC_SYNS } Section;

Document *parse_yalf(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        return NULL;
    }

    Document *doc = calloc(1, sizeof(Document));

    char     line[LINE_BUF];
    Entry   *cur     = NULL;
    Section  sec     = SEC_NONE;
    int      in_ex   = 0;   /* are we inside an "examples:" block? */

    while (fgets(line, LINE_BUF, f)) {
        rtrim(line);

        /* skip blank lines and comments */
        const char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0' || *p == '#') continue;

        int  ind = line_indent(line);
        int  lst = line_is_list(line);
        char key[256];
        char *val = NULL;
        int  kv   = parse_kv(line, key, sizeof(key), &val);

        if (ind == 0 && lst && kv && strcmp(key, "id") == 0) {
            cur    = doc_add_entry(doc);
            cur->id = val ? (uint32_t)atoi(val) : 0;
            free(val);
            sec  = SEC_NONE;
            in_ex = 0;
            continue;
        }

        if (!cur) { free(val); continue; }

        if (ind == 2 && kv && !lst) {
            /* entering a new section resets example tracking */
            if (strcmp(key, "writing_systems") == 0) {
                sec = SEC_WS; in_ex = 0;
            } else if (strcmp(key, "definitions") == 0) {
                sec = SEC_DEFS; in_ex = 0;
            } else if (strcmp(key, "synonyms") == 0) {
                sec = SEC_SYNS; in_ex = 0;
            } else {
                /* plain entry fields — reset section context */
                sec = SEC_NONE; in_ex = 0;
                if (strcmp(key, "word") == 0) {
                    free(cur->word); cur->word = val; val = NULL;
                } else if (strcmp(key, "parent_id") == 0) {
                    cur->parent_id = val ? (uint32_t)atoi(val) : YALF_NULL_ID;
                } else if (strcmp(key, "type") == 0) {
                    free(cur->type); cur->type = val; val = NULL;
                } else if (strcmp(key, "transcription") == 0) {
                    free(cur->transcription); cur->transcription = val; val = NULL;
                } else if (strcmp(key, "etymology") == 0) {
                    free(cur->etymology); cur->etymology = val; val = NULL;
                } else if (strcmp(key, "root_word") == 0) {
                    free(cur->root_word); cur->root_word = val; val = NULL;
                } else if (strcmp(key, "history") == 0) {
                    free(cur->history); cur->history = val; val = NULL;
                }
                /* unknown keys silently ignored */
            }
            free(val);
            continue;
        }

        if (ind == 4) {
            if (sec == SEC_WS && kv && !lst) {
                /* writing_systems: key: "text" */
                if (cur->ws_count < MAX_WS) {
                    WritingSystem *ws = &cur->writing_systems[cur->ws_count++];
                    ws->name = strdup(key);
                    ws->text = val; val = NULL;
                }
            } else if (sec == SEC_DEFS && lst && kv) {
                /* "    - meaning: ..." — start of a new definition */
                if (cur->def_count < MAX_DEFS) {
                    Definition *d = &cur->definitions[cur->def_count++];
                    memset(d, 0, sizeof(*d));
                    if (strcmp(key, "meaning") == 0) {
                        d->meaning = val; val = NULL;
                    }
                    /* other first-line fields handled at ind=6 */
                }
                in_ex = 0;
            } else if (sec == SEC_SYNS && lst && kv) {
                /* "    - word_id: ..." — start of a new synonym */
                if (cur->syn_count < MAX_SYNS) {
                    Synonym *s = &cur->synonyms[cur->syn_count++];
                    s->word_id = YALF_NULL_ID;
                    s->word    = NULL;
                    s->note    = NULL;
                    if (strcmp(key, "word_id") == 0) {
                        s->word_id = val ? (uint32_t)atoi(val) : YALF_NULL_ID;
                    } else if (strcmp(key, "word") == 0) {
                        s->word = val; val = NULL;
                    } else if (strcmp(key, "note") == 0) {
                        s->note = val; val = NULL;
                    }
                }
                in_ex = 0;
            }
            free(val);
            continue;
        }

        if (ind == 6 && kv && !lst) {
            if (sec == SEC_DEFS && cur->def_count > 0) {
                Definition *d = &cur->definitions[cur->def_count - 1];
                if (strcmp(key, "meaning") == 0) {
                    free(d->meaning); d->meaning = val; val = NULL;
                } else if (strcmp(key, "translation_ru") == 0) {
                    free(d->translation_ru); d->translation_ru = val; val = NULL;
                } else if (strcmp(key, "translation_en") == 0) {
                    free(d->translation_en); d->translation_en = val; val = NULL;
                } else if (strcmp(key, "examples") == 0) {
                    in_ex = 1;
                }
                /* translation_de / _fr / _tr etc. are silently dropped */
            } else if (sec == SEC_SYNS && cur->syn_count > 0) {
                Synonym *s = &cur->synonyms[cur->syn_count - 1];
                if (strcmp(key, "word_id") == 0) {
                    s->word_id = val ? (uint32_t)atoi(val) : YALF_NULL_ID;
                } else if (strcmp(key, "word") == 0) {
                    free(s->word); s->word = val; val = NULL;
                } else if (strcmp(key, "note") == 0) {
                    free(s->note); s->note = val; val = NULL;
                }
            }
            free(val);
            continue;
        }

        if (ind == 8 && lst && kv && in_ex && sec == SEC_DEFS) {
            if (cur->def_count > 0) {
                Definition *d = &cur->definitions[cur->def_count - 1];
                if (d->ex_count < MAX_EX) {
                    Example *ex = &d->examples[d->ex_count++];
                    ex->kk = ex->ru = ex->en = NULL;
                    if (strcmp(key, "kk") == 0) { ex->kk = val; val = NULL; }
                    else if (strcmp(key, "ru") == 0) { ex->ru = val; val = NULL; }
                    else if (strcmp(key, "en") == 0) { ex->en = val; val = NULL; }
                }
            }
            free(val);
            continue;
        }

        if (ind == 10 && kv && !lst && in_ex && sec == SEC_DEFS) {
            if (cur->def_count > 0) {
                Definition *d = &cur->definitions[cur->def_count - 1];
                if (d->ex_count > 0) {
                    Example *ex = &d->examples[d->ex_count - 1];
                    if (strcmp(key, "kk") == 0 && !ex->kk) {
                        ex->kk = val; val = NULL;
                    } else if (strcmp(key, "ru") == 0 && !ex->ru) {
                        ex->ru = val; val = NULL;
                    } else if (strcmp(key, "en") == 0 && !ex->en) {
                        ex->en = val; val = NULL;
                    }
                    /* de / fr / tr etc. silently dropped */
                }
            }
            free(val);
            continue;
        }

        free(val);
    }

    fclose(f);
    return doc;
}
