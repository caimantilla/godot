#ifndef DIRECTION_HELPER_HPP
#define DIRECTION_HELPER_HPP


#include "core/math/vector3.h"
#include "core/object/class_db.h"
#include "core/object/object.h"


class DirectionHelper : public Object
{
	GDCLASS(DirectionHelper, Object);

public:
	enum Side3D
	{
		SIDE_3D_LEFT = 0,
		SIDE_3D_RIGHT = 1,
		SIDE_3D_FRONT = 2,
		SIDE_3D_REAR = 3,
		SIDE_3D_TOP = 4,
		SIDE_3D_BOTTOM = 5,
	};

	enum Direction3D
	{
		DIRECTION_3D_LEFT = 0,
		DIRECTION_3D_RIGHT = 1,
		DIRECTION_3D_FORWARD = 2,
		DIRECTION_3D_BACK = 3,
		DIRECTION_3D_UP = 4,
		DIRECTION_3D_DOWN = 5,
	};

	enum DirectionHorizontal
	{
		DIRECTION_HORIZONTAL_LEFT = 0,
		DIRECTION_HORIZONTAL_RIGHT = 1,
		DIRECTION_HORIZONTAL_FORWARD = 2,
		DIRECTION_HORIZONTAL_BACK = 3,
	};

private:
	static DirectionHelper *singleton;

protected:
	static void _bind_methods();

public:
	static DirectionHelper *get_singleton();

	Side3D get_3d_side_opposite(const Side3D p_side) const;
	Vector3 get_3d_direction_angle_deg(const Direction3D p_direction) const;
	Vector3 get_3d_direction_angle_rad(const Direction3D p_direction) const;
	Vector3i get_3d_direction_offset(const Direction3D p_direction) const;
	Direction3D get_3d_direction_opposite(const Direction3D p_direction) const;
	real_t get_horizontal_direction_angle_deg(const DirectionHorizontal p_direction) const;
	real_t get_horizontal_direction_angle_rad(const DirectionHorizontal p_direction) const;
	DirectionHorizontal get_horizontal_direction_rotated(const DirectionHorizontal p_current, const DirectionHorizontal p_by) const;
	DirectionHorizontal get_horizontal_direction_opposite(const DirectionHorizontal p_direction) const;
	Vector2i get_horizontal_direction_offset(const DirectionHorizontal p_direction) const;

	DirectionHelper();
	~DirectionHelper();
};


VARIANT_ENUM_CAST(DirectionHelper::Side3D);
VARIANT_ENUM_CAST(DirectionHelper::Direction3D);
VARIANT_ENUM_CAST(DirectionHelper::DirectionHorizontal);


#endif // DIRECTION_HELPER_HPP
