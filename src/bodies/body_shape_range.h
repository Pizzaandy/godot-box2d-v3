#include "../box2d_globals.h"

class BodyShapeRange {
public:
	BodyShapeRange(b2BodyId body_id) :
			body_id(body_id) {
		shape_count = b2Body_GetShapeCount(body_id);
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
	int shape_count;
};