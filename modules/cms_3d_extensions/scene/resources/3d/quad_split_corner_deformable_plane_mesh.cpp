// TODO:
// - Modify Normals and Tangents
// - Customize Lightmap generation

#include "quad_split_corner_deformable_plane_mesh.h"


void QuadSplitCornerDeformablePlaneMesh::_create_mesh_array(Array &p_arr) const
{
	PlaneMesh::_create_mesh_array(p_arr);

	const Vector3 plane_center = get_center_offset();
	const Size2 plane_size = get_size();
	const Vector4 corner_deformations = get_corner_deformations();

	Point2 sample_point_offset;
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

	sample_point_offset = plane_size * 0.5;

	PackedVector3Array list_vertex = p_arr[RenderingServer::ARRAY_VERTEX];

	Vector3 corner_sep_vertex_ofs_list[4] = {
		Vector3(-corner_separations[CORNER_TOP_LEFT], 0, -corner_separations[CORNER_TOP_LEFT]),
		Vector3(corner_separations[CORNER_TOP_RIGHT], 0, -corner_separations[CORNER_TOP_RIGHT]),
		Vector3(corner_separations[CORNER_BOTTOM_RIGHT], 0, corner_separations[CORNER_BOTTOM_RIGHT]),
		Vector3(-corner_separations[CORNER_BOTTOM_LEFT], 0, corner_separations[CORNER_BOTTOM_LEFT])
	};

	if (get_orientation() == FACE_Z)
	{
		for (int i = 0; i < 4; i++)
		{
			SWAP(corner_sep_vertex_ofs_list[i].y, corner_sep_vertex_ofs_list[i].z);
			corner_sep_vertex_ofs_list[i].y = -corner_sep_vertex_ofs_list[i].y;
		}
	}
	else if (get_orientation() == FACE_X)
	{
		for (int i = 0; i < 4; i++)
		{
			SWAP(corner_sep_vertex_ofs_list[i].y, corner_sep_vertex_ofs_list[i].z);
			SWAP(corner_sep_vertex_ofs_list[i].x, corner_sep_vertex_ofs_list[i].z);
			corner_sep_vertex_ofs_list[i] = -corner_sep_vertex_ofs_list[i];
		}
	}

	for (int vertex_idx = 0; vertex_idx < list_vertex.size(); vertex_idx++)
	{
		Vector3 vertex = list_vertex[vertex_idx] - plane_center;
		Vector3 vertex_ofs;

		switch (get_orientation())
		{
			case FACE_Z: {
				if (vertex.x < 0 && vertex.y > 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
				}
				else if (vertex.x > 0 && vertex.y > 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
				}
				else if (vertex.x > 0 && vertex.y < 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
				}
				else
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
				}
			} break;
			case FACE_X: {
				if (vertex.z > 0 && vertex.y > 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
				}
				else if (vertex.z < 0 && vertex.y > 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
				}
				else if (vertex.z < 0 && vertex.y < 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
				}
				else
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
				}
			} break;
			case FACE_Y:
			default: {
				if (vertex.x < 0 && vertex.z < 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_LEFT];
				}
				else if (vertex.x > 0 && vertex.z < 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_TOP_RIGHT];
				}
				else if (vertex.x > 0 && vertex.z > 0)
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_RIGHT];
				}
				else
				{
					vertex_ofs = corner_sep_vertex_ofs_list[CORNER_BOTTOM_LEFT];
				}
			} break;
		}

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

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "corner_separations", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_NO_EDITOR), "set_corner_separations", "get_corner_separation");

	ADD_GROUP("Corner Separations", "corner_separation_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_top_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_top_left", "get_corner_separation_top_left");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_top_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_top_right", "get_corner_separation_top_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_bottom_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_bottom_right", "get_corner_separation_bottom_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_separation_bottom_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_separation_bottom_left", "get_corner_separation_bottom_left");
}


QuadSplitCornerDeformablePlaneMesh::QuadSplitCornerDeformablePlaneMesh()
{
	set_subdivide_width(4);
	set_subdivide_depth(4);
}
