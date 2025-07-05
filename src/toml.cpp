/*
  FIX:
   - before accessing a value from the toml file, check if the table contains
   it? this might not be necessary. look into it.
   - check if an array has empty string_views ("")
   - check if an array is empty (array=[])
 */

#include "toml.hpp"
#include "error.hpp"
#include <filesystem>
#include <set>
#include <spdlog/spdlog.h>
#include <string_view>

namespace confie {
auto Toml::create(std::filesystem::path&& path) noexcept -> const expected<std::set<Group>> {
  spdlog::trace("Toml::create");

  const auto toml = toml::parse_file(path.c_str());
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

  // FIX: this lambda's return should be handled better
  // ideally, the lambda should return this.
  std::set<Group> groups{};
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

    auto destination = parse_path(std::move(**t_destination));

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

  return groups;
}

auto Toml::parse_optional_array(const toml::table& table, std::string_view&& name) noexcept
    -> const expected<std::optional<std::set<std::filesystem::path>>> {
  spdlog::trace("Toml::parse_optional_array");

  if (!table.contains(name)) {
    return {};
  }

  return parse_array(table, std::move(name));
}

auto Toml::parse_array(const toml::table& table, std::string_view&& name) noexcept
    -> const expected<std::set<std::filesystem::path>> {
  spdlog::trace("Toml::parse_array");

  const auto t_entry = table[name].as_array();
  if (!t_entry) {
    return Error::create(ErrorType::PLACEHOLDER, std::move(name));
  }

  std::set<std::filesystem::path> paths{};
  t_entry->for_each([&](const auto& element) {
    auto string_view = **element.as_string(); // FIX: check
    if (string_view.length() > 0) {
      const auto path = parse_path(std::move(string_view));
      paths.insert(std::move(path));
    }
  });

  return paths;
}

auto Toml::parse_path(std::string_view&& path) noexcept -> const std::filesystem::path {
  spdlog::trace("Toml::parse");

  if (path.at(0) == '~') {
    return parse_tilde(std::move(path));
  }

  return {path};
}

auto Toml::parse_tilde(std::string_view&& path) noexcept -> const std::filesystem::path {
  spdlog::trace("Toml::parse_tilde");
  const static auto home = std::getenv("HOME"); // FIX: make a "global" home var
  return {home + std::string(path.substr(1))};
}
} // namespace confie
