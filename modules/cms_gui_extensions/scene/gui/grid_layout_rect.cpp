#include "grid_layout_rect.h"
#include "scene/theme/theme_db.h"


void GridLayoutRect::set_anchor_mode(const AnchorMode p_mode)
{
	if (p_mode == anchor_mode)
	{
		return;
	}
	anchor_mode = p_mode;
	queue_redraw();
}


GridLayoutRect::AnchorMode GridLayoutRect::get_anchor_mode() const
{
	return anchor_mode;
}


void GridLayoutRect::set_expand_icons(const bool p_enabled)
{
	if (p_enabled == expand_icons)
	{
		return;
	}
	expand_icons = p_enabled;
	queue_redraw();
}


bool GridLayoutRect::is_expand_icons_enabled() const
{
	return expand_icons;
}


void GridLayoutRect::set_fixed_icon_size(const Size2i &p_size)
{
	if (p_size == fixed_icon_size)
	{
		return;
	}
	fixed_icon_size = p_size;
	queue_redraw();
}


Size2i GridLayoutRect::get_fixed_icon_size() const
{
	return fixed_icon_size;
}


bool GridLayoutRect::has_cell(const Point2i &p_cell) const
{
	return cell_map.has(p_cell);
}


bool GridLayoutRect::add_cell(const Point2i &p_cell)
{
	if (cell_map.has(p_cell))
	{
		return false;
	}
	cell_map.insert(p_cell, Cell());
	on_dimensions_changed();
	return true;
}


bool GridLayoutRect::remove_cell(const Point2i &p_cell)
{
	if (cell_map.erase(p_cell))
	{
		on_dimensions_changed();
		return true;
	}
	return false;
}


bool GridLayoutRect::clear()
{
	if (!cell_map.is_empty())
	{
		cell_map.clear();
		on_dimensions_changed();
		return true;
	}
	return false;
}


void GridLayoutRect::set_cell_metadata(const Point2i &p_cell, const Variant &p_metadata)
{
	get_or_create_cell(p_cell).metadata = p_metadata;
}


Variant GridLayoutRect::get_cell_metadata(const Point2i &p_cell) const
{
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Variant(), vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].metadata;
}


void GridLayoutRect::set_cell_color(const Point2i &p_cell, const Color &p_color)
{
	get_or_create_cell(p_cell).color = p_color;
	queue_redraw();
}


Color GridLayoutRect::get_cell_color(const Point2i &p_cell) const
{
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Color(), vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].color;
}


void GridLayoutRect::set_cell_edge(const Point2i &p_cell, const Side p_side, const EdgeType p_type)
{
	ERR_FAIL_INDEX_MSG(p_side, 4, vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	Cell &cell = get_or_create_cell(p_cell);
	if (p_type == cell.edges[p_side].type)
	{
		return;
	}
	cell.edges[p_side].type = p_type;
	queue_redraw();
}


GridLayoutRect::EdgeType GridLayoutRect::get_cell_edge(const Point2i &p_cell, const Side p_side) const
{
	ERR_FAIL_INDEX_V_MSG(p_side, 4, EdgeType::EDGE_NONE, vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), EdgeType::EDGE_NONE, vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].edges[p_side].type;
}


void GridLayoutRect::set_cell_edge_custom_color(const Point2i &p_cell, const Side p_side, const Color &p_color)
{
	ERR_FAIL_INDEX_MSG(p_side, 4, vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	Edge &edge = get_or_create_cell(p_cell).edges[p_side];
	edge.custom_color_enabled = true;
	edge.custom_color = p_color;
	queue_redraw();
}


Color GridLayoutRect::get_cell_edge_custom_color(const Point2i &p_cell, const Side p_side) const
{
	ERR_FAIL_INDEX_V_MSG(p_side, 4, Color(), vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Color(), vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].edges[p_side].custom_color;
}


Color GridLayoutRect::get_cell_edge_color(const Point2i &p_cell, const Side p_side) const
{
	ERR_FAIL_INDEX_V_MSG(p_side, 4, Color(), vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Color(), vformat(RTR("Cell %v does not exist."), p_cell));
	const Edge &edge = cell_map[p_cell].edges[p_side];
	if (edge.custom_color_enabled)
	{
		return edge.custom_color;
	}
	else
	{
		return theme_cache.edge_color;
	}
}


void GridLayoutRect::clear_cell_edge_custom_color(const Point2i &p_cell, const Side p_side)
{
	ERR_FAIL_INDEX_MSG(p_side, 4, vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	Edge &edge = get_or_create_cell(p_cell).edges[p_side];
	if (!edge.custom_color_enabled)
	{
		return;
	}
	edge.custom_color_enabled = false;
	queue_redraw();
}


bool GridLayoutRect::is_cell_edge_custom_color_enabled(const Point2i &p_cell, const Side p_side) const
{
	ERR_FAIL_INDEX_V_MSG(p_side, 4, false, vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), false, vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].edges[p_side].custom_color_enabled;
}


void GridLayoutRect::set_cell_icon(const Point2i &p_cell, const Ref<Texture2D> &p_icon)
{
	Cell &cell = get_or_create_cell(p_cell);
	if (p_icon == cell.icon)
	{
		return;
	}
	cell.icon = p_icon;
	queue_redraw();
}


Ref<Texture2D> GridLayoutRect::get_cell_icon(const Point2i &p_cell) const
{
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Ref<Texture2D>(), vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].icon;
}


void GridLayoutRect::set_cell_icon_modulate(const Point2i &p_cell, const Color &p_modulate)
{
	get_or_create_cell(p_cell).icon_modulate = p_modulate;
	queue_redraw();
}


Color GridLayoutRect::get_cell_icon_modulate(const Point2i &p_cell) const
{
	ERR_FAIL_COND_V_MSG(!cell_map.has(p_cell), Color(), vformat(RTR("Cell %v does not exist."), p_cell));
	return cell_map[p_cell].icon_modulate;
}


Point2i GridLayoutRect::get_cell_at_position(const Point2 &p_position) const
{
	const Rect2 grid_rect = get_grid_rect();
	const Point2 check_position = p_position - grid_rect.position - Vector2(cell_size_half, cell_size_half);

	const Point2 adjusted_pos = check_position / cell_size;
	Point2i cell_point = adjusted_pos.round();
	cell_point += grid_region.position;

	return cell_point;
}


Side GridLayoutRect::get_edge_at_position(const Point2 &p_position) const
{
	const Rect2 grid_rect = get_grid_rect();
	const Point2 check_position = p_position - grid_rect.position; // - Vector2(cell_size_half, cell_size_half);

	Point2 adjusted_pos = check_position / cell_size;
	adjusted_pos -= (adjusted_pos.floor() + Point2(0.5, 0.5));
	// adjusted_pos = adjusted_pos - (adjusted_pos.floor() + Point2(0.5, 0.5));

	real_t abs_x = ABS(adjusted_pos.x);
	real_t abs_y = ABS(adjusted_pos.y);

	if (abs_x > abs_y)
	{
		if (adjusted_pos.x < 0.0)
		{
			return SIDE_LEFT;
		}
		else
		{
			return SIDE_RIGHT;
		}
	}
	else
	{
		if (adjusted_pos.y < 0.0)
		{
			return SIDE_TOP;
		}
		else
		{
			return SIDE_BOTTOM;
		}
	}
}


Rect2 GridLayoutRect::get_cell_rect(const Point2i &p_cell) const
{
	Rect2 cell_rect = get_grid_rect();
	cell_rect.position += Size2(p_cell - grid_region.position) * cell_size;
	cell_rect.size = Size2(cell_size, cell_size);
	return cell_rect;
}


Rect2 GridLayoutRect::get_edge_rect(const Point2i &p_cell, const Side p_side) const
{
	ERR_FAIL_INDEX_V_MSG(p_side, 4, Rect2(), vformat(RTR("Side %d is out of range. Please submit a value between 0-3."), p_side));

	real_t thick = edge_thickness;
	if (thick < 0.0)
	{
		thick = 1.0;
	}
	real_t thick_half = thick * 0.5;

	const Rect2 cell_rect = get_cell_rect(p_cell);
	const Point2 cell_center = cell_rect.get_center();

	Rect2 edge_rect = get_cell_rect(p_cell);

	switch (p_side)
	{
		case SIDE_LEFT: {
			edge_rect.set_position(cell_center + Point2(-cell_size_half + edge_margin_half - thick_half, -cell_size_half - thick_half));
			edge_rect.set_end(cell_center + Point2(-cell_size_half + edge_margin_half + thick_half, cell_size_half + thick_half));
		} break;
		case SIDE_TOP: {
			edge_rect.set_position(cell_center + Point2(-cell_size_half - thick_half, -cell_size_half + edge_margin_half - thick_half));
			edge_rect.set_end(cell_center + Point2(cell_size_half + thick_half, -cell_size_half + edge_margin_half + thick_half));
		} break;
		case SIDE_RIGHT: {
			edge_rect.set_position(cell_center + Point2(cell_size_half - edge_margin_half - thick_half, -cell_size_half - thick_half));
			edge_rect.set_end(cell_center + Point2(cell_size_half - edge_margin_half + thick_half, cell_size_half + thick_half));
		} break;
		case SIDE_BOTTOM: {
			edge_rect.set_position(cell_center + Point2(-cell_size_half - thick_half, cell_size_half - edge_margin_half - thick_half));
			edge_rect.set_end(cell_center + Point2(cell_size_half + thick_half, cell_size_half - edge_margin_half + thick_half));
		} break;
		default: {
			CRASH_NOW_MSG(vformat(RTR("Invalid side %d."), p_side));
		} break;
	}

	return edge_rect;
}


Rect2 GridLayoutRect::get_grid_rect() const
{
	update_dimensions();

	switch (anchor_mode)
	{
		case ANCHOR_MODE_CENTER: {
			const Point2 center = get_size() * 0.5;
			Rect2 ret = Rect2(Point2(), Size2(grid_region.size) * cell_size);
			ret.position = center - (ret.size * 0.5);
			return ret;
		}
		case ANCHOR_MODE_TOP_LEFT:
		default: {
			return Rect2(Point2(), Size2(grid_region.size) * cell_size);
		}
	}
}


Rect2i GridLayoutRect::get_grid_region() const
{
	return grid_region;
}


void GridLayoutRect::on_dimensions_changed()
{
	if (dimensions_dirty)
	{
		return;
	}
	dimensions_dirty = true;
	update_minimum_size();
}


void GridLayoutRect::update_dimensions() const
{
	if (dimensions_dirty)
	{
		dimensions_dirty = false;

		if (cell_map.is_empty())
		{
			grid_region = Rect2i();
			return;
		}

		HashMap<Point2i, Cell>::ConstIterator E = cell_map.begin();
		Point2i current_point = E->key;

		Point2i point_min = current_point;
		Point2i point_max = current_point;

		for (; E; ++E)
		{
			current_point = E->key;
			point_min = point_min.min(current_point);
			point_max = point_max.max(current_point);
		}

		grid_region.set_position(point_min);
		grid_region.set_end(point_max + Point2i(1, 1));
	}
}


GridLayoutRect::Cell &GridLayoutRect::get_or_create_cell(const Point2i &p_point)
{
	if (!cell_map.has(p_point))
	{
		cell_map.insert(p_point, Cell());
		on_dimensions_changed();
	}
	return cell_map[p_point];
}


void GridLayoutRect::exec_draw()
{
	update_dimensions();

	Vector<Point2> point_list_grid;
	Vector<Point2> vertex_list_cell;
	Vector<Color> color_list_cell;
	Vector<Point2> point_list_edge;
	Vector<Color> color_list_edge;

	vertex_list_cell.resize(grid_region.get_area() * 6);
	color_list_cell.resize(vertex_list_cell.size());
	point_list_edge.resize(grid_region.get_area() * 16);
	color_list_edge.resize(grid_region.get_area() * 8);

	int idx_vertex_cell = -1;
	int idx_color_cell = -1;
	int idx_point_edge = -1;
	int idx_color_edge = -1;

	Size2 total_size = get_size();
	Rect2 grid_rect = get_grid_rect();

	// Write points for the grid.
	if (grid_enabled)
	{
		real_t x = grid_rect.position.x;
		while (x < total_size.x)
		{
			point_list_grid.push_back(Point2(x, 0));
			point_list_grid.push_back(Point2(x, total_size.height));
			x += cell_size;
		}

		x = grid_rect.position.x - cell_size;
		while (x > 0)
		{
			point_list_grid.push_back(Point2(x, 0));
			point_list_grid.push_back(Point2(x, total_size.height));
			x -= cell_size;
		}

		real_t y = grid_rect.position.y;
		while (y < total_size.y)
		{
			point_list_grid.push_back(Point2(0, y));
			point_list_grid.push_back(Point2(total_size.width, y));
			y += cell_size;
		}

		y = grid_rect.position.y - cell_size;
		while (y > 0)
		{
			point_list_grid.push_back(Point2(0, y));
			point_list_grid.push_back(Point2(total_size.width, y));
			y -= cell_size;
		}
	}

	// Write the points for each cell's color and edges.
	for (HashMap<Point2i, Cell>::ConstIterator E = cell_map.begin(); E; ++E)
	{
		const Point2i cell_point = E->key;
		const Cell &cell = E->value;

		Rect2 cell_rect = get_cell_rect(cell_point);
		Point2 draw_center = cell_rect.get_center();

		const bool cell_color_enabled = !Math::is_zero_approx(cell.color.a);

		if (cell_color_enabled)
		{
			const Point2 point_bl = draw_center + Point2(-cell_size_half, cell_size_half);
			const Point2 point_tl = draw_center + Point2(-cell_size_half, -cell_size_half);
			const Point2 point_tr = draw_center + Point2(cell_size_half, -cell_size_half);
			const Point2 point_br = draw_center + Point2(cell_size_half, cell_size_half);

			vertex_list_cell.write[++idx_vertex_cell] = point_bl;
			vertex_list_cell.write[++idx_vertex_cell] = point_tr;
			vertex_list_cell.write[++idx_vertex_cell] = point_tl;
			vertex_list_cell.write[++idx_vertex_cell] = point_tr;
			vertex_list_cell.write[++idx_vertex_cell] = point_bl;
			vertex_list_cell.write[++idx_vertex_cell] = point_br;

			color_list_cell.write[++idx_color_cell] = cell.color;
			color_list_cell.write[++idx_color_cell] = cell.color;
			color_list_cell.write[++idx_color_cell] = cell.color;
			color_list_cell.write[++idx_color_cell] = cell.color;
			color_list_cell.write[++idx_color_cell] = cell.color;
			color_list_cell.write[++idx_color_cell] = cell.color;
		}

		if (edge_enabled)
		{
			const Edge edge_left = cell.edges[SIDE_LEFT];
			const Edge edge_right = cell.edges[SIDE_RIGHT];
			const Edge edge_top = cell.edges[SIDE_TOP];
			const Edge edge_bottom = cell.edges[SIDE_BOTTOM];

			const Color edge_left_color = edge_left.custom_color_enabled ? edge_left.custom_color : theme_cache.edge_color;
			const Color edge_right_color = edge_right.custom_color_enabled ? edge_right.custom_color : theme_cache.edge_color;
			const Color edge_top_color = edge_top.custom_color_enabled ? edge_top.custom_color : theme_cache.edge_color;
			const Color edge_bottom_color = edge_bottom.custom_color_enabled ? edge_bottom.custom_color : theme_cache.edge_color;

			if (edge_left.type == EDGE_CLOSE)
			{
				color_list_edge.write[++idx_color_edge] = edge_left_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, cell_size_half); // BL
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, -cell_size_half); // TL
			}
			else if (edge_left.type == EDGE_OPEN)
			{
				color_list_edge.write[++idx_color_edge] = edge_left_color;
				color_list_edge.write[++idx_color_edge] = edge_left_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, cell_size_half); // BL-B
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, edge_opening_size_half); // BL-T
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, -edge_opening_size_half); // TL-B
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half + edge_margin_half, -cell_size_half); // TL-T
			}
			if (edge_top.type == EDGE_CLOSE)
			{
				color_list_edge.write[++idx_color_edge] = edge_top_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half, -cell_size_half + edge_margin_half); // TL
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half, -cell_size_half + edge_margin_half); // TR
			}
			else if (edge_top.type == EDGE_OPEN)
			{
				color_list_edge.write[++idx_color_edge] = edge_top_color;
				color_list_edge.write[++idx_color_edge] = edge_top_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half, -cell_size_half + edge_margin_half); // TL-L
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-edge_opening_size_half, -cell_size_half + edge_margin_half); // TL-R
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(edge_opening_size_half, -cell_size_half + edge_margin_half); // TR-L
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half, -cell_size_half + edge_margin_half); // TR-R
			}
			if (edge_right.type == EDGE_CLOSE)
			{
				color_list_edge.write[++idx_color_edge] = edge_right_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, -cell_size_half); // TR
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, cell_size_half); // BR
			}
			else if (edge_right.type == EDGE_OPEN)
			{
				color_list_edge.write[++idx_color_edge] = edge_right_color;
				color_list_edge.write[++idx_color_edge] = edge_right_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, -cell_size_half); // TR-T
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, -edge_opening_size_half); // TR-B
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, edge_opening_size_half); // BR-T
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half - edge_margin_half, cell_size_half); // BR-B
			}
			if (edge_bottom.type == EDGE_CLOSE)
			{
				color_list_edge.write[++idx_color_edge] = edge_bottom_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half, cell_size_half - edge_margin_half); // BR
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half, cell_size_half - edge_margin_half); // BL
			}
			else if (edge_bottom.type == EDGE_OPEN)
			{
				color_list_edge.write[++idx_color_edge] = edge_bottom_color;
				color_list_edge.write[++idx_color_edge] = edge_bottom_color;

				point_list_edge.write[++idx_point_edge] = draw_center + Point2(cell_size_half, cell_size_half - edge_margin_half); // BR-R
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(edge_opening_size_half, cell_size_half - edge_margin_half); // BR-L
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-edge_opening_size_half, cell_size_half - edge_margin_half); // BL-R
				point_list_edge.write[++idx_point_edge] = draw_center + Point2(-cell_size_half, cell_size_half - edge_margin_half); // BL-L
			}
		}
	}

	vertex_list_cell.resize(idx_vertex_cell + 1);
	color_list_cell.resize(idx_color_cell + 1);
	if (idx_vertex_cell == idx_color_cell && !vertex_list_cell.is_empty() && !color_list_cell.is_empty())
	{
		RenderingServer::get_singleton()->canvas_item_add_triangle_array(get_canvas_item(), PackedInt32Array(), vertex_list_cell, color_list_cell);
	}

	if (grid_enabled && !point_list_grid.is_empty())
	{
		draw_multiline(point_list_grid, grid_color, grid_thickness, false);
	}

	point_list_edge.resize(idx_point_edge + 1);
	color_list_edge.resize(idx_color_edge + 1);
	if (edge_enabled && !point_list_edge.is_empty())
	{
		draw_multiline_colors(point_list_edge, color_list_edge, edge_thickness, false);
	}

	// Draw icons last.
	for (HashMap<Point2i, Cell>::ConstIterator E = cell_map.begin(); E; ++E)
	{
		const Point2i &cell_point = E->key;
		const Cell &cell = E->value;

		if (cell.icon.is_valid() && !Math::is_zero_approx(cell.icon_modulate.a))
		{
			const Rect2 cell_rect = get_cell_rect(cell_point);
			const Point2 draw_center = cell_rect.get_center();

			Rect2 icon_rect = Rect2(Point2(), cell.icon->get_size());
			icon_rect.position = draw_center - (icon_rect.size * 0.5);
			icon_rect.position = icon_rect.position.floor();

			draw_texture_rect(cell.icon, icon_rect, false, cell.icon_modulate);
		}
	}
}


Size2 GridLayoutRect::get_minimum_size() const
{
	update_dimensions();
	return Size2(grid_region.size) * cell_size;
}


bool GridLayoutRect::has_point(const Point2 &p_point) const
{
	const Point2i cell_point = get_cell_at_position(p_point);
	return cell_map.has(cell_point);
}


void GridLayoutRect::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_THEME_CHANGED: {
			cell_size = MAX(theme_cache.cell_size, CELL_SIZE_MIN);
			cell_size_half = cell_size * 0.5;

			grid_thickness = theme_cache.grid_thickness;
			grid_color = theme_cache.grid_color;
			grid_enabled = !Math::is_zero_approx(grid_thickness) && !Math::is_zero_approx(grid_color.a);

			edge_margin = MAX(theme_cache.edge_margin, EDGE_MARGIN_MIN);
			edge_margin_half = edge_margin * 0.5;
			edge_thickness = theme_cache.edge_thickness;
			edge_opening_size = MAX(theme_cache.edge_opening_size, EDGE_OPENING_SIZE_MIN);
			edge_opening_size_half = edge_opening_size * 0.5;
			edge_color = theme_cache.edge_color;
			edge_enabled = !Math::is_zero_approx(edge_thickness) && !Math::is_zero_approx(edge_color.a);

			update_minimum_size();
			queue_redraw();
		} break;
		case NOTIFICATION_DRAW: {
			exec_draw();
		} break;
	}
}


void GridLayoutRect::_validate_property(PropertyInfo &p_property) const
{
	if (p_property.name == "anchor_mode")
	{
		p_property.usage = PROPERTY_USAGE_DEFAULT;
	}
}


void GridLayoutRect::_bind_methods()
{
	BIND_ENUM_CONSTANT(ANCHOR_MODE_TOP_LEFT);
	BIND_ENUM_CONSTANT(ANCHOR_MODE_CENTER);

	BIND_ENUM_CONSTANT(EDGE_NONE);
	BIND_ENUM_CONSTANT(EDGE_CLOSE);
	BIND_ENUM_CONSTANT(EDGE_OPEN);

	ClassDB::bind_method(D_METHOD("set_anchor_mode", "mode"), &GridLayoutRect::set_anchor_mode);
	ClassDB::bind_method(D_METHOD("get_anchor_mode"), &GridLayoutRect::get_anchor_mode);
	ClassDB::bind_method(D_METHOD("set_expand_icons", "enabled"), &GridLayoutRect::set_expand_icons);
	ClassDB::bind_method(D_METHOD("is_expand_icons_enabled"), &GridLayoutRect::is_expand_icons_enabled);
	ClassDB::bind_method(D_METHOD("set_fixed_icon_size", "size"), &GridLayoutRect::set_fixed_icon_size);
	ClassDB::bind_method(D_METHOD("get_fixed_icon_size"), &GridLayoutRect::get_fixed_icon_size);

	ClassDB::bind_method(D_METHOD("has_cell", "cell"), &GridLayoutRect::has_cell);
	ClassDB::bind_method(D_METHOD("add_cell", "cell"), &GridLayoutRect::add_cell);
	ClassDB::bind_method(D_METHOD("remove_cell", "cell"), &GridLayoutRect::remove_cell);
	ClassDB::bind_method(D_METHOD("clear"), &GridLayoutRect::clear);

	ClassDB::bind_method(D_METHOD("set_cell_metadata", "cell", "metadata"), &GridLayoutRect::set_cell_metadata);
	ClassDB::bind_method(D_METHOD("get_cell_metadata", "cell"), &GridLayoutRect::get_cell_metadata);
	ClassDB::bind_method(D_METHOD("set_cell_color", "cell", "color"), &GridLayoutRect::set_cell_color);
	ClassDB::bind_method(D_METHOD("get_cell_color", "cell"), &GridLayoutRect::get_cell_color);
	ClassDB::bind_method(D_METHOD("set_cell_edge", "cell", "edge", "type"), &GridLayoutRect::set_cell_edge);
	ClassDB::bind_method(D_METHOD("get_cell_edge", "cell", "edge"), &GridLayoutRect::get_cell_edge);
	ClassDB::bind_method(D_METHOD("set_cell_edge_custom_color", "cell", "edge", "color"), &GridLayoutRect::set_cell_edge_custom_color);
	ClassDB::bind_method(D_METHOD("get_cell_edge_custom_color", "cell", "edge"), &GridLayoutRect::get_cell_edge_custom_color);
	ClassDB::bind_method(D_METHOD("get_cell_edge_color", "cell", "edge"), &GridLayoutRect::get_cell_edge_color);
	ClassDB::bind_method(D_METHOD("clear_cell_edge_custom_color", "cell", "edge"), &GridLayoutRect::clear_cell_edge_custom_color);
	ClassDB::bind_method(D_METHOD("is_cell_edge_custom_color_enabled", "cell", "edge"), &GridLayoutRect::is_cell_edge_custom_color_enabled);
	ClassDB::bind_method(D_METHOD("set_cell_icon", "cell", "icon"), &GridLayoutRect::set_cell_icon);
	ClassDB::bind_method(D_METHOD("get_cell_icon", "cell"), &GridLayoutRect::get_cell_icon);
	ClassDB::bind_method(D_METHOD("set_cell_icon_modulate", "cell", "modulate"), &GridLayoutRect::set_cell_icon_modulate);
	ClassDB::bind_method(D_METHOD("get_cell_icon_modulate", "cell"), &GridLayoutRect::get_cell_icon_modulate);

	ClassDB::bind_method(D_METHOD("get_cell_at_position", "point"), &GridLayoutRect::get_cell_at_position);
	ClassDB::bind_method(D_METHOD("get_edge_at_position", "point"), &GridLayoutRect::get_edge_at_position);

	ClassDB::bind_method(D_METHOD("get_cell_rect", "cell"), &GridLayoutRect::get_cell_rect);
	ClassDB::bind_method(D_METHOD("get_edge_rect", "cell", "side"), &GridLayoutRect::get_edge_rect);
	ClassDB::bind_method(D_METHOD("get_grid_rect"), &GridLayoutRect::get_grid_rect);

	ClassDB::bind_method(D_METHOD("get_grid_region"), &GridLayoutRect::get_grid_region);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "anchor_mode", PROPERTY_HINT_ENUM, "Top Left,Centered"), "set_anchor_mode", "get_anchor_mode");

	ADD_GROUP("Icon", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "expand_icons"), "set_expand_icons", "is_expand_icons_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "fixed_icon_size"), "set_fixed_icon_size", "get_fixed_icon_size");

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, GridLayoutRect, cell_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, GridLayoutRect, grid_thickness);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, GridLayoutRect, grid_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, GridLayoutRect, edge_margin);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, GridLayoutRect, edge_thickness);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, GridLayoutRect, edge_opening_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, GridLayoutRect, edge_color);
}
