#include "directional_animated_sprite_editor_plugin.h"
#include "../../scene/3d/directional_animated_sprite_controller_3d.h"
#include "scene/main/viewport.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/gui/editor_validation_panel.h"
#include "editor/themes/editor_scale.h"


StringName DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::get_animation_name() const
{
	return StringName(name_line_edit->get_text().strip_edges());
}


void DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::update_validation_panel()
{
	const StringName new_name = get_animation_name();
	const bool name_valid = is_name_valid(new_name);
	if (!name_valid)
	{
		const String reason = get_fail_explanation();
		if (!reason.is_empty())
		{
			validation_panel->set_message(EditorValidationPanel::MSG_ID_DEFAULT, reason, EditorValidationPanel::MSG_ERROR);
		}
	}
}


void DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::_post_popup()
{
	ConfirmationDialog::_post_popup();
	name_line_edit->grab_focus();
	validation_panel->update();
}


void DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::ok_pressed()
{
	if (!apply_changes())
	{
		EditorNode::get_singleton()->show_warning(get_fail_explanation());
		return;
	}
	context_label->set_text(String());
	name_line_edit->set_text(String());
	name_line_edit->set_placeholder(String());
	hide();
}


DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase()
{
	set_min_size(Size2i(150, 50) * EDSCALE);

	VBoxContainer *box = memnew(VBoxContainer);
	add_child(box);

	context_label = memnew(Label);
	box->add_child(context_label);

	name_line_edit = memnew(LineEdit);
	box->add_child(name_line_edit);

	Control *space = memnew(Control);
	space->set_custom_minimum_size(Size2(0, 10 * EDSCALE));
	box->add_child(space);

	validation_panel = memnew(EditorValidationPanel);
	validation_panel->add_line(EditorValidationPanel::MSG_ID_DEFAULT, TTR("Animation name is valid."));
	validation_panel->set_accept_button(get_ok_button());
	validation_panel->set_update_callback(callable_mp(this, &DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase::update_validation_panel));
	box->add_child(validation_panel);

	name_line_edit->connect(SceneStringName(text_changed), callable_mp(validation_panel, &EditorValidationPanel::update).unbind(1));

	register_text_enter(name_line_edit);
}


void DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog::establish_context(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_current_animation_name)
{
	controller = p_controller;
	current_animation_name = p_current_animation_name;

	context_label->set_text(vformat(TTR("Rename animation \"%s\"..."), current_animation_name));
	name_line_edit->set_text(current_animation_name);
	name_line_edit->set_placeholder(TTR("new_animation_name"));
}


bool DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog::is_name_valid(const StringName &p_name) const
{
	return controller != nullptr && (p_name == current_animation_name || controller->can_add_animation(p_name));
}


bool DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog::apply_changes()
{
	if (controller == nullptr)
	{
		return false;
	}
	const StringName new_name = get_animation_name();
	if (new_name == current_animation_name)
	{
		return true;
	}
	else
	{
		if (controller->rename_animation(current_animation_name, new_name))
		{
			EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
			udrd->create_action(vformat(TTR("Sprite Controller \"%s\": Rename Animation \"%s\" to \"%s\""), controller->get_name(), current_animation_name, new_name), UndoRedo::MERGE_DISABLE, controller);
			udrd->add_do_method(controller, SNAME("rename_animation"), current_animation_name, new_name);
			udrd->add_undo_method(controller, SNAME("rename_animation"), new_name, current_animation_name);
			udrd->commit_action(false);
			return true;
		}
		else
		{
			return false;
		}
	}
}


String DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog::get_fail_explanation() const
{
	return vformat(TTR("Cannot rename animation \"%s\" to \"%s\"."), current_animation_name, get_animation_name());
}


DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog::DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog()
{
	set_title(TTR("Rename Animation..."));
}


void DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog::establish_context(DirectionalAnimatedSpriteController3D *p_controller)
{
	controller = p_controller;
	if (controller == nullptr)
	{
		return;
	}
	context_label->set_text(vformat(TTR("Add animation to \"%s\"..."), controller->get_name()));
	name_line_edit->set_placeholder(TTR("new_animation_name"));
}


bool DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog::is_name_valid(const StringName &p_name) const
{
	return controller != nullptr && controller->can_add_animation(p_name);
}


bool DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog::apply_changes()
{
	if (controller == nullptr)
	{
		return false;
	}
	const StringName new_name = get_animation_name();
	if (controller->add_animation(new_name))
	{
		EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
		udrd->create_action(vformat(TTR("Sprite Controller \"%s\": Add Animation \"%s\""), controller->get_name(), new_name), UndoRedo::MERGE_DISABLE, controller);
		udrd->add_do_method(controller, SNAME("add_animation"), new_name);
		udrd->add_undo_method(controller, SNAME("remove_animation"), new_name);
		udrd->commit_action(false);
		return true;
	}
	else
	{
		return false;
	}
}


String DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog::get_fail_explanation() const
{
	return vformat(TTR("Cannot add animation \"%s\"."), get_animation_name());
}


DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog::DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog()
{
	set_title("Add Animation...");
}


void DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog::establish_context(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name)
{
	controller = p_controller;
	target_animation_name = p_animation_name;

	message_label->set_text(vformat(TTR("Remove animation \"%s\" from controller \"%s\"?"), target_animation_name, controller->get_name()));
}


void DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog::_post_popup()
{
	ConfirmationDialog::_post_popup();
	get_ok_button()->grab_focus();
}


void DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog::ok_pressed()
{
	struct RestorePropertyPair final
	{
		StringName name;
		Variant value;
	};

	if (controller == nullptr || !controller->has_animation(target_animation_name))
	{
		return;
	}

	const String anim_property_prefix = "animations/" + target_animation_name + "/";

	LocalVector<RestorePropertyPair> restore_property_list;
	List<PropertyInfo> controller_property_list;
	controller->get_property_list(&controller_property_list);

	for (const PropertyInfo &E : controller_property_list)
	{
		if (Variant::NIL != E.type && E.name.begins_with(anim_property_prefix))
		{
			const RestorePropertyPair restore_property_pair = {
				.name = E.name,
				.value = controller->get(E.name),
			};
			restore_property_list.push_back(restore_property_pair);
		}
	}

	if (controller->remove_animation(target_animation_name))
	{
		EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
		udrd->create_action(vformat(TTR("Sprite Controller \"%s\": Remove Animation \"%s\""), controller->get_name(), target_animation_name), UndoRedo::MERGE_DISABLE, controller);
		udrd->add_do_method(controller, SNAME("remove_animation"), target_animation_name);
		udrd->add_undo_method(controller, SNAME("add_animation"), target_animation_name);
		for (const RestorePropertyPair &E : restore_property_list)
		{
			udrd->add_undo_method(controller, SNAME("set"), E.name, E.value);
		}
		udrd->commit_action(false);
		hide();
	}
	else
	{
		EditorNode::get_singleton()->show_warning(vformat(TTR("Failed to remove animation \"%s\" from controller \"%s\"."), target_animation_name, controller->get_name()));
	}
}


DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog::DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog()
{
	set_title(TTR("Remove Animation..."));

	message_label = memnew(Label);
	add_child(message_label);
}


bool EditorInspectorPluginDirectionalAnimatedSpriteController3D::can_handle(Object *p_object)
{
	return Object::cast_to<DirectionalAnimatedSpriteController3D>(p_object) != nullptr;
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::parse_group(Object *p_object, const String &p_group)
{
	// DirectionalAnimatedSpriteController3D *controller = Object::cast_to<DirectionalAnimatedSpriteController3D>(p_object);
	// if (controller != nullptr)
	// {
	// 	if (p_group == "Animations")
	// 	{
	// 		Button *button = memnew(Button);
	// 		button->set_text(TTR("Add Animation"));
	// 		button->set_tooltip_text(TTR("Add an animation to the controller."));
	// 		button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_theme_changed).bind(button));
	// 		button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_pressed).bind(button, controller));
	// 		add_custom_control(button);
	// 	}
	// 	else if (p_group.begins_with("animations/"))
	// 	{
	// 		StringName animation_name = p_group.get_slicec('/', 1);
	// 		if (controller->has_animation(animation_name))
	// 		{
	// 			Button *rename_button = memnew(Button);
	// 			rename_button->set_text(TTR("Rename Animation"));
	// 			rename_button->set_tooltip_text(vformat(TTR("Rename the \"%s\" animation."), animation_name));
	// 			rename_button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_theme_changed).bind(rename_button));
	// 			rename_button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_pressed).bind(rename_button, controller, animation_name));
	// 			add_custom_control(rename_button);

	// 			Button *remove_button = memnew(Button);
	// 			remove_button->set_text(TTR("Remove Animation"));
	// 			remove_button->set_tooltip_text(vformat(TTR("Remove the \"%s\" animation."), animation_name));
	// 			remove_button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_theme_changed).bind(remove_button));
	// 			remove_button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_pressed).bind(remove_button, controller, animation_name));
	// 			add_custom_control(remove_button);
	// 		}
	// 	}
	// }
}


bool EditorInspectorPluginDirectionalAnimatedSpriteController3D::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	DirectionalAnimatedSpriteController3D *controller = Object::cast_to<DirectionalAnimatedSpriteController3D>(p_object);
	bool hide_property = false;

	// Quick explanation:
	// Using groups seems to have failed. I want to add the buttons to the beginning of the animation drop-down, along with 
	if (controller != nullptr)
	{
		if (p_path.begins_with("animations/"))
		{
			if (p_path.ends_with("/_editor_hook_animation_list_begin"))
			{
				hide_property = true;
			}
			else if (p_path.ends_with("/_editor_hook_animation_list_end"))
			{
				hide_property = true;
				Button *add_anim_button = create_add_animation_button(controller);
				add_custom_control(add_anim_button);
			}
			else if (p_path.ends_with("/_editor_hook_animation"))
			{
				StringName anim_name = p_path.get_slicec('/', 1);
				if (controller->has_animation(anim_name))
				{
					hide_property = true;
					HBoxContainer *box = memnew(HBoxContainer);
					Button *rename_button = create_rename_animation_button(controller, anim_name);
					box->add_child(rename_button);
					Button *remove_button = create_remove_animation_button(controller, anim_name);
					box->add_child(remove_button);
					add_custom_control(box);
				}
			}
		}
	}

	return hide_property;
}


Button *EditorInspectorPluginDirectionalAnimatedSpriteController3D::create_add_animation_button(DirectionalAnimatedSpriteController3D *p_controller)
{
	Button *button = memnew(Button);
	button->set_text(TTR("Add Animation"));
	button->set_tooltip_text(TTR("Add an animation to the controller."));
	button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_theme_changed).bind(button));
	button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_pressed).bind(button, p_controller));
	return button;
}


Button *EditorInspectorPluginDirectionalAnimatedSpriteController3D::create_rename_animation_button(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_anim)
{
	Button *button = memnew(Button);
	// button->set_text(TTR("Rename Animation"));
	button->set_tooltip_text(vformat(TTR("Rename the \"%s\" animation."), p_anim));
	button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_theme_changed).bind(button));
	button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_pressed).bind(button, p_controller, p_anim));
	return button;
}


Button *EditorInspectorPluginDirectionalAnimatedSpriteController3D::create_remove_animation_button(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_anim)
{
	Button *button = memnew(Button);
	// button->set_text(TTR("Remove Animation"));
	button->set_tooltip_text(vformat(TTR("Remove the \"%s\" animation."), p_anim));
	button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_theme_changed).bind(button));
	button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_pressed).bind(button, p_controller, p_anim));
	return button;
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_theme_changed(Button *p_button)
{
	p_button->set_button_icon(p_button->get_editor_theme_icon(SNAME("Add")));
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_add_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller)
{
	add_animation_dialog->establish_context(p_controller);
	show_dialog_for_button(p_button, add_animation_dialog);
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_theme_changed(Button *p_button)
{
	p_button->set_button_icon(p_button->get_editor_theme_icon(SNAME("Rename")));
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_rename_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name)
{
	rename_animation_dialog->establish_context(p_controller, p_animation_name);
	show_dialog_for_button(p_button, rename_animation_dialog);
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_theme_changed(Button *p_button)
{
	p_button->set_button_icon(p_button->get_editor_theme_icon(SNAME("Remove")));
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::on_remove_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name)
{
	remove_animation_dialog->establish_context(p_controller, p_animation_name);
	show_dialog_for_button(p_button, remove_animation_dialog);
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::show_dialog_for_button(Button *p_button, Window *p_dialog)
{
	EditorInterface::get_singleton()->popup_dialog_centered(p_dialog);
}


void EditorInspectorPluginDirectionalAnimatedSpriteController3D::update_dialog_parent_status(Window *p_dialog)
{
	if (!p_dialog->is_visible())
	{
		Node *parent = p_dialog->get_parent();
		if (parent != nullptr)
		{
			parent->remove_child(p_dialog);
		}
	}
}


EditorInspectorPluginDirectionalAnimatedSpriteController3D::EditorInspectorPluginDirectionalAnimatedSpriteController3D()
{
	add_animation_dialog = memnew(DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog);
	add_animation_dialog->connect(SceneStringName(visibility_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::update_dialog_parent_status).bind(add_animation_dialog), CONNECT_DEFERRED);
	rename_animation_dialog = memnew(DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog);
	rename_animation_dialog->connect(SceneStringName(visibility_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::update_dialog_parent_status).bind(rename_animation_dialog), CONNECT_DEFERRED);
	remove_animation_dialog = memnew(DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog);
	remove_animation_dialog->connect(SceneStringName(visibility_changed), callable_mp(this, &EditorInspectorPluginDirectionalAnimatedSpriteController3D::update_dialog_parent_status).bind(remove_animation_dialog), CONNECT_DEFERRED);
}


EditorInspectorPluginDirectionalAnimatedSpriteController3D::~EditorInspectorPluginDirectionalAnimatedSpriteController3D()
{
	add_animation_dialog->queue_free();
	rename_animation_dialog->queue_free();
	remove_animation_dialog->queue_free();
}


void DirectionalAnimatedSpriteEditorPlugin::on_tree_node_added(Node *p_node)
{
	DirectionalAnimatedSpriteController3D *controller_3d = Object::cast_to<DirectionalAnimatedSpriteController3D>(p_node);
	if (controller_3d != nullptr)
	{
		const Viewport *vp = EditorInterface::get_singleton()->get_editor_viewport_3d();
		if (vp != nullptr)
		{
			const Camera3D *cam = vp->get_camera_3d();
			if (cam != nullptr)
			{
				const NodePath path_to_cam = controller_3d->get_path_to(cam);
				controller_3d->set_custom_camera(path_to_cam);
			}
		}
	}
}


void DirectionalAnimatedSpriteEditorPlugin::_notification(int p_what)
{
	if (p_what == NOTIFICATION_READY)
	{
		get_tree()->connect("node_added", callable_mp(this, &DirectionalAnimatedSpriteEditorPlugin::on_tree_node_added));
	}
}


DirectionalAnimatedSpriteEditorPlugin::DirectionalAnimatedSpriteEditorPlugin()
{
	controller_inspector_plugin_3d.instantiate();
	add_inspector_plugin(controller_inspector_plugin_3d);
}


DirectionalAnimatedSpriteEditorPlugin::~DirectionalAnimatedSpriteEditorPlugin()
{
	remove_inspector_plugin(controller_inspector_plugin_3d);
	controller_inspector_plugin_3d.unref();
}
