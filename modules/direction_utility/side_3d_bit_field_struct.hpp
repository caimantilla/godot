#ifndef SIDE_3D_BIT_FIELD_STRUCT_HPP
#define SIDE_3D_BIT_FIELD_STRUCT_HPP


#include "core/io/resource.h"
#include "direction_helper.hpp"


class Side3DBitFieldStruct : public Resource
{
	GDCLASS(Side3DBitFieldStruct, Resource);

private:
	int bits = 0;
	void handle_change();

protected:
	static void _bind_methods();

public:
	void set_bits(const int p_bits);
	int get_bits() const;
	void set_side_enabled(const DirectionHelper::Side3D p_side, const bool p_enabled);
	bool is_side_enabled(const DirectionHelper::Side3D p_side) const;
};


#endif // SIDE_3D_BIT_FIELD_STRUCT_HPP