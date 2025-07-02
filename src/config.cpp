#include "config.hpp"
#include "cli.hpp"
#include "error.hpp"
#include "group.hpp"
#include "toml.hpp"
#include <filesystem>
#include <set>
#include <string>
#include <string_view>

namespace confie {
Config::Config(std::set<Group>&& groups) noexcept : m_groups(std::move(groups)) {}

auto Config::create(Cli&& cli) noexcept -> const expected<Config> {
  std::filesystem::path config_file_path{};
  const auto cfg_path = cli.get_cfg_path();
  const auto home = std::getenv("HOME"); // FIX: make a "global" home var

  if (cfg_path) {
    config_file_path = std::filesystem::path(std::move(*cfg_path));
  } else {
    // INFO: prioritize the toml file
    const auto path_str = std::string(home).append("/.config/confie/config.toml");
    config_file_path = std::filesystem::path(std::move(path_str));
  }

  if (!std::filesystem::exists(config_file_path)) {
    return Error::create(ErrorType::PLACEHOLDER, "Configuration file does not exist.");
  } else if (!std::filesystem::is_regular_file(config_file_path)) {
    return Error::create(ErrorType::PLACEHOLDER, "Configuration is not a file.");
  }

  if (config_file_path.extension() == "yaml") {
    // FIX: fill this in
  }

  auto groups = Toml::create(std::move(config_file_path));
  if (!groups) {
    return Error::create(ErrorType::PLACEHOLDER, "toml");
  }

  return Config(std::move(*groups));
}

auto Config::print() const& noexcept -> const void {
  std::ranges::for_each(m_groups, [&](const auto& entry) { entry.print(); });
}
} // namespace confie
