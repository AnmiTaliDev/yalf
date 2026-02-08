# Contributing to YALF

Thank you for your interest in contributing to YALF (YALF Ain't Localisation Format)!

## How to Contribute

### Reporting Issues

- Check existing issues before creating a new one
- Provide clear description and examples
- Include YALF version and converter version if applicable

### Suggesting Enhancements

- Explain the use case and benefit
- Provide examples of how it would work
- Discuss compatibility with existing format

### Code Contributions

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Test thoroughly
5. Commit with clear messages
6. Push to your fork
7. Open a Pull Request

## Development Setup

### Python Converter

```bash
cd converters/python
pip install -r requirements.txt
python yalf2sqlite.py ../../examples/basic.yalf test.db
```

### Go Converter

```bash
cd converters/go
go mod download
go build -o yalf2sqlite yalf2sqlite.go
./yalf2sqlite -i ../../examples/basic.yalf -o test.db
```

## Code Style

### Python
- Follow PEP 8
- Use type hints where appropriate
- Add docstrings for functions and classes

### Go
- Follow Go conventions
- Run `go fmt` before committing
- Add comments for exported functions

### YAML/YALF
- Use 2 spaces for indentation
- Add comments for complex structures
- Follow examples in `examples/` directory

## Testing

Before submitting:

1. Test with all example files
2. Verify database schema correctness
3. Check data integrity in resulting SQLite databases
4. Test edge cases (empty fields, special characters, etc.)

## Documentation

- Update relevant documentation in `docs/` if changing format
- Add examples to `examples/` for new features
- Update README.md if adding new tools or features

## Commit Messages

Use clear, descriptive commit messages:

- `feat: add support for custom fields`
- `fix: handle null values in etymology`
- `docs: update specification for new field`
- `test: add tests for multilingual examples`

## License

By contributing, you agree that your contributions will be licensed under:
- MIT License for code (converters, tools)
- CC BY 4.0 for documentation

## Questions?

Open an issue for discussion or reach out to the maintainer.

**Maintainer:** AnmiTaliDev <anmitalidev@nuros.org>
