#!/usr/bin/env python3
"""
Plot MOOSE beam results from an Exodus (.e/.exo) file and compare against
the analytical finite Winkler cantilever solution.

This version handles MOOSE beam Exodus output where:
- nodal displacement is stored as vector array 'disp_'  (shape = n_points x 3)
- nodal rotation     is stored as vector array 'rot_'   (shape = n_points x 3)
- elemental shear    is stored as scalar array 'beam_force_y'
- elemental moment   is stored as scalar array 'beam_moment_z'

It plots along the full beam versus depth:
- displacement
- rotation
- shear force
- bending moment

Usage:
    python postprocessWinkler.py beam_out.e

Optional:
    python postprocessWinkler.py beam_out.e --time-index -1
    python postprocessWinkler.py beam_out.e --print-arrays
"""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

try:
    import pyvista as pv
except ImportError as exc:
    raise SystemExit("PyVista is required. Install with: pip install pyvista") from exc


def analytical_winkler(x: np.ndarray, E: float, I: float, k: float, L: float, P: float):
    """
    Finite cantilever on Winkler foundation:
        EI y'''' + k y = 0
    BCs:
        y(0)=0, y'(0)=0, y''(L)=0, EI y'''(L)=P

    Returns:
        y(x), theta(x), M(x), V(x), beta
    """
    EI = E * I
    beta = (k / (4.0 * EI)) ** 0.25

    eta = beta * x
    lam = beta * L
    D = np.cosh(2.0 * lam) - np.cos(2.0 * lam)

    y = (
        P
        / (EI * beta**3 * D)
        * (
            np.sin(lam)
            * np.sinh(lam)
            * (np.sinh(eta) * np.cos(eta) - np.cosh(eta) * np.sin(eta))
            - (np.sin(lam) * np.cosh(lam) + np.cos(lam) * np.sinh(lam))
            * (np.cosh(eta) * np.cos(eta) - 1.0)
        )
    )

    theta = (
        P
        / (EI * beta**2 * D)
        * (
            2.0 * np.sin(lam) * np.sinh(lam) * np.sin(eta) * np.sinh(eta)
            - (np.sin(lam) * np.cosh(lam) + np.cos(lam) * np.sinh(lam))
            * (np.sin(eta) * np.cosh(eta) - np.cos(eta) * np.sinh(eta))
        )
    )

    M = (
        2.0
        * P
        / (beta * D)
        * (
            np.sin(eta) * np.sin(lam) * np.sinh(eta - lam)
            + np.sin(eta - lam) * np.sinh(eta) * np.sinh(lam)
        )
    )

    V = (
        -2.0
        * P
        / D
        * (
            (np.sin(lam) * np.cosh(lam) + np.cos(lam) * np.sinh(lam))
            * (np.sin(eta) * np.cosh(eta) + np.cos(eta) * np.sinh(eta))
            - 2.0 * np.sin(lam) * np.sinh(lam) * np.cos(eta) * np.cosh(eta)
        )
    )

    return y, theta, M, V, beta


def iter_datasets(obj, path=()):
    if obj is None:
        return

    if isinstance(obj, pv.MultiBlock):
        for i in range(obj.n_blocks):
            try:
                child = obj[i]
            except Exception:
                child = None
            yield from iter_datasets(child, path + (i,))
    else:
        yield path, obj


def describe_blocks(obj):
    if isinstance(obj, pv.MultiBlock):
        print(f"MultiBlock with {obj.n_blocks} blocks")
        for path, ds in iter_datasets(obj):
            if ds is None:
                continue
            name = type(ds).__name__
            npts = getattr(ds, "n_points", 0)
            ncells = getattr(ds, "n_cells", 0)
            print(f"  path={path} type={name} n_points={npts} n_cells={ncells}")
    else:
        print(
            f"Single dataset: type={type(obj).__name__}, "
            f"n_points={getattr(obj, 'n_points', 0)}, "
            f"n_cells={getattr(obj, 'n_cells', 0)}"
        )


def find_line_block(dataset):
    candidates = []

    for path, ds in iter_datasets(dataset):
        if ds is None:
            continue
        npts = getattr(ds, "n_points", 0)
        ncells = getattr(ds, "n_cells", 0)
        if npts > 0 and ncells > 0:
            candidates.append((path, ds, npts, ncells))

    if not candidates:
        print("No direct mesh blocks found. Block structure was:")
        describe_blocks(dataset)
        raise RuntimeError("No usable block found in Exodus file.")

    candidates.sort(key=lambda item: (item[2], item[3]), reverse=True)
    path, ds, npts, ncells = candidates[0]
    print(f"Using block path {path} with type={type(ds).__name__}, n_points={npts}, n_cells={ncells}")
    return ds


def get_time_step_mesh(filename: str, time_index: int):
    reader = pv.get_reader(filename)

    if hasattr(reader, "time_values"):
        tvals = reader.time_values
        if len(tvals) == 0:
            dataset = reader.read()
            return find_line_block(dataset), None

        if time_index < 0:
            time_index = len(tvals) + time_index

        reader.set_active_time_point(time_index)
        dataset = reader.read()
        return find_line_block(dataset), tvals[time_index]

    dataset = reader.read()
    return find_line_block(dataset), None


def detect_axis(points: np.ndarray) -> int:
    spans = np.ptp(points, axis=0)
    return int(np.argmax(spans))


def get_cell_centers_1d(mesh):
    centers = mesh.cell_centers()
    pts = np.asarray(centers.points)
    axis = detect_axis(pts)
    return pts[:, axis]


def get_point_coords_1d(mesh):
    pts = np.asarray(mesh.points)
    axis = detect_axis(pts)
    return pts[:, axis]


def print_available_arrays(mesh):
    print("\n=== DATASET INFO ===")
    print(f"type      : {type(mesh).__name__}")
    print(f"n_points  : {mesh.n_points}")
    print(f"n_cells   : {mesh.n_cells}")

    print("\nAll array names:")
    print(list(mesh.array_names))

    print("\nPoint data arrays:")
    for name in mesh.point_data.keys():
        arr = mesh.point_data[name]
        shape = getattr(arr, "shape", None)
        print(f"  {name:30s} shape={shape}")

    print("\nCell data arrays:")
    for name in mesh.cell_data.keys():
        arr = mesh.cell_data[name]
        shape = getattr(arr, "shape", None)
        print(f"  {name:30s} shape={shape}")

    print("\nField data arrays:")
    for name in mesh.field_data.keys():
        arr = mesh.field_data[name]
        shape = getattr(arr, "shape", None)
        print(f"  {name:30s} shape={shape}")


def find_array_by_candidates(mesh, candidates):
    for name in candidates:
        if name in mesh.point_data:
            return "point", name, np.asarray(mesh.point_data[name])
        if name in mesh.cell_data:
            return "cell", name, np.asarray(mesh.cell_data[name])
    return None, None, None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("exodus_file", type=str, help="Path to Exodus .e/.exo file")
    parser.add_argument("--time-index", type=int, default=-1, help="Time index to read (default: last)")
    parser.add_argument("--E", type=float, default=2.068e11, help="Young's modulus")
    parser.add_argument("--I", type=float, default=1.7329e-8, help="Second moment of area")
    parser.add_argument("--k", type=float, default=1.0e5, help="Winkler stiffness")
    parser.add_argument("--L", type=float, default=10.0, help="Beam length")
    parser.add_argument("--P", type=float, default=1.0, help="Head load")
    parser.add_argument("--print-arrays", action="store_true", help="Print all arrays found in the Exodus block")
    args = parser.parse_args()

    exodus_path = Path(args.exodus_file)
    if not exodus_path.exists():
        raise SystemExit(f"File not found: {exodus_path}")

    mesh, tval = get_time_step_mesh(str(exodus_path), args.time_index)

    if args.print_arrays:
        print_available_arrays(mesh)

    # Coordinates along the beam axis
    x_nodes = get_point_coords_1d(mesh)
    x_cells = get_cell_centers_1d(mesh)

    # Sort in beam-axis order first
    node_order = np.argsort(x_nodes)
    cell_order = np.argsort(x_cells)
    x_nodes = x_nodes[node_order]
    x_cells = x_cells[cell_order]

    # MOOSE beam Exodus naming
    disp_loc, disp_name, disp_vec = find_array_by_candidates(mesh, ["disp_"])
    rot_loc, rot_name, rot_vec = find_array_by_candidates(mesh, ["rot_"])
    force_loc, force_name, beam_force_y = find_array_by_candidates(mesh, ["beam_force_y"])
    moment_loc, moment_name, beam_moment_z = find_array_by_candidates(mesh, ["beam_moment_z"])

    print(f"Using displacement array: {disp_name} ({disp_loc})")
    print(f"Using rotation array    : {rot_name} ({rot_loc})")
    print(f"Using shear array       : {force_name} ({force_loc})")
    print(f"Using moment array      : {moment_name} ({moment_loc})")

    if disp_vec is None:
        print_available_arrays(mesh)
        raise RuntimeError("Could not find displacement vector array 'disp_'.")
    if rot_vec is None:
        print_available_arrays(mesh)
        raise RuntimeError("Could not find rotation vector array 'rot_'.")

    if disp_loc != "point":
        raise RuntimeError("Expected 'disp_' to be stored in point_data.")
    if rot_loc != "point":
        raise RuntimeError("Expected 'rot_' to be stored in point_data.")

    disp_vec = np.asarray(disp_vec)[node_order]
    rot_vec = np.asarray(rot_vec)[node_order]

    # Components for bending in the x-y plane
    disp_y = disp_vec[:, 1]
    rot_z = rot_vec[:, 2]

    if beam_force_y is not None:
        beam_force_y = np.asarray(beam_force_y)
        if force_loc == "point":
            beam_force_y = beam_force_y[node_order]
        else:
            beam_force_y = beam_force_y[cell_order]

    if beam_moment_z is not None:
        beam_moment_z = np.asarray(beam_moment_z)
        if moment_loc == "point":
            beam_moment_z = beam_moment_z[node_order]
        else:
            beam_moment_z = beam_moment_z[cell_order]

    # Analytical curves along full beam
    x_ana = np.linspace(0.0, args.L, 1000)
    y_ana, th_ana, M_ana, V_ana, beta = analytical_winkler(
        x_ana, args.E, args.I, args.k, args.L, args.P
    )

    # Convert to depth: depth = 0 at head, increasing downward
    z_nodes = args.L - x_nodes
    z_cells = args.L - x_cells
    z_ana = args.L - x_ana

    # Sort by depth increasing downward
    node_order_z = np.argsort(z_nodes)
    cell_order_z = np.argsort(z_cells)
    ana_order_z = np.argsort(z_ana)

    z_nodes = z_nodes[node_order_z]
    z_cells = z_cells[cell_order_z]
    z_ana = z_ana[ana_order_z]

    disp_y = disp_y[node_order_z]
    rot_z = rot_z[node_order_z]
    y_ana = y_ana[ana_order_z]
    th_ana = th_ana[ana_order_z]
    M_ana = M_ana[ana_order_z]
    V_ana = V_ana[ana_order_z]

    if beam_force_y is not None:
        beam_force_y = beam_force_y[cell_order_z]

    if beam_moment_z is not None:
        beam_moment_z = beam_moment_z[cell_order_z]

    print(f"Read file: {exodus_path}")
    if tval is not None:
        print(f"Time value: {tval}")
    print(f"beta = {beta:.8f}")
    print(f"Head displacement (FE) = {disp_y[0]:.10e}")
    print(f"Head rotation    (FE) = {rot_z[0]:.10e}")


    # Plot side-by-side (horizontal layout)
    fig, axes = plt.subplots(1, 4, figsize=(16, 5), sharey=True)

    # Colors
    c_disp = "tab:blue"
    c_rot = "tab:orange"
    c_moment = "tab:green"
    c_shear = "tab:red"

    # -------------------------
    # Displacement
    ax = axes[0]
    ax.plot(y_ana, z_ana, color=c_disp, label="Analytical")
    ax.plot(-disp_y, z_nodes, "o", ms=3, color=c_disp, alpha=0.7, label="FE")
    ax.set_xlabel("disp_y")
    ax.set_ylabel("Depth")
    ax.set_title("Displacement")
    ax.grid(True)

    # -------------------------
    # Rotation
    ax = axes[1]
    ax.plot(th_ana, z_ana, color=c_rot, label="Analytical")
    ax.plot(rot_z, z_nodes, "o", ms=3, color=c_rot, alpha=0.7, label="FE")
    ax.set_xlabel("rot_z")
    ax.set_title("Rotation")
    ax.grid(True)

    # -------------------------
    # Moment
    ax = axes[2]
    ax.plot(M_ana, z_ana, color=c_moment, label="Analytical")
    if beam_moment_z is not None:
        ax.plot(-beam_moment_z, z_cells, "o", ms=3, color=c_moment, alpha=0.7, label="FE")
    ax.set_xlabel("M_z")
    ax.set_title("Bending Moment")
    ax.grid(True)

    # -------------------------
    # Shear
    ax = axes[3]
    ax.plot(V_ana, z_ana, color=c_shear, label="Analytical")
    if beam_force_y is not None:
        ax.plot(-beam_force_y, z_cells, "o", ms=3, color=c_shear, alpha=0.7, label="FE")
    ax.set_xlabel("V_y")
    ax.set_title("Shear Force")
    ax.grid(True)

    # -------------------------
    # Depth zero at top
    for ax in axes:
        ax.set_ylim(args.L, 0.0)

    # Align zero lines visually
    for ax in axes:
        ax.axvline(0.0, color="k", linewidth=0.5)

    # Force symmetric x-limits
    for ax in axes:
        xmin, xmax = ax.get_xlim()
        m = max(abs(xmin), abs(xmax))
        ax.set_xlim(-m, m)

    # Keep only one legend
    axes[0].legend()

    # Clean layout
    fig.tight_layout()

    # Save
    out_png = exodus_path.with_suffix(".winkler_horizontal.png")
    fig.savefig(out_png, dpi=200)
    print(f"Saved plot: {out_png}")

    plt.show()


if __name__ == "__main__":
    main()