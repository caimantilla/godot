# config.py


def can_build(env, platform):
	env.module_add_dependencies("message_command_utility", ["regex"])
	return True


def configure(env):
	pass
