#pragma once

#include <filesystem>
#include <optional>
#include <range/v3/view.hpp>
#include <set>
#include <string_view>

#ifdef DEBUG
#include <iostream>
#endif

namespace confie {
struct Group {
public:
  Group() = delete;
  // Group(const Group&) = delete;
  Group& operator=(const Group&) = delete;

  [[nodiscard]] explicit Group(const std::string_view&&, const std::filesystem::path&&,
                               const std::set<std::filesystem::path>&&,
                               const std::optional<std::set<std::filesystem::path>>&&,
                               const std::optional<std::set<std::filesystem::path>>&&,
                               const std::optional<std::set<std::filesystem::path>>&&) noexcept;

  auto operator<(const Group&) const& -> const bool;

#ifdef DEBUG
  auto print() const& noexcept -> const void;
#endif

private:
  const std::string_view m_name;
  const std::filesystem::path m_destination;
  const std::set<std::filesystem::path> m_include;
  const std::optional<std::set<std::filesystem::path>> m_exclude;
  const std::optional<std::set<std::filesystem::path>> m_archive;
  const std::optional<std::set<std::filesystem::path>> m_protect;
  [[nodiscard]] auto iterate(const std::filesystem::path&&) const& noexcept
      -> const std::set<std::filesystem::path>; // FIX:: std::ranges::transform
};
} // namespace confie
