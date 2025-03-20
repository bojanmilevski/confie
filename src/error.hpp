#pragma once

#include <expected>
#include <string_view>

namespace confie {
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
  static auto create(const ErrorType&&, const std::string_view&&) noexcept -> const std::unexpected<Error>;
  [[nodiscard]] auto get_message() const& noexcept -> const std::string_view&;
  [[nodiscard]] auto get_error() const& noexcept -> const ErrorType&;

private:
  const ErrorType m_error_type;
  const std::string_view m_message;
  [[nodiscard]] explicit Error(const ErrorType&&, const std::string_view&&) noexcept;
};

template <typename R> using expected = std::expected<R, Error>;
} // namespace confie
