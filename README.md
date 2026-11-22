# N-Body Planetary Simulator

An interactive gravitational N-body simulator written in C, with 2D and 3D visualization using [raylib](https://www.raylib.com/).

The project explores numerical modeling of gravitational systems, from simple two-body configurations to simplified solar system simulations. Users can modify physical parameters, load custom initial conditions and visualize trajectories in real time.

## Main features

- 2D and 3D gravitational N-body simulation.
- Interactive editing of body mass, position, velocity and radius.
- Adjustable numerical time step, trajectory history and visualization scales.
- Collision detection and merging of bodies.
- Toggleable trajectories and velocity vectors.
- Camera zoom, navigation and body tracking.
- Loading and resetting custom simulation files.

## Mathematical model

The simulation is based on Newton's law of universal gravitation and Newton's second law.

### Gravitational interactions

For each body i, the gravitational acceleration is given by:

```text
aᵢ = G ∑ [mⱼ (xⱼ − xᵢ) / ‖xⱼ − xᵢ‖³]
```

The sum runs over every body j other than i. Here, xᵢ and aᵢ are the position and acceleration vectors, mⱼ is the mass of body j, G is the gravitational constant, and ‖ · ‖ denotes the Euclidean norm.

### Numerical integration

The equations of motion are integrated using the semi-implicit Euler method (Euler–Cromer):

```text
vᵢⁿ⁺¹ = vᵢⁿ + Δt · aᵢⁿ
xᵢⁿ⁺¹ = xᵢⁿ + Δt · vᵢⁿ⁺¹
```

The velocity vector vᵢ is updated first, and the new velocity is then used to advance the position vector xᵢ. The time step Δt is expressed in seconds. New states are calculated from the same time level before updating the bodies.

Direct pairwise gravitational interactions require O(N²) operations per time step, where N is the number of bodies.

### Collision handling

When two bodies overlap, they merge into a single body while conserving mass and linear momentum:

```text
m = m₁ + m₂
v = (m₁ v₁ + m₂ v₂) / (m₁ + m₂)
```

The new position is the center of mass of the two bodies, and the combined radius is calculated assuming additive spherical volumes:

```text
x = (m₁ x₁ + m₂ x₂) / (m₁ + m₂)
R = ∛(R₁³ + R₂³)
```

This is a simplified, perfectly inelastic collision model; kinetic energy is not conserved in general.

## Installation and execution

### Requirements

The provided Makefiles target Linux (including Ubuntu). You need GCC, Make, Git, OpenGL/X11 development libraries and [raylib](https://github.com/raysan5/raylib). A graphical environment is required (for example, WSL with graphical application support).

See the [raylib repository](https://github.com/raysan5/raylib) for installation instructions for your system. The Makefiles use headers in `/usr/local/include` and link against raylib and the Linux system libraries. The source files for [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) are included in this project and are compiled automatically.

### Clone the repository

```bash
git clone https://github.com/RomainGuinjard/n_body_planetary_simulator.git
cd n_body_planetary_simulator
```

### Run the 2D simulator

From the repository root:

```bash
make 2d
./2d_model/main.x
```

### Run the 3D simulator

From the repository root:

```bash
make 3d
cd 3d_model
./main3d.x
```

The 3D executable must be launched from `3d_model/` because the default configuration file (`systeme3d.txt`) is loaded relative to the working directory.

To build both versions from the repository root, use `make`. To force a complete rebuild, use `make -B 2d` or `make -B 3d`.

## Controls

| Action | Control |
| --- | --- |
| Pause / resume | Space |
| Toggle trajectories | T |
| Toggle velocity vectors | V |
| Load a new simulation file | L |
| Reset the current simulation | R |
| Reset the camera | C |
| Zoom in / out | Mouse wheel over the simulation area |
| Follow a body in 2D | Left-click the body |
| Pan the 2D camera / stop following | Arrow keys |
| Follow a body in 3D | Left-click a body or its entry in the information panel |
| Stop following a body in 3D | Right-click |
| Navigate the 3D view | Free-camera controls when not following a body |

The body information panel can be scrolled with the mouse wheel. To edit a numerical field, click it, replace its value and press **Enter** to apply the change.

## Simulation and visualization parameters

| Parameter | Description |
| --- | --- |
| `dt` (s) | Numerical time step. Larger values advance simulated time faster but can reduce accuracy. |
| Radius scale correction | Multiplier for the displayed body sizes. |
| Distance scale correction | Multiplier for the displayed distances. |
| Number of previous values | Number of stored positions used to display trajectories (up to 10,000). |
| Velocity vector scale (2D only) | Controls the displayed length of velocity vectors. |
| AU scale (2D only) | Controls distance normalization relative to the initial extent of the system. Reset the simulation after changing this parameter. |

Radius and distance can be displayed using linear, logarithmic or square-root scaling. These display transformations do not change the physical coordinates or gravitational model.

## Loading custom simulations

The simulator accepts custom initial configurations stored in plain text (`.txt`) files.

1. Pause the simulation (`Space`).
2. Click **Load new file** or press `L`.
3. Select a file matching the 2D or 3D input format below.
4. Adjust the time step and visualization parameters if necessary. Press **Enter** to validate each numerical value.
5. Press `R` to restart from the initial conditions using the selected parameters.
6. Press `Space` to resume.

Loading or resetting a simulation restores the initial body configuration while preserving the current simulation and visualization parameter values. In 2D, the reset also recalculates distance normalization and resets the camera.

The first line of a file contains the number of bodies. Each following line defines one body. Separate fields with whitespace. Names must contain no spaces; use positive masses and radii, and RGBA color components from 0 to 255. Scientific notation is supported.

### 2D file format

```text
<number_of_bodies>
<name> <mass> <x> <y> <vx> <vy> <radius> <r> <g> <b> <a>
```

Example — a simplified Sun–Earth system:

```text
2
sun 1.989e30 0 0 0 0 696340000 255 255 0 255
earth 5.972e24 1.496e11 0 0 29780 6371000 0 100 255 255
```

Save the file as, for example, `sun_earth.txt` in `2d_model/simulation_files/` and load it from the 2D interface.

### 3D file format

```text
<number_of_bodies>
<name> <mass> <x> <y> <z> <vx> <vy> <vz> <radius> <r> <g> <b> <a>
```

Example — the same initial configuration in three dimensions:

```text
2
sun 1.989e30 0 0 0 0 0 0 696340000 255 255 0 255
earth 5.972e24 1.496e11 0 0 0 29780 0 6371000 0 100 255 255
```

The additional `z` and `vz` fields are required, even when both are zero. Save the file in `3d_model/` or another accessible directory and load it from the 3D interface.

| Field | Meaning | Unit |
| --- | --- | --- |
| `name` | Body identifier | — |
| `mass` | Mass | kg |
| `x`, `y`, `z` | Position components | m |
| `vx`, `vy`, `vz` | Velocity components | m/s |
| `radius` | Physical radius | m |
| `r`, `g`, `b`, `a` | Red, green, blue and alpha | Integers from 0 to 255 |

### Included configurations

- `2d_model/simulation_files/systeme.txt`: default 2D solar system configuration.
- `2d_model/simulation_files/systeme_complet.txt`: extended 2D system with additional bodies.
- `2d_model/simulation_files/2_bodies.txt` and the three- and four-body files: small N-body test cases.
- `2d_model/simulation_files/collisions.txt`: 2D collision scenario.
- `3d_model/systeme3d.txt`: default 3D solar system configuration.
- `3d_model/collisions3d.txt`: 3D collision scenario.

A time step and display scales suitable for one configuration may not be suitable for another. Start with a sufficiently small time step and adjust the scales and camera zoom until the bodies are visible.

## Project structure

```text
n_body_planetary_simulator/
├── 2d_model/                 # 2D simulation and interface
│   └── simulation_files/     # 2D initial configurations
├── 3d_model/                 # 3D simulation, camera and configurations
├── docs/conception/         # Initial MATLAB modeling scripts
├── Makefile                 # Build targets for both versions
├── tinyfiledialogs.c        # File selection dialog library
├── tinyfiledialogs.h
└── README.md
```

Both implementations separate the physics calculations, input parsing, rendering and interface logic into C modules.

## Limitations

- Accuracy and stability depend on the time step. A large time step can also cause collisions to be missed because overlap is checked at discrete instants.
- The collision model assumes merging rather than bouncing, deformation or fragmentation.
- The 2D implementation can remove bodies beyond a distance threshold; this is a numerical safeguard, not a physical escape criterion.
- The nonlinear display scales distort relative distances and apparent trajectories.
- Single-precision storage and direct pairwise calculations limit numerical accuracy and scalability.
- Trajectories have not been systematically validated against real astronomical ephemerides. The project primarily focuses on numerical implementation and interactive visualization.

## Authors

Nicolas Le Bouar and Romain Guinjard.
