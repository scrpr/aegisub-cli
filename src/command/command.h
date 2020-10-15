// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

/// @file command.h
/// @brief Command base class and main header.
/// @ingroup command

#include <map>
#include <string>
#include <vector>

#include <libaegisub/exception.h>

namespace agi { struct Context; }

/// Commands
namespace cmd {
DEFINE_EXCEPTION(CommandError, agi::Exception);
DEFINE_EXCEPTION(CommandNotFound, CommandError);

	enum CommandFlags {
		/// Default command type
		COMMAND_NORMAL       = 0,

		/// Invoking this command toggles a setting of some sort. Any command
		/// of this type should have IsActive implemented to signal the
		/// current state of the thing being toggled, and invoking the command
		/// twice should be a no-op
		///
		/// This is mutually exclusive with COMMAND_RADIO
		COMMAND_TOGGLE       = 1,

		/// Invoking this command sets a setting to a specific value. Any
		/// command of this type should have IsActive implemented, and if
		/// IsActive returns true, invoking the command should have no effect
		///
		/// This is mutually exclusive with COMMAND_TOGGLE
		COMMAND_RADIO        = 2,

		/// This command has an overridden Validate method
		COMMAND_VALIDATE     = 4,

		/// This command's name may change based on the state of the project
		COMMAND_DYNAMIC_NAME = 8,

		/// This command's help string may change
		COMMAND_DYNAMIC_HELP = 16,

		/// This command's icon may change based on the state of the project
		COMMAND_DYNAMIC_ICON = 32
	};

	/// Holds an individual Command
	class Command {
	public:
		/// Command name
		virtual const char* name() const=0;
		/// String for menu purposes including accelerators, but not hotkeys
		virtual std::string StrMenu(const agi::Context *) const=0;
		/// Plain string for display purposes; should normally be the same as StrMenu
		/// but without accelerators
		virtual std::string StrDisplay(const agi::Context *) const=0;
		/// Short help string describing what the command does
		virtual std::string StrHelp() const=0;

		/// Get this command's type flags
		/// @return Bitmask of CommandFlags
		virtual int Type() const { return COMMAND_NORMAL; }

		/// Command function
		virtual void operator()(agi::Context *c)=0;

		/// Check whether or not it makes sense to call this command at this time
		/// @param c Project context
		///
		/// This function should be very fast, as it is called whenever a menu
		/// containing this command is opened and is called periodically for
		/// any commands used in a toolbar
		///
		/// Note that it is still legal to call commands when this returns
		/// false. In this situation, commands should do nothing.
		///
		/// This function should be overridden iff the command's type flags
		/// include COMMAND_VALIDATE
		virtual bool Validate(const agi::Context *c) { return true; }

		/// Is the selectable value represented by this command currently selected?
		/// @param c Project context
		///
		/// As with Validate, this function should be very fast.
		///
		/// This function should be overridden iff the command's type flags
		/// include COMMAND_TOGGLE or COMMAND_RADIO
		virtual bool IsActive(const agi::Context *c) { return false; }

		/// Destructor
		virtual ~Command() = default;
	};

	/// Init all builtin commands.
	void init_builtin_commands();

	/// Register a command.
	/// @param cmd Command object to register.
	void reg(std::unique_ptr<Command> cmd);

	/// Unregister a command.
	/// @param cmd Command name to unregister. The associated command object is deleted.
	void unreg(std::string const& name);

	/// Call a command.
	/// @param name Name of the command to call.
	/// @param c  Current Context.
	void call(std::string const& name, agi::Context *c);

	/// Retrieve a Command object.
	/// @param Command object.
	Command* get(std::string const& name);

	/// Get a list of registered command names
	std::vector<std::string> get_registered_commands();

	/// Unregister and deletes all commands
	void clear();
} // namespace cmd
