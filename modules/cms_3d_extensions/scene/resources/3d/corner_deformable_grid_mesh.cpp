// TODO:
// - Fix Face-X Orientation
// - Add Normals, Tangents
// - See if it's possible to empty the grid without printing an error
// - Add subdivision support

#include "corner_deformable_grid_mesh.h"


Size2 CornerDeformableGridMesh::calculate_horizontal_size() const
{
	return Size2((grid_size.width * cell_size.width) + (MAX(0, grid_size.width - 1) * cell_separation), (grid_size.height * cell_size.height) + (MAX(0, grid_size.height - 1) * cell_separation));
}


void CornerDeformableGridMesh::update_grid_array_size()
{
	cell_array.resize(grid_size.width);
	for (int x = 0; x < grid_size.width; x++)
	{
		cell_array[x].resize(grid_size.height);
	}
}


void CornerDeformableGridMesh::_update_lightmap_size()
{
	if (get_add_uv2())
	{
		const Size2 size_h = calculate_horizontal_size();
		const float texel_size = get_lightmap_texel_size();
		const float padding = get_uv2_padding();

		const Size2i lm_hint = Size2i(Size2(MAX(1.0, (size_h.width / texel_size) + padding), MAX(1.0, (size_h.height / texel_size) + padding)).ceil());
		set_lightmap_size_hint(lm_hint);
	}
}


void CornerDeformableGridMesh::_create_mesh_array(Array &p_arr) const
{
	const Size2 horizontal_size = calculate_horizontal_size();
	Vector3 base_offset;

	if (!use_top_left)
	{
		base_offset -= (Vector3(horizontal_size.width, 0, horizontal_size.height) * 0.5);
	}

	PackedVector3Array list_vertex;
	// PackedVector3Array list_normal;
	// PackedFloat32Array list_tangent;
	PackedVector2Array list_uv;
	PackedInt32Array list_index;

	int cell_count_total = grid_size.width * grid_size.height;

	list_vertex.resize(cell_count_total * 4);
	// list_normal.resize(cell_count_total * 4);
	// list_tangent.resize(cell_count_total * 16);
	list_uv.resize(cell_count_total * 4);
	list_index.resize(cell_count_total * 6);

	int point_4 = 0;
	int point_6 = 0;

	for (int x = 0; x < grid_size.width; x++)
	{
		for (int y = 0; y < grid_size.height; y++)
		{
			const int point_4_0 = point_4;
			const int point_4_1 = point_4 + 1;
			const int point_4_2 = point_4 + 2;
			const int point_4_3 = point_4 + 3;

			list_index.write[point_6++] = point_4_0;
			list_index.write[point_6++] = point_4_1;
			list_index.write[point_6++] = point_4_2;
			list_index.write[point_6++] = point_4_0;
			list_index.write[point_6++] = point_4_2;
			list_index.write[point_6++] = point_4_3;

			const Cell &cell = cell_array[x][y];
			Vector3 cell_offset = base_offset + Vector3((x * cell_size.width) + (cell_separation * x), 0, (y * cell_size.height) + (cell_separation * y));

			switch (orientation)
			{
				case FACE_X: {
					SWAP(cell_offset.y, cell_offset.z);
					SWAP(cell_offset.x, cell_offset.z);
					cell_offset = -cell_offset;
					cell_offset += center_offset;

					list_vertex.write[point_4_0] = cell_offset + Vector3(cell.corner_deformations[CORNER_BOTTOM_LEFT], 0, cell_size.height);
					list_vertex.write[point_4_1] = cell_offset + Vector3(cell.corner_deformations[CORNER_TOP_LEFT], 0, 0);
					list_vertex.write[point_4_2] = cell_offset + Vector3(cell.corner_deformations[CORNER_TOP_RIGHT], -cell_size.width, 0);
					list_vertex.write[point_4_3] = cell_offset + Vector3(cell.corner_deformations[CORNER_BOTTOM_RIGHT], -cell_size.width, -cell_size.height);
				}
				case FACE_Z: {
					SWAP(cell_offset.y, cell_offset.z);
					cell_offset.y = -cell_offset.y;
					cell_offset += center_offset;

					list_vertex.write[point_4_0] = cell_offset + Vector3(0, -cell_size.height, cell.corner_deformations[CORNER_BOTTOM_LEFT]);
					list_vertex.write[point_4_1] = cell_offset + Vector3(0, 0, cell.corner_deformations[CORNER_TOP_LEFT]);
					list_vertex.write[point_4_2] = cell_offset + Vector3(cell_size.width, 0, cell.corner_deformations[CORNER_TOP_RIGHT]);
					list_vertex.write[point_4_3] = cell_offset + Vector3(cell_size.width, -cell_size.height, cell.corner_deformations[CORNER_BOTTOM_RIGHT]);
				} break;
				case FACE_Y:
				default: {
					cell_offset += center_offset;

					list_vertex.write[point_4_0] = cell_offset + Vector3(0, cell.corner_deformations[CORNER_BOTTOM_LEFT], cell_size.height);
					list_vertex.write[point_4_1] = cell_offset + Vector3(0, cell.corner_deformations[CORNER_TOP_LEFT], 0);
					list_vertex.write[point_4_2] = cell_offset + Vector3(cell_size.width, cell.corner_deformations[CORNER_TOP_RIGHT], 0);
					list_vertex.write[point_4_3] = cell_offset + Vector3(cell_size.width, cell.corner_deformations[CORNER_BOTTOM_RIGHT], cell_size.height);
				} break;
			}

			list_uv.write[point_4_0] = Point2(cell.uv.position.x, cell.uv.position.y + cell.uv.size.height);
			list_uv.write[point_4_1] = cell.uv.position;
			list_uv.write[point_4_2] = Point2(cell.uv.position.x + cell.uv.size.width, cell.uv.position.y);
			list_uv.write[point_4_3] = cell.uv.position + cell.uv.size;

			point_4 += 4;
		}
	}

	p_arr[RenderingServer::ARRAY_VERTEX] = list_vertex;
	p_arr[RenderingServer::ARRAY_TEX_UV] = list_uv;
	p_arr[RenderingServer::ARRAY_INDEX] = list_index;
}


void CornerDeformableGridMesh::set_grid_size(const Size2i &p_size)
{
	ERR_FAIL_COND_MSG(p_size.width < 1 || p_size.height < 1, RTR("Grid size must be positive."));

	if (p_size == grid_size)
	{
		return;
	}

	grid_size = p_size;
	update_grid_array_size();

	notify_property_list_changed();
	_update_lightmap_size();
	request_update();
}


Size2i CornerDeformableGridMesh::get_grid_size() const
{
	return grid_size;
}


void CornerDeformableGridMesh::set_cell_size(const Size2 &p_size)
{
	ERR_FAIL_COND_MSG(p_size.width < 0.001 || p_size.height < 0.001, RTR("Cell size must be positive."));
	cell_size = p_size;
	_update_lightmap_size();
	request_update();
}


Size2 CornerDeformableGridMesh::get_cell_size() const
{
	return cell_size;
}


void CornerDeformableGridMesh::set_cell_separation(const real_t p_distance)
{
	ERR_FAIL_COND_MSG(p_distance < 0, RTR("Cell separation cannot be negative."));

	cell_separation = p_distance;
	_update_lightmap_size();
	request_update();
}


real_t CornerDeformableGridMesh::get_cell_separation() const
{
	return cell_separation;
}


void CornerDeformableGridMesh::set_center_offset(const Vector3 p_offset)
{
	center_offset = p_offset;
	request_update();
}


Vector3 CornerDeformableGridMesh::get_center_offset() const
{
	return center_offset;
}


void CornerDeformableGridMesh::set_use_top_left(const bool p_enabled)
{
	if (p_enabled == use_top_left)
	{
		return;
	}
	use_top_left = p_enabled;
	request_update();
}


bool CornerDeformableGridMesh::is_use_top_left() const
{
	return use_top_left;
}


void CornerDeformableGridMesh::set_orientation(const Orientation p_orientation)
{
	if (p_orientation == orientation)
	{
		return;
	}
	orientation = p_orientation;
	request_update();
}


CornerDeformableGridMesh::Orientation CornerDeformableGridMesh::get_orientation() const
{
	return orientation;
}


void CornerDeformableGridMesh::set_cell_uv(const Point2i &p_cell, const Rect2 &p_uv)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].uv = p_uv;
	request_update();
}


Rect2 CornerDeformableGridMesh::get_cell_uv(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, Rect2());
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, Rect2());

	return cell_array[p_cell.x][p_cell.y].uv;
}


void CornerDeformableGridMesh::set_cell_corner_deformations(const Point2i &p_cell, const Vector4 &p_deformations)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].corner_deformations = p_deformations;
	request_update();
}


Vector4 CornerDeformableGridMesh::get_cell_corner_deformations(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, Vector4());
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, Vector4());

	return cell_array[p_cell.x][p_cell.y].corner_deformations;
}


void CornerDeformableGridMesh::set_cell_corner_deformation_top_left(const Point2i &p_cell, const real_t p_deformation)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_TOP_LEFT] = p_deformation;
	request_update();
}


real_t CornerDeformableGridMesh::get_cell_corner_deformation_top_left(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, 0);
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, 0);

	return cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_TOP_LEFT];
}


void CornerDeformableGridMesh::set_cell_corner_deformation_top_right(const Point2i &p_cell, const real_t p_deformation)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_TOP_RIGHT] = p_deformation;
	request_update();
}


real_t CornerDeformableGridMesh::get_cell_corner_deformation_top_right(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, 0);
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, 0);

	return cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_TOP_RIGHT];
}


void CornerDeformableGridMesh::set_cell_corner_deformation_bottom_right(const Point2i &p_cell, const real_t p_deformation)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_BOTTOM_RIGHT] = p_deformation;
	request_update();
}


real_t CornerDeformableGridMesh::get_cell_corner_deformation_bottom_right(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, 0);
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, 0);

	return cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_BOTTOM_RIGHT];
}


void CornerDeformableGridMesh::set_cell_corner_deformation_bottom_left(const Point2i &p_cell, const real_t p_deformation)
{
	ERR_FAIL_INDEX(p_cell.x, grid_size.width);
	ERR_FAIL_INDEX(p_cell.y, grid_size.height);

	cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_BOTTOM_LEFT] = p_deformation;
	request_update();
}


real_t CornerDeformableGridMesh::get_cell_corner_deformation_bottom_left(const Point2i &p_cell) const
{
	ERR_FAIL_INDEX_V(p_cell.x, grid_size.width, 0);
	ERR_FAIL_INDEX_V(p_cell.y, grid_size.height, 0);

	return cell_array[p_cell.x][p_cell.y].corner_deformations[CORNER_BOTTOM_LEFT];
}


bool CornerDeformableGridMesh::_set(const StringName &p_name, const Variant &p_value)
{
	const String name = p_name;
	if (name.begins_with("cells/"))
	{
		const Point2i cell_point = Point2i(name.get_slicec('/', 1).to_int(), name.get_slicec('/', 2).to_int());

		ERR_FAIL_INDEX_V(cell_point.x, grid_size.width, false);
		ERR_FAIL_INDEX_V(cell_point.y, grid_size.height, false);

		const String cell_property = name.get_slicec('/', 3);

		if (cell_property == "uv")
		{
			set_cell_uv(cell_point, p_value);
			return true;
		}
		else if (cell_property == "corner_deformations")
		{
			const String cell_corner = name.get_slicec('/', 4);

			if (cell_corner.is_empty())
			{
				set_cell_corner_deformations(cell_point, p_value);
				return true;
			}
			else
			{
				if (cell_corner == "top_left")
				{
					set_cell_corner_deformation_top_left(cell_point, p_value);
					return true;
				}
				else if (cell_corner == "top_right")
				{
					set_cell_corner_deformation_top_right(cell_point, p_value);
					return true;
				}
				else if (cell_corner == "bottom_right")
				{
					set_cell_corner_deformation_bottom_right(cell_point, p_value);
					return true;
				}
				else if (cell_corner == "bottom_left")
				{
					set_cell_corner_deformation_bottom_left(cell_point, p_value);
					return true;
				}
			}
		}
	}
	return false;
}


bool CornerDeformableGridMesh::_get(const StringName &p_name, Variant &r_ret) const
{
	const String name = p_name;

	if (name.begins_with("cells/"))
	{
		const Point2i cell_point = Point2i(name.get_slicec('/', 1).to_int(), name.get_slicec('/', 2).to_int());

		ERR_FAIL_INDEX_V(cell_point.x, grid_size.width, false);
		ERR_FAIL_INDEX_V(cell_point.y, grid_size.height, false);

		const String cell_property = name.get_slicec('/', 3);

		if (cell_property == "uv")
		{
			r_ret = get_cell_uv(cell_point);
			return true;
		}
		else if (cell_property == "corner_deformations")
		{
			const String cell_corner = name.get_slicec('/', 4);

			if (cell_corner.is_empty())
			{
				r_ret = get_cell_corner_deformations(cell_point);
				return true;
			}
			else
			{
				if (cell_corner == "top_left")
				{
					r_ret = get_cell_corner_deformation_top_left(cell_point);
					return true;
				}
				else if (cell_corner == "top_right")
				{
					r_ret = get_cell_corner_deformation_top_right(cell_point);
					return true;
				}
				else if (cell_corner == "bottom_right")
				{
					r_ret = get_cell_corner_deformation_bottom_right(cell_point);
					return true;
				}
				else if (cell_corner == "bottom_left")
				{
					r_ret = get_cell_corner_deformation_bottom_left(cell_point);
					return true;
				}
			}
		}
	}

	return false;
}


bool CornerDeformableGridMesh::_property_can_revert(const StringName &p_name) const
{
	const String name = p_name;

	if (name.begins_with("cells/"))
	{
		const String cell_property = name.get_slicec('/', 3);

		if (cell_property == "uv")
		{
			return true;
		}
		else if (cell_property == "corner_deformations")
		{
			const String cell_corner = name.get_slicec('/', 4);

			if (cell_corner.is_empty())
			{
				return true;
			}
			else
			{
				return cell_corner == "top_left" || cell_corner == "top_right" || cell_corner == "bottom_right" || cell_corner == "bottom_left";
			}
		}
	}

	return false;
}


bool CornerDeformableGridMesh::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	const String name = p_name;

	if (name.begins_with("cells/"))
	{
		const String cell_property = name.get_slicec('/', 3);

		if (cell_property == "uv")
		{
			r_property = Rect2(0, 0, 1, 1);
			return true;
		}
		else if (cell_property == "corner_deformations")
		{
			const String cell_corner = name.get_slicec('/', 4);

			if (cell_corner.is_empty())
			{
				r_property = Vector4(0, 0, 0, 0);
				return true;
			}
			else
			{
				if (cell_corner == "top_left" || cell_corner == "top_right" || cell_corner == "bottom_right" || cell_corner == "bottom_left")
				{
					r_property = real_t(0);
					return true;
				}
			}
		}
	}

	return false;
}


void CornerDeformableGridMesh::_get_property_list(List<PropertyInfo> *p_list) const
{
	for (int x = 0; x < grid_size.width; x++)
	{
		for (int y = 0; y < grid_size.height; y++)
		{
			const String prefix = "cells/" + itos(x) + "/" + itos(y) + "/";

			p_list->push_back(PropertyInfo(Variant::RECT2, prefix + "uv"));
			p_list->push_back(PropertyInfo(Variant::VECTOR4, prefix + "corner_deformations", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_NO_EDITOR));

			p_list->push_back(PropertyInfo(Variant::FLOAT, prefix + "corner_deformations/top_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR));
			p_list->push_back(PropertyInfo(Variant::FLOAT, prefix + "corner_deformations/top_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR));
			p_list->push_back(PropertyInfo(Variant::FLOAT, prefix + "corner_deformations/bottom_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR));
			p_list->push_back(PropertyInfo(Variant::FLOAT, prefix + "corner_deformations/bottom_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR));
		}
	}
}


void CornerDeformableGridMesh::_bind_methods()
{
	BIND_ENUM_CONSTANT(FACE_X);
	BIND_ENUM_CONSTANT(FACE_Y);
	BIND_ENUM_CONSTANT(FACE_Z);

	ClassDB::bind_method(D_METHOD("set_grid_size", "size"), &CornerDeformableGridMesh::set_grid_size);
	ClassDB::bind_method(D_METHOD("get_grid_size"), &CornerDeformableGridMesh::get_grid_size);
	ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &CornerDeformableGridMesh::set_cell_size);
	ClassDB::bind_method(D_METHOD("get_cell_size"), &CornerDeformableGridMesh::get_cell_size);
	ClassDB::bind_method(D_METHOD("set_cell_separation", "distance"), &CornerDeformableGridMesh::set_cell_separation);
	ClassDB::bind_method(D_METHOD("get_cell_separation"), &CornerDeformableGridMesh::get_cell_separation);
	ClassDB::bind_method(D_METHOD("set_center_offset", "offset"), &CornerDeformableGridMesh::set_center_offset);
	ClassDB::bind_method(D_METHOD("get_center_offset"), &CornerDeformableGridMesh::get_center_offset);
	ClassDB::bind_method(D_METHOD("set_use_top_left", "enabled"), &CornerDeformableGridMesh::set_use_top_left);
	ClassDB::bind_method(D_METHOD("is_use_top_left"), &CornerDeformableGridMesh::is_use_top_left);
	ClassDB::bind_method(D_METHOD("set_orientation", "direction"), &CornerDeformableGridMesh::set_orientation);
	ClassDB::bind_method(D_METHOD("get_orientation"), &CornerDeformableGridMesh::get_orientation);

	ClassDB::bind_method(D_METHOD("set_cell_uv", "cell", "uv"), &CornerDeformableGridMesh::set_cell_uv);
	ClassDB::bind_method(D_METHOD("get_cell_uv", "cell"), &CornerDeformableGridMesh::get_cell_uv);
	ClassDB::bind_method(D_METHOD("set_cell_corner_deformations", "cell", "deformations"), &CornerDeformableGridMesh::set_cell_corner_deformations);
	ClassDB::bind_method(D_METHOD("get_cell_corner_deformations", "cell"), &CornerDeformableGridMesh::get_cell_corner_deformations);
	ClassDB::bind_method(D_METHOD("set_cell_corner_deformation_top_left", "cell", "deformation"), &CornerDeformableGridMesh::set_cell_corner_deformation_top_left);
	ClassDB::bind_method(D_METHOD("get_cell_corner_deformation_top_left", "cell"), &CornerDeformableGridMesh::get_cell_corner_deformation_top_left);
	ClassDB::bind_method(D_METHOD("set_cell_corner_deformation_top_right", "cell", "deformation"), &CornerDeformableGridMesh::set_cell_corner_deformation_top_right);
	ClassDB::bind_method(D_METHOD("get_cell_corner_deformation_top_right", "cell"), &CornerDeformableGridMesh::get_cell_corner_deformation_top_right);
	ClassDB::bind_method(D_METHOD("set_cell_corner_deformation_bottom_right", "cell", "deformation"), &CornerDeformableGridMesh::set_cell_corner_deformation_bottom_right);
	ClassDB::bind_method(D_METHOD("get_cell_corner_deformation_bottom_right", "cell"), &CornerDeformableGridMesh::get_cell_corner_deformation_bottom_right);
	ClassDB::bind_method(D_METHOD("set_cell_corner_deformation_bottom_left", "cell", "deformation"), &CornerDeformableGridMesh::set_cell_corner_deformation_bottom_left);
	ClassDB::bind_method(D_METHOD("get_cell_corner_deformation_bottom_left", "cell"), &CornerDeformableGridMesh::get_cell_corner_deformation_bottom_left);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "grid_size"), "set_grid_size", "get_grid_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cell_size", PROPERTY_HINT_NONE, "suffix:m"), "set_cell_size", "get_cell_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_separation", PROPERTY_HINT_NONE, "suffix:m"), "set_cell_separation", "get_cell_separation");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "center_offset", PROPERTY_HINT_NONE, "suffix:m"), "set_center_offset", "get_center_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_top_left"), "set_use_top_left", "is_use_top_left");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "orientation", PROPERTY_HINT_ENUM, "Face-X,Face-Y,Face-Z"), "set_orientation", "get_orientation");
}


CornerDeformableGridMesh::CornerDeformableGridMesh()
{
	update_grid_array_size();
}
