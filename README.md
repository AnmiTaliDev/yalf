# YALF

**YALF Ain't Localisation Format**

A YAML-based format for creating structured dictionary entries with support for multiple writing systems, etymologies, translations, and examples.

## Overview

YALF is a flexible and extensible format designed for comprehensive dictionary and lexicon data. It supports:

- Multiple writing systems and orthographies (unlimited variants)
- Etymological information and historical context
- Multilingual definitions and translations
- Usage examples in multiple languages
- Synonym relationships and word derivations
- Phonetic transcriptions

## Features

- **YAML-based**: Human-readable and easy to edit
- **Flexible writing systems**: Support for any number of writing system variants (Cyrillic, Latin, Arabic, etc.)
- **Multilingual**: Unlimited translation languages using `translation_XX` pattern
- **Structured data**: Normalized schema suitable for conversion to databases
- **Extensible**: Add custom fields as needed

## Quick Example

```yaml
- id: 8
  word: "адам"
  type: "noun"
  transcription: "ɑˈdɑm"
  writing_systems:
    latin_2017: "adam"
    latin_2021: "adam"
    arabic: "ادام"
  etymology: "Араб-еврей тілдерінен"
  definitions:
    - meaning: "Ақыл-ойлы, еңбек құралдарын жасай алатын жоғары тіршілік иесі"
      translation_ru: "Человек"
      translation_en: "Human, person"
      examples:
        - kk: "Адам баласы әлеуметтік жан."
          ru: "Человек - существо социальное."
          en: "A human is a social being."
```

## Documentation

Full documentation is available in the [`docs/`](docs/) directory:

- [Format Specification](docs/specification.md) - Complete format specification
- [Examples](docs/examples.md) - Usage examples
- [Schema Reference](docs/schema.md) - Field reference guide

## Tools

### Converters

Located in [`converters/`](converters/) directory:

- **Python** - YALF to SQLite converter (`converters/python/`)
- **Go** - YALF to SQLite converter (`converters/go/`)

See [converters/README.md](converters/README.md) for usage instructions.

## Project Structure

```
yatf/
├── docs/              # Documentation (CC BY 4.0)
│   ├── README.md
│   ├── specification.md
│   ├── examples.md
│   ├── schema.md
│   └── LICENSE
├── converters/        # Conversion tools (MIT)
│   ├── python/
│   ├── go/
│   └── README.md
├── LICENSE            # MIT License for code
├── .gitattributes
└── README.md
```

## License

### Code (Converters and Tools)
MIT License - See [LICENSE](LICENSE) file

### Documentation
CC BY 4.0 - See [docs/LICENSE](docs/LICENSE) file

**Author:** AnmiTaliDev <anmitalidev@nuros.org>

## Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style
- Documentation is updated for new features
- Tools maintain backward compatibility with the YALF format

## Version

Current version: 1.0.0
