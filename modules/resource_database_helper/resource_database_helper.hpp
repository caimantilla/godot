#ifndef RESOURCE_DATABASE_HELPER_HPP
#define RESOURCE_DATABASE_HELPER_HPP


#include "core/object/object.h"
#include "core/io/resource.h"
#include "core/io/dir_access.h"
#include "core/variant/typed_array.h"


class ResourceDatabaseHelper : public Object
{
	GDCLASS(ResourceDatabaseHelper, Object);

	static ResourceDatabaseHelper *singleton;

private:
	void recursive_step_process_folder(const Ref<DirAccess> &p_dir, const String &p_current_folder, List<String> *p_subfolder_list, const String &p_native_type_constraint, List<Ref<Resource>> *r_resources) const;

	TypedArray<Resource> bind_get_resources_in_folders_recursive(const PackedStringArray &p_folders, const String &p_native_type_constraint) const;

protected:
	static void _bind_methods();

public:
	static ResourceDatabaseHelper *get_singleton();

	void get_resources_in_folders_recursive(const PackedStringArray *p_folders, const String &p_native_type_constraint, List<Ref<Resource>> *r_resources) const;

	ResourceDatabaseHelper();
	~ResourceDatabaseHelper();
};


#endif // RESOURCE_DATABASE_HELPER_HPP
