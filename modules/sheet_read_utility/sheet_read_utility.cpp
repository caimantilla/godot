#include "sheet_read_utility.hpp"
#include "core/io/file_access.h"


SheetReadUtility *SheetReadUtility::singleton = nullptr;


SheetReadUtility *SheetReadUtility::get_singleton()
{
	return singleton;
}


Array SheetReadUtility::load_csv_at_path_with_headers(const String &p_path, const NumberMode p_number_mode, const PackedStringArray &p_bool_identifiers_true, const PackedStringArray &p_bool_identifiers_false)
{
	Array ret;
	if (FileAccess::exists(p_path))
	{
		Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
		if (f.is_valid())
		{
			HashSet<String> id_set_true;
			HashSet<String> id_set_false;

			for (const String &E : p_bool_identifiers_true)
			{
				String lower = E.to_lower();
				if (!id_set_true.has(lower))
				{
					id_set_true.insert(lower);
				}
			}
			for (const String &E : p_bool_identifiers_false)
			{
				String lower = E.to_lower();
				if (!id_set_false.has(lower))
				{
					id_set_false.insert(lower);
				}
			}

			if (!f->eof_reached())
			{
				PackedStringArray header = f->get_csv_line();
				while (!f->eof_reached())
				{
					PackedStringArray line = f->get_csv_line();
					Dictionary line_data;

					int column_count = MIN(line.size(), header.size());

					for (int column = 0; column < column_count; column++)
					{
						String column_key = header[column];
						Variant column_value;
						String column_value_str = line[column];
						if (column_value_str.is_empty())
						{
							continue;
						}
						else
						{
							String column_value_str_lower = column_value_str.to_lower();
							if (id_set_true.has(column_value_str_lower))
							{
								column_value = true;
							}
							else if (id_set_false.has(column_value_str_lower))
							{
								column_value = false;
							}
							else
							{
								switch (p_number_mode)
								{
									case NUMBER_MODE_DETECT: {
										if (column_value_str.is_valid_int())
										{
											column_value = column_value_str.to_int();
										}
										else if (column_value_str.is_valid_float())
										{
											column_value = column_value_str.to_float();
										}
									} break;
									case NUMBER_MODE_FORCE_FLOAT: {
										if (column_value_str.is_valid_float())
										{
											column_value = column_value_str.to_float();
										}
									} break;
									default: break;
								}
							}
							if (column_value == Variant())
							{
								column_value = column_value_str;
							}
						}
						line_data[column_key] = column_value;
					}

					if (!line_data.is_empty())
					{
						ret.push_back(line_data);
					}
				}
			}

			f->close();
			return ret;
		}
	}
	ERR_FAIL_V_MSG(ret, vformat(RTR("Failed to load CSV at path: \"%s\""), p_path));
}


void SheetReadUtility::_bind_methods()
{
	BIND_ENUM_CONSTANT(NUMBER_MODE_DISABLE);
	BIND_ENUM_CONSTANT(NUMBER_MODE_DETECT);
	BIND_ENUM_CONSTANT(NUMBER_MODE_FORCE_FLOAT);

	ClassDB::bind_method(D_METHOD("load_csv_at_path_with_headers", "path", "number_mode", "bool_identifiers_true", "bool_identifiers_false"), &SheetReadUtility::load_csv_at_path_with_headers);
}


SheetReadUtility::SheetReadUtility()
{
	CRASH_COND_MSG(singleton != nullptr, RTR("SheetReadUtility is a singleton. Do not instantiate it multiple times."));
	singleton = this;
}


SheetReadUtility::~SheetReadUtility()
{
	singleton = nullptr;
}
