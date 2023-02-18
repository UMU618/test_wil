#include <exception>
#include <iostream>

#include <wil/result.h>

void BlowUp() try { throw std::exception{"Too bad!"}; }
CATCH_LOG()

int main() {
  // std::setlocale(LC_CTYPE, ""); // C Style
  std::locale::global(std::locale("", LC_CTYPE));
  std::cout << "=== test_wil - log ===\n";

  // Print every log message to standard error.
  wil::SetResultLoggingCallback([](wil::FailureInfo const& failure) noexcept {
    constexpr std::size_t sizeOfLogMessageWithNul = 2048;

    wchar_t logMessage[sizeOfLogMessageWithNul];
    if (SUCCEEDED(wil::GetFailureLogString(logMessage, sizeOfLogMessageWithNul,
                                           failure))) {
      std::fputws(logMessage, stderr);
    }
  });

  SetLastError(ERROR_ACCESS_DENIED);
  std::cout << "0. LOG_LAST_ERROR\n";
  LOG_LAST_ERROR();
  LOG_IF_WIN32_ERROR(NO_ERROR);
  LOG_IF_WIN32_ERROR(ERROR_NOACCESS);
  std::cout << "1. LOG_HR\n";
  LOG_HR(E_UNEXPECTED);
  std::cout << "2. CATCH_LOG\n";
  BlowUp();
}
