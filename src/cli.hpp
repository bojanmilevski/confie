#pragma once

#include "error.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace confie {
struct Cli {
public:
  [[nodiscard]] static auto parse(int, char**) noexcept -> const expected<Cli>;
  [[nodiscard]] auto get_cfg_path() const& noexcept -> const std::filesystem::path;

private:
  std::filesystem::path m_cfg_path;

  [[nodiscard]] explicit Cli(std::filesystem::path&&) noexcept;
  [[nodiscard]] static auto parse_path(std::optional<std::string>&&) noexcept -> const expected<std::filesystem::path>;
  static auto parse_log_level(std::optional<std::string>&&) noexcept -> const void;
};
} // namespace confie
