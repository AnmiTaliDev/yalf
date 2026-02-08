# Changelog

All notable changes to the YALF project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-02-08

### Added
- Initial release of YALF format specification
- Complete documentation suite:
  - Format specification
  - Schema reference
  - Usage examples
  - README with quick start
- Python converter (yalf2sqlite):
  - Convert YALF to SQLite database
  - Support for all YALF features
  - Command-line interface
- Go converter (yalf2sqlite):
  - Convert YALF to SQLite database
  - High-performance implementation
  - Cross-platform compilation support
- Example dictionary files:
  - basic.yalf - Simple introduction
  - writing_systems.yalf - Multiple orthographies
  - etymology.yalf - Etymological information
  - synonyms.yalf - Synonym relationships
  - derivations.yalf - Word derivations
  - multilingual.yalf - Extensive translations
  - complete.yalf - Comprehensive example
- Project infrastructure:
  - MIT License for code
  - CC BY 4.0 License for documentation
  - .gitattributes for consistent line endings
  - .editorconfig for editor consistency
  - Contributing guidelines

### Features
- Flexible writing systems support (unlimited variants)
- Multilingual translations (unlimited languages)
- Etymology and historical information tracking
- Synonym relationships with cross-references
- Word derivation tracking via parent_id
- Phonetic transcription support
- Usage examples in multiple languages
- Normalized SQLite database schema
- Full YAML compatibility

### Documentation
- Complete format specification
- Schema reference with all fields
- Multiple usage examples
- Converter documentation
- Query examples for SQLite

[1.0.0]: https://github.com/AnmiTaliDev/yatf/releases/tag/v1.0.0
