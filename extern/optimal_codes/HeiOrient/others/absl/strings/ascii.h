#include <cctype>
#include <string>
namespace absl {
std::string StripLeadingAsciiWhitespace(std::string *str1) {
  std::string str = *str1;
  auto it = str.begin();
  while (it != str.end() && std::isspace(static_cast<unsigned char>(*it))) {
    ++it;
  }
  return str.substr(std::distance(str.begin(), it));
}
} // namespace absl
