#ifndef CONQUEST_UTIL_H_
#define CONQUEST_UTIL_H_


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

#endif // CONQUEST_UTIL_H_