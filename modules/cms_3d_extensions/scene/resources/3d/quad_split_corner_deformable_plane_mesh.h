#ifndef QUAD_SPLIT_CORNER_DEFORMABLE_PLANE_MESH_H
#define QUAD_SPLIT_CORNER_DEFORMABLE_PLANE_MESH_H


#include "corner_deformable_plane_mesh.h"


class QuadSplitCornerDeformablePlaneMesh : public CornerDeformablePlaneMesh
{
	GDCLASS(QuadSplitCornerDeformablePlaneMesh, CornerDeformablePlaneMesh);

private:
	Vector4 corner_separations;

protected:
	static void _bind_methods();

	virtual void _create_mesh_array(Array &p_arr) const override;

public:
	void set_corner_separations(const Vector4 p_corners);
	Vector4 get_corner_separations() const;

	void set_corner_separation_top_left(const real_t p_distance);
	real_t get_corner_separation_top_left() const;
	void set_corner_separation_top_right(const real_t p_distance);
	real_t get_corner_separation_top_right() const;
	void set_corner_separation_bottom_right(const real_t p_distance);
	real_t get_corner_separation_bottom_right() const;
	void set_corner_separation_bottom_left(const real_t p_distance);
	real_t get_corner_separation_bottom_left() const;

	QuadSplitCornerDeformablePlaneMesh();
};


#endif // QUAD_SPLIT_CORNER_DEFORMABLE_PLANE_MESH_H
