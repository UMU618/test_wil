#include <exception>
#include <iostream>

#include <wil/result.h>

void BlowUp() try { throw std::exception{"Too bad!"}; }
CATCH_LOG()

int main() {
  std::setlocale(LC_CTYPE, "");
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

  std::cout << "1. LOG_HR\n";
  LOG_HR(E_UNEXPECTED);
  std::cout << "2. CATCH_LOG\n";
  BlowUp();
}
