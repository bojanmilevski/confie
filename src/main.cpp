#include "config.hpp"
#include <filesystem>
#include <optional>
#include <print>

auto main(const int argc, const char* argv[]) noexcept -> int {
  std::optional<std::filesystem::path> config_path{};

  if (argc > 2) {
    std::println(stderr, "One optional argument [config file] is allowed. If "
                         "not provided, ~/.config/confie/config.toml is used.");
    std::exit(confie::ErrorType::CLI_PARSE);
  } else if (argc == 2) {
    config_path = std::filesystem::path(argv[1]);
  }

  const auto config = confie::Config::create(std::move(config_path));
  if (!config) {
    std::println(stderr, "Error: {}", config.error().get_message());
    std::exit(config.error().get_error());
  }

#ifdef DEBUG
  config->print();
#endif

  return EXIT_SUCCESS;
}
