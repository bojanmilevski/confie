#include "cli.hpp"
#include "CLI/CLI.hpp"
#include "error.hpp"
#include <optional>

#define DESCRIPTION "Placeholder description"

namespace confie {
Cli::Cli(std::optional<std::string_view>&& cfg_path, std::string_view&& log_level) noexcept
    : m_cfg_path(std::move(cfg_path)), m_log_lvl(std::move(log_level)) {}

auto Cli::parse(int argc, char** argv) noexcept -> expected<Cli> {
  CLI::App cli{DESCRIPTION};
  argv = cli.ensure_utf8(argv);

  std::string_view log_level = "info";
  std::optional<std::string_view> cfg_path;

  cli.add_option("-l,--log", log_level, "Log level");
  cli.add_option("-c,--configuration-file", cfg_path, "Configuration file");
  cli.usage(DESCRIPTION);

  try {
    cli.parse(argc, argv);
  } catch (const CLI::ParseError& error) {
    return Error::create(ErrorType::PLACEHOLDER, error.what());
  }

  return Cli(std::move(cfg_path), std::move(log_level));
}

auto Cli::get_cfg_path() const& noexcept -> std::optional<std::string_view> { return m_cfg_path; }
} // namespace confie
