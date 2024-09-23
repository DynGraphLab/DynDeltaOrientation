#pragma once
namespace rpo::utils {
class SimpleIterator {
  size_t val;

 public:
  SimpleIterator(size_t v) : val(v) {}
  using value_type = size_t;
  auto operator*() const { return val; }
  SimpleIterator& operator++() {
    val++;
    return *this;
  }
  SimpleIterator operator++(int) {
    SimpleIterator copy = *this;
    operator++();
    return copy;
  }
  bool operator!=(const SimpleIterator& rhs) const { return val != rhs.val; }

  bool operator==(const SimpleIterator& rhs) const { return val == rhs.val; }
};
template <class Iterator>
struct Range {
  Iterator _b, _e;
  auto begin() { return _b; }
  auto end() { return _e; }
  const auto begin() const { return _b; }
  const auto end() const { return _e; }
  using value_type = typename Iterator::value_type;
  Range(Iterator begin, Iterator end) : _b(begin), _e(end) {}
};
template <class Container>
auto FullRange(const Container& c) {
  return Range(c.begin(), c.end());
}
}  // namespace rpo::utils