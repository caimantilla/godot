#include "message_runtime.hpp"
#include "message_command.hpp"
#include "core/config/project_settings.h"


MessageState *MessageQueuePart::get_state_ptr()
{
	return &state;
}


MessageQueuePart::MessageQueuePart()
{
}


MessageQueuePart::~MessageQueuePart()
{
}


MessageQueuePart *MessageFiber::create_part()
{
	part_queue.resize(part_queue.size() + 1);
	return &part_queue[part_queue.size() - 1];
}


void MessageFiber::step()
{
	// when all finished, end fiber execution
	if (has_finish_cap_been_reached())
	{
		runtime->delete_fiber(this);
		return;
	}
	// quit if there's nothing left to do, step will get called again on another finish, right...?
	if (current_part_queue_idx == part_queue.size())
	{
		return;
	}
	MessageQueuePart &part = part_queue[current_part_queue_idx];
	// if part's already been reached, do nothing
	if (part.reached)
	{
		return;
	}
	part.reached = true;
	MessageCommand::WaitStyle wait_style;

	switch (part.type)
	{
		case MessageQueuePart::TYPE_TEXT:
			wait_style = MessageCommand::WAIT_STYLE_STOP;
			runtime->handle_text_part(part.get_state_ptr(), part.text);
			break;
		case MessageQueuePart::TYPE_COMMAND:
			wait_style = part.command->execute(part.get_state_ptr(), part.command_arguments);
			break;
		default:
			DEV_ASSERT(false);
			wait_style = MessageCommand::WAIT_STYLE_NONE;
			part.state.finish();
			break;
	}

	switch (wait_style)
	{
		case MessageCommand::WAIT_STYLE_NONE:
			handle_immediate_wait_style();
			break;
		case MessageCommand::WAIT_STYLE_PARALLEL:
			current_part_queue_idx++;
			if (part.state.is_finished())
				inc_finish_count();
			else
				// part->state.connect(SNAME("finished"), callable_mp(this, &MessageFiber::on_state_finish_style_parallel), Object::CONNECT_ONE_SHOT);
			break;
		case MessageCommand::WAIT_STYLE_STOP:
			if (part.state.is_finished())
				inc_finish_count();
			else
				part.state.connect(SNAME("finished"), callable_mp(this, &MessageFiber::on_state_finish_style_stop), Object::CONNECT_ONE_SHOT);
			break;
		default:
			DEV_ASSERT(false);
			handle_immediate_wait_style();
			break;
	}
}


void MessageFiber::inc_finish_count()
{
	finished_count++;
	step();
}


bool MessageFiber::has_finish_cap_been_reached() const
{
	return finished_count == part_queue.size();
}


void MessageFiber::handle_immediate_wait_style()
{
	current_part_queue_idx++;
	inc_finish_count();
}


void MessageFiber::on_state_finish_style_parallel()
{
	inc_finish_count();
}


void MessageFiber::on_state_finish_style_stop()
{
	current_part_queue_idx++;
	inc_finish_count();
}


MessageFiber::~MessageFiber()
{
	external_state->finish();
	SceneTree::get_singleton()->queue_delete(external_state);
}


void MessageRuntime::_bind_methods()
{
	// The bound version of add_script_command should push the command to the front so that custom commands take priority over global commands.
	ClassDB::bind_method(D_METHOD("add_script_command", "script"), &MessageRuntime::push_script_command_front);

	GDVIRTUAL_BIND(_handle_text_part, "state", "text_part");
	GDVIRTUAL_BIND(_prepare_all_text, "state", "text_list");

	GLOBAL_DEF_BASIC(PropertyInfo(Variant::PACKED_STRING_ARRAY, "message_command_utility/config/custom_global_command_scripts", PROPERTY_HINT_FILE, "*.gd,*.cs"), PackedStringArray());
}


void MessageRuntime::handle_text_part(MessageState *p_state, const String &p_text_part)
{
	if (!GDVIRTUAL_CALL(_handle_text_part, p_state, p_text_part))
	{
		ERR_PRINT("_handle_text_part must be overriden.");
		p_state->finish();
	}
}


void MessageRuntime::prepare_all_text(MessageState *p_state, const PackedStringArray &p_text_list)
{
	if (!GDVIRTUAL_CALL(_prepare_all_text, p_state, p_text_list))
	{
		p_state->finish();
	}
}


MessageCommand *MessageRuntime::create_command_using_script(const Ref<Script> &p_script)
{
	ERR_FAIL_COND_V(!p_script.is_valid(), nullptr);
	ERR_FAIL_COND_V(!p_script->can_instantiate(), nullptr);
	ERR_FAIL_COND_V_MSG(p_script->get_instance_base_type() != SNAME("MessageCommand"), nullptr, "Script must extend MessageCommand.");

	MessageCommand *command = memnew(MessageCommand);
	give_command_dependencies(command);
	command->set_script(p_script);

	return command;
}


MessageCommand *MessageRuntime::create_command_using_script_at_path(const String &p_path)
{
	Ref<Script> script = ResourceLoader::load(p_path, "Script");
	return create_command_using_script(script);
}


void MessageRuntime::push_script_command_front(const Ref<Script> &p_script)
{
	MessageCommand *command = create_command_using_script(p_script);
	ERR_FAIL_NULL(command);

	command_list.push_front(command);
}


void MessageRuntime::push_script_command_back(const Ref<Script> &p_script)
{
	MessageCommand *command = create_command_using_script(p_script);
	ERR_FAIL_NULL(command);

	command_list.push_back(command);
}


void MessageRuntime::load_global_commands()
{
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
		return;
#endif
	PackedStringArray custom_global_command_script_paths = GLOBAL_GET("message_command_utility/config/custom_global_command_scripts");
	for (const String &script_path : custom_global_command_script_paths)
	{
		MessageCommand *command = create_command_using_script_at_path(script_path);
		if (command == nullptr)
		{
			ERR_PRINT("Invalid command script at path: " + script_path);
			continue;
		}
		command_list.push_back(command);
	}
}


void MessageRuntime::give_command_dependencies(MessageCommand *p_command)
{
	p_command->runtime = this;
	p_command->context_object_list = &context_object_list;
}


MessageCommand *MessageRuntime::get_command_by_name(const String &p_name) const
{
	for (MessageCommand *command : command_list)
	{
		if (command->is_command_name_valid(p_name))
			return command;
	}
	return nullptr;
}


void MessageRuntime::delete_fiber(MessageFiber *p_fiber)
{
	fiber_set.erase(p_fiber);
	memdelete(p_fiber);
}


MessageState *MessageRuntime::execute(const String &p_text)
{
	MessageState *master_state = memnew(MessageState);
	if (p_text.is_empty())
	{
		ERR_PRINT("Input text cannot be empty.");
		master_state->finish();
		return master_state;
	}
	MessageFiber *fiber = memnew(MessageFiber);
	fiber_set.insert(fiber);
	fiber->runtime = this;
	fiber->external_state = master_state;

	PackedStringArray text_part_strings;

	TypedArray<RegExMatch> matches = regex.search_all(p_text);
	LocalVector<CommandParseResult> command_parse_results;
	command_parse_results.resize(matches.size());
	for (int i = 0; i < matches.size(); i++)
	{
		CommandParseResult &parse_result = command_parse_results[i];
		parse_result.match = matches.get(i);
		parse_result.begin = parse_result.match->get_start(0);
		parse_result.end = parse_result.match->get_end(0);
	}

	for (int i = 0; i < command_parse_results.size(); i++)
	{
		CommandParseResult &parse_result = command_parse_results[i];
		String without_brackets = p_text.substr(parse_result.begin + 1, parse_result.end - parse_result.begin - 1);
		without_brackets = without_brackets.strip_edges();
		PackedStringArray split = without_brackets.split(" ", false, 2);
		String command_name;
		String command_args;
		if (split.size() == 1)
		{
			command_name = split.get(0);
		}
		else if (split.size() == 2)
		{
			command_name = split.get(0);
			command_args = split.get(1);
		}
		MessageCommand *command = get_command_by_name(command_name);
		if (command == nullptr)
		{
			parse_result.ok = false;
		}
		else
		{
			parse_result.ok = true;
			parse_result.command = command;
			parse_result.args = command_args;
		}
	}

	if (command_parse_results.is_empty())
	{
		MessageQueuePart *part = fiber->create_part();
		part->type = MessageQueuePart::TYPE_TEXT;
		part->text = p_text;
	}
	else
	{
		for (int i = 0; i < command_parse_results.size(); i++)
		{
			CommandParseResult &parse_result = command_parse_results[i];
			if (parse_result.begin > 0)
			{
				MessageQueuePart *pre_text_part = fiber->create_part();
				pre_text_part->type = MessageQueuePart::TYPE_TEXT;
				pre_text_part->text = p_text.substr(0, parse_result.begin);
				text_part_strings.push_back(pre_text_part->text);
			}
			MessageQueuePart *command_part = fiber->create_part();
			if (parse_result.ok)
			{
				command_part->type = MessageQueuePart::TYPE_COMMAND;
				command_part->command = parse_result.command;
				command_part->command_arguments = parse_result.args;
			}
			else
			{
				command_part->type = MessageQueuePart::TYPE_TEXT;
				command_part->text = p_text;
				text_part_strings.push_back(command_part->text);
			}
		}
		CommandParseResult &last_parse_result = command_parse_results[command_parse_results.size() - 1];
		if (last_parse_result.end < p_text.length())
		{
			MessageQueuePart *last_text_part = fiber->create_part();
			last_text_part->type = MessageQueuePart::TYPE_TEXT;
			last_text_part->text = p_text.substr(last_parse_result.end);
			text_part_strings.push_back(last_text_part->text);
		}
	}

	MessageState *msg_prepare_state = memnew(MessageState);
	PrepareMessageStateContainer *prepare_state_container = memnew(PrepareMessageStateContainer(msg_prepare_state)); // handles deletion and stuff, an event handler basically...

	prepare_all_text(msg_prepare_state, text_part_strings);

	if (msg_prepare_state->is_finished())
	{
		fiber->step();
	}
	else
	{
		msg_prepare_state->connect(SNAME("finished"), callable_mp(fiber, &MessageFiber::step), CONNECT_ONE_SHOT);
	}
	return master_state;
}


void MessageRuntime::add_context(Object *p_object)
{
	ERR_FAIL_NULL(p_object);
	ERR_FAIL_COND(context_object_list.has(p_object));
	context_object_list.push_back(p_object);
}


bool MessageRuntime::remove_context(Object *p_object)
{
	ERR_FAIL_NULL_V(p_object, false);
	return context_object_list.erase(p_object);
}


MessageRuntime::MessageRuntime()
{
	load_global_commands();
}


MessageRuntime::~MessageRuntime()
{
	for (MessageFiber *fiber : fiber_set)
	{
		memdelete(fiber);
	}
	for (MessageCommand *command : command_list)
	{
		memdelete(command);
	}
}
