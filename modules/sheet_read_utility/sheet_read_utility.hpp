#ifndef SHEET_READ_UTILITY_HPP
#define SHEET_READ_UTILITY_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"


class SheetReadUtility final : public Object
{
	GDCLASS(SheetReadUtility, Object);

public:
	enum NumberMode
	{
		NUMBER_MODE_DISABLE = 0,
		NUMBER_MODE_DETECT = 1,
		NUMBER_MODE_FORCE_FLOAT = 2,
	};

private:
	static SheetReadUtility *singleton;

protected:
	static void _bind_methods();

public:
	static SheetReadUtility *get_singleton();

	Array load_csv_at_path_with_headers(const String &p_path, const NumberMode p_number_mode, const PackedStringArray &p_bool_identifiers_true, const PackedStringArray &p_bool_identifiers_false);

	SheetReadUtility();
	~SheetReadUtility();
};


VARIANT_ENUM_CAST(SheetReadUtility::NumberMode);


#endif // SHEET_READ_UTILITY_HPP
