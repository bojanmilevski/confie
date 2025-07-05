#include "config.hpp"
#include "cli.hpp"
#include "error.hpp"
#include "group.hpp"
#include "toml.hpp"
#include <filesystem>
#include <set>
#include <spdlog/spdlog.h>
#include <string_view>

namespace confie {
Config::Config(std::set<Group>&& groups) noexcept : m_groups(std::move(groups)) {}

auto Config::create(Cli&& cli) noexcept -> const expected<Config> {
  spdlog::trace("Config::create");

  auto cfg_path = cli.get_cfg_path();

  if (cfg_path.extension() == "yaml") {
    // FIX: fill this in
  }

  auto groups = Toml::create(std::move(cfg_path));
  if (!groups) {
    return Error::create(ErrorType::PLACEHOLDER, "toml");
  }

  return Config(std::move(*groups));
}

auto Config::print() const& noexcept -> const void {
  spdlog::trace("Config::print");
  std::ranges::for_each(m_groups, [&](const auto& entry) { entry.print(); });
}
} // namespace confie
