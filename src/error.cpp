#include "error.hpp"
#include <expected>
#include <spdlog/spdlog.h>
#include <string_view>

namespace confie {
Error::Error(ErrorType&& type, std::string_view&& message) noexcept
    : m_type{std::move(type)}, m_message{std::move(message)} {
  spdlog::trace("Error::Error");
}

auto Error::create(ErrorType&& error_type, std::string_view&& message) noexcept -> const std::unexpected<Error> {
  return std::unexpected(Error(std::move(error_type), std::move(message)));
}

auto Error::what() noexcept -> std::string_view& { return m_message; }

auto Error::error() noexcept -> ErrorType& { return m_type; }
} // namespace confie
