#include <boost/any.hpp>
#include <boost/unordered_map.hpp>
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

////////////////////////////////////////////////////////////
// metaprogramming stuff
////////////////////////////////////////////////////////////
template<class... T>
using void_t = void;

template<class C> struct IsArray: std::false_type {};
template<class T> struct IsArray<std::vector<T>>: std::true_type {};
template<class T, size_t N> struct IsArray<std::array<T, N>>: std::true_type {};

template<class M> struct IsMap: std::false_type {};
template<class K, class V> struct IsMap<std::map<K, V>>: std::true_type {};
template<class K, class V> struct IsMap<std::unordered_map<K, V>>: std::true_type {};

////////////////////////////////////////////////////////////
// Print stuff
////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
// Type manager stuff
////////////////////////////////////////////////////////////
template<class T>
void write_any(std::ostream& out, boost::any const& value)
{
  auto const& v = boost::any_cast<T const&>(value);
  write(out, v);
};

class TypeManager {
 private:
  
  using WriterFunc = void (*)(std::ostream&, boost::any const&);
  
  boost::unordered_map<boost::typeindex::type_index, WriterFunc> type2func;

  template<class T>
  void RegisterType() {
    auto typeId = boost::typeindex::type_id<T>();
    if (type2func.count(typeId) == 0) {
      type2func.insert(std::make_pair(typeId, write_any<T>));
    }
  };

  TypeManager() {
    RegisterType<int>();
    RegisterType<std::string>();
    RegisterType<double>();
    RegisterType<std::vector<std::string>>();
    RegisterType<std::vector<double>>();
    RegisterType<std::map<std::string, double>>();
  }
 public:
      
  static TypeManager& Global() {
    static TypeManager manager;
    return manager;
  };

  WriterFunc GetWriter(boost::typeindex::type_index const& typeId) const {
    auto it = type2func.find(typeId);
    assert(it != type2func.end());
    return it->second;
  };
};

void PrintAny(std::ostream& out, boost::any const& value)
{
  auto& typeId = value.type();
  auto func = TypeManager::Global().GetWriter(typeId);
  func(out, value);
};

int main()
{
  std::vector<int> vec = {1,2,3};
  std::map<std::string, double> m = {{"one", 1.0}, {"two", 2.0}};
  write(std::cout, m);
  std::cout << std::endl;

  auto f = TypeManager::Global().GetWriter(boost::typeindex::type_id<int>());

  boost::any v{m};
  std::cout << "Any print:\n";
  PrintAny(std::cout, v);
}
