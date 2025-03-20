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
auto Config::create(const std::optional<std::filesystem::path>&& path) noexcept -> const expected<Config> {
  std::filesystem::path config_file_path{};
  const static auto home = std::getenv("HOME");

  if (path) {
    config_file_path = std::filesystem::path(*path);
  } else {
    const auto path_str = std::basic_string_view(std::string(home).append("/.config/confie/config.toml"));
    config_file_path = std::filesystem::path(path_str);
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
  t_groups->for_each([&](const auto& group) -> expected<void> {
    const auto table = group.as_table();
    if (!table) {
      return Error::create(ErrorType::PLACEHOLDER, "group");
    }

    const auto name = (*table)["name"].as_string();
    if (!name) {
      return Error::create(ErrorType::PLACEHOLDER, "name");
    }

    const auto t_destination = (*table)["destination"].as_string();
    if (!t_destination) {
      return Error::create(ErrorType::PLACEHOLDER, "destination");
    }

    const auto destination = parse_path(std::move(**t_destination));

    const auto include = parse_array_field(*table, "include");
    if (!include) {
      return Error::create(ErrorType::PLACEHOLDER, "include");
    }

    const auto exclude = parse_optional_array_field(*table, "exclude");
    if (!exclude) {
      return Error::create(ErrorType::PLACEHOLDER, "exclude");
    }

    const auto archive = parse_optional_array_field(*table, "archive");
    if (!archive) {
      return Error::create(ErrorType::PLACEHOLDER, "archive");
    }

    const auto protect = parse_optional_array_field(*table, "protect");
    if (!protect) {
      return Error::create(ErrorType::PLACEHOLDER, "protect");
    }

    groups.insert(Group(std::move(**name), std::move(destination), std::move(*include), std::move(*exclude),
                        std::move(*archive), std::move(*protect)));

    return {};
  });

  return Config(std::move(groups));
}

#ifdef DEBUG
auto Config::print() const& noexcept -> const void {
  std::ranges::for_each(m_groups, [&](const auto& entry) { entry.print(); });
}
#endif

Config::Config(const std::set<Group>&& groups) noexcept : m_groups(std::move(groups)) {}

auto Config::parse_optional_array_field(const toml::table& table, const std::string_view&& name) noexcept
    -> const expected<std::optional<std::set<std::filesystem::path>>> {
  if (!table.contains(name)) {
    return {};
  }

  return parse_array_field(table, std::move(name));
}

auto Config::parse_array_field(const toml::table& table, const std::string_view&& name) noexcept
    -> const expected<std::set<std::filesystem::path>> {
  const auto t_entry = table[name].as_array();
  if (!t_entry) {
    return Error::create(ErrorType::PLACEHOLDER, std::move(name));
  }

  std::set<std::filesystem::path> paths{};
  t_entry->for_each([&](const auto& element) {
    const auto string = **element.as_string(); // FIX: check
    if (string.length() > 0) {
      const auto path = parse_path(std::move(string));
      paths.insert(std::move(path));
    }
  });

  return paths;
}

auto Config::parse_path(const std::string&& path) noexcept -> const std::filesystem::path {
  if (path.at(0) == '~') {
    return parse_tilde(std::move(path));
  }

  return {path};
}

auto Config::parse_tilde(const std::string&& path) noexcept -> const std::filesystem::path {
  const static auto home = std::getenv("HOME");
  return {home + path.substr(1)};
}
} // namespace confie
