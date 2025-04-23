<p align="center">
  <img src="https://github.com/Pizzaandy/godot-box2d-v3/blob/main/logo.png?raw=true" />
</p>

Godot Box2D v3 is a high-performance physics extension for Godot 4.3+ that implements Box2D v3. It can be considered a successor to the [Godot Box2D](https://github.com/appsinacup/godot-box2d) extension.

## Features

- Improved stability and performance, especially for large piles of bodies
- Faster, more accurate queries and character movement (e.g. `move_and_collide`, `move_and_slide`)
- Continuous collision detection (CCD) enabled by default for dynamic vs static collisions
- Additional APIs to make the physics server easier and faster to use (docs coming soon)

## Unsupported Features / Limitations
Most of the Physics API has been implemented, but the following features do not have parity with Godot Physics:
- `WorldBoundaryShape2D` unsupported (planned for a future version of Box2D)
- `SeparationRayShape2D` unsupported
- Space parameters (`space_set_param`) unsupported because Box2D v3 uses a different solver
- "Cast Ray" CCD mode (`CCDMode::CCD_MODE_CAST_RAY`) unsupported
- Contacts are not visible when `Visible Collision Shapes` is enabled
- Convex polygons currently cannot have more than 8 vertices (does not affect `CollisionPolygon2D`)

## Planned Features
- [Cross-platform determinism](https://box2d.org/posts/2024/08/determinism/)
- Serialization + Rollback
