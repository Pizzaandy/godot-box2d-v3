#include "box2d/box2d.h"

class ChainSegmentRange {
public:
	ChainSegmentRange(b2ChainId chain_id) :
			chain_id(chain_id) {
		segment_count = b2Chain_GetSegmentCount(chain_id);
		shape_ids = new b2ShapeId[segment_count];
		b2Chain_GetSegments(chain_id, shape_ids, segment_count);
	}

	~ChainSegmentRange() {
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
		return Iterator(shape_ids, segment_count);
	}

private:
	b2ChainId chain_id;
	b2ShapeId *shape_ids;
	int segment_count;
};