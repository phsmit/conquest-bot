#pragma once

template<class T>
class RangeClass {
  T n;
public:
  RangeClass(T n): n(n) {}

  class Iterator {
  private:
    T i;
  public:
    Iterator(T i): i(i) {}

    bool operator!=(const Iterator &other) const {
      return i != other.i;
    }

    const Iterator &operator++() {
      ++i;
      return *this;
    }

    T operator*() const {
      return i;
    }
  };

  Iterator begin() {
    return Iterator(0);
  }

  Iterator end() {
    return Iterator(n);
  }

};

template<class T>
RangeClass<T> range(T i) {
  return RangeClass<T>(i);
}

template<class T>
typename T::value_type sum(T container) {
  typename T::value_type sum_ = 0;
  for (auto value : container) sum_ += value;
  return sum_;
}
