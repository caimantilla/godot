#include "side_3d_bit_field_struct.hpp"
#include "core/config/engine.h"


void Side3DBitFieldStruct::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_bits", "bits"), &Side3DBitFieldStruct::set_bits);
	ClassDB::bind_method(D_METHOD("get_bits"), &Side3DBitFieldStruct::get_bits);
	ClassDB::bind_method(D_METHOD("set_side_enabled", "side", "enabled"), &Side3DBitFieldStruct::set_side_enabled);
	ClassDB::bind_method(D_METHOD("is_side_enabled", "side"), &Side3DBitFieldStruct::is_side_enabled);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "bits", PROPERTY_HINT_FLAGS, "Left,Right,Front,Rear,Top,Bottom"), "set_bits", "get_bits");
}


void Side3DBitFieldStruct::set_bits(const int p_bits)
{
	if (p_bits == bits)
		return;
	bits = p_bits;
	handle_change();
}


int Side3DBitFieldStruct::get_bits() const
{
	return bits;
}


void Side3DBitFieldStruct::set_side_enabled(const DirectionHelper::Side3D p_side, const bool p_enabled)
{
	if (p_enabled)
		set_bits(bits | (1 << p_side));
	else
		set_bits(bits &~ (1 << p_side));
}


bool Side3DBitFieldStruct::is_side_enabled(const DirectionHelper::Side3D p_side) const
{
	return (bits & (1 << p_side)) != 0;
}


void Side3DBitFieldStruct::handle_change()
{
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
	{
		PackedStringArray name_parts;

		if (is_side_enabled(DirectionHelper::SIDE_3D_LEFT))
			name_parts.push_back("Left");
		if (is_side_enabled(DirectionHelper::SIDE_3D_RIGHT))
			name_parts.push_back("Right");
		if (is_side_enabled(DirectionHelper::SIDE_3D_FRONT))
			name_parts.push_back("Front");
		if (is_side_enabled(DirectionHelper::SIDE_3D_REAR))
			name_parts.push_back("Rear");
		if (is_side_enabled(DirectionHelper::SIDE_3D_TOP))
			name_parts.push_back("Top");
		if (is_side_enabled(DirectionHelper::SIDE_3D_BOTTOM))
			name_parts.push_back("Bottom");
		
		String new_name = String("\n").join(name_parts);
		if (new_name != get_name())
		{
			set_name(new_name);
			return;
		}
	}
#endif
	emit_changed();
}
