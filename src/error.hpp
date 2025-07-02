#pragma once

#include <expected>
#include <string_view>

namespace confie {
enum ErrorType {
  PLACEHOLDER,
};

struct Error {
public:
  Error() = delete;
  Error& operator=(const Error&) = delete;
  static auto create(ErrorType&&, std::string_view&&) noexcept -> const std::unexpected<Error>;
  [[nodiscard]] auto get_message() const& noexcept -> const std::string_view&;
  [[nodiscard]] auto get_error() const& noexcept -> const ErrorType&;

private:
  ErrorType m_error_type;
  std::string_view m_message;
  [[nodiscard]] explicit Error(ErrorType&&, std::string_view&&) noexcept;
};

template <typename R> using expected = std::expected<R, Error>;
} // namespace confie
