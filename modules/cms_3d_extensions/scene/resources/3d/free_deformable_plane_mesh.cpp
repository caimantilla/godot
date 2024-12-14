// TODO:
// - Modify Normals and Tangets
// - Add more deformation interpolation types than just linear/spherical

#include "free_deformable_plane_mesh.h"


void FreeDeformablePlaneMesh::_create_mesh_array(Array &p_arr) const
{
	PlaneMesh::_create_mesh_array(p_arr);

	const Vector3 center = get_center_offset();

	Vector2 sample_point_scale;
	Vector3::Axis sample_axis_x;
	Vector3::Axis sample_axis_y;
	Vector3::Axis deform_axis;

	switch (get_orientation())
	{
		case FACE_X: {
			sample_point_scale = Vector2(-1, -1);
			sample_axis_x = Vector3::AXIS_Z;
			sample_axis_y = Vector3::AXIS_Y;
			deform_axis = Vector3::AXIS_X;
		} break;
		case FACE_Z: {
			sample_point_scale = Vector2(1, -1);
			sample_axis_x = Vector3::AXIS_X;
			sample_axis_y = Vector3::AXIS_Y;
			deform_axis = Vector3::AXIS_Z;
		} break;
		case FACE_Y:
		default: {
			sample_point_scale = Vector2(1, 1);
			sample_axis_x = Vector3::AXIS_X;
			sample_axis_y = Vector3::AXIS_Z;
			deform_axis = Vector3::AXIS_Y;
		} break;
	}

	PackedVector3Array list_vertex = p_arr[RenderingServer::ARRAY_VERTEX];

	for (int vertex_idx = 0; vertex_idx < list_vertex.size(); vertex_idx++)
	{
		const Vector3 vertex = list_vertex[vertex_idx] - center;
		const Point2 sample_point = Point2(vertex[sample_axis_x], vertex[sample_axis_y]) * sample_point_scale;

		real_t accum_deform = 0.0;

		for (const Deformation &deformation : deformation_list)
		{
			const real_t lerp_weight = (
				MAX(0.0, deformation.size.width - ABS(deformation.position.x - sample_point.x))
				* (1.0 / deformation.size.width)
				* MAX(0.0, deformation.size.height - ABS(deformation.position.y - sample_point.y))
				* (1.0 / deformation.size.height)
			);
			accum_deform += Math::lerp(real_t(0), deformation.level, lerp_weight);
		}

		list_vertex.write[vertex_idx][deform_axis] += accum_deform;
	}

	p_arr[RenderingServer::ARRAY_VERTEX] = list_vertex;
}


void FreeDeformablePlaneMesh::set_deformation_count(const int p_count)
{
	deformation_list.resize(p_count);
	request_update();
	notify_property_list_changed();
}


int FreeDeformablePlaneMesh::get_deformation_count() const
{
	return deformation_list.size();
}


void FreeDeformablePlaneMesh::set_deformation_position(const int p_deformation, const Point2 &p_point)
{
	ERR_FAIL_INDEX(p_deformation, deformation_list.size());
	deformation_list[p_deformation].position = p_point;
	request_update();
}


Point2 FreeDeformablePlaneMesh::get_deformation_position(const int p_deformation) const
{
	ERR_FAIL_INDEX_V(p_deformation, deformation_list.size(), Point2());
	return deformation_list[p_deformation].position;
}


void FreeDeformablePlaneMesh::set_deformation_level(const int p_deformation, const real_t p_level)
{
	ERR_FAIL_INDEX(p_deformation, deformation_list.size());
	deformation_list[p_deformation].level = p_level;
	request_update();
}


real_t FreeDeformablePlaneMesh::get_deformation_level(const int p_deformation) const
{
	ERR_FAIL_INDEX_V(p_deformation, deformation_list.size(), 0);
	return deformation_list[p_deformation].level;
}


void FreeDeformablePlaneMesh::set_deformation_size(const int p_deformation, const Size2 &p_size)
{
	ERR_FAIL_INDEX(p_deformation, deformation_list.size());
	deformation_list[p_deformation].size = p_size;
	request_update();
}


Size2 FreeDeformablePlaneMesh::get_deformation_size(const int p_deformation) const
{
	ERR_FAIL_INDEX_V(p_deformation, deformation_list.size(), Size2());
	return deformation_list[p_deformation].size;
}


void FreeDeformablePlaneMesh::_get_property_list(List<PropertyInfo> *p_list) const
{
	p_list_helper.get_property_list(p_list);
}


bool FreeDeformablePlaneMesh::_set(const StringName &p_name, const Variant &p_value)
{
	return p_list_helper.property_set_value(p_name, p_value);
}


bool FreeDeformablePlaneMesh::_get(const StringName &p_name, Variant &r_ret) const
{
	return p_list_helper.property_get_value(p_name, r_ret);
}


bool FreeDeformablePlaneMesh::_property_can_revert(const StringName &p_name) const
{
	return p_list_helper.property_can_revert(p_name);
}


bool FreeDeformablePlaneMesh::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	return p_list_helper.property_get_revert(p_name, r_property);
}


void FreeDeformablePlaneMesh::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_deformation_count", "count"), &FreeDeformablePlaneMesh::set_deformation_count);
	ClassDB::bind_method(D_METHOD("get_deformation_count"), &FreeDeformablePlaneMesh::get_deformation_count);
	ClassDB::bind_method(D_METHOD("set_deformation_position", "deformation", "position"), &FreeDeformablePlaneMesh::set_deformation_position);
	ClassDB::bind_method(D_METHOD("get_deformation_position", "deformation"), &FreeDeformablePlaneMesh::get_deformation_position);
	ClassDB::bind_method(D_METHOD("set_deformation_level", "deformation", "level"), &FreeDeformablePlaneMesh::set_deformation_level);
	ClassDB::bind_method(D_METHOD("get_deformation_level", "deformation"), &FreeDeformablePlaneMesh::get_deformation_level);
	ClassDB::bind_method(D_METHOD("set_deformation_size", "deformation", "size"), &FreeDeformablePlaneMesh::set_deformation_size);
	ClassDB::bind_method(D_METHOD("get_deformation_size", "deformation"), &FreeDeformablePlaneMesh::get_deformation_size);

	ADD_ARRAY_COUNT("Deformations", "deformation_count", "set_deformation_count", "get_deformation_count", "deformation_");
	Deformation defaults;
	p_list_helper_base.set_prefix("deformation_");
	p_list_helper_base.set_array_length_getter(&FreeDeformablePlaneMesh::get_deformation_count);
	p_list_helper_base.register_property(PropertyInfo(Variant::FLOAT, "level", PROPERTY_HINT_NONE, "suffix:m"), defaults.level, &FreeDeformablePlaneMesh::set_deformation_level, &FreeDeformablePlaneMesh::get_deformation_level);
	p_list_helper_base.register_property(PropertyInfo(Variant::VECTOR2, "size", PROPERTY_HINT_NONE, "suffix:m"), defaults.size, &FreeDeformablePlaneMesh::set_deformation_size, &FreeDeformablePlaneMesh::get_deformation_size);
	p_list_helper_base.register_property(PropertyInfo(Variant::VECTOR2, "position", PROPERTY_HINT_NONE, "suffix:m"), defaults.position, &FreeDeformablePlaneMesh::set_deformation_position, &FreeDeformablePlaneMesh::get_deformation_position);
	PropertyListHelper::register_base_helper(&p_list_helper_base);
}


FreeDeformablePlaneMesh::FreeDeformablePlaneMesh()
{
	p_list_helper.setup_for_instance(p_list_helper_base, this);
}
