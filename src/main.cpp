#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <print>
#include <range/v3/range.hpp>
#include <set>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

#ifdef DEBUG
#include <iostream>
#endif

enum ErrorType {
  CLI_PARSE,
  CONFIG_PARSE,
  FILE_READ,
  NOT_DIR,
  NOT_EXIST,
  NOT_FILE,
  PLACEHOLDER,
};

struct Error {
public:
  Error() = delete;
  //  Error(const Error&) = delete;
  Error& operator=(const Error&) = delete;

  static auto create(const ErrorType&& error_type, const std::string_view&& message) noexcept
      -> std::unexpected<Error> {
    return std::unexpected(Error(std::move(error_type), std::move(message)));
  }

  [[nodiscard]] auto get_message() const noexcept -> std::string_view { return m_message; }

  [[nodiscard]] auto get_error() const noexcept -> ErrorType { return m_error_type; }

private:
  const ErrorType m_error_type;
  const std::string_view m_message;

  [[nodiscard]] explicit Error(const ErrorType&& error_type, const std::string_view&& message) noexcept
      : m_error_type{std::move(error_type)}, m_message{std::move(message)} {}
};

template <typename R> using expected = std::expected<R, Error>;

struct Group {
public:
  Group() = delete;
  // Group(const Group&) = delete;
  Group& operator=(const Group&) = delete;
  ~Group() = default;

  [[nodiscard]] explicit Group(const std::string_view&& name, const std::filesystem::path&& destination,
                               const std::set<std::filesystem::path>&& include,
                               const std::optional<std::set<std::filesystem::path>>&& exclude,
                               const std::optional<std::set<std::filesystem::path>>&& archive,
                               const std::optional<std::set<std::filesystem::path>>&& protect) noexcept
      : m_name{std::move(name)}, m_destination{std::move(destination)}, m_include{std::move(include)},
        m_exclude{std::move(exclude)}, m_archive{std::move(archive)}, m_protect{std::move(protect)} {}

  const bool operator<(const Group& other) const { return m_name < other.m_name; }

#ifdef DEBUG
  [[nodiscard]] const auto print() const& noexcept {
    std::cout << "Name: " << m_name << '\n';
    std::cout << "Destination: " << m_destination << '\n';

    std::ranges::for_each(m_include, [&](const auto& entry) { std::cout << "Include: " << entry << '\n'; });

    if (m_exclude.has_value()) {
      std::ranges::for_each(*m_exclude, [&](const auto& entry) { std::cout << "Exclude: " << entry << '\n'; });
    }

    if (m_archive.has_value()) {
      std::ranges::for_each(*m_archive, [&](const auto& entry) { std::cout << "Archive: " << entry << '\n'; });
    }

    if (m_protect.has_value()) {
      std::ranges::for_each(*m_protect, [&](const auto& entry) { std::cout << "Protect: " << entry << '\n'; });
    }
  }
#endif

private:
  const std::string_view m_name;
  const std::filesystem::path m_destination;
  const std::set<std::filesystem::path> m_include;
  const std::optional<std::set<std::filesystem::path>> m_exclude;
  const std::optional<std::set<std::filesystem::path>> m_archive;
  const std::optional<std::set<std::filesystem::path>> m_protect;

  // FIX:: std::ranges::transform
  [[nodiscard]] auto iterate(const std::filesystem::path&& entry) const noexcept -> std::set<std::filesystem::path> {
    if (std::filesystem::is_regular_file(entry)) {
      return {};
    }

    std::set<std::filesystem::path> paths{};

    std::ranges::for_each(entry, [&](const auto& path) {
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
};

struct Configuration {
public:
  Configuration() = delete;
  // Configuration(const Configuration&) = delete;
  Configuration& operator=(const Configuration&) = delete;

  [[nodiscard]] static auto create(const std::optional<std::filesystem::path>&& path) noexcept
      -> expected<Configuration> {
    std::filesystem::path config_file_path{};
    const auto home = std::getenv("HOME");

    if (path) {
      config_file_path = std::filesystem::path(*path);
    } else {
      const auto path_str = std::basic_string_view(std::string(home).append("/.config/confie/config.toml"));
      config_file_path = std::filesystem::path(path_str);
    }

    if (!std::filesystem::exists(config_file_path)) {
      return Error::create(NOT_EXIST, "Configuration file does not exist.");
    } else if (!std::filesystem::is_regular_file(config_file_path)) {
      return Error::create(NOT_FILE, "Configuration is not a file.");
    }

    const auto toml = toml::parse_file(config_file_path.c_str());
    if (!toml) {
      return Error::create(PLACEHOLDER, "Unable to parse toml file");
    }

    if (!toml.table().contains("group")) {
      return Error::create(PLACEHOLDER, "Groups missing in configuration file");
    }

    const auto t_groups = toml["group"].as_array();
    if (!t_groups) {
      return Error::create(PLACEHOLDER, "groupS");
    }

    std::set<Group> groups{};
    t_groups->for_each([&](const auto& group) -> expected<void> {
      const auto table = group.as_table();
      if (!table) {
        return Error::create(PLACEHOLDER, "group");
      }

      const auto name = (*table)["name"].as_string();
      if (!name) {
        return Error::create(PLACEHOLDER, "name");
      }

      const auto t_destination = (*table)["destination"].as_string();
      if (!t_destination) {
        return Error::create(PLACEHOLDER, "destination");
      }

      const auto destination = parse_path(std::move(**t_destination));

      const auto include = parse_array_field(*table, "include");
      if (!include) {
        return Error::create(PLACEHOLDER, "include");
      }

      const auto exclude = parse_optional_array_field(*table, "exclude");
      if (!exclude) {
        return Error::create(PLACEHOLDER, "exclude");
      }

      const auto archive = parse_optional_array_field(*table, "archive");
      if (!archive) {
        return Error::create(PLACEHOLDER, "archive");
      }

      const auto protect = parse_optional_array_field(*table, "protect");
      if (!protect) {
        return Error::create(PLACEHOLDER, "protect");
      }

      groups.insert(Group(std::move(**name), std::move(destination), std::move(*include), std::move(*exclude),
                          std::move(*archive), std::move(*protect)));

      return {};
    });

    return Configuration(std::move(groups));
  }

#ifdef DEBUG
  auto print() const& noexcept -> void {
    std::ranges::for_each(m_groups, [&](const auto& entry) { entry.print(); });
  }
#endif

private:
  const std::set<Group> m_groups{};

  [[nodiscard]] explicit Configuration(const std::set<Group>&& groups) noexcept : m_groups(std::move(groups)) {}

  [[nodiscard]] auto static parse_optional_array_field(const toml::table& table, const std::string_view&& name) noexcept
      -> expected<std::optional<std::set<std::filesystem::path>>> {
    if (!table.contains(name)) {
      return {};
    }

    return parse_array_field(table, std::move(name));
  }

  [[nodiscard]] auto static parse_array_field(const toml::table& table, const std::string_view&& name) noexcept
      -> expected<std::set<std::filesystem::path>> {
    const auto t_entry = table[name].as_array();
    if (!t_entry) {
      return Error::create(PLACEHOLDER, std::move(name));
    }

    std::set<std::filesystem::path> paths{};
    t_entry->for_each([&](const auto& element) {
      const auto string = **element.as_string(); // FIX: check
      if (string.length() > 0) {
        const auto path = parse_path(std::move(string));
        paths.insert(std::move(path));
      }
    });

    return paths;
  }

  [[nodiscard]] auto static parse_path(const std::string&& path) noexcept -> std::filesystem::path {
    if (path.at(0) == '~') {
      return parse_tilde(std::move(path));
    }

    return {path};
  }

  [[nodiscard]] auto static parse_tilde(const std::string&& path) noexcept -> std::filesystem::path {
    const static auto home = std::getenv("HOME");
    return {home + path.substr(1)};
  }
};

auto main(const int argc, const char* argv[]) noexcept -> int {
  std::optional<std::filesystem::path> config_path{};

  if (argc > 2) {
    std::println(stderr, "One optional argument [config file] is allowed. If "
                         "not provided, ~/.config/confie/config.toml is used.");
    std::exit(ErrorType::CLI_PARSE);
  } else if (argc == 2) {
    config_path = std::filesystem::path(argv[1]);
  }

  const auto config = Configuration::create(std::move(config_path));
  if (!config) {
    std::println(stderr, "Error: {}", config.error().get_message());
    std::exit(config.error().get_error());
  }

#ifdef DEBUG
  config->print();
#endif

  return EXIT_SUCCESS;
}
