#ifndef GRID_LAYOUT_RECT_HPP
#define GRID_LAYOUT_RECT_HPP


#include "scene/gui/control.h"


class GridLayoutRect : public Control
{
	GDCLASS(GridLayoutRect, Control);

public:
	enum AnchorMode
	{
		ANCHOR_MODE_TOP_LEFT = 0,
		ANCHOR_MODE_CENTER = 1,
	};

	enum EdgeType : uint8_t
	{
		EDGE_NONE = 0,
		EDGE_CLOSE = 1,
		EDGE_OPEN = 2,
	};

private:
	enum
	{
		CELL_SIZE_MIN = 8,
		EDGE_MARGIN_MIN = 0,
		EDGE_OPENING_SIZE_MIN = 2,
	};

private:
	struct Cell final
	{
		Variant metadata;
		Color color = Color(1, 1, 1, 0);
		EdgeType edges[4] = { EDGE_NONE, EDGE_NONE, EDGE_NONE, EDGE_NONE }; // corresponds to Side enum
		Ref<Texture2D> icon;
		Color icon_modulate = Color(1, 1, 1, 1);
	};

	struct ThemeCache
	{
		int cell_size = 16;
		int grid_thickness = 1;
		Color grid_color = Color(0, 0, 0, 0.5);
		int edge_margin = 2;
		int edge_thickness = 2;
		int edge_opening_size = 4;
		Color edge_color = Color(1, 1, 1, 1);
	} theme_cache;

	real_t cell_size;
	real_t cell_size_half;

	real_t grid_thickness;
	Color grid_color;
	bool grid_enabled;

	real_t edge_margin;
	real_t edge_margin_half;
	real_t edge_thickness;
	real_t edge_opening_size;
	real_t edge_opening_size_half;
	Color edge_color;
	bool edge_enabled;

private:
	AnchorMode anchor_mode = ANCHOR_MODE_CENTER;
	bool expand_icons = false;
	Size2i fixed_icon_size;

	HashMap<Point2i, Cell> cell_map;
	mutable bool dimensions_dirty = false;
	mutable Rect2i grid_region;

	void on_dimensions_changed();
	void update_dimensions() const;
	Cell &get_or_create_cell(const Point2i &p_point);
	void exec_draw();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _validate_property(PropertyInfo &p_property) const;

public:
	void set_anchor_mode(const AnchorMode p_mode);
	AnchorMode get_anchor_mode() const;
	void set_expand_icons(bool p_enabled);
	bool is_expand_icons_enabled() const;
	void set_fixed_icon_size(const Size2i &p_size);
	Size2i get_fixed_icon_size() const;

	bool has_cell(const Point2i &p_cell) const;
	bool add_cell(const Point2i &p_cell);
	bool remove_cell(const Point2i &p_cell);
	bool clear();

	void set_cell_metadata(const Point2i &p_cell, const Variant &p_metadata);
	Variant get_cell_metadata(const Point2i &p_cell) const;
	void set_cell_color(const Point2i &p_cell, const Color &p_color);
	Color get_cell_color(const Point2i &p_cell) const;
	void set_cell_edge(const Point2i &p_cell, const Side p_side, const EdgeType p_type);
	EdgeType get_cell_edge(const Point2i &p_cell, const Side p_side) const;
	void set_cell_icon(const Point2i &p_cell, const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_cell_icon(const Point2i &p_cell) const;
	void set_cell_icon_modulate(const Point2i &p_cell, const Color &p_modulate);
	Color get_cell_icon_modulate(const Point2i &p_cell) const;

	Point2i get_cell_at_position(const Point2 &p_position) const;
	Side get_edge_at_position(const Point2 &p_position) const;

	Rect2 get_cell_rect(const Point2i &p_cell) const;
	Rect2 get_edge_rect(const Point2i &p_cell, const Side p_side) const;
	Rect2 get_grid_rect() const;

	Rect2i get_grid_region() const;

	virtual Size2 get_minimum_size() const override;
	virtual bool has_point(const Point2 &p_point) const override;
};


VARIANT_ENUM_CAST(GridLayoutRect::AnchorMode);
VARIANT_ENUM_CAST(GridLayoutRect::EdgeType);


#endif // GRID_LAYOUT_RECT_HPP
