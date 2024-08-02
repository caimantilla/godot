#include "side_3d_int_struct.hpp"
#include "core/config/engine.h"


void Side3DIntStruct::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_left", "left"), &Side3DIntStruct::set_left);
	ClassDB::bind_method(D_METHOD("get_left"), &Side3DIntStruct::get_left);
	ClassDB::bind_method(D_METHOD("set_right", "right"), &Side3DIntStruct::set_right);
	ClassDB::bind_method(D_METHOD("get_right"), &Side3DIntStruct::get_right);
	ClassDB::bind_method(D_METHOD("set_front", "front"), &Side3DIntStruct::set_front);
	ClassDB::bind_method(D_METHOD("get_front"), &Side3DIntStruct::get_front);
	ClassDB::bind_method(D_METHOD("set_rear", "rear"), &Side3DIntStruct::set_rear);
	ClassDB::bind_method(D_METHOD("get_rear"), &Side3DIntStruct::get_rear);
	ClassDB::bind_method(D_METHOD("set_top", "top"), &Side3DIntStruct::set_top);
	ClassDB::bind_method(D_METHOD("get_top"), &Side3DIntStruct::get_top);
	ClassDB::bind_method(D_METHOD("set_bottom", "bottom"), &Side3DIntStruct::set_bottom);
	ClassDB::bind_method(D_METHOD("get_bottom"), &Side3DIntStruct::get_bottom);

	ClassDB::bind_method(D_METHOD("set_side", "side", "value"), &Side3DIntStruct::set_side);
	ClassDB::bind_method(D_METHOD("get_side", "side"), &Side3DIntStruct::get_side);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "left"), "set_left", "get_left");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "right"), "set_right", "get_right");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "front"), "set_front", "get_front");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rear"), "set_rear", "get_rear");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "top"), "set_top", "get_top");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bottom"), "set_bottom", "get_bottom");
}


void Side3DIntStruct::set_left(const int p_left)
{
	if (p_left == left)
		return;
	left = p_left;
	handle_change();
}


int Side3DIntStruct::get_left() const
{
	return left;
}


void Side3DIntStruct::set_right(const int p_right)
{
	if (p_right == right)
		return;
	right = p_right;
	handle_change();
}


int Side3DIntStruct::get_right() const
{
	return right;
}


void Side3DIntStruct::set_front(const int p_front)
{
	if (p_front == front)
		return;
	front = p_front;
	handle_change();
}


int Side3DIntStruct::get_front() const
{
	return front;
}


void Side3DIntStruct::set_rear(const int p_rear)
{
	if (p_rear == rear)
		return;
	rear = p_rear;
	handle_change();
}


int Side3DIntStruct::get_rear() const
{
	return rear;
}


void Side3DIntStruct::set_top(const int p_top)
{
	if (p_top == top)
		return;
	top = p_top;
	handle_change();
}


int Side3DIntStruct::get_top() const
{
	return top;
}


void Side3DIntStruct::set_bottom(const int p_bottom)
{
	if (p_bottom == bottom)
		return;
	bottom = p_bottom;
	handle_change();
}


int Side3DIntStruct::get_bottom() const
{
	return bottom;
}


void Side3DIntStruct::set_side(const DirectionHelper::Side3D p_side, const int p_value)
{
	switch (p_side)
	{
		case DirectionHelper::SIDE_3D_LEFT:
			set_left(p_value);
			break;
		case DirectionHelper::SIDE_3D_RIGHT:
			set_right(p_value);
			break;
		case DirectionHelper::SIDE_3D_FRONT:
			set_front(p_value);
			break;
		case DirectionHelper::SIDE_3D_REAR:
			set_rear(p_value);
			break;
		case DirectionHelper::SIDE_3D_TOP:
			set_top(p_value);
			break;
		case DirectionHelper::SIDE_3D_BOTTOM:
			set_bottom(p_value);
			break;
		default:
			ERR_FAIL();
	}
}


int Side3DIntStruct::get_side(const DirectionHelper::Side3D p_side) const
{
	switch (p_side)
	{
		case DirectionHelper::SIDE_3D_LEFT:
			return left;
		case DirectionHelper::SIDE_3D_RIGHT:
			return right;
		case DirectionHelper::SIDE_3D_FRONT:
			return front;
		case DirectionHelper::SIDE_3D_REAR:
			return rear;
		case DirectionHelper::SIDE_3D_TOP:
			return top;
		case DirectionHelper::SIDE_3D_BOTTOM:
			return bottom;
		default:
			ERR_FAIL_V(0);
	}
}


void Side3DIntStruct::handle_change()
{
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
	{
		String new_name = "Left: " + itos(left) + "\nRight: " + itos(right) + "\nFront: " + itos(front) + "\nRear: " + itos(rear) + "\nTop: " + itos(top) + "\nBottom: " + itos(bottom);
		if (new_name != get_name())
		{
			set_name(new_name);
			return;
		}
	}
#endif
	emit_changed();
}
