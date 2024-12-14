#ifndef CORNER_DEFORMABLE_GRID_MESH_H
#define CORNER_DEFORMABLE_GRID_MESH_H


#include "scene/resources/3d/primitive_meshes.h"


class CornerDeformableGridMesh : public PrimitiveMesh
{
	GDCLASS(CornerDeformableGridMesh, PrimitiveMesh);

public:
	enum Orientation
	{
		FACE_X,
		FACE_Y,
		FACE_Z,
	};

private:
	struct Cell final
	{
		Rect2 uv = Rect2(0, 0, 1, 1);
		Vector4 corner_deformations;
	};

	Size2i grid_size = Size2i(1, 1);
	Size2 cell_size = Size2(1, 1);
	real_t cell_separation = 0;
	Vector3 center_offset;
	bool use_top_left = false;
	Orientation orientation = FACE_Y;
	LocalVector<LocalVector<Cell>> cell_array;

	Size2 calculate_horizontal_size() const;
	void update_grid_array_size();

protected:
	static void _bind_methods();

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;	
	void _get_property_list(List<PropertyInfo> *p_list) const;

	virtual void _update_lightmap_size() override;
	virtual void _create_mesh_array(Array &p_arr) const override;

public:
	void set_grid_size(const Size2i &p_size);
	Size2i get_grid_size() const;
	void set_cell_size(const Size2 &p_size);
	Size2 get_cell_size() const;
	void set_cell_separation(const real_t p_distance);
	real_t get_cell_separation() const;
	void set_center_offset(const Vector3 p_offset);
	Vector3 get_center_offset() const;
	void set_use_top_left(const bool p_enabled);
	bool is_use_top_left() const;
	void set_orientation(const Orientation p_direction);
	Orientation get_orientation() const;

	void set_cell_uv(const Point2i &p_cell, const Rect2 &p_uv);
	Rect2 get_cell_uv(const Point2i &p_cell) const;
	void set_cell_corner_deformations(const Point2i &p_cell, const Vector4 &p_deformations);
	Vector4 get_cell_corner_deformations(const Point2i &p_cell) const;
	void set_cell_corner_deformation_top_left(const Point2i &p_cell, const real_t p_deformation);
	real_t get_cell_corner_deformation_top_left(const Point2i &p_cell) const;
	void set_cell_corner_deformation_top_right(const Point2i &p_cell, const real_t p_deformation);
	real_t get_cell_corner_deformation_top_right(const Point2i &p_cell) const;
	void set_cell_corner_deformation_bottom_right(const Point2i &p_cell, const real_t p_deformation);
	real_t get_cell_corner_deformation_bottom_right(const Point2i &p_cell) const;
	void set_cell_corner_deformation_bottom_left(const Point2i &p_cell, const real_t p_deformation);
	real_t get_cell_corner_deformation_bottom_left(const Point2i &p_cell) const;

	CornerDeformableGridMesh();
};


VARIANT_ENUM_CAST(CornerDeformableGridMesh::Orientation);


#endif // CORNER_DEFORMABLE_GRID_MESH_H
