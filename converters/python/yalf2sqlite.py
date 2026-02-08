#!/usr/bin/env python3
"""
YALF to SQLite Converter

Converts YALF (YALF Ain't Localisation Format) dictionary files to SQLite database.

Author: AnmiTaliDev <anmitalidev@nuros.org>
License: MIT
"""

import sqlite3
import yaml
import json
import argparse
import sys
from pathlib import Path
from typing import List, Dict, Any, Optional


class YALFConverter:
    """Converter from YALF format to SQLite database."""

    def __init__(self, db_path: str):
        """Initialize converter with database path."""
        self.db_path = db_path
        self.conn: Optional[sqlite3.Connection] = None

    def create_schema(self):
        """Create database schema for YALF data."""
        cursor = self.conn.cursor()

        # Main entries table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS entries (
                id INTEGER PRIMARY KEY,
                word TEXT NOT NULL,
                parent_id INTEGER,
                type TEXT NOT NULL,
                transcription TEXT,
                root_word TEXT,
                etymology TEXT,
                history TEXT,
                FOREIGN KEY (parent_id) REFERENCES entries(id)
            )
        """)

        # Writing systems table (flexible key-value storage)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS writing_systems (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                entry_id INTEGER NOT NULL,
                system_name TEXT NOT NULL,
                representation TEXT NOT NULL,
                FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE,
                UNIQUE(entry_id, system_name)
            )
        """)

        # Definitions table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS definitions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                entry_id INTEGER NOT NULL,
                meaning TEXT NOT NULL,
                position INTEGER NOT NULL,
                FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE
            )
        """)

        # Translations table (flexible for any language)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS translations (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                definition_id INTEGER NOT NULL,
                language_code TEXT NOT NULL,
                translation TEXT NOT NULL,
                FOREIGN KEY (definition_id) REFERENCES definitions(id) ON DELETE CASCADE
            )
        """)

        # Examples table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS examples (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                definition_id INTEGER NOT NULL,
                position INTEGER NOT NULL,
                FOREIGN KEY (definition_id) REFERENCES definitions(id) ON DELETE CASCADE
            )
        """)

        # Example translations table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS example_translations (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                example_id INTEGER NOT NULL,
                language_code TEXT NOT NULL,
                text TEXT NOT NULL,
                FOREIGN KEY (example_id) REFERENCES examples(id) ON DELETE CASCADE
            )
        """)

        # Synonyms table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS synonyms (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                entry_id INTEGER NOT NULL,
                word_id INTEGER,
                word TEXT NOT NULL,
                note TEXT,
                FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE,
                FOREIGN KEY (word_id) REFERENCES entries(id)
            )
        """)

        # Create indexes for better query performance
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_entries_word ON entries(word)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_entries_type ON entries(type)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_entries_parent ON entries(parent_id)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_writing_systems_entry ON writing_systems(entry_id)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_definitions_entry ON definitions(entry_id)")
        cursor.execute("CREATE INDEX IF NOT EXISTS idx_synonyms_entry ON synonyms(entry_id)")

        self.conn.commit()

    def insert_entry(self, entry: Dict[str, Any]):
        """Insert a single YALF entry into the database."""
        cursor = self.conn.cursor()

        # Insert main entry
        cursor.execute("""
            INSERT INTO entries (id, word, parent_id, type, transcription, root_word, etymology, history)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            entry['id'],
            entry['word'],
            entry.get('parent_id'),
            entry['type'],
            entry.get('transcription'),
            entry.get('root_word'),
            entry.get('etymology'),
            entry.get('history')
        ))

        entry_id = entry['id']

        # Insert writing systems
        if 'writing_systems' in entry:
            for system_name, representation in entry['writing_systems'].items():
                cursor.execute("""
                    INSERT INTO writing_systems (entry_id, system_name, representation)
                    VALUES (?, ?, ?)
                """, (entry_id, system_name, representation))

        # Insert definitions
        if 'definitions' in entry:
            for pos, definition in enumerate(entry['definitions']):
                cursor.execute("""
                    INSERT INTO definitions (entry_id, meaning, position)
                    VALUES (?, ?, ?)
                """, (entry_id, definition['meaning'], pos))

                definition_id = cursor.lastrowid

                # Insert translations
                for key, value in definition.items():
                    if key.startswith('translation_'):
                        lang_code = key.replace('translation_', '')
                        cursor.execute("""
                            INSERT INTO translations (definition_id, language_code, translation)
                            VALUES (?, ?, ?)
                        """, (definition_id, lang_code, value))

                # Insert examples
                if 'examples' in definition:
                    for ex_pos, example in enumerate(definition['examples']):
                        cursor.execute("""
                            INSERT INTO examples (definition_id, position)
                            VALUES (?, ?)
                        """, (definition_id, ex_pos))

                        example_id = cursor.lastrowid

                        # Insert example translations
                        for lang_code, text in example.items():
                            cursor.execute("""
                                INSERT INTO example_translations (example_id, language_code, text)
                                VALUES (?, ?, ?)
                            """, (example_id, lang_code, text))

        # Insert synonyms
        if 'synonyms' in entry:
            for synonym in entry['synonyms']:
                cursor.execute("""
                    INSERT INTO synonyms (entry_id, word_id, word, note)
                    VALUES (?, ?, ?, ?)
                """, (entry_id, synonym.get('word_id'), synonym['word'], synonym.get('note')))

        self.conn.commit()

    def convert(self, yalf_path: str, overwrite: bool = False):
        """Convert YALF file to SQLite database."""
        # Check if database exists
        if Path(self.db_path).exists() and not overwrite:
            print(f"Error: Database {self.db_path} already exists. Use --overwrite to replace it.")
            return False

        # Remove existing database if overwrite is True
        if overwrite and Path(self.db_path).exists():
            Path(self.db_path).unlink()

        # Read YALF file
        try:
            with open(yalf_path, 'r', encoding='utf-8') as f:
                entries = yaml.safe_load(f)
        except Exception as e:
            print(f"Error reading YALF file: {e}")
            return False

        if not isinstance(entries, list):
            print("Error: YALF file must contain a list of entries")
            return False

        # Connect to database
        try:
            self.conn = sqlite3.connect(self.db_path)
            self.create_schema()
        except Exception as e:
            print(f"Error creating database: {e}")
            return False

        # Insert entries
        try:
            for entry in entries:
                self.insert_entry(entry)
            print(f"Successfully converted {len(entries)} entries to {self.db_path}")
            return True
        except Exception as e:
            print(f"Error inserting entries: {e}")
            self.conn.rollback()
            return False
        finally:
            if self.conn:
                self.conn.close()


def main():
    """Main entry point for the converter."""
    parser = argparse.ArgumentParser(
        description='Convert YALF dictionary files to SQLite database',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s dictionary.yalf output.db
  %(prog)s input.yaml output.db --overwrite
        """
    )

    parser.add_argument('input', help='Input YALF file (.yalf or .yaml)')
    parser.add_argument('output', help='Output SQLite database file (.db or .sqlite)')
    parser.add_argument('--overwrite', '-f', action='store_true',
                       help='Overwrite output database if it exists')
    parser.add_argument('--version', action='version', version='%(prog)s 1.0.0')

    args = parser.parse_args()

    # Check input file exists
    if not Path(args.input).exists():
        print(f"Error: Input file {args.input} not found")
        sys.exit(1)

    # Convert
    converter = YALFConverter(args.output)
    success = converter.convert(args.input, args.overwrite)

    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
