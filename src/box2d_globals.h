#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

#ifdef TRACY_ENABLE
#include "../tracy/public/tracy/Tracy.hpp"
#define TracyZoneScoped(name) ZoneScopedN(name)
#else
#define TracyZoneScoped(name)
#endif

using namespace godot;

extern float BOX2D_PIXELS_PER_METER;
extern float BOX2D_LINEAR_SLOP;

/// Mask bit used by all bodies.
const uint64_t BODY_MASK_BIT = (1ULL << 63);

/// Mask bit used by all areas.
const uint64_t AREA_MASK_BIT = (1ULL << 62);

/// Mask bit used by all monitorable areas.
const uint64_t AREA_MONITORABLE_MASK_BIT = (1ULL << 61);

_FORCE_INLINE_ void box2d_set_pixels_per_meter(float p_value) {
	BOX2D_PIXELS_PER_METER = p_value;
}

_FORCE_INLINE_ Vector2 to_godot(const b2Vec2 p_vec) {
	float scale = BOX2D_PIXELS_PER_METER;
	return Vector2(scale * p_vec.x, scale * p_vec.y);
}

_FORCE_INLINE_ Vector2 to_godot_normalized(const b2Vec2 p_vec) {
	return Vector2(p_vec.x, p_vec.y).normalized();
}

_FORCE_INLINE_ b2Vec2 to_box2d(const Vector2 p_vec) {
	float scale = 1 / BOX2D_PIXELS_PER_METER;
	return scale * b2Vec2{ (float)p_vec.x, (float)p_vec.y };
}

_FORCE_INLINE_ b2Vec2 to_box2d_normalized(const Vector2 p_vec) {
	return b2Normalize(b2Vec2{ p_vec.x, p_vec.y });
}

_FORCE_INLINE_ float to_box2d(float p_length) {
	float scale = 1 / BOX2D_PIXELS_PER_METER;
	return scale * p_length;
}

_FORCE_INLINE_ float to_godot(float p_length) {
	float scale = BOX2D_PIXELS_PER_METER;
	return scale * p_length;
}

_FORCE_INLINE_ b2Transform to_box2d(Transform2D p_transform) {
	return b2Transform{ to_box2d(p_transform.get_origin()), b2MakeRot(p_transform.get_rotation()) };
}

/// Range for iterating body shapes.
class BodyShapeRange {
public:
	explicit BodyShapeRange(b2BodyId body_id) :
			body_id(body_id), shape_ids(nullptr) {
		shape_count = b2Body_GetShapeCount(body_id);
		if (shape_count == 0) {
			return;
		}
		shape_ids = new b2ShapeId[shape_count];
		b2Body_GetShapes(body_id, shape_ids, shape_count);
	}

	~BodyShapeRange() {
		delete[] shape_ids;
	}

	class Iterator {
	public:
		Iterator(b2ShapeId *ids, int index) :
				shape_ids(ids), index(index) {}

		b2ShapeId operator*() const {
			return shape_ids[index];
		}

		Iterator &operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator &other) const {
			return index != other.index;
		}

	private:
		b2ShapeId *shape_ids;
		int index;
	};

	Iterator begin() const {
		return Iterator(shape_ids, 0);
	}

	Iterator end() const {
		return Iterator(shape_ids, shape_count);
	}

private:
	b2BodyId body_id;
	b2ShapeId *shape_ids;
	int shape_count = 0;
};

/// Range for iterating body joints.
class BodyJointRange {
public:
	explicit BodyJointRange(b2BodyId body_id) :
			body_id(body_id), joint_ids(nullptr) {
		joint_count = b2Body_GetJointCount(body_id);
		if (joint_count == 0) {
			return;
		}
		joint_ids = new b2JointId[joint_count];
		b2Body_GetJoints(body_id, joint_ids, joint_count);
	}

	~BodyJointRange() {
		delete[] joint_ids;
	}

	class Iterator {
	public:
		Iterator(b2JointId *ids, int index) :
				joint_ids(ids), index(index) {}

		b2JointId operator*() const {
			return joint_ids[index];
		}

		Iterator &operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator &other) const {
			return index != other.index;
		}

	private:
		b2JointId *joint_ids;
		int index;
	};

	Iterator begin() const {
		return Iterator(joint_ids, 0);
	}

	Iterator end() const {
		return Iterator(joint_ids, joint_count);
	}

private:
	b2BodyId body_id;
	b2JointId *joint_ids;
	int joint_count = 0;
};

/// Range for iterating chain segment shapes.
class ChainSegmentRange {
public:
	explicit ChainSegmentRange(b2ChainId chain_id) :
			chain_id(chain_id) {
		segment_count = b2Chain_GetSegmentCount(chain_id);
		shape_ids = memnew_arr(b2ShapeId, segment_count);
		b2Chain_GetSegments(chain_id, shape_ids, segment_count);
	}

	~ChainSegmentRange() {
		memdelete_arr(shape_ids);
	}

	class Iterator {
	public:
		Iterator(b2ShapeId *ids, int index) :
				shape_ids(ids), index(index) {}

		b2ShapeId operator*() const {
			return shape_ids[index];
		}

		Iterator &operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator &other) const {
			return index != other.index;
		}

	private:
		b2ShapeId *shape_ids;
		int index;
	};

	Iterator begin() const {
		return Iterator(shape_ids, 0);
	}

	Iterator end() const {
		return Iterator(shape_ids, segment_count);
	}

private:
	b2ChainId chain_id;
	b2ShapeId *shape_ids;
	int segment_count;
};

struct ShapeCollidePoint {
	Vector2 point = Vector2();
	/// Positive if penetrating
	float depth = 0.0f;
};

struct ShapeCollideResult {
	ShapeCollidePoint points[2];
	Vector2 normal = Vector2();
	int32_t point_count = 0;
};

struct Box2DShapeGeometry {
	b2ShapeType type = b2ShapeType::b2_shapeTypeCount;

	union {
		b2Capsule capsule;
		b2Circle circle;
		b2Polygon polygon;
		b2Segment segment;
		b2ChainSegment chain_segment;
	};

	explicit Box2DShapeGeometry() = default;

	Box2DShapeGeometry(const b2Capsule &p_shape) :
			type(b2ShapeType::b2_capsuleShape) {
		capsule = p_shape;
	}

	Box2DShapeGeometry(const b2Circle &p_shape) :
			type(b2ShapeType::b2_circleShape) {
		circle = p_shape;
	}

	Box2DShapeGeometry(const b2Polygon &p_shape) :
			type(b2ShapeType::b2_polygonShape) {
		polygon = p_shape;
	}

	Box2DShapeGeometry(const b2Segment &p_shape) :
			type(b2ShapeType::b2_segmentShape) {
		segment = p_shape;
	}

	Box2DShapeGeometry(const b2ChainSegment &p_shape) :
			type(b2ShapeType::b2_chainSegmentShape) {
		chain_segment = p_shape;
	}

	Box2DShapeGeometry(const b2ShapeId &p_shape_id) {
		type = b2Shape_GetType(p_shape_id);
		switch (type) {
			case b2ShapeType::b2_capsuleShape: {
				capsule = b2Shape_GetCapsule(p_shape_id);
				break;
			}
			case b2ShapeType::b2_circleShape: {
				circle = b2Shape_GetCircle(p_shape_id);
				break;
			}
			case b2ShapeType::b2_polygonShape: {
				polygon = b2Shape_GetPolygon(p_shape_id);
				break;
			}
			case b2ShapeType::b2_segmentShape: {
				segment = b2Shape_GetSegment(p_shape_id);
				break;
			}
			case b2ShapeType::b2_chainSegmentShape: {
				chain_segment = b2Shape_GetChainSegment(p_shape_id);
				break;
			}
			default: {
				ERR_FAIL_MSG("Invalid shape type");
			}
		}
	}

	Box2DShapeGeometry inflated(float p_radius) {
		p_radius = to_box2d(p_radius);
		switch (type) {
			case b2ShapeType::b2_capsuleShape: {
				capsule.radius += p_radius;
				return Box2DShapeGeometry(capsule);
			}
			case b2ShapeType::b2_circleShape: {
				circle.radius += p_radius;
				return Box2DShapeGeometry(circle);
			}
			case b2ShapeType::b2_polygonShape: {
				polygon.radius += p_radius;
				return Box2DShapeGeometry(polygon);
			}
			case b2ShapeType::b2_segmentShape: {
				b2Capsule capsule;
				capsule.center1 = segment.point1;
				capsule.center2 = segment.point2;
				capsule.radius = p_radius;
				return Box2DShapeGeometry(capsule);
			}
			case b2ShapeType::b2_chainSegmentShape: {
				ERR_FAIL_V_MSG(Box2DShapeGeometry(chain_segment), "Chain segments cannot have a radius");
			}
			default: {
				ERR_FAIL_V_MSG(Box2DShapeGeometry(), "Invalid shape type");
			}
		}
	}
};

float box2d_compute_safe_fraction(float p_unsafe_fraction, float p_total_distance);

ShapeCollideResult box2d_collide_shapes(
		const Box2DShapeGeometry &p_shape_a,
		const b2Transform &xfa,
		const Box2DShapeGeometry &p_shape_b,
		const b2Transform &xfb,
		bool p_swapped = false);

b2ShapeProxy box2d_make_shape_proxy(const Box2DShapeGeometry &p_shape);
