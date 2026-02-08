# YALF Schema Reference

Quick reference guide for all YALF fields.

## Entry Level Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `id` | Integer | Yes | Unique identifier for the entry |
| `word` | String | Yes | Word in original/primary script |
| `type` | String | Yes | Part of speech (noun, verb, adjective, etc.) |
| `parent_id` | Integer/null | No | Reference to parent entry for derived words |
| `transcription` | String | No | Phonetic transcription (IPA recommended) |
| `writing_systems` | Object | No | Map of writing system identifiers to representations |
| `root_word` | String | No | Root or base form of the word |
| `etymology` | String | No | Brief etymology note |
| `history` | String | No | Detailed etymological and historical information |
| `definitions` | List | Yes | List of definition objects |
| `synonyms` | List | No | List of synonym objects |

## Writing Systems Object

The `writing_systems` field is a flexible map structure:

```yaml
writing_systems:
  <identifier>: <string>
  <identifier>: <string>
  ...
```

### Common Identifiers

You can use any identifiers that make sense for your dictionary. Common examples:

- `cyrillic` - Cyrillic script
- `latin` - Latin script
- `arabic` - Arabic script
- `latin_2017` - Latin script (2017 standard)
- `latin_2021` - Latin script (2021 standard)
- `latin_my` - Custom Latin orthography
- `traditional` - Traditional writing system
- `simplified` - Simplified writing system
- `romanization` - Romanized form

### Example

```yaml
writing_systems:
  cyrillic: "қазақ"
  latin_2017: "qazaq"
  latin_2021: "qazaq"
  arabic: "قازاق"
```

## Definition Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `meaning` | String | Yes | Definition in source language |
| `translation_XX` | String | No | Translation in language XX (e.g., translation_en, translation_ru) |
| `examples` | List | No | List of example objects |

### Translation Fields

Use the pattern `translation_XX` where XX is any language code:

- `translation_en` - English
- `translation_ru` - Russian
- `translation_de` - German
- `translation_fr` - French
- `translation_es` - Spanish
- `translation_zh` - Chinese
- `translation_ar` - Arabic
- `translation_tr` - Turkish
- etc.

### Example

```yaml
definitions:
  - meaning: "Адам"
    translation_en: "Human"
    translation_ru: "Человек"
    translation_de: "Mensch"
    examples:
      - kk: "Адам баласы"
        en: "Human being"
```

## Example Object

Example objects use language codes as keys:

```yaml
examples:
  - <lang_code>: <string>
    <lang_code>: <string>
    ...
```

### Common Language Codes

- `kk` - Kazakh
- `en` - English
- `ru` - Russian
- `tr` - Turkish
- `uz` - Uzbek
- `ky` - Kyrgyz
- `de` - German
- `fr` - French
- `es` - Spanish
- `zh` - Chinese
- `ar` - Arabic
- `ja` - Japanese

### Example

```yaml
examples:
  - kk: "Ол жақсы адам."
    ru: "Он хороший человек."
    en: "He is a good person."
    de: "Er ist ein guter Mensch."
```

## Synonym Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `word_id` | Integer/null | No | Reference to synonym's entry ID |
| `word` | String | Yes | The synonym word |
| `note` | String | No | Additional context or usage note |

### Example

```yaml
synonyms:
  - word_id: 42
    word: "адамзат"
    note: "В значении 'человечество'"
  - word_id: null
    word: "жан"
    note: "Разговорный вариант"
```

## Word Types

Common values for the `type` field:

### Parts of Speech

- `noun` - Noun (зат есім)
- `verb` - Verb (етістік)
- `adjective` - Adjective (сын есім)
- `adverb` - Adverb (үстеу)
- `pronoun` - Pronoun (есімдік)
- `numeral` - Numeral (сан есім)
- `interjection` - Interjection (одағай)
- `conjunction` - Conjunction (жалғаулық)
- `particle` - Particle (шылау)
- `postposition` - Postposition (септік)

### Other Types

- `phrase` - Phrase
- `idiom` - Idiomatic expression
- `proverb` - Proverb
- `proper_noun` - Proper noun
- `abbreviation` - Abbreviation
- `acronym` - Acronym

## Data Types Reference

### Integer
Whole numbers: `1`, `42`, `1000`

### String
Text in quotes: `"адам"`, `"Hello"`, `"مرحبا"`

For multiline strings, use YAML block syntax:
```yaml
history: |
  This is a long
  multiline text
  with line breaks.
```

### null
Represents absence of value: `null`

### List
Sequence of items:
```yaml
items:
  - item1
  - item2
  - item3
```

### Object/Map
Key-value pairs:
```yaml
object:
  key1: value1
  key2: value2
```

## Validation Rules

### Required Fields
Every entry MUST have:
- `id` (unique integer)
- `word` (non-empty string)
- `type` (non-empty string)
- `definitions` (non-empty list)

Every definition MUST have:
- `meaning` (non-empty string)

Every synonym MUST have:
- `word` (non-empty string)

### Uniqueness
- Entry `id` values must be unique within a dictionary
- `word_id` in synonyms should reference existing entry IDs

### References
- `parent_id` should reference an existing entry `id`
- `word_id` in synonyms should reference an existing entry `id` or be `null`

## Complete Schema Template

```yaml
- id: <integer>                    # Required
  word: "<string>"                 # Required
  parent_id: <integer|null>        # Optional
  type: "<string>"                 # Required
  transcription: "<string>"        # Optional
  writing_systems:                 # Optional
    <identifier>: "<string>"
    <identifier>: "<string>"
  root_word: "<string>"            # Optional
  etymology: "<string>"            # Optional
  history: "<string>"              # Optional
  definitions:                     # Required
    - meaning: "<string>"          # Required
      translation_XX: "<string>"   # Optional (any language)
      translation_YY: "<string>"   # Optional (any language)
      examples:                    # Optional
        - <lang>: "<string>"
          <lang>: "<string>"
        - <lang>: "<string>"
          <lang>: "<string>"
    - meaning: "<string>"
      # ... more definitions
  synonyms:                        # Optional
    - word_id: <integer|null>      # Optional
      word: "<string>"             # Required
      note: "<string>"             # Optional
    - word_id: <integer|null>
      word: "<string>"
      # ... more synonyms
```

## License

This documentation is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

**Author:** AnmiTaliDev <anmitalidev@nuros.org>
