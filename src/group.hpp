#pragma once

#include <filesystem>
#include <optional>
#include <range/v3/view.hpp>
#include <set>
#include <string_view>

namespace confie {
struct Group {
public:
  Group() = delete;
  Group& operator=(const Group&) = delete;

  [[nodiscard]] explicit Group(std::string_view&&, std::filesystem::path&&, std::set<std::filesystem::path>&&,
                               std::optional<std::set<std::filesystem::path>>&&,
                               std::optional<std::set<std::filesystem::path>>&&,
                               std::optional<std::set<std::filesystem::path>>&&) noexcept;

  auto operator<(const Group&) const& -> const bool;

  auto print() const& noexcept -> const void;

private:
  std::string_view m_name;
  std::filesystem::path m_destination;
  std::set<std::filesystem::path> m_include;
  std::optional<std::set<std::filesystem::path>> m_exclude;
  std::optional<std::set<std::filesystem::path>> m_archive;
  std::optional<std::set<std::filesystem::path>> m_protect;
  [[nodiscard]] auto iterate(const std::filesystem::path&) const& noexcept
      -> const std::set<std::filesystem::path>; // FIX:: std::ranges::transform
};
} // namespace confie
