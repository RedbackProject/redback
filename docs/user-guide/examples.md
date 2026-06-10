# Examples

This page provides an overview of available examples in the Redback repository.

## Benchmark Tests

Redback includes several benchmark tests located in the `tests/` directory:

- **Benchmark 1**: Thermal diffusion
- **Benchmark 7**: Hydro-mechanical (HM) coupling
- **Benchmark 8**: Thermo-hydro-mechanical (THM) coupling

### Running Benchmarks

```bash
cd tests/
./run_tests
```

## Example Input Files

Example input files are provided in the `examples/` directory. These demonstrate:

- Different physics coupling options
- Material property definitions
- Boundary condition specifications
- Output options

### Running an Example

```bash
cd examples/
mpiexec -n 1 ../redback-opt -i <example_file.i>
```

## Post-Processing Examples

Python scripts for post-processing are available in `scripts/image_formatter/`:

- **`dat_to_png.py`**: Convert raw binary data to PNG images
- **`raw_to_png.py`**: Convert raw format to PNG stack

## Documentation Figures

Scripts in `doc/theory/data/create_figures/` generate publication-quality figures from simulation results.

---

**More examples coming soon!** Check back for additional use cases and applications.
