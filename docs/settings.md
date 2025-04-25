# Settings

This document lists the project settings exposed by this extension as well as relevant built-in project settings.

⚠️ Enable "Advanced Settings" in the top-right corner of the project settings window to see the settings listed here.

## Table of Contents

- [Common](#common)
- [2D](#2d)
- [Godot Box2D 3D](#box2d)

## Common

These settings are part of Godot's default project settings and can be found under `Physics -> Common`.

| Category | Name                     | Supported | Notes |
|----------|--------------------------|-----------|-------|
| -        | Physics Ticks per Second | ✅        | -     |
| -       | Enable Object Picking     | ❌        | -     |

## 2D

These settings are part of Godot's default project settings and can be found under `Physics -> 2D`.

| Category | Name                        | Supported | Notes                              |
|----------|-----------------------------|-----------|------------------------------------|
| -        | Run on Separate Thread      | ❌        | -                                  |
| -        | Physics Engine              | ✅        | -                                  |
| -        | Default Gravity             | ✅        | -                                  |
| -        | Default Gravity Vector      | ✅        | -                                  |
| -        | Default Linear Damp         | ✅        | -                                  |
| -        | Default Angular Damp        | ✅        | -                                  |
| -        | Sleep Threshold Linear      | ❌        | -                                  |
| -        | Sleep Threshold Angular     | ❌        | -                                  |
| -        | Time Before Sleep           | ❌        | -                                  |
| Solver   | Solver Iterations           | ❌        | See extension settings [below](#box2d). |
| Solver   | Contact Recycle Radius      | ❌        | -                                  |
| Solver   | Contact Max Separation      | ❌        | -                                  |
| Solver   | Contact Max Allowed Penetration | ❌     | -                                  |
| Solver   | Default Contact Bias        | ❌        | -                                  |

## Box2D

These settings are exposed by Godot Jolt and can be found under `Physics -> Box 2D`.

| Category | Name                      | Description | Notes |
|----------|---------------------------|-------------|-------|
| -        | Substeps                  | Number of substeps to perform per physics update. Increasing this value will increase accuracy. | - |
| -        | Pixels per Meter          | Adjusts the scale of the simulation. The default scale is `100 px / m` to match Godot's default gravity of `980`. | - |
| Solver   | Contact Hertz             | Contact stiffness in cycles per second. Increasing this value increases the speed of overlap recovery, but can introduce jitter. | - |
| Solver   | Contact Damping Ratio     | Contact bounciness. You can speed up overlap recovery by decreasing this with the trade-off that overlap resolution becomes more energetic. | - |
| Solver   | Joint Hertz               | Joint stiffness in cycles per second. | - |
| Solver   | Joint Damping Ratio       | Joint bounciness. | - |
| Solver   | Friction Mixing Rule      | Godot calculates friction by taking the minimum of two friction values e.g. `min(frictionA, frictionB)`.<br>Box2D calculates friction by taking the geometric mean e.g. `sqrt(frictionA * frictionB)`.<br>Use `Minimum (Godot)` for parity with Godot Physics or `Geometric Mean (Box2D)` to account for both friction values. | - |
| Solver   | Restitution Mixing Rule   | Godot calculates bounce by adding two bounce values and limiting the result to 1 e.g. `min(bounceA, bounceB, 1)`.<br>Box2D calculates bounce by taking the max of two bounce values e.g. `max(bounceA, bounceB)`.<br>Use `Additive (Godot)` for parity with Godot Physics or `Maximum (Box2D)` for bounce values higher than one. | - |
| Advanced | Presolve Enabled          | Godot Box2D v3 implements a number of Godot Physics features by calling a pre-solve function for every contact. This has a small cost and can be disabled.<br>**Collision Exceptions** and **One-way Collisions** for rigidbodies will not work without presolve enabled. | - |
