#pragma once
template <typename VT, class Tag>
class StrongType {
 public:
  using ValueType = VT;
  inline StrongType() : m_value() {}
  inline explicit StrongType(ValueType const& val) : m_value(val) {}
  inline operator ValueType() const { return m_value; }
  inline StrongType& operator=(StrongType const& newVal) {
    m_value = newVal.m_value;
    return *this;
  }
  inline StrongType& operator++() {
    m_value++;
    return *this;
  }
  inline StrongType operator++(int) {
    StrongType copy = *this;
    operator++();
    return copy;
  }
  inline StrongType& operator--() {
    m_value--;
    return *this;
  }
  inline StrongType operator--(int) {
    StrongType copy = *this;
    operator--();
    return copy;
  }
  inline StrongType& operator+=(ValueType val) {
    m_value += val;
    return *this;
  }
  inline StrongType& operator-=(ValueType val) {
    m_value -= val;
    return *this;
  }
  // friends defined inside class body are inline and are hidden from non-ADL lookup
  friend StrongType operator+(
      StrongType lhs,         // passing lhs by value helps optimize chained a+b+c
      const StrongType& rhs)  // otherwise, both parameters may be const references
  {
    lhs += rhs;  // reuse compound assignment
    return lhs;  // return the result by value (uses move constructor)
  }

  friend StrongType operator-(
      StrongType lhs,         // passing lhs by value helps optimize chained a+b+c
      const StrongType& rhs)  // otherwise, both parameters may be const references
  {
    lhs -= rhs;  // reuse compound assignment
    return lhs;  // return the result by value (uses move constructor)
  }

 private:
  //
  // data
  ValueType m_value;
};