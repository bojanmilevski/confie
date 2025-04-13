/*
  FIX:
   - before accessing a value from the toml file, check if the table contains
   it? this might not be necessary. look into it.
   - check if an array has empty strings ("")
   - check if an array is empty (array=[])
 */

#include "config.hpp"
#include "error.hpp"
#include "group.hpp"
#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

namespace confie {
auto Config::create(std::optional<std::filesystem::path>&& path) noexcept -> const expected<Config> {
  std::filesystem::path config_file_path{};
  const static auto home = std::getenv("HOME"); // FIX: make a "global" home var

  if (path) {
    config_file_path = std::filesystem::path(std::move(*path));
  } else {
    const auto path_str = std::string(home).append("/.config/confie/config.toml");
    config_file_path = std::filesystem::path(std::move(path_str));
  }

  if (!std::filesystem::exists(config_file_path)) {
    return Error::create(ErrorType::NOT_EXIST, "Configuration file does not exist.");
  } else if (!std::filesystem::is_regular_file(config_file_path)) {
    return Error::create(ErrorType::NOT_FILE, "Configuration is not a file.");
  }

  const auto toml = toml::parse_file(config_file_path.c_str());
  if (!toml) {
    return Error::create(ErrorType::PLACEHOLDER, "Unable to parse toml file");
  }

  if (!toml.table().contains("group")) {
    return Error::create(ErrorType::PLACEHOLDER, "Groups missing in configuration file");
  }

  const auto t_groups = toml["group"].as_array();
  if (!t_groups) {
    return Error::create(ErrorType::PLACEHOLDER, "groupS");
  }

  std::set<Group> groups{};
  // FIX: this lambda's return should be handled better
  t_groups->for_each([&](const auto& t_group) -> expected<void> {
    const auto table = t_group.as_table();
    if (!table) {
      return Error::create(ErrorType::PLACEHOLDER, "group");
    }

    auto name = (*table)["name"].as_string();
    if (!name) {
      return Error::create(ErrorType::PLACEHOLDER, "name");
    }

    auto t_destination = (*table)["destination"].as_string();
    if (!t_destination) {
      return Error::create(ErrorType::PLACEHOLDER, "destination");
    }

    // FIX: avoid cast to std::string
    auto destination = parse_path(std::move(std::string(**t_destination)));

    auto include = parse_array(*table, "include");
    if (!include) {
      return Error::create(ErrorType::PLACEHOLDER, "include");
    }

    auto exclude = parse_optional_array(*table, "exclude");
    if (!exclude) {
      return Error::create(ErrorType::PLACEHOLDER, "exclude");
    }

    auto archive = parse_optional_array(*table, "archive");
    if (!archive) {
      return Error::create(ErrorType::PLACEHOLDER, "archive");
    }

    auto protect = parse_optional_array(*table, "protect");
    if (!protect) {
      return Error::create(ErrorType::PLACEHOLDER, "protect");
    }

    auto group = Group(std::move(**name), std::move(destination), std::move(*include), std::move(*exclude),
                       std::move(*archive), std::move(*protect));

    groups.insert(std::move(group));

    return {};
  });

  return Config(std::move(groups));
}

#ifdef DEBUG
auto Config::print() const& noexcept -> const void {
  std::ranges::for_each(m_groups, [&](const auto& entry) { entry.print(); });
}
#endif

Config::Config(std::set<Group>&& groups) noexcept : m_groups(std::move(groups)) {}

auto Config::parse_optional_array(const toml::table& table, std::string_view&& name) noexcept
    -> const expected<std::optional<std::set<std::filesystem::path>>> {
  if (!table.contains(name)) {
    return {};
  }

  return parse_array(table, std::move(name));
}

auto Config::parse_array(const toml::table& table, std::string_view&& name) noexcept
    -> const expected<std::set<std::filesystem::path>> {
  const auto t_entry = table[name].as_array();
  if (!t_entry) {
    return Error::create(ErrorType::PLACEHOLDER, std::move(name));
  }

  std::set<std::filesystem::path> paths{};
  t_entry->for_each([&](const auto& element) {
    auto string = **element.as_string(); // FIX: check
    if (string.length() > 0) {
      const auto path = parse_path(std::move(string));
      paths.insert(std::move(path));
    }
  });

  return paths;
}

auto Config::parse_path(std::string&& path) noexcept -> const std::filesystem::path {
  if (path.at(0) == '~') {
    return parse_tilde(std::move(path));
  }

  return {path};
}

auto Config::parse_tilde(std::string&& path) noexcept -> const std::filesystem::path {
  const static auto home = std::getenv("HOME"); // FIX: make a "global" home var
  return {home + path.substr(1)};
}
} // namespace confie
