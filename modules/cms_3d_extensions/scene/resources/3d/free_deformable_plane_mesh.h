#ifndef FREE_DEFORMABLE_PLANE_MESH_H
#define FREE_DEFORMABLE_PLANE_MESH_H


#include "scene/property_list_helper.h"
#include "scene/resources/3d/primitive_meshes.h"


class FreeDeformablePlaneMesh : public PlaneMesh
{
	GDCLASS(FreeDeformablePlaneMesh, PlaneMesh);

private:
	struct Deformation final
	{
		real_t level = 0.0;
		Point2 position;
		Size2 size = Vector2(0.5, 0.5);
	};

	LocalVector<Deformation> deformation_list;

	static inline PropertyListHelper p_list_helper_base;
	PropertyListHelper p_list_helper;

	real_t sample_deformation(const Point2 &p_point) const;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

	virtual void _create_mesh_array(Array &p_arr) const override;

public:
	void set_deformation_count(const int p_count);
	int get_deformation_count() const;
	void set_deformation_position(const int p_deformation, const Point2 &p_point);
	Point2 get_deformation_position(const int p_deformation) const;
	void set_deformation_level(const int p_deformation, const real_t p_level);
	real_t get_deformation_level(const int p_deformation) const;
	void set_deformation_size(const int p_deformation, const Size2 &p_size);
	Size2 get_deformation_size(const int p_deformation) const;

	FreeDeformablePlaneMesh();
};


#endif // FREE_DEFORMABLE_PLANE_MESH_H
