#ifndef CORNER_DEFORMABLE_PLANE_MESH_H
#define CORNER_DEFORMABLE_PLANE_MESH_H


#include "scene/resources/3d/primitive_meshes.h"


class CornerDeformablePlaneMesh : public PlaneMesh
{
	GDCLASS(CornerDeformablePlaneMesh, PlaneMesh);

private:
	Vector4 corner_deformations;

protected:
	static void _bind_methods();
	virtual void _create_mesh_array(Array &p_arr) const override;

public:
	void set_corner_deformations(const Vector4 &p_corners);
	Vector4 get_corner_deformations() const;

	void set_corner_deformation_top_left(const real_t p_deform);
	real_t get_corner_deformation_top_left() const;
	void set_corner_deformation_top_right(const real_t p_deform);
	real_t get_corner_deformation_top_right() const;
	void set_corner_deformation_bottom_right(const real_t p_deform);
	real_t get_corner_deformation_bottom_right() const;
	void set_corner_deformation_bottom_left(const real_t p_deform);
	real_t get_corner_deformation_bottom_left() const;
};


#endif // CORNER_DEFORMABLE_PLANE_MESH_H
