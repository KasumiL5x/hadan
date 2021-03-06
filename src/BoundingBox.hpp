#ifndef __bounding_box__
#define __bounding_box__

#include <cc/Vec3.hpp>

class BoundingBox {
public:
	enum class Corner {
		TopLeftBack,
		TopLeftFront,
		TopRightBack,
		TopRightFront,
		BottomLeftBack,
		BottomLeftFront,
		BottomRightBack,
		BottomRightFront
	};

public:
	BoundingBox();
	BoundingBox( const cc::Vec3f& center, const cc::Vec3f& halfExtents );

	const cc::Vec3f& getCenter() const;
	const cc::Vec3f& getHalfExtents() const;
	void setCenter( const cc::Vec3f& center );
	void setHalfExtents( const cc::Vec3f& halfExtents );

	float minX() const;
	float minY() const;
	float minZ() const;
	float maxX() const;
	float maxY() const;
	float maxZ() const;

	cc::Vec3f getCorner( Corner corner ) const;
	float getDiagonalDistance() const;

private:
	cc::Vec3f _center;
	cc::Vec3f _halfExtents;
};

#endif /* __bounding_box__ */