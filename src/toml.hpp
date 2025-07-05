#pragma once

#include "error.hpp"
#include "group.hpp"
#include <filesystem>
#include <set>
#include <string_view>
#include <toml++/toml.hpp>

namespace confie {
struct Toml {
public:
  [[nodiscard]] static auto create(std::filesystem::path&&) noexcept -> const expected<std::set<Group>>;
  Toml(const Toml&) = delete;
  Toml& operator=(const Toml&) = delete;

private:
  [[nodiscard]] static auto parse_optional_array(const toml::table&, std::string_view&&) noexcept
      -> const expected<std::optional<std::set<std::filesystem::path>>>;
  [[nodiscard]] static auto parse_array(const toml::table&, std::string_view&&) noexcept
      -> const expected<std::set<std::filesystem::path>>;
  [[nodiscard]] static auto parse_path(std::string_view&&) noexcept -> const std::filesystem::path;
  [[nodiscard]] static auto parse_tilde(std::string_view&&) noexcept -> const std::filesystem::path;
};
} // namespace confie
