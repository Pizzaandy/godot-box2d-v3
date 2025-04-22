<p align="center">
  <img src="https://github.com/Pizzaandy/godot-box2d-v3/blob/main/logo.png?raw=true" />
</p>

Godot Box2D v3 is a high-performance physics extension for Godot 4.3+ that implements Box2D v3. It can be considered a successor to the [Godot Box2D](https://github.com/appsinacup/godot-box2d) extension.

## Features

- Improved stability and performance, especially for large piles of bodies
- Faster, more accurate shape cast queries and character movement (e.g. `move_and_collide`, `move_and_slide`)
- Continuous collision detection (CCD) enabled by default for dynamic vs static collisions
- [Cross-platform determinism](https://box2d.org/posts/2024/08/determinism/) (needs testing)
- Additional APIs to make the physics server easier and faster to use (docs coming soon)

## Unsupported Features
All Physics APIs are implemented, but the following features do not have feature parity with Godot Physics:
- `WorldBoundaryShape2D` (planned for a future version of Box2D)
- `SeparationRayShape2D`
