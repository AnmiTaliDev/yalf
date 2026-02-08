# YALF Examples

This document provides practical examples of YALF format usage.

## Basic Entry

A minimal dictionary entry with required fields only:

```yaml
- id: 1
  word: "кітап"
  type: "noun"
  definitions:
    - meaning: "Оқуға арналған басылым"
      translation_en: "Book"
```

## Entry with Multiple Writing Systems

Demonstrating the flexible `writing_systems` field:

```yaml
- id: 2
  word: "қазақ"
  type: "noun"
  transcription: "qɑˈzɑq"
  writing_systems:
    cyrillic: "қазақ"
    latin_2017: "qazaq"
    latin_2021: "qazaq"
    latin_my: "qazaq"
    arabic: "قازاق"
    chinese: "哈萨克"
  definitions:
    - meaning: "Қазақстанның негізгі халқы"
      translation_ru: "Казах"
      translation_en: "Kazakh"
```

## Entry with Etymology

Including detailed etymological information:

```yaml
- id: 3
  word: "кітапхана"
  type: "noun"
  transcription: "kɪtɑpχɑˈnɑ"
  writing_systems:
    latin_2017: "kitapkhana"
    latin_2021: "kitapxana"
  root_word: "кітап"
  etymology: "Араб тілінен"
  history: "Араб тілінің 'kitāb' (كتاب - кітап) және парсы тілінің 'xāna' (خانه - үй) сөздерінен құралған. Парсы тілі арқылы түркі тілдеріне енген."
  definitions:
    - meaning: "Кітаптар сақталатын және оқылатын мекеме"
      translation_ru: "Библиотека"
      translation_en: "Library"
      examples:
        - kk: "Мен кітапханаға бардым."
          ru: "Я пошёл в библиотеку."
          en: "I went to the library."
```

## Entry with Multiple Definitions

One word with several distinct meanings:

```yaml
- id: 4
  word: "жарық"
  type: "noun"
  transcription: "ʒɑˈrɯq"
  writing_systems:
    latin_2017: "zharıq"
    latin_2021: "jarıq"
  definitions:
    - meaning: "Көзге көрінетін энергия түрі"
      translation_ru: "Свет"
      translation_en: "Light"
      examples:
        - kk: "Жарық өшті."
          ru: "Свет погас."
          en: "The light went out."
    - meaning: "Дүние, әлем"
      translation_ru: "Мир, свет"
      translation_en: "World"
      examples:
        - kk: "Бала жарық көрді."
          ru: "Ребёнок появился на свет."
          en: "The child came into the world."
```

## Entry with Synonyms

Demonstrating synonym relationships:

```yaml
- id: 5
  word: "үй"
  type: "noun"
  transcription: "ʉj"
  writing_systems:
    latin_2017: "úı"
    latin_2021: "úı"
  definitions:
    - meaning: "Тұруға арналған ғимарат"
      translation_ru: "Дом"
      translation_en: "House, home"
      examples:
        - kk: "Менің үйім қаладан тыс."
          ru: "Мой дом за городом."
          en: "My house is outside the city."
  synonyms:
    - word_id: null
      word: "пәтер"
      note: "Көпқабатты үй"
    - word_id: null
      word: "тұрғын үй"
      note: "Формальный вариант"
```

## Entry with Parent Reference

Derived word referencing its parent:

```yaml
- id: 6
  word: "оқу"
  type: "verb"
  transcription: "ɔˈqʊ"
  writing_systems:
    latin_2017: "oqu"
    latin_2021: "oqu"
  definitions:
    - meaning: "Білім алу"
      translation_ru: "Учиться"
      translation_en: "To study, to learn"

- id: 7
  word: "оқушы"
  parent_id: 6
  type: "noun"
  transcription: "ɔqʊˈʃɯ"
  writing_systems:
    latin_2017: "oqushı"
    latin_2021: "oquşı"
  root_word: "оқу"
  definitions:
    - meaning: "Оқитын адам"
      translation_ru: "Ученик, студент"
      translation_en: "Student, pupil"
      examples:
        - kk: "Ол мектеп оқушысы."
          ru: "Он школьник."
          en: "He is a school student."
```

## Multilingual Example

Entry with translations in multiple languages:

```yaml
- id: 9
  word: "достық"
  type: "noun"
  transcription: "dɔsˈtɯq"
  writing_systems:
    latin_2017: "dostıq"
    latin_2021: "dostıq"
  definitions:
    - meaning: "Адамдар арасындағы жақын қарым-қатынас"
      translation_ru: "Дружба"
      translation_en: "Friendship"
      translation_de: "Freundschaft"
      translation_fr: "Amitié"
      translation_tr: "Dostluk"
      examples:
        - kk: "Достық - бұл қымбат қазына."
          ru: "Дружба - это драгоценное сокровище."
          en: "Friendship is a precious treasure."
          de: "Freundschaft ist ein kostbarer Schatz."
          fr: "L'amitié est un trésor précieux."
```

## Advanced Example with Custom Fields

YALF is extensible - you can add custom fields:

```yaml
- id: 10
  word: "ғылым"
  type: "noun"
  transcription: "ʁɯˈlɯm"
  writing_systems:
    latin_2017: "ǵılım"
    latin_2021: "ǵılım"
  etymology: "Араб тілінен"
  history: "Араб тілінің 'ʿilm' (علم - білім, ғылым) сөзінен шыққан."
  definitions:
    - meaning: "Жүйелі білім жиынтығы"
      translation_ru: "Наука"
      translation_en: "Science"
      examples:
        - kk: "Ғылым қоғамды дамытады."
          ru: "Наука развивает общество."
          en: "Science develops society."
  # Custom fields
  difficulty_level: "intermediate"
  frequency_rank: 1500
  semantic_field: "education"
  tags:
    - "abstract"
    - "education"
    - "intellectual"
```

## Complete Dictionary File

A complete YALF file with multiple entries:

```yaml
# Kazakh-English-Russian Dictionary
# Format: YALF 1.0

- id: 1
  word: "сәлем"
  type: "interjection"
  transcription: "sæˈlem"
  writing_systems:
    latin_2017: "sálem"
    latin_2021: "sálem"
  etymology: "Араб тілінен"
  definitions:
    - meaning: "Сәлемдесу сөзі"
      translation_ru: "Привет, здравствуйте"
      translation_en: "Hello, hi"
      examples:
        - kk: "Сәлем! Қалың қалай?"
          ru: "Привет! Как дела?"
          en: "Hi! How are you?"

- id: 2
  word: "рахмет"
  type: "interjection"
  transcription: "rɑχˈmet"
  writing_systems:
    latin_2017: "rakhmet"
    latin_2021: "raxmet"
  etymology: "Араб тілінен"
  definitions:
    - meaning: "Алғыс білдіру сөзі"
      translation_ru: "Спасибо"
      translation_en: "Thank you, thanks"
      examples:
        - kk: "Көмегіңіз үшін рахмет!"
          ru: "Спасибо за помощь!"
          en: "Thanks for your help!"
  synonyms:
    - word_id: null
      word: "ыраз"
      note: "Формальный вариант"

- id: 3
  word: "кеш"
  type: "interjection"
  transcription: "keʃ"
  writing_systems:
    latin_2017: "kesh"
    latin_2021: "keş"
  definitions:
    - meaning: "Кешірім сұрау сөзі"
      translation_ru: "Извините, простите"
      translation_en: "Sorry, excuse me"
      examples:
        - kk: "Кешіріңіз, кешіктім."
          ru: "Извините, я опоздал."
          en: "Sorry, I'm late."
```

## License

This documentation is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

**Author:** AnmiTaliDev <anmitalidev@nuros.org>
