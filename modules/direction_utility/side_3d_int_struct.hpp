#ifndef SIDE_3D_INT_STRUCT_HPP
#define SIDE_3D_INT_STRUCT_HPP


#include "core/io/resource.h"
#include "direction_helper.hpp"


class Side3DIntStruct : public Resource
{
	GDCLASS(Side3DIntStruct, Resource);

private:
	int left = -1;
	int right = -1;
	int front = -1;
	int rear = -1;
	int top = -1;
	int bottom = -1;

	void handle_change();

protected:
	static void _bind_methods();

public:
	void set_left(const int p_left);
	int get_left() const;
	void set_right(const int p_right);
	int get_right() const;
	void set_front(const int p_front);
	int get_front() const;
	void set_rear(const int p_rear);
	int get_rear() const;
	void set_top(const int p_top);
	int get_top() const;
	void set_bottom(const int p_bottom);
	int get_bottom() const;

	void set_side(const DirectionHelper::Side3D p_side, const int p_value);
	int get_side(const DirectionHelper::Side3D p_side) const;
};


#endif // SIDE_3D_INT_STRUCT_HPP
