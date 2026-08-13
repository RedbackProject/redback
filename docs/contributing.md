# Contributing to REDBACK

We welcome contributions to REDBACK! This guide explains how to contribute code, documentation, and examples.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally
3. Create a new branch for your feature: `git checkout -b feature/my-feature`
4. Make your changes
5. Push to your fork: `git push origin feature/my-feature`
6. Create a Pull Request on GitHub

## Code Style

Redback follows MOOSE coding conventions. Before committing, run the formatter:

```bash
./formatter.sh
```

## Testing

Always run tests before submitting a pull request:

```bash
./run_tests
```

Ensure that:
- All existing tests pass
- New features include appropriate tests
- Tests are added to `tests/` directory

## Documentation

When adding new features:

1. Update relevant `.md` files in `docs/`
2. Add docstrings to code (for Python files)
3. Include examples if applicable

### Build the documentation locally

Install MkDocs and build locally:

```bash
mkdocs serve
```

Then open `http://localhost:8000` in your browser.

## Reporting Issues

- Check existing [issues](https://github.com/RedbackProject/redback/issues) first
- Provide a clear description and minimal reproducer
- Include output files and error messages

---

Thank you for contributing to Redback!
