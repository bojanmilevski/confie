#pragma once

#include "error.hpp"
#include <optional>
#include <string_view>

namespace confie {
struct Cli {
public:
  [[nodiscard]] static auto parse(int, char**) noexcept -> expected<Cli>;
  [[nodiscard]] auto get_cfg_path() const& noexcept -> std::optional<std::string_view>;

private:
  std::optional<std::string_view> m_cfg_path;
  std::string_view m_log_lvl;

  [[nodiscard]] explicit Cli(std::optional<std::string_view>&&, std::string_view&&) noexcept;
};
} // namespace confie
