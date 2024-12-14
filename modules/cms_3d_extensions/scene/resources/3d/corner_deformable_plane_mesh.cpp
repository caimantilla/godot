// TODO:
// - Modify Normals and Tangents

#include "corner_deformable_plane_mesh.h"


void CornerDeformablePlaneMesh::_create_mesh_array(Array &p_arr) const
{
	PlaneMesh::_create_mesh_array(p_arr);

	const Vector3 plane_center = get_center_offset();
	const Size2 plane_size = get_size();

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

	for (int vertex_idx = 0; vertex_idx < list_vertex.size(); vertex_idx++)
	{
		const Vector3 vertex = list_vertex[vertex_idx] - plane_center;
		const Point2 sample_point = sample_point_scale * (sample_point_offset + Point2(vertex[sample_axis_x], vertex[sample_axis_y]));

		const real_t accum_deform = (
			Math::lerp(real_t(0), corner_deformations[CORNER_TOP_LEFT], real_t((1.0 - sample_point.x) * (1.0 - sample_point.y)))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_TOP_RIGHT], real_t(sample_point.x * (1.0 - sample_point.y)))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_BOTTOM_RIGHT], real_t(sample_point.x * sample_point.y))
			+ Math::lerp(real_t(0), corner_deformations[CORNER_BOTTOM_LEFT], real_t((1.0 - sample_point.x) * sample_point.y))
		);

		list_vertex.write[vertex_idx][deform_axis] += accum_deform;
	}

	p_arr[RenderingServer::ARRAY_VERTEX] = list_vertex;
}


void CornerDeformablePlaneMesh::set_corner_deformations(const Vector4 &p_corners)
{
	corner_deformations = p_corners;
	request_update();
}


Vector4 CornerDeformablePlaneMesh::get_corner_deformations() const
{
	return corner_deformations;
}


void CornerDeformablePlaneMesh::set_corner_deformation_top_left(const real_t p_deform)
{
	corner_deformations[CORNER_TOP_LEFT] = p_deform;
	request_update();
}


real_t CornerDeformablePlaneMesh::get_corner_deformation_top_left() const
{
	return corner_deformations[CORNER_TOP_LEFT];
}


void CornerDeformablePlaneMesh::set_corner_deformation_top_right(const real_t p_deform)
{
	corner_deformations[CORNER_TOP_RIGHT] = p_deform;
	request_update();
}


real_t CornerDeformablePlaneMesh::get_corner_deformation_top_right() const
{
	return corner_deformations[CORNER_TOP_RIGHT];
}


void CornerDeformablePlaneMesh::set_corner_deformation_bottom_right(const real_t p_deform)
{
	corner_deformations[CORNER_BOTTOM_RIGHT] = p_deform;
	request_update();
}


real_t CornerDeformablePlaneMesh::get_corner_deformation_bottom_right() const
{
	return corner_deformations[CORNER_BOTTOM_RIGHT];
}


void CornerDeformablePlaneMesh::set_corner_deformation_bottom_left(const real_t p_deform)
{
	corner_deformations[CORNER_BOTTOM_LEFT] = p_deform;
	request_update();
}


real_t CornerDeformablePlaneMesh::get_corner_deformation_bottom_left() const
{
	return corner_deformations[CORNER_BOTTOM_LEFT];
}


void CornerDeformablePlaneMesh::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_corner_deformations", "deformations"), &CornerDeformablePlaneMesh::set_corner_deformations);
	ClassDB::bind_method(D_METHOD("get_corner_deformations"), &CornerDeformablePlaneMesh::get_corner_deformations);
	ClassDB::bind_method(D_METHOD("set_corner_deformation_top_left", "deformation"), &CornerDeformablePlaneMesh::set_corner_deformation_top_left);
	ClassDB::bind_method(D_METHOD("get_corner_deformation_top_left"), &CornerDeformablePlaneMesh::get_corner_deformation_top_left);
	ClassDB::bind_method(D_METHOD("set_corner_deformation_top_right", "deformation"), &CornerDeformablePlaneMesh::set_corner_deformation_top_right);
	ClassDB::bind_method(D_METHOD("get_corner_deformation_top_right"), &CornerDeformablePlaneMesh::get_corner_deformation_top_right);
	ClassDB::bind_method(D_METHOD("set_corner_deformation_bottom_right", "deformation"), &CornerDeformablePlaneMesh::set_corner_deformation_bottom_right);
	ClassDB::bind_method(D_METHOD("get_corner_deformation_bottom_right"), &CornerDeformablePlaneMesh::get_corner_deformation_bottom_right);
	ClassDB::bind_method(D_METHOD("set_corner_deformation_bottom_left", "deformation"), &CornerDeformablePlaneMesh::set_corner_deformation_bottom_left);
	ClassDB::bind_method(D_METHOD("get_corner_deformation_bottom_left"), &CornerDeformablePlaneMesh::get_corner_deformation_bottom_left);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "corner_deformations", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_NO_EDITOR), "set_corner_deformations", "get_corner_deformations");

	ADD_GROUP("Corner Deformations", "corner_deformation_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_deformation_top_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_deformation_top_left", "get_corner_deformation_top_left");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_deformation_top_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_deformation_top_right", "get_corner_deformation_top_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_deformation_bottom_right", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_deformation_bottom_right", "get_corner_deformation_bottom_right");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "corner_deformation_bottom_left", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_EDITOR), "set_corner_deformation_bottom_left", "get_corner_deformation_bottom_left");
}
