#include <algorithm>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <range/v3/range.hpp>
#include <set>
#include <variant>

enum ErrorType {
  FILE_READ,
  NOT_DIR,
  NOT_EXIST,
  NOT_FILE,
};

struct Error {
public:
  static auto create(const ErrorType&& error_type, const std::string&& message) -> std::unexpected<Error> {
    return std::unexpected(Error(std::move(error_type), std::move(message)));
  }

  [[nodiscard]] auto get_message() const -> std::string { return m_message; }

  [[nodiscard]] auto get_error() const -> ErrorType { return m_error_type; }

private:
  const ErrorType m_error_type;
  const std::string m_message;

  [[nodiscard]] explicit Error(const ErrorType&& error_type, const std::string&& message)
      : m_error_type{std::move(error_type)}, m_message{std::move(message)} {}
};

template <typename R> using expected = std::expected<R, Error>;

struct File {
public:
  [[nodiscard]] explicit File() : m_path{} {}

private:
  const std::filesystem::path m_path;
};

struct Folder {
public:
  [[nodiscard]] explicit Folder() : m_path{}, m_entries{} {}

private:
  const std::filesystem::path m_path;
  const std::set<std::variant<File, Folder>> m_entries;
};

struct Group {
public:
private:
  std::string m_name;
  std::filesystem::path m_destination;
  std::set<std::filesystem::path> m_include;
  std::optional<std::set<std::filesystem::path>> m_exclude;
  std::optional<std::set<std::filesystem::path>> m_archive;
  std::optional<std::set<std::filesystem::path>> m_protect;
};

struct Configuration {
public:
  [[nodiscard]] static auto create(const std::optional<std::filesystem::path>&& path) -> expected<Configuration> {
    std::filesystem::path config_file{};
    const auto home = std::getenv("HOME");

    if (path.has_value()) {
      config_file = std::filesystem::path(*path);
    } else {
      const auto path_as_str = std::string(home).append("/.config/confie/config.toml");
      config_file = std::filesystem::path(path_as_str);
    }

    if (!std::filesystem::exists(config_file)) {
      return Error::create(NOT_EXIST, "Configuration file does not exist.");
    } else if (!std::filesystem::is_regular_file(config_file)) {
      return Error::create(NOT_FILE, "Configuration is not a file.");
    }

    std::ifstream file(config_file);
    if (!file.is_open()) {
      return Error::create(FILE_READ, "Failed reading configuration file.");
    }

    std::string line{};
    std::set<std::filesystem::path> paths{};

    while (std::getline(file, line)) {
      if (line.length() > 0) {
        if (line.at(0) == '~') {
          line = home + line.substr(1);
        }

        paths.insert(std::filesystem::path(line));
      }
    }

    return Configuration(std::move(config_file), std::move(paths));
  }

  [[nodiscard]] auto get_paths() const -> std::set<std::filesystem::path> { return m_paths; }

private:
  const std::filesystem::path m_config_file;
  const std::set<std::filesystem::path> m_paths;
  const std::set<Group> m_groups{};

  [[nodiscard]] explicit Configuration(const std::filesystem::path&& config_file,
                                       const std::set<std::filesystem::path>&& paths)
      : m_config_file(std::move(config_file)), m_paths(std::move(paths)), m_groups{} {}
};

// FIX:: std::ranges::transform
auto iterate(const Configuration&& config_file) -> std::set<std::filesystem::path> {
  std::set<std::filesystem::path> paths{};
  std::ranges::for_each(config_file.get_paths(), [&](const auto& path) {
    if (std::filesystem::is_regular_file(path)) {
      paths.insert(std::move(path));
    } else if (std::filesystem::is_directory(path)) {
      const auto iterator = std::filesystem::recursive_directory_iterator(std::move(path));
      std::ranges::for_each(iterator, [&](const auto& p) {
        if (std::filesystem::is_regular_file(p)) {
          paths.insert(std::move(p));
        }
      });
    }
  });

  return paths;
}

auto main(const int argc, const char* argv[]) -> int {
  std::optional<std::filesystem::path> config_path{};

  if (argc > 2) {
    std::println(stderr, "One optional argument [config file] is allowed. If "
                         "not provided, ~/.config/confie/config.toml is used.");
    return EXIT_FAILURE;
  } else if (argc == 2) {
    config_path = std::filesystem::path(argv[1]);
  }

  const auto config = Configuration::create(std::move(config_path));
  if (!config.has_value()) {
    std::println(stderr, "Error: {}", config.error().get_message());
    return config.error().get_error();
  }

  const auto entries = iterate(std::move(*config));
  std::ranges::for_each(entries, [&](const auto& e) { std::cout << e << '\n'; });

  return EXIT_SUCCESS;
}
