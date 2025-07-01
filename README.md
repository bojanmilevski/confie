# CONFIE

Modern, no-overhead, non-opinionated, light and configurable config file backup
manager.

# WHAT?

Although it's name is a play on the word "configuration", `confie` is just a
simple file/folder backup manager. In essence, its nothing more than just a
fancy copy-pasting tool that gets its values from a configuration file.

The reason why its called `confie` is because its intended to integrate with,
and is lightweight like `cronie`.

# YET ANOTHER CONFIGURATION MANAGER?

From my experience, the existing ones are opinionated and mess with VSC files.
I have not used any of the existing tools extensively because of these reasons.
I may be wrong, but most probably they need configuring to not get in your way.
I have not looked in too deep in these tools, and I'm not planning to.

`confie` just does what its suppose to - backup up your configs to a folder and
just that. It does not touch `.git`, `.svn`, `README`, `LICENSE` and the sort.

# FUNCTIONALITY

Here what `confie` does:

- Backup entries
  - files
  - folders (recursive)
- Groups
  - Collection of configuration files/folders.
  - You might want to backup your dotfiles to one repository.
  - ...or backup `/etc`, `/var` files to another one.
- `exclude`
  - Remove entries from a folder tree.
- `archive`
  - Backs up an entry **only** once and never again.
  - Preserves the initial state of an entry.
- `protect`
  - Do not delete a backed-up entry if deleted locally.
- VSC-agnostic
  - Does not depend on a certain VCS
  - Does not touch, nor use VCS files.
- Backs-up **only** when changes are detected.
- Deploy a configuration from a remote repository.
- Time-based backups.
- Differently named backups.
- `.config/confie` is the only "dependency".
- Integrates with `cronie`.

Here what `confie` does not do:

- The need of special configuration, metadata or cache files.
- Enforce a certain VCS.
- Mess with your VCS files.
- Special configuration to keep `README`, `LICENSE` and so on.
- Some sort of special file structure.

# BUILD

To build cronie, just run:

```sh
./cmake.sh
```

This script will take care of everything in order to compile and run `cronie`.

By default, it builds the debug profile, which is handy in order to see more
verbose log messages.

It runs on a simple example configuration file that is reproducable on a clean
Linux install.

# USAGE

```sh
# NOT IMPLEMENTED
confie backup
confie backup --configuration-file ./var.toml
confie backup -c ./etc.toml
confie backup --timestamp -c ./dotfiles.toml
confie backup --watch -c ./dotfiles.toml # not recommended for cronjobs
confie deploy "https://github.com/bojanmilevski/dots.git" "$HOME"
```

# THANKS

To all of the already existing configuration managers for being an example of
what a configuration manager is not suppose to be.
