#include "cli.hpp"
#include "config.hpp"
#include "error.hpp"
#include "spdlog/spdlog.h"
#include <print>
#include <spdlog/common.h>

auto exit(confie::Error&& error) noexcept -> void {
  std::println(stderr, "Error: {}", error.get_message());
  std::exit(error.get_error());
}

auto main(int argc, char* argv[]) noexcept -> int {
  spdlog::set_level(spdlog::level::debug); // FIX: this should be in confie::Cli
  spdlog::set_pattern("[%d.%m.%Y %H:%M:%S:%e] [%^%l%$] %v");

  auto cli = confie::Cli::parse(argc, argv);
  if (!cli) {
    exit(std::move(cli.error()));
  }

  const auto config = confie::Config::create(std::move(*cli));
  if (!config) {
    exit(std::move(cli.error()));
  }

#ifdef DEBUG
  config->print();
#endif

  return EXIT_SUCCESS;
}
