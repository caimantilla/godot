#include "resource_database_helper.hpp"
#include "core/io/resource_loader.h"


ResourceDatabaseHelper *ResourceDatabaseHelper::singleton = nullptr;


ResourceDatabaseHelper *ResourceDatabaseHelper::get_singleton()
{
	return singleton;
}


void ResourceDatabaseHelper::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_resources_in_folders_recursive", "folders", "type_hint"), &ResourceDatabaseHelper::bind_get_resources_in_folders_recursive, DEFVAL(String()));
}


void ResourceDatabaseHelper::get_resources_in_folders_recursive(const PackedStringArray *p_folders, const String &p_native_type_constraint, List<Ref<Resource>> *r_resources) const
{
	List<String> scannable_folders;

	for (int i = 0; i < p_folders->size(); i++)
	{
		bool push_ok = true;
		const String &curr_folder = p_folders->operator[](i);
		for (int j = 0; j < p_folders->size(); j++)
		{
			const String &other_folder = p_folders->operator[](j);
			if (i != j && !other_folder.begins_with(curr_folder))
			{
				push_ok = false;
			}
		}
		if (push_ok)
		{
			scannable_folders.push_back(curr_folder);
		}
	}

	for (const String &curr_root_path : scannable_folders)
	{
		Ref<DirAccess> dir = DirAccess::open(curr_root_path);
		if (!dir.is_valid())
		{
			continue;
		}

		List<String> subfolder_queue;
		recursive_step_process_folder(dir, curr_root_path, &subfolder_queue, p_native_type_constraint, r_resources);

		for (List<String>::Element *E = subfolder_queue.front(); E; E = E->next())
		{
			const String &subfolder = E->get();
			if (dir->change_dir(subfolder) != OK)
			{
				continue;
			}
			recursive_step_process_folder(dir, subfolder, &subfolder_queue, p_native_type_constraint, r_resources);
		}
	}
}


void ResourceDatabaseHelper::recursive_step_process_folder(const Ref<DirAccess> &p_dir, const String &p_current_folder, List<String> *p_subfolder_list, const String &p_native_type_constraint, List<Ref<Resource>> *r_resources) const
{
	if (p_dir->list_dir_begin() != OK)
	{
		return;
	}
	String curr_name = p_dir->get_next();
	while (!curr_name.is_empty())
	{
		// include_navigational and include_hidden don't seem to affect get_next(), so skip any names that begin with a period.
		if (curr_name[0] != '.')
		{
			const String curr_path = p_current_folder.path_join(curr_name);
			if (p_dir->current_is_dir())
			{
				print_line(curr_path);
				p_subfolder_list->push_back(curr_path);
			}
			else
			{
				if (ResourceLoader::exists(curr_path, p_native_type_constraint))
				{
					Ref<Resource> res = ResourceLoader::load(curr_path, p_native_type_constraint);
					if (res.is_valid())
					{
						r_resources->push_back(res);
					}
				}
			}
		}
		curr_name = p_dir->get_next();
	}
	p_dir->list_dir_end();
}


TypedArray<Resource> ResourceDatabaseHelper::bind_get_resources_in_folders_recursive(const PackedStringArray &p_folders, const String &p_native_type_constraint) const
{
	List<Ref<Resource>> resource_list;
	get_resources_in_folders_recursive(&p_folders, p_native_type_constraint, &resource_list);

	TypedArray<Resource> resource_array;
	resource_array.resize(resource_list.size());
	int i = 0;
	for (const Ref<Resource> &res : resource_list)
	{
		resource_array.set(i, res);
		i++;
	}
	return resource_array;
}


ResourceDatabaseHelper::ResourceDatabaseHelper()
{
	CRASH_COND_MSG(singleton != nullptr, RTR("ResourceDatabaseHelper is a singleton. Do not instantiate it multiple times."));
	singleton = this;
}


ResourceDatabaseHelper::~ResourceDatabaseHelper()
{
	singleton = nullptr;
}
