# YALF Converters

Tools for converting YALF (YALF Ain't Localisation Format) dictionary files to other formats.

## Available Converters

### yalf2sqlite - YALF to SQLite Converter

Converts YALF dictionary files to SQLite database format for efficient querying and integration with applications.

#### Features

- Converts YALF YAML files to normalized SQLite database
- Supports flexible writing systems (unlimited variants)
- Handles multiple translations per definition
- Preserves all YALF features: etymology, history, examples, synonyms
- Creates indexes for fast queries
- Available in Python and Go

## Python Implementation

Located in `python/` directory.

### Requirements

- Python 3.7+
- PyYAML

### Installation

```bash
cd python
pip install -r requirements.txt
```

### Usage

```bash
cd python

# Basic usage
python yalf2sqlite.py input.yalf output.db

# Overwrite existing database
python yalf2sqlite.py input.yalf output.db --overwrite

# Show help
python yalf2sqlite.py --help

# Show version
python yalf2sqlite.py --version
```

### Examples

```bash
cd python

# Convert a dictionary file
python yalf2sqlite.py kazakh_dict.yalf kazakh.db

# Overwrite existing database
python yalf2sqlite.py updated_dict.yalf dictionary.db -f
```

## Go Implementation

Located in `go/` directory.

### Requirements

- Go 1.21+
- gcc (for SQLite CGO)

### Installation

```bash
cd go
go mod download
```

### Building

```bash
cd go

# Build for current platform
go build -o yalf2sqlite yalf2sqlite.go

# Build for Linux
GOOS=linux GOARCH=amd64 go build -o yalf2sqlite-linux yalf2sqlite.go

# Build for Windows
GOOS=windows GOARCH=amd64 go build -o yalf2sqlite.exe yalf2sqlite.go

# Build for macOS
GOOS=darwin GOARCH=amd64 go build -o yalf2sqlite-macos yalf2sqlite.go
```

### Usage

```bash
cd go

# Basic usage
./yalf2sqlite -i input.yalf -o output.db

# Overwrite existing database
./yalf2sqlite -i input.yalf -o output.db -f

# Show help
./yalf2sqlite -h

# Show version
./yalf2sqlite -version
```

### Examples

```bash
cd go

# Convert a dictionary file
./yalf2sqlite -i kazakh_dict.yalf -o kazakh.db

# Overwrite existing database
./yalf2sqlite -i updated_dict.yalf -o dictionary.db --overwrite
```

## Database Schema

The converter creates the following tables:

### entries
Main entries table with word information.

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key, unique entry ID |
| word | TEXT | Word in original script |
| parent_id | INTEGER | Parent entry ID (for derived words) |
| type | TEXT | Part of speech |
| transcription | TEXT | Phonetic transcription |
| root_word | TEXT | Root form of the word |
| etymology | TEXT | Brief etymology |
| history | TEXT | Detailed etymological history |

### writing_systems
Writing system representations (flexible key-value).

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| entry_id | INTEGER | Reference to entry |
| system_name | TEXT | Writing system identifier |
| representation | TEXT | Word in this writing system |

### definitions
Word definitions/meanings.

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| entry_id | INTEGER | Reference to entry |
| meaning | TEXT | Definition in source language |
| position | INTEGER | Order of definition |

### translations
Translations for definitions (flexible for any language).

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| definition_id | INTEGER | Reference to definition |
| language_code | TEXT | Language code (en, ru, etc.) |
| translation | TEXT | Translated definition |

### examples
Usage examples for definitions.

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| definition_id | INTEGER | Reference to definition |
| position | INTEGER | Order of example |

### example_translations
Translations of examples in multiple languages.

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| example_id | INTEGER | Reference to example |
| language_code | TEXT | Language code |
| text | TEXT | Example text in this language |

### synonyms
Synonym relationships.

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Primary key |
| entry_id | INTEGER | Reference to entry |
| word_id | INTEGER | Reference to synonym entry (if exists) |
| word | TEXT | Synonym word |
| note | TEXT | Usage note |

## Query Examples

Once converted to SQLite, you can query the database:

```sql
-- Find all nouns
SELECT word FROM entries WHERE type = 'noun';

-- Find entries with specific writing system
SELECT e.word, ws.representation 
FROM entries e 
JOIN writing_systems ws ON e.id = ws.entry_id 
WHERE ws.system_name = 'latin_2021';

-- Find words with English translation containing "human"
SELECT DISTINCT e.word 
FROM entries e
JOIN definitions d ON e.id = d.entry_id
JOIN translations t ON d.id = t.definition_id
WHERE t.language_code = 'en' AND t.translation LIKE '%human%';

-- Find all examples for a word
SELECT et.language_code, et.text
FROM entries e
JOIN definitions d ON e.id = d.entry_id
JOIN examples ex ON d.id = ex.definition_id
JOIN example_translations et ON ex.id = et.example_id
WHERE e.word = 'адам'
ORDER BY ex.position, et.language_code;

-- Find synonyms of a word
SELECT s.word, s.note
FROM entries e
JOIN synonyms s ON e.id = s.entry_id
WHERE e.word = 'адам';
```

## Performance

Both implementations:
- Create indexes on frequently queried columns
- Use transactions for batch inserts
- Normalize data to reduce redundancy
- Support databases with thousands of entries

## License

MIT License

**Author:** AnmiTaliDev <anmitalidev@nuros.org>

## Contributing

These converters are part of the YALF format project. See the main project documentation for more information about the YALF format.
