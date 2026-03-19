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

#include "opts.h"
#include "parser.h"
#include "compiler.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YALFC_VERSION "1.0.0"

/* Global options — read by buf.c, compiler.c etc. */
YalfcOpts g_opts = { 0, 0 };

static void print_help(const char *prog)
{
    printf(
        "Usage: %s [OPTIONS] <input.yalf> [input2.yalf ...]\n"
        "\n"
        "Compile YALF text dictionaries to the binary .byalf format.\n"
        "\n"
        "Options:\n"
        "  -o, --output <file>   Merge all inputs into a single output file\n"
        "  -v, --verbose         Print compilation stats after each file\n"
        "  -q, --quiet           Suppress warnings (e.g. field truncation)\n"
        "  -h, --help            Show this help and exit\n"
        "      --version         Show version and exit\n"
        "\n"
        "Without -o each input is compiled to its own .byalf:\n"
        "  dict.yalf  →  dict.byalf\n"
        "\n"
        "With -o all inputs are merged into one output file.\n"
        "\n"
        "Examples:\n"
        "  %s dict.yalf\n"
        "  %s -v *.yalf\n"
        "  %s -o combined.byalf a.yalf b.yalf c.yalf\n"
        "  %s -vq -o /tmp/dict.byalf dict.yalf\n",
        prog, prog, prog, prog, prog);
}

static void print_version(void)
{
    printf("yalfc %s\n"
           "Copyright (C) 2026 AnmiTaliDev <anmitalidev@nuros.org>\n"
           "License: GNU GPL v3 or later <https://www.gnu.org/licenses/gpl-3.0.html>\n",
           YALFC_VERSION);
}

/*
 * Replace the ".yalf" extension with ".byalf", or append ".byalf" if no
 * ".yalf" suffix is present.  Caller must free() the result.
 */
static char *derive_out_path(const char *in)
{
    const char *ext = strrchr(in, '.');
    size_t base_len;

    if (ext && strcmp(ext, ".yalf") == 0)
        base_len = (size_t)(ext - in);
    else
        base_len = strlen(in);

    char *out = malloc(base_len + 7); /* ".byalf\0" */
    memcpy(out, in, base_len);
    memcpy(out + base_len, ".byalf", 7);
    return out;
}

/* Pretty-print byte count: "1.23 KB", "456 B", etc. */
static void fmt_bytes(char *buf, size_t bufsz, size_t n)
{
    if (n >= 1024 * 1024)
        snprintf(buf, bufsz, "%.2f MB", (double)n / (1024.0 * 1024.0));
    else if (n >= 1024)
        snprintf(buf, bufsz, "%.2f KB", (double)n / 1024.0);
    else
        snprintf(buf, bufsz, "%zu B", n);
}

int main(int argc, char *argv[])
{
    const char *out_path  = NULL;   /* -o value */
    int         errors    = 0;

    static const struct option long_opts[] = {
        { "output",  required_argument, NULL, 'o' },
        { "verbose", no_argument,       NULL, 'v' },
        { "quiet",   no_argument,       NULL, 'q' },
        { "help",    no_argument,       NULL, 'h' },
        { "version", no_argument,       NULL,  0  },
        { NULL,      0,                 NULL,  0  },
    };

    int opt, longidx;
    while ((opt = getopt_long(argc, argv, "o:vqh", long_opts, &longidx)) != -1) {
        switch (opt) {
        case 'o':
            out_path = optarg;
            break;
        case 'v':
            g_opts.verbose = 1;
            break;
        case 'q':
            g_opts.quiet = 1;
            break;
        case 'h':
            print_help(argv[0]);
            return 0;
        case 0:
            /* long-only option matched */
            if (strcmp(long_opts[longidx].name, "version") == 0) {
                print_version();
                return 0;
            }
            break;
        default:
            fprintf(stderr, "Try '%s --help' for usage.\n", argv[0]);
            return 1;
        }
    }

    /* Remaining arguments are input files. */
    int n_inputs = argc - optind;

    if (n_inputs == 0) {
        fprintf(stderr, "yalfc: no input files\n"
                        "Try '%s --help' for usage.\n", argv[0]);
        return 1;
    }

    /*
     * Two modes:
     *
     *  a) -o given → merge all inputs into one output file.
     *  b) -o not given → compile each input to its own derived .byalf.
     */
    if (out_path) {
        Document *merged = calloc(1, sizeof(Document));

        for (int i = optind; i < argc; i++) {
            Document *doc = parse_yalf(argv[i]);
            if (!doc) { errors++; continue; }
            doc_merge(merged, doc);
            doc_free(doc);
        }

        if (merged->count == 0 && !errors) {
            fprintf(stderr, "yalfc: no entries found in input files\n");
            doc_free(merged);
            return 1;
        }

        if (!errors || merged->count > 0) {
            CompileStats stats = { 0, 0, 0 };
            if (compile_yalf(merged, out_path, &stats) != 0) {
                errors++;
            } else if (g_opts.verbose) {
                char szstr[32];
                fmt_bytes(szstr, sizeof(szstr), stats.file_bytes);
                /* list all input files in verbose mode */
                for (int i = optind; i < argc; i++)
                    printf("  +  %s\n", argv[i]);
                printf("  →  %s\n"
                       "  entries : %d\n"
                       "  pool    : %d strings\n"
                       "  size    : %s\n",
                       out_path,
                       stats.entries,
                       stats.pool_strings,
                       szstr);
            }
        }

        doc_free(merged);

    } else {
        for (int i = optind; i < argc; i++) {
            const char *in      = argv[i];
            char       *derived = derive_out_path(in);

            Document *doc = parse_yalf(in);
            if (!doc) { errors++; free(derived); continue; }

            CompileStats stats = { 0, 0, 0 };
            int rc = compile_yalf(doc, derived, &stats);
            doc_free(doc);

            if (rc != 0) {
                errors++;
            } else if (g_opts.verbose) {
                char szstr[32];
                fmt_bytes(szstr, sizeof(szstr), stats.file_bytes);
                printf("%s  →  %s\n"
                       "  entries : %d\n"
                       "  pool    : %d strings\n"
                       "  size    : %s\n",
                       in, derived,
                       stats.entries,
                       stats.pool_strings,
                       szstr);
            }

            free(derived);
        }
    }

    return errors ? 1 : 0;
}
