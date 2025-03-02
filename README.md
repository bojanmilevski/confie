# CONFIE

`confie` is a no-overhead, non-opinionated, light, configurable, modern config
file backup manager.

Although it's name is a play on the word "configuration", `confie` is just a
simple file/folder backup manager. In essence, its nothing more than just a
fancy copy-pasting tool that gets its values from a configuration file.

The reason why its called `confie` is because its intended to integrate with
`cronie` or similar cron daemons.

## ANOTHER CONFIGURATION MANAGER?

From my experience, the existing ones are opinionated and mess with VSC files.
I have not used any of the existing tools extensively because of these reasons.
I may be wrong, but most probably they need configuring to not get in your way.
I have not looked in too deep in these tools, and I'm not planning to.

`confie` just does what its suppose to - backup up your configs to a folder and
just that. It does not touch `.git`, `.svn`, `README`, `LICENSE` and the sort.

## FUNCTIONALITY

Here is a list of what `confie` does:

- Reads a configuration file of your presets.
  - A preset is just a pre-configured collection of configuration files.
    - You might want to backup your dotfiles to one repository.
    - Backup your `/etc`, `/var` files to another one.
- Option to ignore certain files in a directory.
- set-and-forget backups:
  - Is able to backup certain files just once, and never touch them again if
    changed locally, if so told.
- Can keep files in remote if deleted locally, if told.
- Does not care about your VCS, not its files. `confie` only cares about the
  files you tell it about.
- Changes are only made to remote if local changes are detected.
- Option to deploy a system backup from a remote repository. This is great for
  when you reinstall your system.
- Ability to make time-based backups. This marks your repos with a timestamp.
- Differently named backups for a single preset.
- Except the configuration files, no other files are required for `confie` to
  work.
- Integrates with `cronie`.

Here is a list of what `confie` does not do:

- The need of special configuration, metadata or cache files.
- Enforce a certain VCS.
- Mess with your VCS files.
- Special configuration to keep `README`, `LICENSE` and so on.
- Some sort of special file structure.

## INSTRUCTIONS

### MAIN CONFIGURATION FILE

`confie` requires a single configuration file to run.

- Either in `~/.config/confie/config.toml`
- or provide one: `confie ./config.toml`
- An example config file can be seen in `TODO`:

```toml
TODO
```

#### PRESETS

The "main" configuration file is able to point to presets:

`~/.config/confie/config.toml`:

```toml
TODO
```

`~/.config/confie/dotfiles.toml`:

```toml
TODO
```

`~/.config/confie/system-configs.toml`:

```toml
TODO
```

### RUNNING

#### BACKUP

To run `confie`:

- If no config file is provided, just run `confie backup`
- `confie backup ./config.toml` to provide one.

#### DEPLOY

TODO

### CRONIE

Here is a simple `cronie` command that runs every 3 hours:

```sh
* */3 * * * confie backup
```

## LICENSE

This software, like all my other software, is licensed under the [GPL v3.0 License](https://www.gnu.org/licenses/gpl-3.0.en.html).

## SPECIAL THANKS

To all of the already existing configuration managers for being an example of
what a configuration manager is not suppose to be.
