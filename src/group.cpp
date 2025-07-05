#include "group.hpp"
#include <filesystem>
#include <optional>
#include <range/v3/view.hpp>
#include <set>
#include <spdlog/spdlog.h>
#include <string_view>

namespace confie {
Group::Group(std::string_view&& name, std::filesystem::path&& destination, std::set<std::filesystem::path>&& include,
             std::optional<std::set<std::filesystem::path>>&& exclude,
             std::optional<std::set<std::filesystem::path>>&& archive,
             std::optional<std::set<std::filesystem::path>>&& protect) noexcept
    : m_name{std::move(name)}, m_destination{std::move(destination)}, m_include{std::move(include)},
      m_exclude{std::move(exclude)}, m_archive{std::move(archive)}, m_protect{std::move(protect)} {
  spdlog::trace("Group::Group");
}

auto Group::operator<(const Group& other) const& -> const bool {
  spdlog::trace("Group::operator<");
  return m_name < other.m_name;
}

auto Group::print() const& noexcept -> const void {
  spdlog::trace("Group::print");

  spdlog::debug("Name: {}", m_name);
  spdlog::debug("Destination: {}", m_destination.c_str());

  spdlog::debug("Include:");
  std::ranges::for_each(m_include, [&](const auto& entry) { spdlog::debug("  - {}", entry.c_str()); });

  spdlog::debug("Exclude:");
  if (m_exclude.has_value()) {
    std::ranges::for_each(*m_exclude, [&](const auto& entry) { spdlog::debug("  - {}", entry.c_str()); });
  }

  spdlog::debug("Archive:");
  if (m_archive.has_value()) {
    std::ranges::for_each(*m_archive, [&](const auto& entry) { spdlog::debug("  - {}", entry.c_str()); });
  }

  spdlog::debug("Protect:");
  if (m_protect.has_value()) {
    std::ranges::for_each(*m_protect, [&](const auto& entry) { spdlog::debug("  - {}", entry.c_str()); });
  }
}

// FIX:: std::ranges::transform
auto Group::iterate(const std::filesystem::path& entry) const& noexcept -> const std::set<std::filesystem::path> {
  spdlog::trace("Group::iterate");

  if (std::filesystem::is_regular_file(entry)) {
    return {};
  }

  std::set<std::filesystem::path> paths{};

  std::ranges::for_each(entry, [&](const auto& path) {
    if (std::filesystem::is_regular_file(path)) {
      paths.insert(std::move(path));
    } else if (std::filesystem::is_directory(path)) {
      const auto iterator = std::filesystem::recursive_directory_iterator(std::move(path));
      std::ranges::for_each(iterator, [&](const auto& p) {
        if (std::filesystem::is_regular_file(p)) {
          paths.insert(std::move(p));
        }
      });
    }
  });

  return paths;
}
} // namespace confie
