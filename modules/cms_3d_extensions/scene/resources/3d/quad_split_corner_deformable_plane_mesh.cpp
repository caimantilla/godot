// TODO:
// - Add Normals and Tangents
// - Stop ignoring subdivision
// - Customize Lightmap generation

#include "quad_split_corner_deformable_plane_mesh.h"


void QuadSplitCornerDeformablePlaneMesh::_create_mesh_array(Array &p_arr) const
{
	const Vector3 plane_center = get_center_offset();
	const Size2 plane_size = get_size();
	const Size2 plane_size_half = plane_size * 0.5;
	const Vector4 corner_deformations = get_corner_deformations();

	PackedVector3Array list_vertex;
	PackedVector2Array list_uv;
	PackedInt32Array list_index;

	list_vertex.resize(16);
	list_uv.resize(16);
	list_index.resize(24);

	Point2 sample_point_offset = plane_size * 0.5;
	Vector2 sample_point_scale = Point2(1, 1) / plane_size;
	Vector3::Axis sample_axis_x;
	Vector3::Axis sample_axis_y;
	Vector3::Axis deform_axis;

	switch (get_orientation())
	{
		case FACE_X: {
			sample_point_scale = -sample_point_scale;
			sample_axis_x = Vector3::AXIS_Z;
			sample_axis_y = Vector3::AXIS_Y;
			deform_axis = Vector3::AXIS_X;
		} break;
		case FACE_Z: {
			sample_point_scale *= Vector2(1, -1);
			sample_axis_x = Vector3::AXIS_X;
			sample_axis_y = Vector3::AXIS_Y;
			deform_axis = Vector3::AXIS_Z;
		} break;
		case FACE_Y:
		default: {
			sample_axis_x = Vector3::AXIS_X;
			sample_axis_y = Vector3::AXIS_Z;
			deform_axis = Vector3::AXIS_Y;
		} break;
	}

	Vector3 corner_sep_vertex_ofs_list[4] = {
		Vector3(-corner_separations[CORNER_TOP_LEFT], 0, -corner_separations[CORNER_TOP_LEFT]),
		Vector3(corner_separations[CORNER_TOP_RIGHT], 0, -corner_separations[CORNER_TOP_RIGHT]),
		Vector3(corner_separations[CORNER_BOTTOM_RIGHT], 0, corner_separations[CORNER_BOTTOM_RIGHT]),
		Vector3(-corner_separations[CORNER_BOTTOM_LEFT], 0, corner_separations[CORNER_BOTTOM_LEFT])
	};


	int idx_vertex = 0;
	int idx_uv = 0;
	int idx_index = 0;
	int idx_index_pointer = 0;

	list_vertex.write[idx_vertex++] = Vector3(-plane_size_half.width, 0, 0) + corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
	list_vertex.write[idx_vertex++] = Vector3(-plane_size_half.width, 0, -plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
	list_vertex.write[idx_vertex++] = Vector3(0, 0, -plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
	list_vertex.write[idx_vertex++] = corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];

	list_vertex.write[idx_vertex++] = corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
	list_vertex.write[idx_vertex++] = Vector3(0, 0, -plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
	list_vertex.write[idx_vertex++] = Vector3(plane_size_half.width, 0, -plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
	list_vertex.write[idx_vertex++] = Vector3(plane_size_half.width, 0, 0) + corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];

	list_vertex.write[idx_vertex++] = Vector3(0, 0, plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
	list_vertex.write[idx_vertex++] = corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
	list_vertex.write[idx_vertex++] = Vector3(plane_size_half.width, 0, 0) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
	list_vertex.write[idx_vertex++] = Vector3(plane_size_half.width, 0, plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];

	list_vertex.write[idx_vertex++] = Vector3(-plane_size_half.width, 0, plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
	list_vertex.write[idx_vertex++] = Vector3(-plane_size_half.width, 0, 0) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
	list_vertex.write[idx_vertex++] = corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
	list_vertex.write[idx_vertex++] = Vector3(0, 0, plane_size_half.height) + corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];

	list_uv.write[idx_uv++] = Point2(0, 0.5);
	list_uv.write[idx_uv++] = Point2(0, 0);
	list_uv.write[idx_uv++] = Point2(0.5, 0);
	list_uv.write[idx_uv++] = Point2(0.5, 0.5);

	list_uv.write[idx_uv++] = Point2(0.5, 0.5);
	list_uv.write[idx_uv++] = Point2(0.5, 0);
	list_uv.write[idx_uv++] = Point2(1, 0);
	list_uv.write[idx_uv++] = Point2(1, 0.5);

	list_uv.write[idx_uv++] = Point2(0.5, 1);
	list_uv.write[idx_uv++] = Point2(0.5, 0.5);
	list_uv.write[idx_uv++] = Point2(1, 0.5);
	list_uv.write[idx_uv++] = Point2(1, 1);

	list_uv.write[idx_uv++] = Point2(0, 1);
	list_uv.write[idx_uv++] = Point2(0, 0.5);
	list_uv.write[idx_uv++] = Point2(0.5, 0.5);
	list_uv.write[idx_uv++] = Point2(0.5, 1);

	for (int i = 0; i < 4; i++)
	{
		list_index.write[idx_index++] = idx_index_pointer;
		list_index.write[idx_index++] = idx_index_pointer + 1;
		list_index.write[idx_index++] = idx_index_pointer + 2;
		list_index.write[idx_index++] = idx_index_pointer;
		list_index.write[idx_index++] = idx_index_pointer + 2;
		list_index.write[idx_index++] = idx_index_pointer + 3;

		idx_index_pointer += 4;
	}

	if (get_orientation() == FACE_Z)
	{
		for (int i = 0; i < list_vertex.size(); i++)
		{
			Vector3 vertex = list_vertex[i];
			SWAP(vertex.y, vertex.z);
			vertex.y = -vertex.y;
			list_vertex.write[i] = vertex;
		}
	}
	else if (get_orientation() == FACE_X)
	{
		for (int i = 0; i < list_vertex.size(); i++)
		{
			Vector3 vertex = list_vertex[i];
			SWAP(vertex.y, vertex.z);
			SWAP(vertex.x, vertex.z);
			vertex = -vertex;
			list_vertex.write[i] = vertex;
		}
	}

	for (int vertex_idx = 0; vertex_idx < list_vertex.size(); vertex_idx++)
	{
		Vector3 vertex = list_vertex[vertex_idx];
		Vector3 vertex_ofs;

		vertex += vertex_ofs;

		const Point2 sample_point = sample_point_scale * (sample_point_offset + Point2(vertex[sample_axis_x], vertex[sample_axis_y]));

		const real_t accum_deform = (
			Math::lerp(real_t(0), corner_deformations[CORNER_TOP_LEFT], real_t((1.0 - sample_point.x) * (1.0 - sample_point.y)))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_TOP_RIGHT], real_t(sample_point.x * (1.0 - sample_point.y)))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_BOTTOM_RIGHT], real_t(sample_point.x * sample_point.y))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_BOTTOM_LEFT], real_t((1.0 - sample_point.x) * sample_point.y))
		);

		vertex[deform_axis] += accum_deform;
		vertex += plane_center;

		list_vertex.write[vertex_idx] = vertex;
	}

	p_arr[RenderingServer::ARRAY_VERTEX] = list_vertex;
	p_arr[RenderingServer::ARRAY_TEX_UV] = list_uv;
	p_arr[RenderingServer::ARRAY_INDEX] = list_index;
}


void QuadSplitCornerDeformablePlaneMesh::set_corner_separations(const Vector4 p_corners)
{
	corner_separations = p_corners;
	_update_lightmap_size();
	request_update();
}


Vector4 QuadSplitCornerDeformablePlaneMesh::get_corner_separations() const
{
	return corner_separations;
}


void QuadSplitCornerDeformablePlaneMesh::set_corner_separation_top_left(const real_t p_distance)
{
	corner_separations[CORNER_TOP_LEFT] = p_distance;
	_update_lightmap_size();
	request_update();
}


real_t QuadSplitCornerDeformablePlaneMesh::get_corner_separation_top_left() const
{
	return corner_separations[CORNER_TOP_LEFT];
}


void QuadSplitCornerDeformablePlaneMesh::set_corner_separation_top_right(const real_t p_distance)
{
	corner_separations[CORNER_TOP_RIGHT] = p_distance;
	_update_lightmap_size();
	request_update();
}


real_t QuadSplitCornerDeformablePlaneMesh::get_corner_separation_top_right() const
{
	return corner_separations[CORNER_TOP_RIGHT];
}


void QuadSplitCornerDeformablePlaneMesh::set_corner_separation_bottom_right(const real_t p_distance)
{
	corner_separations[CORNER_BOTTOM_RIGHT] = p_distance;
	_update_lightmap_size();
	request_update();
}


real_t QuadSplitCornerDeformablePlaneMesh::get_corner_separation_bottom_right() const
{
	return corner_separations[CORNER_BOTTOM_RIGHT];
}


void QuadSplitCornerDeformablePlaneMesh::set_corner_separation_bottom_left(const real_t p_distance)
{
	corner_separations[CORNER_BOTTOM_LEFT] = p_distance;
	_update_lightmap_size();
	request_update();
}


real_t QuadSplitCornerDeformablePlaneMesh::get_corner_separation_bottom_left() const
{
	return corner_separations[CORNER_BOTTOM_LEFT];
}


void QuadSplitCornerDeformablePlaneMesh::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_corner_separations", "corners"), &QuadSplitCornerDeformablePlaneMesh::set_corner_separations);
	ClassDB::bind_method(D_METHOD("get_corner_separations"), &QuadSplitCornerDeformablePlaneMesh::get_corner_separations);
	ClassDB::bind_method(D_METHOD("set_corner_separation_top_left", "distance"), &QuadSplitCornerDeformablePlaneMesh::set_corner_separation_top_left);
	ClassDB::bind_method(D_METHOD("get_corner_separation_top_left"), &QuadSplitCornerDeformablePlaneMesh::get_corner_separation_top_left);
	ClassDB::bind_method(D_METHOD("set_corner_separation_top_right", "distance"), &QuadSplitCornerDeformablePlaneMesh::set_corner_separation_top_right);
	ClassDB::bind_method(D_METHOD("get_corner_separation_top_right"), &QuadSplitCornerDeformablePlaneMesh::get_corner_separation_top_right);
	ClassDB::bind_method(D_METHOD("set_corner_separation_bottom_right", "distance"), &QuadSplitCornerDeformablePlaneMesh::set_corner_separation_bottom_right);
	ClassDB::bind_method(D_METHOD("get_corner_separation_bottom_right"), &QuadSplitCornerDeformablePlaneMesh::get_corner_separation_bottom_right);
	ClassDB::bind_method(D_METHOD("set_corner_separation_bottom_left", "distance"), &QuadSplitCornerDeformablePlaneMesh::set_corner_separation_bottom_left);
	ClassDB::bind_method(D_METHOD("get_corner_separation_bottom_left"), &QuadSplitCornerDeformablePlaneMesh::get_corner_separation_bottom_left);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "corner_separations", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_NO_EDITOR), "set_corner_separations", "get_corner_separations");

	ADD_GROUP("Corner Separations", "corner_separation_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_top_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_top_left", "get_corner_separation_top_left");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_top_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_top_right", "get_corner_separation_top_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_bottom_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_bottom_right", "get_corner_separation_bottom_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_bottom_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_bottom_left", "get_corner_separation_bottom_left");
}


QuadSplitCornerDeformablePlaneMesh::QuadSplitCornerDeformablePlaneMesh()
{
}
