#include <algorithm>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <range/v3/all.hpp>

enum Error {
  FILE_READ,
  NOT_DIR,
  NOT_EXIST,
  NOT_FILE,
};

struct ConfigFile {
private:
  const std::filesystem::path m_config_file;
  const std::vector<std::filesystem::path> m_paths;

  [[nodiscard]] explicit ConfigFile(
      const std::filesystem::path& config_file,
      const std::vector<std::filesystem::path>& paths)
      : m_config_file(std::move(config_file)), m_paths(std::move(paths)) {}

public:
  [[nodiscard]] static auto create() -> std::expected<ConfigFile, Error> {
    const auto home = std::getenv("HOME");
    const auto path_as_str =
        std::string(home).append("/.config/confie/config.toml");
    const auto config_file = std::filesystem::path(path_as_str);

    if (!std::filesystem::exists(config_file)) {
      return std::unexpected(Error::NOT_EXIST);
    } else if (!std::filesystem::is_regular_file(config_file)) {
      return std::unexpected(Error::NOT_FILE);
    }

    std::ifstream file(config_file);
    if (!file.is_open()) {
      return std::unexpected(Error::FILE_READ);
    }

    std::string line{};
    std::vector<std::filesystem::path> paths{};

    while (std::getline(file, line)) {
      paths.push_back(line);
    }

    return ConfigFile(config_file, paths);
  }

  operator std::filesystem::path() const { return m_config_file; }

  [[nodiscard]] const std::vector<std::filesystem::path> get_paths() const {
    return m_paths;
  }
};

auto iterate(const ConfigFile& config_file)
    -> std::expected<std::vector<std::filesystem::path>, Error> {
  std::vector<std::filesystem::path> paths{};

  std::ranges::for_each(config_file.get_paths(), [&](const auto& path) {
    if (std::filesystem::is_regular_file(path)) {
      paths.push_back(path);
    } else if (std::filesystem::is_directory(path)) {
      const auto iterator = std::filesystem::recursive_directory_iterator(path);
      std::ranges::for_each(iterator, [&](const auto& e) {
        if (std::filesystem::is_regular_file(e)) {
          paths.push_back(e);
        }
      });
    }
  });

  return paths;
}

auto main(const int argc, const char* argv[]) -> int {
  const auto config_file = ConfigFile::create();
  if (!config_file.has_value()) {
    if (config_file.error() == Error::NOT_EXIST) {
      std::println(stderr, "Configuration does not exist!");
    } else if (config_file.error() == Error::NOT_DIR) {
      std::println(stderr, "Configuration is not a regular file!");
    } else if (config_file.error() == Error::FILE_READ) {
      std::println(stderr, "Error reading configuration file!");
    } else {
      std::println(stderr, "Unknown error occured!");
    }

    return config_file.error();
  }

  const auto res = iterate(*config_file);
  if (!res.has_value()) {
    std::println(stderr, "Error iterating!");
    return res.error();
  }

  std::ranges::for_each(*res, [&](const auto& e) { std::cout << e << '\n'; });
  return EXIT_SUCCESS;
}
