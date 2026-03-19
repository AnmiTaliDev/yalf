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

#ifndef PARSER_H
#define PARSER_H

#include "yalf.h"

/*
 * Parse a YALF text file (.yalf) into an in-memory Document.
 *
 * Returns a heap-allocated Document on success, or NULL on I/O or fatal
 * parse error. The caller is responsible for calling doc_free().
 *
 * Fields not present in the file are left as NULL / 0 / YALF_NULL_ID.
 * Unknown keys are silently ignored so future extensions don't break old
 * compilers.
 *
 * Only kk/ru/en are captured for examples; de/fr/tr etc. are skipped.
 * Only translation_ru/translation_en are captured for definitions.
 */
Document *parse_yalf(const char *path);

/* Release all memory owned by doc (including the Document itself). */
void doc_free(Document *doc);

/*
 * Append all entries from src into dst.
 *
 * Entries are copied by value (strings are strdup'd), so src may be freed
 * afterwards without affecting dst.  Duplicate IDs are allowed here; the
 * compiler will sort by ID and the caller is responsible for avoiding
 * collisions (a warning is printed unless g_opts.quiet is set).
 */
void doc_merge(Document *dst, const Document *src);

#endif /* PARSER_H */
