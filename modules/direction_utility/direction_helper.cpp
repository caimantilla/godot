#include "direction_helper.hpp"
#include "core/math/math_funcs.h"


DirectionHelper *DirectionHelper::singleton = nullptr;


DirectionHelper *DirectionHelper::get_singleton()
{
	return singleton;
}


void DirectionHelper::_bind_methods()
{
	BIND_ENUM_CONSTANT(SIDE_3D_LEFT);
	BIND_ENUM_CONSTANT(SIDE_3D_RIGHT);
	BIND_ENUM_CONSTANT(SIDE_3D_FRONT);
	BIND_ENUM_CONSTANT(SIDE_3D_REAR);
	BIND_ENUM_CONSTANT(SIDE_3D_TOP);
	BIND_ENUM_CONSTANT(SIDE_3D_BOTTOM);

	BIND_ENUM_CONSTANT(DIRECTION_3D_LEFT);
	BIND_ENUM_CONSTANT(DIRECTION_3D_RIGHT);
	BIND_ENUM_CONSTANT(DIRECTION_3D_FORWARD);
	BIND_ENUM_CONSTANT(DIRECTION_3D_BACK);
	BIND_ENUM_CONSTANT(DIRECTION_3D_UP);
	BIND_ENUM_CONSTANT(DIRECTION_3D_DOWN);

	BIND_ENUM_CONSTANT(DIRECTION_HORIZONTAL_LEFT);
	BIND_ENUM_CONSTANT(DIRECTION_HORIZONTAL_RIGHT);
	BIND_ENUM_CONSTANT(DIRECTION_HORIZONTAL_FORWARD);
	BIND_ENUM_CONSTANT(DIRECTION_HORIZONTAL_BACK);

	ClassDB::bind_method(D_METHOD("get_3d_side_opposite", "side"), &DirectionHelper::get_3d_side_opposite);
	ClassDB::bind_method(D_METHOD("get_3d_direction_angle_deg", "direction"), &DirectionHelper::get_3d_direction_angle_deg);
	ClassDB::bind_method(D_METHOD("get_3d_direction_angle_rad", "direction"), &DirectionHelper::get_3d_direction_angle_rad);
	ClassDB::bind_method(D_METHOD("get_3d_direction_offset", "direction"), &DirectionHelper::get_3d_direction_offset);
	ClassDB::bind_method(D_METHOD("get_3d_direction_opposite", "direction"), &DirectionHelper::get_3d_direction_opposite);
	ClassDB::bind_method(D_METHOD("get_horizontal_direction_angle_deg", "direction"), &DirectionHelper::get_horizontal_direction_angle_deg);
	ClassDB::bind_method(D_METHOD("get_horizontal_direction_angle_rad", "direction"), &DirectionHelper::get_horizontal_direction_angle_rad);
	ClassDB::bind_method(D_METHOD("get_horizontal_direction_rotated", "current", "by"), &DirectionHelper::get_horizontal_direction_rotated);
	ClassDB::bind_method(D_METHOD("get_horizontal_direction_opposite", "direction"), &DirectionHelper::get_horizontal_direction_opposite);
	ClassDB::bind_method(D_METHOD("get_horizontal_direction_offset", "direction"), &DirectionHelper::get_horizontal_direction_offset);
}


DirectionHelper::Side3D DirectionHelper::get_3d_side_opposite(const Side3D p_side) const
{
	switch (p_side)
	{
		case SIDE_3D_LEFT:
			return SIDE_3D_RIGHT;
		case SIDE_3D_RIGHT:
			return SIDE_3D_LEFT;
		case SIDE_3D_FRONT:
			return SIDE_3D_REAR;
		case SIDE_3D_REAR:
			return SIDE_3D_FRONT;
		case SIDE_3D_TOP:
			return SIDE_3D_BOTTOM;
		case SIDE_3D_BOTTOM:
			return SIDE_3D_TOP;
		default:
			ERR_FAIL_V_MSG(SIDE_3D_LEFT, "Invalid side " + itos(p_side) + ".");
	}
}


Vector3 DirectionHelper::get_3d_direction_angle_deg(const Direction3D p_direction) const
{
	switch (p_direction) {
		case DIRECTION_3D_FORWARD:
			return Vector3(0, 0, 0);
		case DIRECTION_3D_LEFT:
			return Vector3(0, 90, 0);
		case DIRECTION_3D_BACK:
			return Vector3(0, 180, 0);
		case DIRECTION_3D_RIGHT:
			return Vector3(0, 270, 0);
		case DIRECTION_3D_UP:
			return Vector3(90, 0, 0);
		case DIRECTION_3D_DOWN:
			return Vector3(-90, 0, 0);
		default:
			ERR_FAIL_V_MSG(Vector3(), "Invalid direction " + itos(p_direction) + ".");
	}
}


Vector3 DirectionHelper::get_3d_direction_angle_rad(const Direction3D p_direction) const
{
	Vector3 deg = get_3d_direction_angle_deg(p_direction);
	return Vector3(Math::deg_to_rad(deg.x), Math::deg_to_rad(deg.y), Math::deg_to_rad(deg.z));
}


Vector3i DirectionHelper::get_3d_direction_offset(const Direction3D p_direction) const
{
	switch (p_direction)
	{
		case DIRECTION_3D_LEFT:
			return Vector3i(-1, 0, 0);
		case DIRECTION_3D_RIGHT:
			return Vector3i(1, 0, 0);
		case DIRECTION_3D_FORWARD:
			return Vector3i(0, 0, -1);
		case DIRECTION_3D_BACK:
			return Vector3i(0, 0, 1);
		case DIRECTION_3D_UP:
			return Vector3i(0, 1, 0);
		case DIRECTION_3D_DOWN:
			return Vector3i(0, -1, 0);
		default:
			ERR_FAIL_V_MSG(Vector3i(), "Invalid direction " + itos(p_direction) + ".");
	}
}


DirectionHelper::Direction3D DirectionHelper::get_3d_direction_opposite(const Direction3D p_direction) const
{
	switch (p_direction)
	{
		case DIRECTION_3D_LEFT:
			return DIRECTION_3D_RIGHT;
		case DIRECTION_3D_RIGHT:
			return DIRECTION_3D_LEFT;
		case DIRECTION_3D_FORWARD:
			return DIRECTION_3D_BACK;
		case DIRECTION_3D_BACK:
			return DIRECTION_3D_FORWARD;
		case DIRECTION_3D_UP:
			return DIRECTION_3D_DOWN;
		case DIRECTION_3D_DOWN:
			return DIRECTION_3D_UP;
		default:
			ERR_FAIL_V_MSG(DIRECTION_3D_LEFT, "Invalid direction " + itos(p_direction) + ".");
	}
}


real_t DirectionHelper::get_horizontal_direction_angle_deg(const DirectionHorizontal p_direction) const
{
	switch (p_direction)
	{
		case DIRECTION_HORIZONTAL_FORWARD:
			return 0;
		case DIRECTION_HORIZONTAL_BACK:
			return 180;
		case DIRECTION_HORIZONTAL_LEFT:
			return 90;
		case DIRECTION_HORIZONTAL_RIGHT:
			return 270;
		default:
			ERR_FAIL_V_MSG(0, "Invalid direction " + itos(p_direction) + ".");
	}
}


real_t DirectionHelper::get_horizontal_direction_angle_rad(const DirectionHorizontal p_direction) const
{
	real_t deg = get_horizontal_direction_angle_deg(p_direction);
	return Math::deg_to_rad(deg);
}


DirectionHelper::DirectionHorizontal DirectionHelper::get_horizontal_direction_rotated(const DirectionHorizontal p_current, const DirectionHorizontal p_by) const
{
	switch (p_by)
	{
		case DIRECTION_HORIZONTAL_BACK:
			switch (p_current)
			{
				case DIRECTION_HORIZONTAL_FORWARD:
					return DIRECTION_HORIZONTAL_BACK;
				case DIRECTION_HORIZONTAL_BACK:
					return DIRECTION_HORIZONTAL_FORWARD;
				case DIRECTION_HORIZONTAL_LEFT:
					return DIRECTION_HORIZONTAL_RIGHT;
				case DIRECTION_HORIZONTAL_RIGHT:
					return DIRECTION_HORIZONTAL_LEFT;
				default:
					ERR_FAIL_V_MSG(p_current, "Attempted to rotate invalid direction " + itos(p_current) + " back.");
			}
		case DIRECTION_HORIZONTAL_LEFT:
			switch (p_current)
			{
				case DIRECTION_HORIZONTAL_FORWARD:
					return DIRECTION_HORIZONTAL_LEFT;
				case DIRECTION_HORIZONTAL_BACK:
					return DIRECTION_HORIZONTAL_RIGHT;
				case DIRECTION_HORIZONTAL_LEFT:
					return DIRECTION_HORIZONTAL_BACK;
				case DIRECTION_HORIZONTAL_RIGHT:
					return DIRECTION_HORIZONTAL_FORWARD;
				default:
					ERR_FAIL_V_MSG(p_current, "Attempted to rotate invalid direction " + itos(p_current) + " left.");
			}
		case DIRECTION_HORIZONTAL_RIGHT:
			switch (p_current)
			{
				case DIRECTION_HORIZONTAL_FORWARD:
					return DIRECTION_HORIZONTAL_RIGHT;
				case DIRECTION_HORIZONTAL_BACK:
					return DIRECTION_HORIZONTAL_LEFT;
				case DIRECTION_HORIZONTAL_LEFT:
					return DIRECTION_HORIZONTAL_FORWARD;
				case DIRECTION_HORIZONTAL_RIGHT:
					return DIRECTION_HORIZONTAL_BACK;
				default:
					ERR_FAIL_V_MSG(p_current, "Attempted to rotate invalid direction " + itos(p_current) + " right.");
			}
		case DIRECTION_HORIZONTAL_FORWARD:
			ERR_FAIL_V_MSG(p_current, "Do not rotate a direction forward. It is pointless.");
		default:
			ERR_FAIL_V_MSG(p_current, "Attempted to rotate " + itos(p_current) + " by invalid direction: " + itos(p_by));
	}
}


DirectionHelper::DirectionHorizontal DirectionHelper::get_horizontal_direction_opposite(const DirectionHorizontal p_direction) const
{
	switch (p_direction)
	{
		case DIRECTION_HORIZONTAL_FORWARD:
			return DIRECTION_HORIZONTAL_BACK;
		case DIRECTION_HORIZONTAL_BACK:
			return DIRECTION_HORIZONTAL_FORWARD;
		case DIRECTION_HORIZONTAL_LEFT:
			return DIRECTION_HORIZONTAL_RIGHT;
		case DIRECTION_HORIZONTAL_RIGHT:
			return DIRECTION_HORIZONTAL_LEFT;
		default:
			ERR_FAIL_V_MSG(p_direction, "Invalid direction: " + itos(p_direction));
	}
}


Vector2i DirectionHelper::get_horizontal_direction_offset(const DirectionHorizontal p_direction) const
{
	switch (p_direction)
	{
		case DIRECTION_HORIZONTAL_FORWARD:
			return Vector2i(0, -1);
		case DIRECTION_HORIZONTAL_BACK:
			return Vector2i(0, 1);
		case DIRECTION_HORIZONTAL_LEFT:
			return Vector2i(-1, 0);
		case DIRECTION_HORIZONTAL_RIGHT:
			return Vector2i(1, 0);
		default:
			ERR_FAIL_V_MSG(Vector2i(), "Invalid direction: " + itos(p_direction));
	}
}


DirectionHelper::DirectionHelper()
{
	CRASH_COND_MSG(singleton != nullptr, "DirectionHelper is a singleton. Do not instantiate it multiple times.");
	singleton = this;
}


DirectionHelper::~DirectionHelper()
{
	singleton = nullptr;
}
