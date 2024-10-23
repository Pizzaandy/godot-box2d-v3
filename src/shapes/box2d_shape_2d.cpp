#include "box2d_shape_2d.h"

Box2DShape2D::~Box2DShape2D() {
	for (Box2DShapeInstance *inst : instances) {
		inst->assign_shape(nullptr);
	}
}