package main

import (
	"database/sql"
	"flag"
	"fmt"
	"os"
	"path/filepath"

	_ "github.com/mattn/go-sqlite3"
	"gopkg.in/yaml.v3"
)

// Entry represents a YALF dictionary entry
type Entry struct {
	ID             int               `yaml:"id"`
	Word           string            `yaml:"word"`
	ParentID       *int              `yaml:"parent_id"`
	Type           string            `yaml:"type"`
	Transcription  string            `yaml:"transcription,omitempty"`
	WritingSystems map[string]string `yaml:"writing_systems,omitempty"`
	RootWord       string            `yaml:"root_word,omitempty"`
	Etymology      string            `yaml:"etymology,omitempty"`
	History        string            `yaml:"history,omitempty"`
	Definitions    []Definition      `yaml:"definitions"`
	Synonyms       []Synonym         `yaml:"synonyms,omitempty"`
}

// Definition represents a word definition with translations
type Definition struct {
	Meaning  string                 `yaml:"meaning"`
	Examples []map[string]string    `yaml:"examples,omitempty"`
	Extra    map[string]interface{} `yaml:",inline"`
}

// Synonym represents a synonym entry
type Synonym struct {
	WordID *int   `yaml:"word_id"`
	Word   string `yaml:"word"`
	Note   string `yaml:"note,omitempty"`
}

// Converter handles YALF to SQLite conversion
type Converter struct {
	db *sql.DB
}

// NewConverter creates a new converter with database connection
func NewConverter(dbPath string) (*Converter, error) {
	db, err := sql.Open("sqlite3", dbPath)
	if err != nil {
		return nil, fmt.Errorf("failed to open database: %w", err)
	}
	return &Converter{db: db}, nil
}

// Close closes the database connection
func (c *Converter) Close() error {
	return c.db.Close()
}

// CreateSchema creates the database schema
func (c *Converter) CreateSchema() error {
	schema := `
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
	);

	CREATE TABLE IF NOT EXISTS writing_systems (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		entry_id INTEGER NOT NULL,
		system_name TEXT NOT NULL,
		representation TEXT NOT NULL,
		FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE,
		UNIQUE(entry_id, system_name)
	);

	CREATE TABLE IF NOT EXISTS definitions (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		entry_id INTEGER NOT NULL,
		meaning TEXT NOT NULL,
		position INTEGER NOT NULL,
		FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE
	);

	CREATE TABLE IF NOT EXISTS translations (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		definition_id INTEGER NOT NULL,
		language_code TEXT NOT NULL,
		translation TEXT NOT NULL,
		FOREIGN KEY (definition_id) REFERENCES definitions(id) ON DELETE CASCADE
	);

	CREATE TABLE IF NOT EXISTS examples (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		definition_id INTEGER NOT NULL,
		position INTEGER NOT NULL,
		FOREIGN KEY (definition_id) REFERENCES definitions(id) ON DELETE CASCADE
	);

	CREATE TABLE IF NOT EXISTS example_translations (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		example_id INTEGER NOT NULL,
		language_code TEXT NOT NULL,
		text TEXT NOT NULL,
		FOREIGN KEY (example_id) REFERENCES examples(id) ON DELETE CASCADE
	);

	CREATE TABLE IF NOT EXISTS synonyms (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		entry_id INTEGER NOT NULL,
		word_id INTEGER,
		word TEXT NOT NULL,
		note TEXT,
		FOREIGN KEY (entry_id) REFERENCES entries(id) ON DELETE CASCADE,
		FOREIGN KEY (word_id) REFERENCES entries(id)
	);

	CREATE INDEX IF NOT EXISTS idx_entries_word ON entries(word);
	CREATE INDEX IF NOT EXISTS idx_entries_type ON entries(type);
	CREATE INDEX IF NOT EXISTS idx_entries_parent ON entries(parent_id);
	CREATE INDEX IF NOT EXISTS idx_writing_systems_entry ON writing_systems(entry_id);
	CREATE INDEX IF NOT EXISTS idx_definitions_entry ON definitions(entry_id);
	CREATE INDEX IF NOT EXISTS idx_synonyms_entry ON synonyms(entry_id);
	`

	_, err := c.db.Exec(schema)
	return err
}

// InsertEntry inserts a single YALF entry into the database
func (c *Converter) InsertEntry(entry Entry) error {
	tx, err := c.db.Begin()
	if err != nil {
		return err
	}
	defer tx.Rollback()

	// Insert main entry
	_, err = tx.Exec(`
		INSERT INTO entries (id, word, parent_id, type, transcription, root_word, etymology, history)
		VALUES (?, ?, ?, ?, ?, ?, ?, ?)
	`, entry.ID, entry.Word, entry.ParentID, entry.Type, entry.Transcription,
		entry.RootWord, entry.Etymology, entry.History)
	if err != nil {
		return fmt.Errorf("failed to insert entry: %w", err)
	}

	// Insert writing systems
	for systemName, representation := range entry.WritingSystems {
		_, err = tx.Exec(`
			INSERT INTO writing_systems (entry_id, system_name, representation)
			VALUES (?, ?, ?)
		`, entry.ID, systemName, representation)
		if err != nil {
			return fmt.Errorf("failed to insert writing system: %w", err)
		}
	}

	// Insert definitions
	for pos, definition := range entry.Definitions {
		result, err := tx.Exec(`
			INSERT INTO definitions (entry_id, meaning, position)
			VALUES (?, ?, ?)
		`, entry.ID, definition.Meaning, pos)
		if err != nil {
			return fmt.Errorf("failed to insert definition: %w", err)
		}

		definitionID, _ := result.LastInsertId()

		// Insert translations (from Extra field)
		for key, value := range definition.Extra {
			if len(key) > 12 && key[:12] == "translation_" {
				langCode := key[12:]
				if strVal, ok := value.(string); ok {
					_, err = tx.Exec(`
						INSERT INTO translations (definition_id, language_code, translation)
						VALUES (?, ?, ?)
					`, definitionID, langCode, strVal)
					if err != nil {
						return fmt.Errorf("failed to insert translation: %w", err)
					}
				}
			}
		}

		// Insert examples
		for exPos, example := range definition.Examples {
			result, err := tx.Exec(`
				INSERT INTO examples (definition_id, position)
				VALUES (?, ?)
			`, definitionID, exPos)
			if err != nil {
				return fmt.Errorf("failed to insert example: %w", err)
			}

			exampleID, _ := result.LastInsertId()

			// Insert example translations
			for langCode, text := range example {
				_, err = tx.Exec(`
					INSERT INTO example_translations (example_id, language_code, text)
					VALUES (?, ?, ?)
				`, exampleID, langCode, text)
				if err != nil {
					return fmt.Errorf("failed to insert example translation: %w", err)
				}
			}
		}
	}

	// Insert synonyms
	for _, synonym := range entry.Synonyms {
		_, err = tx.Exec(`
			INSERT INTO synonyms (entry_id, word_id, word, note)
			VALUES (?, ?, ?, ?)
		`, entry.ID, synonym.WordID, synonym.Word, synonym.Note)
		if err != nil {
			return fmt.Errorf("failed to insert synonym: %w", err)
		}
	}

	return tx.Commit()
}

// Convert converts a YALF file to SQLite database
func (c *Converter) Convert(yalfPath string) error {
	// Read YALF file
	data, err := os.ReadFile(yalfPath)
	if err != nil {
		return fmt.Errorf("failed to read YALF file: %w", err)
	}

	// Parse YAML
	var entries []Entry
	err = yaml.Unmarshal(data, &entries)
	if err != nil {
		return fmt.Errorf("failed to parse YAML: %w", err)
	}

	// Create schema
	err = c.CreateSchema()
	if err != nil {
		return fmt.Errorf("failed to create schema: %w", err)
	}

	// Insert entries
	for _, entry := range entries {
		err = c.InsertEntry(entry)
		if err != nil {
			return fmt.Errorf("failed to insert entry %d: %w", entry.ID, err)
		}
	}

	fmt.Printf("Successfully converted %d entries\n", len(entries))
	return nil
}

func main() {
	var (
		input     string
		output    string
		overwrite bool
		version   bool
	)

	flag.StringVar(&input, "input", "", "Input YALF file (.yalf or .yaml)")
	flag.StringVar(&input, "i", "", "Input YALF file (shorthand)")
	flag.StringVar(&output, "output", "", "Output SQLite database file (.db or .sqlite)")
	flag.StringVar(&output, "o", "", "Output SQLite database file (shorthand)")
	flag.BoolVar(&overwrite, "overwrite", false, "Overwrite output database if it exists")
	flag.BoolVar(&overwrite, "f", false, "Overwrite output database if it exists (shorthand)")
	flag.BoolVar(&version, "version", false, "Print version information")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "Usage: %s [OPTIONS] -i INPUT -o OUTPUT\n\n", filepath.Base(os.Args[0]))
		fmt.Fprintf(os.Stderr, "Convert YALF dictionary files to SQLite database\n\n")
		fmt.Fprintf(os.Stderr, "Options:\n")
		flag.PrintDefaults()
		fmt.Fprintf(os.Stderr, "\nExamples:\n")
		fmt.Fprintf(os.Stderr, "  %s -i dictionary.yalf -o output.db\n", filepath.Base(os.Args[0]))
		fmt.Fprintf(os.Stderr, "  %s -i input.yaml -o output.db -f\n", filepath.Base(os.Args[0]))
	}

	flag.Parse()

	if version {
		fmt.Println("yalf2sqlite version 1.0.0")
		fmt.Println("Author: AnmiTaliDev <anmitalidev@nuros.org>")
		os.Exit(0)
	}

	if input == "" || output == "" {
		flag.Usage()
		os.Exit(1)
	}

	// Check input file exists
	if _, err := os.Stat(input); os.IsNotExist(err) {
		fmt.Fprintf(os.Stderr, "Error: Input file %s not found\n", input)
		os.Exit(1)
	}

	// Check if output exists
	if _, err := os.Stat(output); err == nil && !overwrite {
		fmt.Fprintf(os.Stderr, "Error: Database %s already exists. Use -f to overwrite\n", output)
		os.Exit(1)
	}

	// Remove existing database if overwrite is true
	if overwrite {
		os.Remove(output)
	}

	// Create converter
	converter, err := NewConverter(output)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	defer converter.Close()

	// Convert
	err = converter.Convert(input)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("Successfully created database: %s\n", output)
}
