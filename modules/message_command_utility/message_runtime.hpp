#ifndef MESSAGE_RUNTIME_HPP
#define MESSAGE_RUNTIME_HPP


#include "core/object/script_language.h"
#include "scene/main/node.h"
#include "message_state.hpp"
#include "modules/regex/regex.h"


class MessageCommand;
class MessageRuntime;


class MessageQueuePart
{
public:
	enum Type
	{
		TYPE_TEXT = 0,
		TYPE_COMMAND = 1,
	};

	union
	{
		String text; // used for TYPE_TEXT
		String command_arguments; // used for TYPE_COMMAND
	};
	Type type;
	MessageState state;
	bool reached = false;
	MessageCommand *command;

	MessageState *get_state_ptr();

	MessageQueuePart();
	~MessageQueuePart();
};


class MessageFiber : public Object
{
public:
	int current_part_queue_idx = 0;
	LocalVector<MessageQueuePart> part_queue;
	int finished_count = 0;

	MessageRuntime *runtime;
	MessageState *external_state;

	MessageQueuePart *create_part();
	void step();
	void inc_finish_count();
	bool has_finish_cap_been_reached() const;
	void handle_immediate_wait_style();
	void on_state_finish_style_parallel();
	void on_state_finish_style_stop();
	~MessageFiber();
};


class MessageRuntime : public Node
{
	friend class MessageFiber;

	GDCLASS(MessageRuntime, Node);

	struct CommandParseResult
	{
	public:
		Ref<RegExMatch> match;
		bool ok;
		MessageCommand *command = nullptr;
		String args;
		int begin;
		int end;
	};

	class PrepareMessageStateContainer : public Object
	{
	public:
		MessageState *state;

		void on_state_finished()
		{
			SceneTree::get_singleton()->queue_delete(state);
			memdelete(this);
		}

		PrepareMessageStateContainer(MessageState *p_state)
		{
			state = p_state;
			state->connect(SNAME("finished"), callable_mp(this, &PrepareMessageStateContainer::on_state_finished));
		}
	};

private:
	RegEx regex = RegEx("\\[.*?\\]");
	List<MessageCommand *> command_list;
	LocalVector<Object *> context_object_list;
	HashSet<MessageFiber *> fiber_set;

	void load_global_commands();
	void give_command_dependencies(MessageCommand *p_command);
	MessageCommand *get_command_by_name(const String &p_name) const;
	void delete_fiber(MessageFiber *p_fiber);

protected:
	static void _bind_methods();

	MessageCommand *create_command_using_script(const Ref<Script> &p_script);
	MessageCommand *create_command_using_script_at_path(const String &p_path);

	GDVIRTUAL2(_handle_text_part, MessageState *, const String &);
	GDVIRTUAL2(_prepare_all_text, MessageState *, const PackedStringArray &);

public:
	virtual void handle_text_part(MessageState *p_state, const String &p_text_part);
	virtual void prepare_all_text(MessageState *p_state, const PackedStringArray &p_text_list);
	MessageState *execute(const String &p_text);
	void add_context(Object *p_object);
	bool remove_context(Object *p_object);
	void push_script_command_front(const Ref<Script> &p_script);
	void push_script_command_back(const Ref<Script> &p_script);

	MessageRuntime();
	~MessageRuntime();
};


#endif // MESSAGE_RUNTIME_HPP
