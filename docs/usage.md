# Usage Guide

## Running REDBACK Simulations

### Basic Syntax

```bash
mpiexec -n <num_procs> redback-opt -i <input_file>
```

## Input File Format

REDBACK uses MOOSE input files (`.i` format). See the `tests/` and `examples/` directories for example input files.

We suggest to use [VSCode](https://code.visualstudio.com/) as a code editor ([MOOSE set up](https://mooseframework.inl.gov/help/development/VSCode.html)).

## Output Files

By default, REDBACK generates:

- **`.e` (Exodus II)**: Binary visualization files
- **`.csv`**: Text output for post-processing

## Post-Processing: Visualizing Results

Use [Paraview](https://www.paraview.org/) to open `.e` files:

```bash
paraview results.e
```

Refer to this [TeachBook](https://interactivetextbooks.citg.tudelft.nl/computational-modelling/scientific_visualisation/intro.html) from M. Lesueur to learn about scientific visualisation using Paraview.