# Settings

This document lists the project settings exposed by this extension as well as relevant built-in project settings.

⚠️ Enable "Advanced Settings" in the top-right corner of the project settings window to see the settings listed here.

## Table of Contents

- [Common](#common)
- [2D](#3d)
- [Godot Box2D 3D](#box2d)

## Common

These settings are part of Godot's default project settings and can be found under "Physics" / "Common".

<table>
  <thead>
    <tr>
      <th>Category</th>
      <th>Name</th>
      <th>Supported</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>-</td>
      <td>Physics Ticks per Second</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>✅</td>
      <td>Enable Object Picking</td>
      <td>❌</td>
      <td>-</td>
    </tr>
  </tbody>
</table>

## 2D

These settings are part of Godot's default project settings and can be found under "Physics / 2D".

<table>
  <thead>
    <tr>
      <th>Category</th>
      <th>Name</th>
      <th>Supported</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>-</td>
      <td>Run on Separate Thread</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Physics Engine</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Default Gravity</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Default Gravity Vector</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Default Linear Damp</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Default Angular Damp</td>
      <td>✅</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Sleep Threshold Linear</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Sleep Threshold Angular</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Time Before Sleep</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Solver Iterations</td>
      <td>❌</td>
      <td>See extension settings <a href="#box2d">below</a>.</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Contact Recycle Radius</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Contact Max Separation</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Contact Max Allowed Penetration</td>
      <td>❌</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Default Contact Bias</td>
      <td>❌</td>
      <td>-</td>
    </tr>
  </tbody>
</table>

## Box2D

These settings are exposed by Godot Jolt and can be found under "Physics / Box 2D".

<table>
  <thead>
    <tr>
      <th>Category</th>
      <th>Name</th>
      <th>Description</th>
      <th>Notes</th>
    <tr>
  </thead>
  <tbody>
    <tr>
      <td>-</td>
      <td>Substeps</td>
      <td>
        Number of substeps to perform per physics update. Increasing this value will increase accuracy.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>-</td>
      <td>Pixels per Meter</td>
      <td>
        Adjusts the scale of the simulation. The default scale is 100 px / m to match Godot's default gravity of 980.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Contact Hertz</td>
      <td>
        Contact stiffness in cycles per second. Increasing this value increases the speed of overlap recovery, but can introduce jitter.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Contact Damping Ratio</td>
      <td>
        Contact bounciness. You can speed up overlap recovery by decreasing this with the trade-off that overlap resolution becomes more energetic.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Joint Hertz</td>
      <td>
        Joint stiffness in cycles per second.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Joint Damping Ratio</td>
      <td>
        Joint bounciness.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Friction Mixing Rule</td>
      <td>
        Godot calculates friction by taking the minimum of two friction values (e.g. `min(frictionA, frictionB)`).
        Box2D calculates friction by taking the geometric mean of two friction values (e.g. `sqrt(frictionA * frictionB)`).
        Use `Minimum (Godot)` for parity with Godot Physics and `Geometric Mean (Box2D)` for more physically accurate friction.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Solver</td>
      <td>Restitution Mixing Rule</td>
      <td>
        Godot calculates restitution by adding two bounce values and clamping the result (e.g. `max(bounceA + bounceB, 1)`).
        Box2D calculates restitution by taking the maximum of two bounce values (e.g. `max(bounceA, bounceB)`).
        Use `Additive (Godot)` for parity with Godot Physics and `Maximum (Box2D)` for bounce values higher than 1.
      </td>
      <td>-</td>
    </tr>
    <tr>
      <td>Advanced</td>
      <td>Presolve Enabled</td>
      <td>
        Godot Box2D v3 implements a number of Godot Physics features by calling a pre-solve function for every contact.
        This has a small performance cost and can be disabled. The following features will not work without presolve enabled:
        - Collision exceptions
        - One-way collisions for rigidbodies
      </td>
      <td>-</td>
    </tr>
  </tbody>
</table>
