#ifndef DIRECTIONAL_ANIMATED_SPRITE_EDITOR_PLUGIN_H
#define DIRECTIONAL_ANIMATED_SPRITE_EDITOR_PLUGIN_H


#include "scene/gui/dialogs.h"
#include "editor/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"


class DirectionalAnimatedSpriteController3D;
class EditorValidationPanel;
class Label;
class LineEdit;
class Window;


class DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase : public ConfirmationDialog
{
	GDCLASS(DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase, ConfirmationDialog);

private:
	void update_validation_panel();

protected:
	Label *context_label;
	LineEdit *name_line_edit;
	EditorValidationPanel *validation_panel;

	virtual bool is_name_valid(const StringName &p_name) const = 0;
	virtual bool apply_changes() = 0;
	virtual String get_fail_explanation() const = 0;

public:
	StringName get_animation_name() const;

	virtual void _post_popup() override;
	virtual void ok_pressed() override;

	DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase();
};


class DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog final : public DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase
{
	GDCLASS(DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog, DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase);

private:
	StringName current_animation_name;
	DirectionalAnimatedSpriteController3D *controller = nullptr;

protected:
	virtual bool is_name_valid(const StringName &p_name) const override;
	virtual bool apply_changes() override;
	virtual String get_fail_explanation() const override;

public:
	void establish_context(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_current_animation_name);

	DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog();
};


class DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog final : public DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase
{
	GDCLASS(DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog, DirectionalAnimatedSpriteControllerEditorAnimationNameDialogBase);

private:
	DirectionalAnimatedSpriteController3D *controller = nullptr;

protected:
	virtual bool is_name_valid(const StringName &p_name) const override;
	virtual bool apply_changes() override;
	virtual String get_fail_explanation() const override;

public:
	void establish_context(DirectionalAnimatedSpriteController3D *p_controller);

	DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog();
};


class DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog final : public ConfirmationDialog
{
	GDCLASS(DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog, ConfirmationDialog);

private:
	Label *message_label;

	DirectionalAnimatedSpriteController3D *controller = nullptr;
	StringName target_animation_name;

public:
	void establish_context(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name);

	virtual void _post_popup() override;
	virtual void ok_pressed() override;

	DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog();
};


class EditorInspectorPluginDirectionalAnimatedSpriteController3D final : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginDirectionalAnimatedSpriteController3D, EditorInspectorPlugin);

private:
	DirectionalAnimatedSpriteController3DEditorAnimationCreateDialog *add_animation_dialog;
	DirectionalAnimatedSpriteController3DEditorAnimationRenameDialog *rename_animation_dialog;
	DirectionalAnimatedSpriteController3DEditorAnimationRemoveDialog *remove_animation_dialog;

	Button *create_add_animation_button(DirectionalAnimatedSpriteController3D *p_controller);
	Button *create_rename_animation_button(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_anim);
	Button *create_remove_animation_button(DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_anim);

	void on_add_animation_button_theme_changed(Button *p_button);
	void on_add_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller);
	void on_rename_animation_button_theme_changed(Button *p_button);
	void on_rename_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name);
	void on_remove_animation_button_theme_changed(Button *p_button);
	void on_remove_animation_button_pressed(Button *p_button, DirectionalAnimatedSpriteController3D *p_controller, const StringName &p_animation_name);

	void show_dialog_for_button(Button *p_button, Window *p_dialog);
	void update_dialog_parent_status(Window *p_dialog);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_group(Object *p_object, const String &p_group) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;

	EditorInspectorPluginDirectionalAnimatedSpriteController3D();
	~EditorInspectorPluginDirectionalAnimatedSpriteController3D();
};


class DirectionalAnimatedSpriteEditorPlugin final : public EditorPlugin
{
	GDCLASS(DirectionalAnimatedSpriteEditorPlugin, EditorPlugin);

private:
	Ref<EditorInspectorPluginDirectionalAnimatedSpriteController3D> controller_inspector_plugin_3d;

	void on_tree_node_added(Node *p_node);

protected:
	void _notification(int p_what);

public:
	DirectionalAnimatedSpriteEditorPlugin();
	~DirectionalAnimatedSpriteEditorPlugin();
};


#endif // DIRECTIONAL_ANIMATED_SPRITE_EDITOR_PLUGIN_H
