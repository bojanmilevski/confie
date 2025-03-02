#include <cstdlib>
#include <expected>
#include <filesystem>
#include <iostream>
#include <print>
#include <range/v3/all.hpp>

enum Error {
  NOT_DIR,
  NOT_EXIST,
};

auto iterate(const std::filesystem::path &path) -> std::expected<void, Error> {
  const auto entry = std::filesystem::directory_entry(path);
  if (!entry.is_directory() || entry.is_symlink()) {
    return std::unexpected(Error::NOT_DIR);
  }

  std::cout << entry << '\n';

  const auto iterator = std::filesystem::directory_iterator(entry);
  std::ranges::for_each(iterator, iterate);  // FIX: implicit conversion

  return {};
}

struct ConfigFile {
 private:
  std::filesystem::path m_path;

  [[nodiscard]] explicit ConfigFile(const std::filesystem::path &&path)
      : m_path(std::move(path)) {}

 public:
  [[nodiscard]] static auto create() -> std::expected<ConfigFile, Error> {
    const auto home = getenv("HOME");
    const auto path_as_str =
        std::string(home).append("/.config/confie/config.toml");
    auto path = std::filesystem::path(path_as_str);

    if (!std::filesystem::exists(path)) {
      return std::unexpected(Error::NOT_EXIST);
    }

    return ConfigFile(std::move(path));
  }

  const std::string get_path() const { return m_path; }
};

auto main(const int argc, const char *argv[]) -> int {
  if (argc != 2) {
    std::println(stderr, "Path required!");
    return EXIT_FAILURE;
  }

  const auto config_file = ConfigFile::create();
  if (!config_file.has_value()) {
    if (config_file.error() == Error::NOT_EXIST) {
      std::println(stderr, "Config file does not exist!");
    } else {
      std::println(stderr, "Unknown error occured!");
    }

    return config_file.error();
  }

  const auto res = iterate(argv[1]);  // FIX: implicit conversion
  if (!res.has_value()) {
    std::println(stderr, "Unrecoverable error!");
    return res.error();
  }

  return EXIT_SUCCESS;
}
