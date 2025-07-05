#include "cli.hpp"
#include "error.hpp"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

#define DESCRIPTION "Placeholder description"

namespace confie {
Cli::Cli(std::filesystem::path&& cfg_path) noexcept : m_cfg_path(std::move(cfg_path)) { spdlog::trace("Cli::Cli"); }

auto Cli::parse(int argc, char** argv) noexcept -> const expected<Cli> {
  spdlog::trace("Cli::parse");

  CLI::App cli{DESCRIPTION};
  argv = cli.ensure_utf8(argv);

  std::string log_level = "info";
  std::optional<std::string> cfg_path;

  cli.add_option("-l,--log", log_level, "Log level");
  cli.add_option("-c,--config", cfg_path, "Configuration file");
  cli.usage(DESCRIPTION);

  try {
    cli.parse(argc, argv);
  } catch (const CLI::ParseError& error) {
    return Error::create(ErrorType::PLACEHOLDER, error.what());
  }

  parse_log_level(std::move(log_level));

  auto path = parse_path(std::move(cfg_path));
  if (!path) {
    return Error::create(std::move(path.error().error()), std::move(path.error().what()));
  }

  return Cli(std::move(*path));
}

auto Cli::parse_path(std::optional<std::string>&& path) noexcept -> const expected<std::filesystem::path> {
  spdlog::trace("Cli::parse_path");

  std::filesystem::path cfg_path{};
  const auto home = std::getenv("HOME"); // FIX: make a "global" home var

  if (path) {
    cfg_path = std::filesystem::path(std::move(*path));
  } else {
    const auto path_str = std::string(home).append("/.config/confie/config.toml");
    cfg_path = std::filesystem::path(std::move(path_str));
  }

  if (!std::filesystem::exists(cfg_path)) {
    return Error::create(ErrorType::PLACEHOLDER, "Configuration file does not exist.");
  } else if (!std::filesystem::is_regular_file(cfg_path)) {
    return Error::create(ErrorType::PLACEHOLDER, "Configuration is not a file.");
  }

  return cfg_path;
}

auto Cli::parse_log_level(std::optional<std::string>&& log_level) noexcept -> const void {
  spdlog::trace("Cli::parse_log_level");

  spdlog::set_pattern("[%d.%m.%Y %H:%M:%S:%e] [%^%l%$] %v");

  if (!log_level) {
    spdlog::set_level(spdlog::level::off);
  } else {
    if (*log_level == "info" || *log_level == "i") {
      spdlog::set_level(spdlog::level::info);
    } else if (*log_level == "warn" || *log_level == "w") {
      spdlog::set_level(spdlog::level::warn);
    } else if (*log_level == "error" || *log_level == "e") {
      spdlog::set_level(spdlog::level::err);
    } else if (*log_level == "debug" || *log_level == "d") {
      spdlog::set_level(spdlog::level::debug);
    } else if (*log_level == "trace" || *log_level == "t") {
      spdlog::set_level(spdlog::level::trace);
    } else if (*log_level == "critical" || *log_level == "c") {
      spdlog::set_level(spdlog::level::critical);
    }
  }
}

auto Cli::get_cfg_path() const& noexcept -> const std::filesystem::path {
  spdlog::trace("Cli::get_cfg_path");
  return m_cfg_path;
}
} // namespace confie
