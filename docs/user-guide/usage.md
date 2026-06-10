# Usage Guide

## Running Redback Simulations

### Basic Syntax

```bash
mpiexec -n <num_procs> redback-opt -i <input_file>
```

### Example

```bash
mpiexec -n 4 redback-opt -i benchmark_1.i
```

## Input File Format

Redback uses MOOSE input files (`.i` format). See the `tests/` and `examples/` directories for example input files.

## Common Options

| Option | Description |
|--------|-------------|
| `-i` | Input file path |
| `--help` | Display help message |
| `-n` | Number of processes (with mpiexec) |

## Output Files

By default, Redback generates:

- **`.e` (Exodus II)**: Binary visualization files
- **`.csv`**: Text output for post-processing
- **`.out`**: Console output log

## Post-Processing

### Visualizing Results

Use [Paraview](https://www.paraview.org/) or [VisIt](https://visit.llnl.gov/) to open `.e` files:

```bash
paraview results.e
```

### Analyzing Data

CSV files can be analyzed with Python, MATLAB, or other tools:

```python
import pandas as pd
data = pd.read_csv('results.csv')
print(data.head())
```

## Advanced Topics

See the [API Reference](../api.md) for more details on specific kernels and materials.

---

**Need help?** Check the [FAQ](../faq.md) or examples in the repository.
