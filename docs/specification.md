# YALF Format Specification

Version 1.0.0

## Overview

YALF (YALF Ain't Localisation Format) is a YAML-based format designed for creating comprehensive dictionary entries with support for multiple writing systems, etymological information, and multilingual definitions.

## File Structure

YALF files are valid YAML documents containing a list of dictionary entries. Each file should use the `.yalf` or `.yaml` extension.

## Entry Structure

Each dictionary entry is a YAML object with the following fields:

### Required Fields

#### `id`
- Type: Integer
- Description: Unique identifier for the entry
- Example: `8`

#### `word`
- Type: String
- Description: The word in its original/primary script
- Example: `"адам"`

#### `type`
- Type: String
- Description: Part of speech or word type
- Common values: `"noun"`, `"verb"`, `"adjective"`, `"adverb"`, `"pronoun"`, etc.
- Example: `"noun"`

#### `definitions`
- Type: List of Definition objects
- Description: List of meanings with translations and examples
- See [Definitions Section](#definitions)

### Optional Fields

#### `parent_id`
- Type: Integer or null
- Description: Reference to parent entry ID for derived words
- Example: `null` or `15`

#### `transcription`
- Type: String
- Description: Phonetic transcription (IPA recommended)
- Example: `"ɑˈdɑm"`

#### `writing_systems`
- Type: Object (map)
- Description: Word representations in different writing systems or orthographies
- Keys: Writing system identifiers (user-defined)
- Values: String representations
- Example:
  ```yaml
  writing_systems:
    latin_2017: "adam"
    latin_2021: "adam"
    latin_my: "adam"
    cyrillic: "адам"
    arabic: "ادام"
  ```

#### `root_word`
- Type: String
- Description: Root or base form of the word
- Example: `"адам"`

#### `etymology`
- Type: String
- Description: Brief etymology note
- Example: `"Араб-еврей тілдерінен"`

#### `history`
- Type: String
- Description: Detailed etymological and historical information
- Example: `"Арабтың 'ādam' (آدم - Адам, бірінші адам, адамзат) сөзінен шыққан..."`

#### `synonyms`
- Type: List of Synonym objects
- Description: List of synonymous words
- See [Synonyms Section](#synonyms)

## Definitions

The `definitions` field contains a list of meaning objects. Each definition has:

### Definition Fields

#### `meaning`
- Type: String (required)
- Description: Definition in the source language
- Example: `"Ақыл-ойлы, еңбек құралдарын жасай алатын жоғары тіршілік иесі"`

#### `translation_ru`
- Type: String (optional)
- Description: Russian translation
- Example: `"Человек"`

#### `translation_en`
- Type: String (optional)
- Description: English translation
- Example: `"Human, person"`

#### Additional translations
- You can add translations in any language using the pattern `translation_XX` where XX is the language code
- Examples: `translation_de`, `translation_fr`, `translation_zh`, etc.

#### `examples`
- Type: List of Example objects (optional)
- Description: Usage examples
- See [Examples Section](#examples)

### Example Structure

Each example in the `examples` list should contain:

- `kk`: Example in Kazakh (or source language)
- `ru`: Russian translation (optional)
- `en`: English translation (optional)
- Additional language codes can be used

Example:
```yaml
examples:
  - kk: "Адам баласы әлеуметтік жан."
    ru: "Человек - существо социальное."
    en: "A human is a social being."
```

## Synonyms

The `synonyms` field contains a list of synonym objects:

### Synonym Fields

#### `word_id`
- Type: Integer or null
- Description: Reference to the synonym's entry ID if it exists in the dictionary
- Example: `null` or `42`

#### `word`
- Type: String (required)
- Description: The synonym word
- Example: `"адамзат"`

#### `note`
- Type: String (optional)
- Description: Additional context or usage note
- Example: `"В значении 'человечество'"`

## Complete Example

```yaml
- id: 8
  word: "адам"
  parent_id: null
  type: "noun"
  transcription: "ɑˈdɑm"
  writing_systems:
    latin_2017: "adam"
    latin_2021: "adam"
    latin_my: "adam"
  root_word: "адам"
  etymology: "Араб-еврей тілдерінен"
  history: "Арабтың 'ādam' (آدم - Адам, бірінші адам, адамзат) сөзінен шыққан, ол өз кезегінде еврей тілінің 'adam' (אָדָם - адам) сөзінен алынған. Ислам және христиан дінінің таралуымен бірге түркі тілдеріне енген."
  definitions:
    - meaning: "Ақыл-ойлы, еңбек құралдарын жасай алатын жоғары тіршілік иесі"
      translation_ru: "Человек"
      translation_en: "Human, person"
      examples:
        - kk: "Адам баласы әлеуметтік жан."
          ru: "Человек - существо социальное."
          en: "A human is a social being."
        - kk: "Ол жақсы адам."
          ru: "Он хороший человек."
          en: "He is a good person."
    - meaning: "Біреу, кейбір адам"
      translation_ru: "Кто-то, некто"
      translation_en: "Someone, somebody"
      examples:
        - kk: "Бір адам келді."
          ru: "Кто-то пришёл."
          en: "Someone came."
  synonyms:
    - word_id: null
      word: "адамзат"
      note: "В значении 'человечество'"
    - word_id: null
      word: "жан"
      note: "Разговорный вариант"
```

## Best Practices

1. **Unique IDs**: Ensure each entry has a unique `id`
2. **Consistent Writing Systems**: Use consistent identifiers across your dictionary (e.g., always use `latin_2017`, not sometimes `latin2017`)
3. **IPA Transcription**: Use International Phonetic Alphabet for transcriptions
4. **Link Related Entries**: Use `parent_id` and `word_id` in synonyms to create connections
5. **Multiple Definitions**: Separate distinct meanings into different definition objects
6. **Context in Examples**: Provide diverse examples that illustrate usage
7. **Etymology Citations**: Include source language scripts and meanings in history field

## Extensibility

YALF is designed to be extensible:

- Add custom fields at the entry level
- Use any language codes for translations (`translation_XX`)
- Define custom writing system identifiers in `writing_systems`
- Add additional fields to definitions, examples, or synonyms as needed

## License

This documentation is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

**Author:** AnmiTaliDev <anmitalidev@nuros.org>
