#pragma once

#include "error.hpp"
#include "group.hpp"
#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

namespace confie {
struct Config {
public:
  Config() = delete;
  // Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  [[nodiscard]] static auto create(std::optional<std::filesystem::path>&&) noexcept -> const expected<Config>;

#ifdef DEBUG
  auto print() const& noexcept -> const void;
#endif

private:
  std::set<Group> m_groups;

  [[nodiscard]] explicit Config(const std::set<Group>&&) noexcept;
  [[nodiscard]] static auto parse_optional_array(const toml::table&, std::string_view&&) noexcept
      -> const expected<std::optional<std::set<std::filesystem::path>>>;
  [[nodiscard]] static auto parse_array(const toml::table&, std::string_view&&) noexcept
      -> const expected<std::set<std::filesystem::path>>;
  [[nodiscard]] static auto parse_path(std::string&&) noexcept -> const std::filesystem::path;
  [[nodiscard]] static auto parse_tilde(std::string&&) noexcept -> const std::filesystem::path;
};
} // namespace confie
