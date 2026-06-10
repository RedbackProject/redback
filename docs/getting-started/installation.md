# Installation Guide

## Prerequisites

- MOOSE framework installed and configured
- C++ compiler (gcc/clang)
- Python 3.x
- Git

## Step-by-Step Installation

### 1. Clone the Repository

```bash
cd ~/projects  # or your preferred location
git clone https://github.com/RedbackProject/redback.git
cd redback
```

### 2. Compile Redback

```bash
make -j 4
```

Replace `4` with the number of processors on your machine for faster compilation.

### 3. Verify Installation

Run the test suite to verify everything is working:

```bash
./run_tests
```

## Building the Documentation

### Install MkDocs

```bash
pip install -r requirements-docs.txt
```

### Serve Documentation Locally

```bash
mkdocs serve
```

Then open `http://localhost:8000` in your browser.

## Troubleshooting

### MOOSE_DIR not set

If you get an error about `MOOSE_DIR`, set it explicitly:

```bash
export MOOSE_DIR=/path/to/moose
```

### Compilation Errors

Make sure you have the required dependencies installed. Consult the [MOOSE documentation](https://mooseframework.inl.gov/) for your system.

---

**Need help?** See the [FAQ](../faq.md) or open an [issue on GitHub](https://github.com/RedbackProject/redback/issues).
