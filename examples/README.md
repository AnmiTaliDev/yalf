# YALF Examples

This directory contains example YALF dictionary files demonstrating various features and use cases.

## Example Files

### basic.yalf
**Purpose:** Introduction to YALF format  
**Features:**
- Minimal required fields
- Simple definitions
- Basic translations
- One example per definition

**Best for:** Getting started with YALF

### writing_systems.yalf
**Purpose:** Demonstrate flexible writing systems  
**Features:**
- Multiple orthographies (Cyrillic, Latin variants, Arabic, Chinese)
- Unlimited writing system variants
- Cross-script representation

**Best for:** Multilingual dictionaries, orthography documentation

### etymology.yalf
**Purpose:** Show etymological information  
**Features:**
- Etymology field (brief origin)
- History field (detailed explanation)
- Source language scripts and meanings
- Root word tracking

**Best for:** Historical dictionaries, etymological research

### synonyms.yalf
**Purpose:** Demonstrate synonym relationships  
**Features:**
- Synonym lists with notes
- Cross-references using word_id
- Usage context notes
- Bidirectional relationships

**Best for:** Thesaurus-style dictionaries

### derivations.yalf
**Purpose:** Show word derivation tracking  
**Features:**
- parent_id relationships
- Word family trees
- Root word references
- Derived forms

**Best for:** Morphological dictionaries, word formation studies

### multilingual.yalf
**Purpose:** Extensive multilingual translations  
**Features:**
- Translations in 8+ languages
- Multiple translation languages per definition
- Examples in multiple languages
- International vocabulary

**Best for:** Polyglot dictionaries, language learning

### complete.yalf
**Purpose:** Comprehensive feature showcase  
**Features:**
- All YALF features in one file
- Multiple definitions per entry
- Complete etymology and history
- Synonyms and derivations
- Multilingual examples
- Multiple writing systems

**Best for:** Reference implementation, testing converters

## Using These Examples

### Reading Examples
Simply open any `.yalf` file in a text editor to see the YAML structure.

### Testing Converters
Convert examples to SQLite to test the converters:

```bash
# Python converter
cd ../converters/python
python yalf2sqlite.py ../../examples/complete.yalf test.db

# Go converter
cd ../converters/go
./yalf2sqlite -i ../../examples/complete.yalf -o test.db
```

### Creating Your Own Dictionary
1. Start with `basic.yalf` as a template
2. Add features as needed from other examples
3. Follow the [format specification](../docs/specification.md)

## File Naming Convention

YALF files use the `.yalf` extension, but `.yaml` and `.yml` also work since YALF is valid YAML.

## Language Codes Used

These examples use the following language codes:
- `kk` - Kazakh (Қазақ тілі)
- `ru` - Russian (Русский)
- `en` - English
- `de` - German (Deutsch)
- `fr` - French (Français)
- `tr` - Turkish (Türkçe)
- `es` - Spanish (Español)
- `it` - Italian (Italiano)
- `pt` - Portuguese (Português)
- `zh` - Chinese (中文)
- `ar` - Arabic (العربية)
- `ja` - Japanese (日本語)

## Writing System Identifiers Used

- `cyrillic` - Cyrillic script
- `latin_2017` - Kazakh Latin alphabet (2017 standard)
- `latin_2021` - Kazakh Latin alphabet (2021 standard)
- `latin_my` - Custom Latin orthography
- `arabic` - Arabic script
- `chinese` - Chinese characters

**Note:** These are just examples. YALF supports any writing system identifiers you need.

## Tips for Creating Examples

1. **Start simple**: Begin with required fields only
2. **Add incrementally**: Add optional fields as needed
3. **Be consistent**: Use the same writing system identifiers throughout your dictionary
4. **Document choices**: Add comments explaining your conventions
5. **Validate structure**: Test with converters to ensure valid YAML

## License

These examples are part of the YALF project and are provided under the MIT License. See [../LICENSE](../LICENSE) for details.

**Author:** AnmiTaliDev <anmitalidev@nuros.org>

## See Also

- [YALF Specification](../docs/specification.md)
- [Schema Reference](../docs/schema.md)
- [Converters](../converters/README.md)
