# Installation Guide

## Prerequisites

REDBACK is a MOOSE application, so the first thing you need is to install MOOSE following these [instructions](https://mooseframework.inl.gov/getting_started/installation/index.html) (depending on your setup).

With MOOSE installed (and tested), you can now install REDBACK in a similar manner.

## Step-by-Step Installation

### 1. Clone the Repository

```bash
cd ~/projects
git clone https://github.com/RedbackProject/redback.git
cd redback
git checkout master
```

You have to clone REDBACK into `${MOOSE_DIR}/../redback` so it's a sibling to the MOOSE directory.

### 2. Compile REDBACK

```bash
make -j 4 NavierStokes=0 PorousFlow=0
```

- Replace `4` with the number of processors on your machine for faster compilation.

- Replace `0` by `1` if you would like to compile REDBACK with those MOOSE modules.

### 3. Verify Installation

Run the test suite to verify everything is working:

```bash
./run_tests -j 4
```

## Stay up to date

Do not forget to update regularly MOOSE and REDBACK, using `git pull` on the main branch (`git checkout master`). Then recompile and test.

## Troubleshooting compilation Errors

Consult the [MOOSE documentation](https://mooseframework.inl.gov/help/troubleshooting.html#a855675d-be11-4288-94e4-f4635133a15f) for troubleshooting compilation errors. If your issue can not be solved here, you may go through the [MOOSE Discussion forum](https://github.com/idaholab/moose/discussions) for help, or open an [issue on GitHub](https://github.com/RedbackProject/redback/issues).