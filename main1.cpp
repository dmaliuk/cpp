#include <any.h>
#include <cassert>
#include <vector>
#include <map>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <stdint.h>
#include <ostream>
#include <type_traits>
#include <unordered_map>
#include <array>

// metaprogramming stuff
template<class... T>
using void_t = void;

template<class C> struct IsArray: std::false_type {};
template<class T> struct IsArray<std::vector<T>>: std::true_type {};
template<class T, size_t N> struct IsArray<std::array<T, N>>: std::true_type {};

template<class M> struct IsMap: std::false_type {};
template<class K, class V> struct IsMap<std::map<K, V>>: std::true_type {};
template<class K, class V> struct IsMap<std::unordered_map<K, V>>: std::true_type {};

// IO stuff
template<class T>
void write(std::ostream& out, T const& value);

template<class T>
void writer_helper(std::ostream& out, T const& value, ...)
{
  out << value;
};
    
template<>
void writer_helper(std::ostream& out, std::string const& value, ...)
{
  out << '"' << value << '"';
};

template<class C>
void writer_helper(std::ostream& os, C const& vec, std::enable_if_t<IsArray<C>::value>*)
{
  os << '[';
  for (int i = 0; i < vec.size(); ++i)
  {
    write(os, vec[i]);
    if ((i + 1) < vec.size())
      os << ", ";
  }
  os << ']';
};

template<class M>
void writer_helper(std::ostream& os, M const& m, std::enable_if_t<IsMap<M>::value>*)
{
  os << '{';
  for (auto const& kv: m)
  {
    write(os, kv.first);
    os << ": ";
    write(os, kv.second);
    os << ", ";
  }
  os << '}';
}

template<class T>
void write(std::ostream& out, T const& value)
{
  writer_helper(out, value, (void*)nullptr);
}


int main()
{
  std::vector<int> vec = {1,2,3};
  std::map<std::string, double> m = {{"one", 1.0}, {"two", 2.0}};
  write(std::cout, m);
  std::cout << std::endl;

}
