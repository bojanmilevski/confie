#include "cli.hpp"
#include "config.hpp"
#include "error.hpp"
#include <print>
#include <spdlog/spdlog.h>

auto exit(confie::Error&& error) noexcept -> const void {
  spdlog::trace("exit");
  std::println(stderr, "Error: {}", error.what());
  std::exit(std::move(error.error()));
}

auto main(int argc, char* argv[]) noexcept -> int {
  auto cli = confie::Cli::parse(argc, argv);
  if (!cli) {
    exit(std::move(cli.error()));
  }

  auto config = confie::Config::create(std::move(*cli));
  if (!config) {
    exit(std::move(cli.error()));
  }

#ifdef DEBUG
  config->print();
#endif

  return EXIT_SUCCESS;
}
