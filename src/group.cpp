#include "group.hpp"
#include <filesystem>
#include <optional>
#include <range/v3/view.hpp>
#include <set>
#include <string_view>

#ifdef DEBUG
#include <iostream>
#endif

namespace confie {
Group::Group(const std::string_view&& name, const std::filesystem::path&& destination,
             const std::set<std::filesystem::path>&& include,
             const std::optional<std::set<std::filesystem::path>>&& exclude,
             const std::optional<std::set<std::filesystem::path>>&& archive,
             const std::optional<std::set<std::filesystem::path>>&& protect) noexcept
    : m_name{std::move(name)}, m_destination{std::move(destination)}, m_include{std::move(include)},
      m_exclude{std::move(exclude)}, m_archive{std::move(archive)}, m_protect{std::move(protect)} {}

auto Group::operator<(const Group& other) const& -> const bool { return m_name < other.m_name; }

#ifdef DEBUG
auto Group::print() const& noexcept -> const void {
  std::cout << "Name: " << m_name << '\n';
  std::cout << "Destination: " << m_destination << '\n';

  std::ranges::for_each(m_include, [&](const auto& entry) { std::cout << "Include: " << entry << '\n'; });

  if (m_exclude.has_value()) {
    std::ranges::for_each(*m_exclude, [&](const auto& entry) { std::cout << "Exclude: " << entry << '\n'; });
  }

  if (m_archive.has_value()) {
    std::ranges::for_each(*m_archive, [&](const auto& entry) { std::cout << "Archive: " << entry << '\n'; });
  }

  if (m_protect.has_value()) {
    std::ranges::for_each(*m_protect, [&](const auto& entry) { std::cout << "Protect: " << entry << '\n'; });
  }
}
#endif

// FIX:: std::ranges::transform
auto Group::iterate(const std::filesystem::path&& entry) const& noexcept -> const std::set<std::filesystem::path> {
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
