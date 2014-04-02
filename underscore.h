/*
UNDERSCORE_CPP by Viktor Sehr

TODO:
Important
  mutable_container as rvalue!!
  Transform!
  is_container or more containers for pretty_print
  Sub view
  mutable sub_view only from mutable containers
  tokenize string


Not so important:
  iota
  Where view

Done:
-- make_string with accessors
-- add asserts
-- find_first_of
-- find_first_not_of
-- overload find for set etc
-- mutate tag
-- subview tag (index etc)
-- accessor tags (front, back, middle)
-- math tags
-- pretty_print container
-- return values of rvalue containers
-- contains_any_of/is_any_of
-- string functions (trim, replace_seq, to_lower, to_upper etc)
-- replace string
-- rotated
-- checked_delete
-- type_name

*/


#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <deque>

// Configuration
#ifndef UNDERSCORE_ASSERT
  #define UNDERSCORE_ASSERT(condition) if ((condition) == false) std::cout << "Underscore Assert Failed at: " << __LINE__ << std::endl;
#endif

#ifndef PIPE_OPERATOR
  #define PIPE_OPERATOR operator|
#endif

#ifndef UNDERSCORE_STATIC_ASSERT
  #define UNDERSCORE_STATIC_ASSERT static_assert
#endif

// C++14 backwards compatiblity
#define UNDERSCORE_CBEGIN(container)  container.begin()
#define UNDERSCORE_CEND(container)    container.end()
#define UNDERSCORE_RBEGIN(container)  container.rbegin()
#define UNDERSCORE_REND(container)    container.rend()
#define UNDERSCORE_CRBEGIN(container) container.rbegin()
#define UNDERSCORE_CREND(container)   container.rend()
#define UNDERSCORE_MOVE_STRINGSTREAM(source, destination) \
  destination.str(source.str()); \
  source.clear();


/////////////////////////////////////////////////////////////////////////////////////////////
/// Detail
/////////////////////////////////////////////////////////////////////////////////////////////
namespace UnderscoreDetail {

  // mutable_container_base
  template <typename ContainerType>
  class mutable_container_base {
    typedef mutable_container_base<ContainerType> my_type;
  public:
    typedef typename ContainerType::const_iterator const_iterator;
    typedef typename ContainerType::iterator iterator;
    typedef typename ContainerType::value_type value_type;
    typedef typename ContainerType::reference reference;
    typedef typename ContainerType::const_reference const_reference;
    typedef typename ContainerType::size_type size_type;
    mutable_container_base(ContainerType* container)
    : container_(container) {
      static_assert(!std::is_const<ContainerType>::value, "Underscore Library Error: Cannot mutate a const container)");
    }
    mutable_container_base(const mutable_container_base& other) 
    : container_(other.container_) {}
    mutable_container_base& operator=(const mutable_container_base& other) {
      container_ = other.container_;
      return *this;
    }
    operator ContainerType&() { return container_; }
    operator const ContainerType&() const { return container_; }
    size_t size() const { return container_->size(); }
    bool empty() const { return container_->empty(); }
    // Mutating functions
    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last) { container_->assign(first, last); }
    void assign(size_type n, const value_type& val) { container_->assign(n, val); }
    void clear() { container_->clear(); }
    void push_back(const value_type& value) { container_->push_back(value); }
    void push_back(value_type&& value) { container_->push_back(std::forward(value)); }
    void emplace_back(value_type&& value) { container_->emplace_back(std::forward(value)); }
    void push_front(value_type&& value) { container_->push_front(std::forward(value)); }
    void push_front(const value_type& value) { container_->push_front(value); }
    void emplace_front(value_type&& value) { container_->emplace_front(std::forward(value)); }
    void pop_back() { container_->pop_back(); }
    void pop_front() { container_->pop_front(); }
    iterator erase(const iterator& pos) { return container_->erase(pos); }
    iterator erase(const iterator& first, const iterator& last) { return container_->erase(first, last); }
    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) { container_->insert(position, first, last); }
    iterator insert(iterator position, const value_type& value) { return container_->insert(position, value); }
    void insert(iterator position, size_type n, const value_type& value) { container_->insert(position, n, value); }
    void resize(size_type n, const value_type& val = value_type()) { container_->resize(n, val); }
    void reserve(size_type n) { container_->reserve(n); }
    void shrink_to_fit() { container_->shrink_to_fit(); }
    void swap(ContainerType& other) { container_->swap(other); }
    void swap(my_type& other) { std::swap(container_, other->container_); }
    // Element access
    reference front() { return container_->front(); }
    const_reference front() const { return container_->front(); }
    reference back() { return container_->back(); }
    const_reference back() const { return container_->back(); }
    reference at(size_type idx) { return container_->at(idx); }
    const_reference at(size_type idx) const { return container_->at(idx); }
    reference operator[](size_type idx) { return (*container_)[idx]; }
    const_reference operator[](size_type idx) const { return (*container_)[idx]; }
    value_type* data() { return container_->data(); }
    const value_type* data() const { return container_->data(); }
    // Iterator access
    iterator begin() { return container_->begin(); }
    iterator end() { return container_->end(); }
    iterator begin() const { return container_->begin(); }
    iterator end() const { return container_->end(); }
    const_iterator cbegin() const { return container_->cbegin(); }
    const_iterator cend() const { return container_->cend(); }
    iterator rbegin() { return container_->rbegin(); }
    iterator rend() { return container_->rend(); }
    iterator rbegin() const { return container_->rbegin(); }
    iterator rend() const { return container_->rend(); }
    const_iterator crbegin() const { return container_->crbegin(); }
    const_iterator crend() const { return container_->crend(); }
  private:
    ContainerType* container_;
  };

  // find_first_not_of
  template <typename IteratorType0, typename IteratorType1>
  IteratorType0
  find_first_not_of(IteratorType0 first, const IteratorType0 last, const IteratorType1& s_first, const IteratorType1& s_last) {
    for(; first != last; ++first)
      if (std::find(s_first, s_last, *first) == s_last)
        return first;
    return last;
  }

  // find_last_not_of
  template <typename IteratorType0, typename IteratorType1>
  IteratorType0
  find_last_not_of(const IteratorType0& first, const IteratorType0& last, const IteratorType1& s_first, const IteratorType1& s_last) {
    if (first == last)
      return last;
    auto pos = last;
    do{
      --pos;
      if (std::find(s_first, s_last, *pos) == s_last)
        return pos;
    } while(last != first);
    return last;
  }


} // namespace UnerscoreDetail




#define IMPLEMENTS_1ARG_TAG(TAG_NAME) \
  template <typename ArgType0> \
  struct TAG_NAME##1Arg { \
    TAG_NAME##1Arg(const ArgType0& arg0) \
    : arg0(arg0) {} \
    const ArgType0& arg0; \
  };
#define IMPLEMENTS_2ARG_TAG(TAG_NAME) \
  template <typename ArgType0, typename ArgType1> \
  struct TAG_NAME##2Arg { \
    TAG_NAME##2Arg(const ArgType0& arg0, const ArgType1& arg1) \
    : arg0(arg0), arg1(arg1) {} \
    const ArgType0& arg0; \
    const ArgType1& arg1; \
  };
#define IMPLEMENTS_3ARG_TAG(TAG_NAME) \
  template <typename ArgType0, typename ArgType1, typename ArgType2> \
  struct TAG_NAME##3Arg { \
    TAG_NAME##3Arg(const ArgType0& arg0, const ArgType1& arg1, const ArgType2& arg2) \
    : arg0(arg0), arg1(arg1), arg2(arg2) {} \
    const ArgType0& arg0; \
    const ArgType1& arg1; \
    const ArgType2& arg2; \
  };
#define IMPLEMENTS_4ARG_TAG(TAG_NAME) \
  template <typename ArgType0, typename ArgType1, typename ArgType2, typename ArgType3> \
  struct TAG_NAME##4Arg { \
    TAG_NAME##4Arg(const ArgType0& arg0, const ArgType1& arg1, const ArgType2& arg2, const ArgType3& arg3) \
    : arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {} \
    const ArgType0& arg0; \
    const ArgType1& arg1; \
    const ArgType2& arg2; \
    const ArgType3& arg3; \
  };
#define IMPLEMENTS_2ARG_TAG_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  struct TAG_NAME##2Arg { \
    TAG_NAME##2Arg(const ArgType& arg0, const ArgType& arg1) \
    : arg0(arg0), arg1(arg1) {} \
    const ArgType& arg0; \
    const ArgType& arg1; \
  };
#define IMPLEMENTS_3ARG_TAG_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  struct TAG_NAME##3Arg { \
    TAG_NAME##3Arg(const ArgType& arg0, const ArgType& arg1, const ArgType& arg2) \
    : arg0(arg0), arg1(arg1), arg2(arg2) {} \
    const ArgType& arg0; \
    const ArgType& arg1; \
    const ArgType& arg2; \
  };
#define IMPLEMENTS_4ARG_TAG_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  struct TAG_NAME##4Arg { \
    TAG_NAME##4Arg(const ArgType& arg0, const ArgType& arg1, const ArgType& arg2, const ArgType& arg3) \
    : arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {} \
    const ArgType& arg0; \
    const ArgType& arg1; \
    const ArgType& arg2; \
    const ArgType& arg3; \
  };
#define IMPLEMENTS_1ARG_OPERATOR(TAG_NAME) \
  template <typename ArgType0> \
  TAG_NAME##1Arg<ArgType0> \
  operator() (const ArgType0& arg0) const { \
    return TAG_NAME##1Arg<ArgType0>(arg0); \
  }
#define IMPLEMENTS_2ARG_OPERATOR(TAG_NAME) \
  template <typename ArgType0, typename ArgType1> \
  TAG_NAME##2Arg<ArgType0, ArgType1> \
  operator() (const ArgType0& arg0, const ArgType1& arg1) const { \
    return TAG_NAME##2Arg<ArgType0, ArgType1>(arg0, arg1); \
  }
#define IMPLEMENTS_3ARG_OPERATOR(TAG_NAME) \
  template <typename ArgType0, typename ArgType1, typename ArgType2> \
  TAG_NAME##3Arg<ArgType0, ArgType1, ArgType2> \
  operator() (const ArgType0& arg0, const ArgType1& arg1, const ArgType2& arg2) const { \
    return TAG_NAME##3Arg<ArgType0, ArgType1, ArgType2>(arg0, arg1, arg2); \
  }
#define IMPLEMENTS_4ARG_OPERATOR(TAG_NAME) \
  template <typename ArgType0, typename ArgType1, typename ArgType2, typename ArgType3> \
  TAG_NAME##4Arg<ArgType0, ArgType1, ArgType2, ArgType3> \
  operator() (const ArgType0& arg0, const ArgType1& arg1, const ArgType2& arg2, const ArgType3& arg3) const { \
    return TAG_NAME##4Arg<ArgType0, ArgType1, ArgType2, ArgType3>(arg0, arg1, arg2, arg3); \
  }
#define IMPLEMENTS_1ARG_OPERATOR_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  TAG_NAME##1Arg<ArgType> \
  operator() (const ArgType& arg0) const { \
    return TAG_NAME##1Arg<ArgType>(arg0); \
  }
#define IMPLEMENTS_2ARG_OPERATOR_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  TAG_NAME##2Arg<ArgType> \
  operator() (const ArgType& arg0, const ArgType& arg1) const { \
    return TAG_NAME##2Arg<ArgType>(arg0, arg1); \
  }
#define IMPLEMENTS_3ARG_OPERATOR_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  TAG_NAME##3Arg<ArgType> \
  operator() (const ArgType& arg0, const ArgType& arg1, const ArgType& arg2) const { \
    return TAG_NAME##3Arg<ArgType>(arg0, arg1, arg2); \
  }
#define IMPLEMENTS_4ARG_OPERATOR_SAME_TYPE(TAG_NAME) \
  template <typename ArgType> \
  TAG_NAME##4Arg<ArgType> \
  operator() (const ArgType& arg0, const ArgType& arg1, const ArgType& arg2, const ArgType& arg3) const { \
    return TAG_NAME##4Arg<ArgType>(arg0, arg1, arg2, arg3); \
  }
#define CREATE_TAG_0_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  struct TAG_NAME { \
    TAG_NAME() {} \
    const TAG_NAME operator()() const { return TAG_NAME();} \
  };}
#define CREATE_TAG_1_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_1ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_1ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_0_1_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_1ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    const TAG_NAME operator()() const { return TAG_NAME();} \
    IMPLEMENTS_1ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_2_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_2ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_2ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_0_1_2_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_1ARG_TAG( TAG_NAME ) \
  IMPLEMENTS_2ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_1ARG_OPERATOR( TAG_NAME ) \
    IMPLEMENTS_2ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_3_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_3ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_3ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_4_ARG( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_4ARG_TAG( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_4ARG_OPERATOR( TAG_NAME ) \
  };}
#define CREATE_TAG_0_1_2_ARG_SAME_TYPE( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_1ARG_TAG( TAG_NAME ) \
  IMPLEMENTS_2ARG_TAG_SAME_TYPE( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_1ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
    IMPLEMENTS_2ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
  };}
#define CREATE_TAG_2_ARG_SAME_TYPE( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_2ARG_TAG_SAME_TYPE( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_2ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
  };}
#define CREATE_TAG_3_ARG_SAME_TYPE( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_3ARG_TAG_SAME_TYPE( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_3ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
  }}
#define CREATE_TAG_4_ARG_SAME_TYPE( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_4ARG_TAG_SAME_TYPE( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_4ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
  };}
#define CREATE_TAG_1_2_3_4_ARG_SAME_TYPE( TAG_NAME ) \
  namespace UnderscoreTags { \
  IMPLEMENTS_1ARG_TAG( TAG_NAME ) \
  IMPLEMENTS_2ARG_TAG_SAME_TYPE( TAG_NAME ) \
  IMPLEMENTS_3ARG_TAG_SAME_TYPE( TAG_NAME ) \
  IMPLEMENTS_4ARG_TAG_SAME_TYPE( TAG_NAME ) \
  struct TAG_NAME { \
    TAG_NAME() {}\
    IMPLEMENTS_1ARG_OPERATOR( TAG_NAME ) \
    IMPLEMENTS_2ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
    IMPLEMENTS_3ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
    IMPLEMENTS_4ARG_OPERATOR_SAME_TYPE( TAG_NAME ) \
  };}
#define CREATE_TAG_TEMPLATE( TAG_NAME ) \
  namespace UnderscoreTags { \
  template <typename ValueType> \
  struct TAG_NAME { \
    typedef ValueType value_type; \
  };}


#define CREATE_PIPE_0ARG(TAG_NAME, FUNCTION) \
  CREATE_TAG_0_ARG(TAG_NAME ) \
  template <typename ValueType> \
  ValueType \
  PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::TAG_NAME& tag) { \
    return FUNCTION(value); \
  }
#define CREATE_PIPE_1ARG(TAG_NAME, FUNCTION) \
  CREATE_TAG_1_ARG(TAG_NAME) \
  template <typename ValueType, typename ArgType0> \
  ValueType \
  PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::TAG_NAME##1Arg<ArgType0>& tag) { \
    return FUNCTION(value, tag.arg0); \
  }





// accumulate - stl
CREATE_TAG_0_1_2_ARG( AccumulateTag );
template <typename ContainerType>
typename ContainerType::value_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AccumulateTag&) {
  return std::accumulate(std::begin(container), std::end(container), typename ContainerType::value_type());
}
template <typename ContainerType, typename ArgType0>
typename ContainerType::value_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AccumulateTag1Arg<ArgType0>& tag) {
  return std::accumulate(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0, typename ArgType1>
typename ContainerType::value_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AccumulateTag2Arg<ArgType0, ArgType1>& tag) {
  return std::accumulate(std::begin(container), std::end(container), tag.arg0, tag.arg1);
}

// adressof - stl
CREATE_TAG_0_ARG( AdressOfTag );
template <typename ValueType>
const ValueType*
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::AdressOfTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ValueType>::value == false, "Underscore Library Error: Cannot return adress of an rvalue.");
  return std::addressof(value);
}
template <typename ValueType>
ValueType*
PIPE_OPERATOR(ValueType& value, const UnderscoreTags::AdressOfTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ValueType>::value == false, "Underscore Library Error: Cannot return adress of an rvalue.");
  return std::addressof(value);
}

// all_of - stl
CREATE_TAG_1_ARG( AllOfTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AllOfTag1Arg<ArgType0>& tag) {
  return std::all_of(std::begin(container), std::end(container), tag.arg0);
}

// all_of_equal - stl
CREATE_TAG_1_ARG( AllOfEqualTag );
template <typename ContainerType, typename ArgType0>
bool 
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AllOfEqualTag1Arg<ArgType0>& tag) {
  for(auto it = UNDERSCORE_CBEGIN(container), it_end = UNDERSCORE_CEND(container); it != it_end; ++it) 
    if (*it != tag.arg0)
      return false;
  return true;
}

// any_of - stl
CREATE_TAG_1_ARG( AnyOfTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AnyOfTag1Arg<ArgType0>& tag) {
  return std::any_of(std::begin(container), std::end(container), tag.arg0);
}

// any_of_equal - stl
CREATE_TAG_1_ARG( AnyOfEqualTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AnyOfEqualTag1Arg<ArgType0>& tag) {
  const auto& last = std::end(container);
  return std::find(std::begin(container), last, tag.arg0) != last;
}

// back_inserter - stl
CREATE_TAG_0_ARG( BackInserterTag );
template <typename ContainerType>
typename std::back_insert_iterator<ContainerType>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::BackInserterTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  UNDERSCORE_STATIC_ASSERT(std::is_const<ContainerType>::value == false, "Underscore Library Error: Cannot create a std::back_inserter for a const container.");
  return std::back_inserter(container);
}

// begin - stl
CREATE_TAG_0_ARG( BeginTag );
template <typename ContainerType> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::BeginTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::begin(container);
}
template <typename ContainerType> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::BeginTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::begin(container);
}

// binary_search - stl
CREATE_TAG_1_ARG( BinarySearchTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::BinarySearchTag1Arg<ArgType0>& tag) {
  return std::binary_search(std::begin(container), std::end(container), tag.arg0);
}

// cbegin - stl
CREATE_TAG_0_ARG( CBeginTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CBeginTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::begin(container);
}

// cend - stl
CREATE_TAG_0_ARG( CEndTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CEndTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::end(container);
}

// rbegin - stl
CREATE_TAG_0_ARG( RbeginTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::RbeginTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UNDERSCORE_RBEGIN(container);
}

// rend - stl
CREATE_TAG_0_ARG( RendTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::RendTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UNDERSCORE_REND(container);
}

// crbegin - stl
CREATE_TAG_0_ARG( CrbeginTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CrbeginTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UNDERSCORE_CRBEGIN(container);
}

// crend - stl
CREATE_TAG_0_ARG( CrendTag );
template <typename ContainerType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CrendTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UNDERSCORE_CREND(container);
}

// count - stl
CREATE_TAG_1_ARG( CountTag );
template <typename ContainerType, typename ArgType0>
size_t
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CountTag1Arg<ArgType0>& tag) {
  return std::count(std::begin(container), std::end(container), tag.arg0);
}

// count_if - stl
CREATE_TAG_1_ARG( CountIfTag );
template <typename ContainerType, typename ArgType0>
size_t
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::CountIfTag1Arg<ArgType0>& tag) {
  return std::count_if(std::begin(container), std::end(container), tag.arg0);
}

// end - stl
CREATE_TAG_0_ARG( EndTag );
template <typename ContainerType> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::EndTag& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::end(container);
}
template <typename ContainerType> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::EndTag& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::end(container);
}

// equal
CREATE_TAG_1_ARG( EqualTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::EqualTag1Arg<ArgType0>& tag) {
  const auto& left_size = std::distance(std::begin(container), std::end(container));
  const auto& right_size = std::distance(std::begin(tag.arg0), std::end(tag.arg0));
  if (left_size != right_size)
    return false;
  return std::equal(std::begin(container), std::end(container), std::begin(tag.arg0));
}

// find - stl
CREATE_TAG_1_ARG( FindTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find(std::begin(container), std::end(container), tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0> // immutable set
typename std::set<ValueType, CompareType, AllocatorType>::const_iterator
PIPE_OPERATOR(const std::set<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0>  // mutable set
typename std::set<ValueType, CompareType, AllocatorType>::iterator
PIPE_OPERATOR(std::set<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0> // immutable multiset
typename std::multiset<ValueType, CompareType, AllocatorType>::const_iterator
PIPE_OPERATOR(const std::multiset<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0>  // mutable multiset
typename std::multiset<ValueType, CompareType, AllocatorType>::iterator
PIPE_OPERATOR(std::multiset<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0> // immutable unordered_set
typename std::unordered_set<ValueType, CompareType, AllocatorType>::const_iterator
PIPE_OPERATOR(const std::unordered_set<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}
template <typename ValueType, typename CompareType, typename AllocatorType, typename ArgType0>  // mutable unordered_set
typename std::unordered_set<ValueType, CompareType, AllocatorType>::iterator
PIPE_OPERATOR(std::unordered_set<ValueType>& container, const UnderscoreTags::FindTag1Arg<ArgType0>& tag) {
  return container.find(tag.arg0);
}

// find_if - stl
CREATE_TAG_1_ARG( FindIfTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindIfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_if(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindIfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_if(std::begin(container), std::end(container), tag.arg0);
}

// find_if_not - stl
CREATE_TAG_0_1_ARG( FindIfNotTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindIfNotTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_if_not(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindIfNotTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_if_not(std::begin(container), std::end(container), tag.arg0);
}

// find_index - underscore
CREATE_TAG_1_ARG( FindIndexTag );
template <typename ContainerType, typename ArgType0>
typename ContainerType::size_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindIndexTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find(std::begin(container), std::end(container), tag.arg0);
  return std::distance(std::begin(container), pos);
}

// find_index_if - underscore
CREATE_TAG_1_ARG( FindIndexIfTag );
template <typename ContainerType, typename ArgType0>
typename ContainerType::size_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindIndexIfTag1Arg<ArgType0>& tag) {
  return std::distance(std::begin(container), std::find_if(std::begin(container), std::end(container), tag.arg0));
}

// find_index_if_not - underscore
CREATE_TAG_1_ARG( FindIndexIfNotTag );
template <typename ContainerType, typename ArgType0>
typename ContainerType::size_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindIndexIfNotTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find_if_not(std::begin(container), std::end(container), tag.arg0);
  return std::distance(std::begin(container), pos);
}

// for_each - stl
CREATE_TAG_1_ARG( ForEachTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::ForEachTag1Arg<ArgType0>& tag) {
  std::for_each(std::begin(container), std::end(container), tag.arg0);
  return container;
}

// front_inserter - stl
CREATE_TAG_0_ARG( FrontInserterTag );
template <typename ContainerType>
std::front_insert_iterator<ContainerType>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FrontInserterTag& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  UNDERSCORE_STATIC_ASSERT(std::is_const<ContainerType>::value == false, "Underscore Library Error: Cannot create a std::front_inserter for a const container.");
  return std::front_inserter(container);
}

// get - stl
namespace UnderscoreTags { template <size_t TupleIndex> struct GetTag {}; }
template <typename TupleType, size_t TupleIndex> // immutable
const typename std::tuple_element<TupleIndex, TupleType>::type&
PIPE_OPERATOR(const TupleType& tpl, const UnderscoreTags::GetTag<TupleIndex>&) {
  return std::get<TupleIndex>(tpl);
}
template <typename TupleType, size_t TupleIndex> // mutable
typename std::tuple_element<TupleIndex, TupleType>::type&
PIPE_OPERATOR(TupleType& tpl, const UnderscoreTags::GetTag<TupleIndex>&) {
  return std::get<TupleIndex>(tpl);
}
template <typename TupleType, size_t TupleIndex> // rvalue
typename std::tuple_element<TupleIndex, TupleType>::type
PIPE_OPERATOR(TupleType&& tpl, const UnderscoreTags::GetTag<TupleIndex>&) {
  return std::get<TupleIndex>(tpl);
}

// hash - stl
CREATE_TAG_0_ARG( HashTag );
template <typename ValueType>
size_t
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::HashTag&) {
  const std::hash<ValueType> hasher;
  return hasher(value);
}

// includes
CREATE_TAG_1_ARG( IncludesTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::IncludesTag1Arg<ArgType0>& tag) {
  return std::includes(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// inserter - stl
CREATE_TAG_1_ARG( InserterTag );
template <typename ContainerType, typename ArgType0>
std::insert_iterator<ContainerType>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::InserterTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  UNDERSCORE_STATIC_ASSERT(std::is_const<ContainerType>::value == false, "Underscore Library Error: Cannot create a std::insert_iterator for a const container.");
  return std::inserter(container, tag.arg0);
}

// is_sorted - stl
CREATE_TAG_0_1_ARG( IsSortedTag );
template <typename ContainerType>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::IsSortedTag&) {
  return std::is_sorted(std::begin(container), std::end(container));
}
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::IsSortedTag1Arg<ArgType0>& tag) {
  return std::is_sorted(std::begin(container), std::end(container), tag.arg0);
}

// lexicographical_compare
CREATE_TAG_1_ARG( LexicographicalCompareTag );
template <typename ContainerType, typename ArgType0> // mutable
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::LexicographicalCompareTag1Arg<ArgType0>& tag) {
  return std::lexicographical_compare(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// lower_bound
CREATE_TAG_1_ARG( LowerBoundTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::LowerBoundTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::lower_bound(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::LowerBoundTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::lower_bound(std::begin(container), std::end(container), tag.arg0);
}

// max_element
CREATE_TAG_1_ARG(MaxElementTag);
template <typename ContainerType> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MaxElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::max_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MaxElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::max_element(std::begin(container), std::end(container));
}

// min_element - stl
CREATE_TAG_0_ARG( MinElementTag );
template <typename ContainerType> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MinElementTag&) {
  return std::min_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MinElementTag&) {
  return std::min_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // prevent r-value
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MinElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<decltype(container)>::value == false, "Underscore Library Error: Cannot return iterator to element in r-value container.");
  return std::min_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable_container
typename ContainerType::iterator
PIPE_OPERATOR(UnderscoreDetail::mutable_container_base<ContainerType>&& container, const UnderscoreTags::MinElementTag&) {
  return std::min_element(std::begin(container), std::end(container));
}

// minmax_element - stl
CREATE_TAG_0_ARG( MinMaxElementTag );
template <typename ContainerType> // immutable
typename std::pair<typename ContainerType::const_iterator, typename ContainerType::const_iterator>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MinMaxElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::minmax_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable
typename std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MinMaxElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::minmax_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // prevent rvalue
typename std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MinMaxElementTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<decltype(container)>::value == false, "Underscore Library Error: Cannot return iterator to element in r-value container.");
  return std::minmax_element(std::begin(container), std::end(container));
}

// move - stl
CREATE_TAG_0_ARG( MoveTag );
template <typename ValueType>
ValueType
PIPE_OPERATOR(ValueType& value, const UnderscoreTags::MoveTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_const<ValueType>::value == false, "Underscore Library Error: Cannot make an r-value of a constant value.");
  return std::move(value);
}

// next - stl
CREATE_TAG_0_1_ARG( NextTag );
template <typename IteratorType>
IteratorType
PIPE_OPERATOR(const IteratorType& iterator, const UnderscoreTags::NextTag&) {
  return std::next(iterator);
}
template <typename IteratorType, typename DifferenceType>
IteratorType
PIPE_OPERATOR(const IteratorType& iterator, const UnderscoreTags::NextTag1Arg<DifferenceType>& tag) {
  return std::next(iterator, tag.arg0);
}

// none_of - stl
CREATE_TAG_1_ARG( NoneOfTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::NoneOfTag1Arg<ArgType0>& tag) {
  return std::none_of(std::begin(container), std::end(container), tag.arg0);
}

// none_of_equal - stl
CREATE_TAG_1_ARG( NoneOfEqualTag );
template <typename ContainerType, typename ArgType0>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::NoneOfEqualTag1Arg<ArgType0>& tag) {
  return std::find(std::begin(container), std::end(container), tag.arg0) == std::end(container);
}

// prev - stl
CREATE_TAG_0_1_ARG( PrevTag );
template <typename IteratorType> // immutable
IteratorType
PIPE_OPERATOR(const IteratorType& iterator, const UnderscoreTags::PrevTag&) {
  return std::prev(iterator);
}
template <typename IteratorType, typename DifferenceType> // mutable
IteratorType
PIPE_OPERATOR(const IteratorType& iterator, const UnderscoreTags::PrevTag1Arg<DifferenceType>& tag) {
  return std::prev(iterator, tag.arg0);
}

// upper_bound - stl
CREATE_TAG_1_ARG( UpperBoundTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::UpperBoundTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::upper_bound(std::begin(container), std::end(container), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::UpperBoundTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::upper_bound(std::begin(container), std::end(container), tag.arg0);
}

// Todo..
// set_intersection
// set_union
// set_...
// mismatch
// tranformed

// transform
namespace UnderscoreTags {
  template <typename FunctorType>
  struct TransformTag1Arg {
    TransformTag1Arg(const FunctorType& arg0) : arg0(arg0) {}
    const FunctorType& arg0;
  };
  template <typename FunctorType, typename PreallocatedContainerType>
  struct TransformTag2Arg {
    TransformTag2Arg(FunctorType& arg0, PreallocatedContainerType& arg1) : arg0(arg0), arg1(arg1) {}
    FunctorType& arg0;
    PreallocatedContainerType& arg1;
  };
  struct TransformTag {
    template <typename ArgType0>
    TransformTag1Arg<ArgType0> operator()(const ArgType0& functor) const {
      return TransformTag1Arg<ArgType0>(functor);
    }
    template <typename ArgType0, typename ArgType1>
    TransformTag2Arg<ArgType0, ArgType1> operator()(ArgType0& arg0, ArgType1& arg1) const {
      return TransformTag2Arg<ArgType0, ArgType1>(arg0, arg1);
    }
  };
}
template <template <typename, typename> class ContainerType, typename ValueType, typename AllocType, typename FunctorType>
ContainerType <
  typename std::result_of<FunctorType(ValueType)>::type, 
  std::allocator<typename std::result_of<FunctorType(ValueType)>::type> >
PIPE_OPERATOR(const ContainerType<ValueType, AllocType>& container, const UnderscoreTags::TransformTag1Arg<FunctorType>& tag) {
  typedef typename std::result_of<FunctorType(ValueType)>::type ResultType;
  typedef ContainerType<ResultType, std::allocator<ResultType> > ResultContainerType;
  ResultContainerType result_container(container.size());
  std::transform(std::begin(container), std::end(container), std::begin(result_container), tag.arg0);
  return result_container;
}

// transform_to
namespace UnderscoreTags {
  template <typename FunctorType, typename PreallocatedContainerType>
  struct TransformToTag2Arg {
    TransformToTag2Arg(const FunctorType& arg0, PreallocatedContainerType& arg1) : arg0(arg0), arg1(arg1) {}
    const FunctorType& arg0;
    PreallocatedContainerType& arg1;
  };
  struct TransformToTag {
    
    template <typename FunctorType, typename PreallocatedContainerType>
    TransformToTag2Arg<FunctorType, PreallocatedContainerType> 
    operator()(const FunctorType& functor, PreallocatedContainerType& preallocated_container) {
      return TransformToTag2Arg<FunctorType, PreallocatedContainerType>(functor, preallocated_container);
    }
    
    /*
    template <typename FunctorType, typename PreallocatedContainerType>
    TransformToTag2Arg<FunctorType, PreallocatedContainerType> 
    operator()(const FunctorType& functor, PreallocatedContainerType&& preallocated_container) {
      return TransformToTag2Arg<FunctorType, PreallocatedContainerType>(functor, preallocated_container);
    }
    */
    
  };
}
template<typename ContainerType, typename FunctorType, typename PreallocatedContainerType>
typename std::decay<PreallocatedContainerType>::type
PIPE_OPERATOR(const ContainerType& container, UnderscoreTags::TransformToTag2Arg<FunctorType, PreallocatedContainerType>& tag) {
  tag.arg1.resize(container.size());
  typedef typename std::decay<decltype(tag.arg1)>::type ResultContainerType;
  ResultContainerType transform_container(std::move(tag.arg1));
  transform_container.resize(container.size());
  std::transform(std::begin(container), std::end(container), std::begin(transform_container), tag.arg0);
  return transform_container;
}






/*
template <template <typename, typename> class ContainerType, typename ValueType, typename AllocType, typename FunctorType>
ContainerType <
  typename std::result_of<FunctorType(ValueType)>::type, 
  std::allocator<typename std::result_of<FunctorType(ValueType)>::type> >
PIPE_OPERATOR(const ContainerType<ValueType, AllocType>& container, const UnderscoreTags::TransformTag2Arg<FunctorType>& tag) {
  typedef typename std::result_of<FunctorType(ValueType)>::type ResultType;
  typedef ContainerType<ResultType, std::allocator<ResultType> > ResultContainerType;
  ResultContainerType result_container(container.size());
  std::transform(std::begin(container), std::end(container), std::begin(result_container), tag.arg0);
  return result_container;
}
*/




/*
template <typename ContainerType, typename ArgType0, typename ArgType1>
ArgType1
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::TransformTag2Arg<ArgType0, ArgType1>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_const<ArgType1>::value == false, "Underscore Library Error: Cannot transform to a const container.");
  typedef typename std::result_of<ArgType0(typename ContainerType::value_type)>::type ResultType;
  auto result_container = std::move(tag.arg1);
  result_container.resize(container.size());
  std::transform(std::begin(container), std::end(container), std::begin(result_container), tag.arg0);
  return result_container;
}


/*
template <typename OutContainerType, typename ContainerType, typename ArgType0>
typename OutContainerType
PIPE_OPERATOR(const std::tuple<ContainerType>& container, const UnderscoreTags::TransformTag1Arg<ArgType0>& tag) { \
  typedef typename std::result_of<ArgType0(typename ContainerType::value_type)>::type ResultType;
  OutContainerType result;
  std::transform(std::begin(container), std::end(container), result.begin(), tag.arg0);
  return result;
}
template <typename OutContainerType, typename ContainerTypeA, typename ContainerTypeB, typename ArgType0>
typename OutContainerType
PIPE_OPERATOR(const std::tuple<ContainerTypeA, ContainerTypeB>& tupledContainers, const UnderscoreTags::TransformTag1Arg<ArgType0>& tag) { \
  typedef typename std::result_of<ArgType0(typename ContainerTypeA::value_type, typename ContainerTypeB::value_type)>::type ResultType;
  const auto& a = std::get<0>(tupledContainers);
  const auto& b = std::get<1>(tupledContainers);
  OutContainerType result;
  std::transform(std::begin(a), std::end(a), std::begin(b), result.begin(), tag.arg0);
  return result;
}

template <typename ContainerType, typename ArgType0, typename ArgType1>
typename ArgType1
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::TransformTag2Arg<ArgType0, ArgType1>& tag) { \
  typedef typename std::result_of<ArgType0(typename ContainerType::value_type)>::type ResultType;
  tag.arg1.resize(container.size());
  std::transform(std::begin(container), std::end(container), std::begin(tag.arg1), tag.arg0);
  return tag.arg1;
}
*/

// To containers....
#define CREATE_MUTABLE_TAG_1_ARG( TAG_NAME, ARG_TYPE_NAME_0, ARG_NAME_0) \
  namespace UnderscoreTags { \
    template <typename ARG_TYPE_NAME_0> \
    struct TAG_NAME##1Arg { \
      TAG_NAME##1Arg(ARG_TYPE_NAME_0& arg0) : arg0(arg0) {} \
      ARG_TYPE_NAME_0& arg0; \
    }; \
    struct TAG_NAME { \
      TAG_NAME operator()() const { return TAG_NAME(); } \
      template <typename ARG_TYPE_NAME_0> \
      TAG_NAME##1Arg<ARG_TYPE_NAME_0> operator()(ARG_TYPE_NAME_0& ARG_NAME_0) const { \
        return TAG_NAME##1Arg<ARG_TYPE_NAME_0>(ARG_NAME_0); \
  }};}

#define CREATE_TO_SPECIFIC_CONTAINER_PIPE(TAG_NAME, CONTAINER_NAME) \
  template <typename ContainerType> \
  CONTAINER_NAME<typename ContainerType::value_type> \
  PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::TAG_NAME&) { \
    return CONTAINER_NAME<typename ContainerType::value_type>(std::begin(container), std::end(container)); \
  } \
  template <typename ContainerType, typename PreallocatedContainer>  \
  CONTAINER_NAME<typename ContainerType::value_type>  \
  PIPE_OPERATOR(const ContainerType& container, UnderscoreTags::TAG_NAME##1Arg<PreallocatedContainer>& tag) { \
    tag.arg0.assign(std::begin(container), std::end(container)); \
    return tag.arg0; \
  } /*
  template <typename ContainerType, typename PreallocatedContainer>  \
  CONTAINER_NAME<typename ContainerType::value_type>  \
  PIPE_OPERATOR(ContainerType&& container, UnderscoreTags::TAG_NAME##1Arg<PreallocatedContainer>& tag) { \
    tag.arg0.assign( \
      std::make_move_iterator(std::begin(container)),  \
      std::make_move_iterator(std::end(container))); \
    return tag.arg0; \
  } \
  template <typename ContainerType> \
  CONTAINER_NAME<typename ContainerType::value_type> \
  PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::TAG_NAME&) { \
    return CONTAINER_NAME<typename ContainerType::value_type>( \
      std::make_move_iterator(std::begin(container)),  \
      std::make_move_iterator(std::end(container))); \
  } \
  */

#define CREATE_TO_SPECIFIC_SET_PIPE(TAG_NAME, CONTAINER_NAME) \
  CREATE_TAG_0_ARG( TAG_NAME ); \
  template <typename ContainerType> \
  CONTAINER_NAME<typename ContainerType::value_type> \
  PIPE_OPERATOR(const ContainerType& value, const UnderscoreTags::TAG_NAME&) { \
    return CONTAINER_NAME<typename ContainerType::value_type>(std::begin(value), std::end(value)); \
  }

/*
#define CREATE_TO_SPECIFIC_MAP_PIPE(TAG_NAME, CONTAINER_NAME) \
  CREATE_TAG_0_ARG( TAG_NAME ); \
  template <typename ContainerType> \
  CONTAINER_NAME<typename ContainerType::value_type> \
  PIPE_OPERATOR(const ContainerType& value, const UnderscoreTags::TAG_NAME&) { \
    for
    CONTAINER_NAME<
    return CONTAINER_NAME<typename ContainerType::value_type>(std::begin(value), std::end(value)); \
  }
*/
// to_vector
CREATE_MUTABLE_TAG_1_ARG(ToVectorTag, PreallocatedVectorType, preallocated_vector);
CREATE_TO_SPECIFIC_CONTAINER_PIPE( ToVectorTag, std::vector );

// to_deque
CREATE_MUTABLE_TAG_1_ARG(ToDequeTag, PreallocatedDequeType, preallocated_deque);
CREATE_TO_SPECIFIC_CONTAINER_PIPE( ToDequeTag, std::deque );

// to_list
CREATE_MUTABLE_TAG_1_ARG(ToListTag, PreallocatedListType, preallocated_list);
CREATE_TO_SPECIFIC_CONTAINER_PIPE( ToListTag, std::list );

// to_set, to_multiset, to_unordered_set
CREATE_TO_SPECIFIC_SET_PIPE( ToSetTag, std::set );
CREATE_TO_SPECIFIC_SET_PIPE( ToMultiSetTag, std::multiset );
CREATE_TO_SPECIFIC_SET_PIPE( ToUnorderedSetTag, std::unordered_set );
//CREATE_TO_SPECIFIC_CONTAINER_TAG( ToMapTag, std::map ); // todo
//CREATE_TO_SPECIFIC_CONTAINER_TAG( ToMultiMapTag, std::multi_map ); // todo

// CMath
CREATE_PIPE_0ARG(SinTag, ::sin);
CREATE_PIPE_0ARG(CosTag, ::cos);
CREATE_PIPE_0ARG(TanTag, ::tan);
CREATE_PIPE_0ARG(AsinTag, ::asin);
CREATE_PIPE_0ARG(AcosTag, ::acos);
CREATE_PIPE_0ARG(AtanTag, ::atan);
CREATE_PIPE_0ARG(SinhTag, ::sinh);
CREATE_PIPE_0ARG(CoshTag, ::cosh);
CREATE_PIPE_0ARG(TanhTag, ::tanh);
// CREATE_PIPE_0ARG(AsinhTag, ::asinh); todo
// CREATE_PIPE_0ARG(AcoshTag, ::acosh); todo
// CREATE_PIPE_0ARG(AtanhTag, ::atanh); todo
CREATE_PIPE_0ARG(AbsTag, ::abs);
CREATE_PIPE_0ARG(FabsTag, ::fabs);
CREATE_PIPE_0ARG(ExpTag, ::exp);
CREATE_PIPE_0ARG(Exp2Tag, ::exp2);
CREATE_PIPE_0ARG(LogTag, ::log);
CREATE_PIPE_0ARG(Log10Tag, ::log10);
CREATE_PIPE_0ARG(FloorTag, ::floor);
CREATE_PIPE_0ARG(CeilTag, ::ceil);
CREATE_PIPE_0ARG(TruncTag, ::trunc);
CREATE_PIPE_0ARG(ModfTag, ::modf);
CREATE_PIPE_0ARG(PowTag, ::pow);

// round
CREATE_TAG_0_ARG( RoundTag );
template <typename ValueType>
ValueType
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::RoundTag&) {
  if( std::is_integral<ValueType>::value )
    return value;
  return ::floor(value + static_cast<ValueType>(0.5));
}

// erase_all
CREATE_TAG_1_ARG( EraseAllTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseAllTag1Arg<ArgType0>& tag) {
  const auto& end = std::end(container);
  const auto& new_end = std::remove(std::begin(container), end, tag.arg0);
  container.erase(new_end, end);
  return container;
}

// erase_all_unstable
namespace UnderscoreDetail {
  // remove_unstable
  template <typename IteratorType, typename ValueType>
  IteratorType
  remove_unstable(IteratorType first, IteratorType last, const ValueType& val) {
    while(first != last) {
      if (*first == val) {
        --last;
        *first = std::move(*last);
      } else {
        ++first;
      }
    }
    return last;
  }
}
CREATE_TAG_1_ARG( EraseAllUnstableTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseAllUnstableTag1Arg<ArgType0>& tag) {
  const auto& end = std::end(container);
  const auto& new_end = UnderscoreDetail::remove_unstable(std::begin(container), end, tag.arg0);
  container.erase(new_end, end);
  return container;
}

// erase_all_if
CREATE_TAG_1_ARG( EraseAllIfTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseAllIfTag1Arg<ArgType0>& tag) {
  const auto& end = std::end(container);
  const auto& new_end = std::remove_if(std::begin(container), end, tag.arg0);
  container.erase(new_end, end);
  return container;
}

// erase_all_if_unstable
namespace UnderscoreDetail {
  template <typename IteratorType, typename PredicateType>
  IteratorType
  remove_if_unstable(IteratorType first, IteratorType last, PredicateType& predicate) {
    while(first != last) {
      if (predicate(*first)) {
        --last;
        *first = std::move(*last);
      } else {
        ++first;
      }
    }
    return last;
  }
}
CREATE_TAG_1_ARG( EraseAllIfUnstableTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseAllIfUnstableTag1Arg<ArgType0>& tag) {
  const auto& end = std::end(container);
  const auto& new_end = UnderscoreDetail::remove_if_unstable(std::begin(container), end, tag.arg0);
  container.erase(new_end, end);
  return container;
}

// erase_consecutive_duplicates
CREATE_TAG_1_ARG( EraseConsecutiveDuplicatesTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseConsecutiveDuplicatesTag&) {
  const auto& end = std::end(container);
  const auto& new_end = std::unique(std::begin(container), std::end(container));
  container.erase(new_end, end);
  return container;
}

// erase_duplicates_stable
CREATE_TAG_0_ARG( EraseDuplicatesStableTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseDuplicatesStableTag&) {
  auto new_end = std::end(container);
  for (auto i = std::begin(container); i != new_end; ++i) {
    for (auto j = std::next(i); j != new_end; ) {
      if ((*j) != (*i)) {
        ++j;
        continue;
      }
      std::rotate(j, std::next(j), new_end);
      --new_end;
    }
  }
  container.erase(new_end, container.end());
  return container;
}

// erase_duplicates_unstable
CREATE_TAG_0_ARG( EraseDuplicatesUnstableTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseDuplicatesUnstableTag&) {
  const auto& begin = std::begin(container);
  const auto& end = std::end(container);
  std::sort(begin, end);
  const auto& new_end = std::unique(begin, end);
  container.erase(new_end, end);
  return container;
}

// erase_first
CREATE_TAG_1_ARG( EraseFirstTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseFirstTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find(std::begin(container), std::end(container),  tag.arg0);
  container.erase(pos);
  return container;
}

// erase_first_if
CREATE_TAG_1_ARG( EraseFirstIfTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseFirstIfTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find_if(std::begin(container), std::end(container), tag.arg0);
  container.erase(pos);
  return container;
}

// erase_last
CREATE_TAG_1_ARG( EraseLastTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseLastTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find(UNDERSCORE_RBEGIN(container), UNDERSCORE_REND(container), tag.arg0);
  container.erase(pos);
  return container;
}

// erase_last_if
CREATE_TAG_1_ARG( EraseLastIfTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::EraseLastIfTag1Arg<ArgType0>& tag) {
  const auto& pos = std::find_if(UNDERSCORE_RBEGIN(container), UNDERSCORE_REND(container), tag.arg0);
  container.erase(pos);
  return container;
}

// trim
namespace UnderscoreDetail {
  const static std::array<char, 4> default_trim_characters = {' ', '\t', '\n', '\r'};
  // trim_left
  template <typename ContainerType, typename IteratorType>
  void
  trim_left(ContainerType& container, const IteratorType& first, const IteratorType& last) {
    const auto& new_begin = UnderscoreDetail::find_first_not_of(std::begin(container), std::end(container), first, last);
    container.erase(std::begin(container), new_begin);
  }
  // trim_right
  template <typename ContainerType, typename IteratorType>
  void
  trim_right(ContainerType& container, const IteratorType& first, const IteratorType& last) {
    const auto& new_end = UnderscoreDetail::find_last_not_of(std::begin(container), std::end(container), first, last);
    container.erase(std::next(new_end), std::end(container));
  }
} // UnderscoreDetail
CREATE_TAG_0_ARG( TrimTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TrimTag&) {
  UnderscoreDetail::trim_right(container, UnderscoreDetail::default_trim_characters.begin(), UnderscoreDetail::default_trim_characters.end());
  UnderscoreDetail::trim_left(container, UnderscoreDetail::default_trim_characters.begin(), UnderscoreDetail::default_trim_characters.end());
  return container;
}

// trim_left
CREATE_TAG_0_ARG( TrimLeftTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TrimLeftTag&) {
  UnderscoreDetail::trim_left(container, UnderscoreDetail::default_trim_characters.begin(), UnderscoreDetail::default_trim_characters.end());
  return container;
}

// trim_right
CREATE_TAG_0_ARG( TrimRightTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TrimRightTag&) {
  UnderscoreDetail::trim_right(container, UnderscoreDetail::default_trim_characters.begin(), UnderscoreDetail::default_trim_characters.end());
  return container;
}

// inserted_back
CREATE_TAG_1_ARG( InsertedBackTag );
template <typename ContainerType, typename ArgType0>
ContainerType
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::InsertedBackTag1Arg<ArgType0>& tag) {
  container.insert(std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
  return container;
}

// inserted_front
CREATE_TAG_1_ARG( InsertedFrontTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::InsertedFrontTag1Arg<ArgType0>& tag) {
  container.insert(std::begin(container), std::begin(tag.arg0), std::end(tag.arg0));
  return container;
}

// pipe
namespace UnderscoreTags {
  template <typename FunctorType>
  struct PipeTag1Arg {
    PipeTag1Arg(FunctorType functor) : functor(functor) {}
    FunctorType functor;
  };
  template <typename FunctorType, typename ArgType0>
  struct PipeTag2Arg {
    PipeTag2Arg(FunctorType functor, const ArgType0& arg0) : functor(functor), arg0(arg0) {}
    FunctorType functor;
    const ArgType0& arg0;
  };
  struct PipeTag{
    template <typename FunctorType>
    PipeTag1Arg<FunctorType>
    operator()(FunctorType func) const {
      return PipeTag1Arg<FunctorType>(func);
    }
    template <typename FunctorType, typename ArgType0>
    PipeTag2Arg<FunctorType, ArgType0>
    operator()(FunctorType func, const ArgType0& arg0) const {
      return PipeTag2Arg<FunctorType, ArgType0>(func, arg0);
    }
  };
}
template <typename Left, typename FunctorType>
typename std::result_of<FunctorType(Left)>::type
PIPE_OPERATOR(const Left& left, UnderscoreTags::PipeTag1Arg<FunctorType> tag) {
  return tag.functor(left);
}
template <typename Left, typename FunctorType, typename ArgType0>
typename std::result_of<FunctorType(Left, ArgType0)>::type
PIPE_OPERATOR(const Left& left, const UnderscoreTags::PipeTag2Arg<FunctorType, ArgType0>& tag) {
  return tag.functor(left, tag.arg0);
}

// pop_back
CREATE_TAG_0_1_ARG( PopBackTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PopBackTag&) {
  container.pop_back();
  return container;
}
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PopBackTag1Arg<ArgType0>& tag) {
  for(size_t i = 0; i < tag.arg0; ++i)
    container.pop_back();
  return container;
}

// pop_front
CREATE_TAG_0_1_ARG( PopFrontTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PopFrontTag&) {
  container.erase(container.begin());
  return container;
}
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PopFrontTag1Arg<ArgType0>& tag) {
  container.erase(container.begin(), std::next(container.begin(), tag.arg0));
  return container;
}

// push_back
CREATE_TAG_0_1_ARG( PushBackTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PushBackTag&) {
  container.emplace_back();
  return container;
}
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PushBackTag1Arg<ArgType0>& tag) {
  container.push_back(tag.arg0);
  return container;
}

// push_front
CREATE_TAG_0_1_ARG( PushFrontTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PushFrontTag&) {
  container.emplace_front();
  return container;
}
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::PushFrontTag1Arg<ArgType0>& tag) {
  container.push_front(tag.arg0);
  return container;
}

#define CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( TAG_NAME, STL_ALGORITHM_NAME ) \
  template <typename ContainerType> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME& tag) { \
    STL_ALGORITHM_NAME(std::begin(container), std::end(container)); \
    return container; \
  }
#define CREATE_MUTABLE_PIPE_IMPLEMENTATION_0_OR_1ARG( TAG_NAME, STL_ALGORITHM_NAME ) \
  template <typename ContainerType> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME& tag) { \
    STL_ALGORITHM_NAME(std::begin(container), std::end(container)); \
    return container; \
  } \
  template <typename ContainerType, typename ArgType0> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME##1Arg<ArgType0>& tag) { \
    STL_ALGORITHM_NAME(std::begin(container), std::end(container), tag.arg0); \
    return container; \
  }
#define CREATE_MUTABLE_PIPE_IMPLEMENTATION_1ARG( TAG_NAME, STL_ALGORITHM_NAME ) \
  template <typename ContainerType, typename ArgType0> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME##1Arg<ArgType0>& tag) { \
    STL_ALGORITHM_NAME(std::begin(container), std::end(container), tag.arg0); \
    return container; \
  }
#define CREATE_MUTABLE_PIPE_IMPLEMENTATION_2ARG( TAG_NAME, STL_ALGORITHM_NAME ) \
  template <typename ContainerType, typename ArgType0, typename ArgType1> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME##2Arg<ArgType0, ArgType1>& tag) { \
    STL_ALGORITHM_NAME(std::begin(container), std::end(container), tag.arg0, tag.arg1); \
    return container; \
  }
#define CREATE_MUTABLE_PIPE_IMPLEMENTATION_SPECIAL( TAG_NAME, CODE ) \
  template <typename ContainerType, typename ArgType0> \
  ContainerType \
  PIPE_OPERATOR(ContainerType container, const UnderscoreTags::TAG_NAME <ArgType0>& tag) { \
    CODE; \
    return container; \
  }

// fill
CREATE_TAG_0_1_ARG( FillTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_1ARG( FillTag, std::fill );

// generate
CREATE_TAG_0_1_ARG( GenerateTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_1ARG( GenerateTag, std::generate );

// reverse
CREATE_TAG_0_ARG( ReverseTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( ReverseTag, std::reverse );

// unique
CREATE_TAG_0_ARG( UniqueTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( UniqueTag, std::unique );

// replace
CREATE_TAG_2_ARG( ReplaceTag );
template <typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::ReplaceTag2Arg<typename ContainerType::value_type, typename ContainerType::value_type>& tag) {
  std::replace(std::begin(container), std::end(container), tag.arg0, tag.arg1);
  return container;
}

// replace_if
CREATE_TAG_2_ARG( ReplaceIfTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_2ARG( ReplaceIfTag, std::replace_if );

// shuffle
CREATE_TAG_0_ARG( ShuffleTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( ShuffleTag, std::shuffle );

// random_shuffle
CREATE_TAG_0_ARG( RandomShuffleTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( RandomShuffleTag, std::random_shuffle );

// sort
CREATE_TAG_0_1_ARG( SortTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0_OR_1ARG( SortTag, std::sort );

// stable_sort
CREATE_TAG_0_1_ARG( StableSortTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0_OR_1ARG( StableSortTag, std::stable_sort );

// stable_partition - todo add predicate
CREATE_TAG_0_1_ARG( StablePartitionTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0_OR_1ARG( StablePartitionTag, std::stable_partition );

// partition - todo add predicate
CREATE_TAG_0_1_ARG( PartitionTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_1ARG( PartitionTag, std::partition );

// nth_element - todo add predicate
CREATE_TAG_0_1_ARG( NthElementTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_SPECIAL( NthElementTag1Arg, {std::nth_element(std::begin(container), std::begin(container) + tag.arg0, std::end(container));}  );

// next_permutation
CREATE_TAG_0_ARG( NextPermutationTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( NextPermutationTag, std::next_permutation );

// prev_permutation
CREATE_TAG_0_ARG( PrevPermutationTag );
CREATE_MUTABLE_PIPE_IMPLEMENTATION_0ARG( PrevPermutationTag, std::prev_permutation );

// rotated
CREATE_TAG_1_ARG( RotatedTag );
template <typename ContainerType, typename ArgType0>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::RotatedTag1Arg<ArgType0>& tag) {
  const auto& first = std::begin(container);
  std::rotate(first, std::next(first, tag.arg0), std::end(container));
  return container;
}

// as
CREATE_TAG_TEMPLATE( AsTag );
template <typename InType, typename OutType>
OutType
PIPE_OPERATOR(const InType& value, const UnderscoreTags::AsTag<OutType>&) {
  UNDERSCORE_STATIC_ASSERT(std::is_pointer<InType>::value, "");
  UNDERSCORE_STATIC_ASSERT(std::is_pointer<OutType>::value, "");
  return dynamic_cast<OutType>(value);
}

// as_const
CREATE_TAG_0_ARG( AsConstTag );
template <typename ValueType>
const ValueType&
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::AsConstTag&) {
  return value;
}

// clamp
CREATE_TAG_2_ARG_SAME_TYPE( ClampTag );
template <typename ValueType>
ValueType
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::ClampTag2Arg<ValueType>& tag) {
  if(value < tag.arg0) return tag.arg0;
  if(value > tag.arg1) return tag.arg1;
  return value;
}

// checked_delete
CREATE_TAG_0_ARG( CheckedDeleteTag );
template <typename PointerType>
const UnderscoreTags::CheckedDeleteTag&
operator>>(const UnderscoreTags::CheckedDeleteTag& tag, PointerType& ptr) {
  static_assert(std::is_pointer<PointerType>::value == true, "");
  const auto& dummy = sizeof(*ptr);
  delete ptr;
  ptr = nullptr;
  return tag;
}

// checked_delete_array
CREATE_TAG_0_ARG( CheckedDeleteArrayTag );
template <typename PointerType>
const UnderscoreTags::CheckedDeleteArrayTag&
operator>>(const UnderscoreTags::CheckedDeleteArrayTag& tag, PointerType& ptr) {
  static_assert(std::is_pointer<PointerType>::value == true, "");
  const auto& dummy = sizeof(*ptr);
  delete [] ptr;
  ptr = nullptr;
  return tag;
}

// deref
CREATE_TAG_0_ARG( DerefTag );
template <typename DereferableType> // immutable
decltype(*DereferableType())
PIPE_OPERATOR(const DereferableType& value, const UnderscoreTags::DerefTag&) {
  return (*value);
}
template <typename DereferableType> // mutable
decltype(*DereferableType())
PIPE_OPERATOR(DereferableType& value, const UnderscoreTags::DerefTag&) {
  return (*value);
}

// find_first_of
CREATE_TAG_1_ARG( FindFirstOfTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindFirstOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_first_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindFirstOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return std::find_first_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// find_first_not_of
CREATE_TAG_1_ARG( FindFirstNotOfTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindFirstNotOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_first_not_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindFirstNotOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_first_not_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// find_last_of
namespace UnderscoreDetail {
  template <typename IteratorType0, typename IteratorType1>
  IteratorType0
  find_last_of(const IteratorType0& first, IteratorType0 last, const IteratorType1& s_first, const IteratorType1& s_last) {
    if (first == last)
      return last;
    //const auto before_first = std::prev(first);
    do{
      --last;
      if (std::find(s_first, s_last, *last) != s_last)
        return last;
    } while(last != first);
    /*
    for(--last; last != first; --last)
      if (std::find(s_first, s_last, *last) != s_last)
        return last;
        */
    return first;
  }
}
CREATE_TAG_1_ARG( FindLastOfTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindLastOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_last_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindLastOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_last_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// find_last_not_of
CREATE_TAG_1_ARG( FindLastNotOfTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FindLastNotOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_last_not_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FindLastNotOfTag1Arg<ArgType0>& tag) {
  UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "");
  return UnderscoreDetail::find_last_not_of(std::begin(container), std::end(container), std::begin(tag.arg0), std::end(tag.arg0));
}

// max_value
CREATE_TAG_0_ARG( MaxValueTag );
template <typename ContainerType> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MaxValueTag&) {
  return *std::max_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MaxValueTag&) {
  return *std::max_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MaxValueTag&) {
  return *std::max_element(std::begin(container), std::end(container));
}

// mean_value
CREATE_TAG_0_ARG( MeanValueTag );
template <typename ContainerType>
typename std::decay<typename ContainerType::value_type>::type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MeanValueTag&) {
  const auto& first = std::begin(container);
  const auto& last = std::end(container);
  const auto& num_elements = static_cast<typename ContainerType::value_type>(container.size());
  UNDERSCORE_ASSERT(num_elements > 0);
  const auto& sum = std::accumulate(std::next(first), last, *first) / num_elements;
  return sum;
}

// min_value
CREATE_TAG_0_ARG( MinValueTag );
template <typename ContainerType> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MinValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::min_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MinValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::min_element(std::begin(container), std::end(container));
}
template <typename ContainerType> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MinValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::min_element(std::begin(container), std::end(container));
}

// minmax_value
CREATE_TAG_0_ARG( MinMaxValueTag );
template <typename ContainerType> // immutable
typename std::pair<typename ContainerType::const_reference, typename ContainerType::const_reference>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MinMaxValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  const auto& locations = std::minmax_element(std::begin(container), std::end(container));
  return std::pair<typename ContainerType::const_reference, typename ContainerType::const_reference>(*locations.first, *locations.second);
}
template <typename ContainerType> // mutable
typename std::pair<typename ContainerType::reference, typename ContainerType::reference>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MinMaxValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  auto locations = std::minmax_element(std::begin(container), std::end(container));
  return std::pair<typename ContainerType::reference, typename ContainerType::reference>(*locations.first, *locations.second);
}
template <typename ContainerType> // rvalue
typename std::pair<typename ContainerType::value_type, typename ContainerType::value_type>
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MinMaxValueTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  auto locations = std::minmax_element(std::begin(container), std::end(container));
  return std::pair<typename ContainerType::value_type, typename ContainerType::value_type>(*locations.first, *locations.second);
}

// reinterval
CREATE_TAG_4_ARG_SAME_TYPE( ReintervalTag );
template <typename ValueType>
ValueType
PIPE_OPERATOR(ValueType value, const UnderscoreTags::ReintervalTag4Arg<ValueType>& tag) {
  UNDERSCORE_ASSERT(tag.arg0 != tag.arg1);
  value -= tag.arg0;
  value /= (tag.arg1 - tag.arg0);
  value *= (tag.arg3 - tag.arg2);
  value += tag.arg2;
  return value;
}

// reinterval_clamped
CREATE_TAG_4_ARG_SAME_TYPE( ReintervalClampedTag );
template <typename ValueType>
ValueType
PIPE_OPERATOR(ValueType value, const UnderscoreTags::ReintervalClampedTag4Arg<ValueType>& tag) {
  UNDERSCORE_ASSERT(tag.arg0 != tag.arg1);
  value -= tag.arg0;
  value /= (tag.arg1 - tag.arg0);
  value *= (tag.arg3 - tag.arg2);
  value += tag.arg2;
  value = std::max(value, tag.arg2);
  value = std::min(value, tag.arg3);
  return value;
}

// to
CREATE_TAG_TEMPLATE( ToTag );
template <typename InType, typename OutType>
OutType
PIPE_OPERATOR(const InType& value, const UnderscoreTags::ToTag<OutType>&) {
  return (const OutType)(value);
}

// to_container - todo preallocation, name it copy
CREATE_TAG_TEMPLATE( ToContainerTag );
template <typename ContainerType, typename OutContainerType>
OutContainerType
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::ToContainerTag<OutContainerType>&) {
  return OutContainerType(std::begin(container), std::end(container));
}

// to_container - todo preallocation, name it copy
CREATE_TAG_2_ARG( CopyIfTag );
template <typename ContainerType, typename PredicateType, typename OutContainerType>
typename std::decay<OutContainerType>::type
PIPE_OPERATOR(const ContainerType& container, UnderscoreTags::CopyIfTag2Arg<PredicateType, OutContainerType>& tag) {
  const auto& predicate = tag.arg0;
  auto output_container = std::move(tag.arg1);
  const auto& out_allocated_end = output_container.end();
  const auto& input_end = container.end();
  // Copy allocated part
  auto input_iterator = container.begin();
  auto output_iterator = output_container.begin();
  for(; input_iterator != input_end && output_iterator != out_allocated_end; ++input_iterator) {
    if (predicate(*input_iterator)) {
      (*output_iterator) = (*input_iterator);
      ++output_iterator;
    }
  }
  // Push back the rest
  while (input_iterator != input_end) {
    if (predicate(*input_iterator)) {
      output_container.push_back(*input_iterator);
    }
  }
  // Erase slack if needed
  if (output_iterator != output_container.end())
    output_container.erase(output_iterator, output_container.end());
  return output_container;
}

// mutate
CREATE_TAG_0_ARG( MutateTag );
template <typename ContainerType>
UnderscoreDetail::mutable_container_base<ContainerType>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MutateTag&) {
  UNDERSCORE_STATIC_ASSERT(std::is_const<ContainerType>::value == false, "Underscore Library Error: Cannot mutate a const container.");
  return UnderscoreDetail::mutable_container_base<ContainerType>(std::addressof(container));
}

// empty
CREATE_TAG_0_ARG( EmptyTag );
template <typename ContainerType>
bool
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::EmptyTag&) {
  return container.empty();
}

// size
CREATE_TAG_0_ARG( SizeTag );
template <typename ContainerType>
typename ContainerType::size_type
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::SizeTag&) {
  return container.size();
}

// front
CREATE_TAG_0_ARG( FrontTag );
template <typename ContainerType> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FrontTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.front();
}
template <typename ContainerType> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FrontTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.front();
}
template <typename ContainerType> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::FrontTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.front();
}

// back
CREATE_TAG_0_ARG( BackTag );
template <typename ContainerType> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::BackTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.back();
}
template <typename ContainerType> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::BackTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.back();
}
template <typename ContainerType> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::BackTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return container.back();
}

// middle
CREATE_TAG_0_ARG( MiddleTag );
template <typename ContainerType> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::MiddleTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), container.size() / 2);
}
template <typename ContainerType> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::MiddleTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), container.size() / 2);
}
template <typename ContainerType> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::MiddleTag&) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), container.size() / 2);
}

// is_any_of
CREATE_TAG_1_2_3_4_ARG_SAME_TYPE( IsAnyOfTag );
template <typename ValueType, typename ArgType0>
bool
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::IsAnyOfTag1Arg<ArgType0>& tag) {
  return value == tag.arg0;
}
template <typename ValueType, typename ArgType0>
bool
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::IsAnyOfTag2Arg<ArgType0>& tag) {
  return value == tag.arg0 || value == tag.arg1;
}
template <typename ValueType, typename ArgType0>
bool
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::IsAnyOfTag3Arg<ArgType0>& tag) {
  return value == tag.arg0 || value == tag.arg1 || value == tag.arg2;
}
template <typename ValueType, typename ArgType0>
bool
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::IsAnyOfTag4Arg<ArgType0>& tag) {
  return value == tag.arg0 || value == tag.arg1 || value == tag.arg2 || value == tag.arg3;
}

// at
CREATE_TAG_1_ARG( AtTag );
template <typename ContainerType, typename ArgType0> // immutable
typename ContainerType::const_reference
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::AtTag1Arg<ArgType0>& tag) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // mutable
typename ContainerType::reference
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::AtTag1Arg<ArgType0>& tag) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), tag.arg0);
}
template <typename ContainerType, typename ArgType0> // rvalue
typename ContainerType::value_type
PIPE_OPERATOR(ContainerType&& container, const UnderscoreTags::AtTag1Arg<ArgType0>& tag) {
  UNDERSCORE_ASSERT(!container.empty());
  return *std::next(container.begin(), tag.arg0);
}

// value_or_default
CREATE_TAG_2_ARG( ValueOrDefaultTag );
template <typename MapType, typename ArgType0, typename ArgType1> // immutable
const ArgType1&
PIPE_OPERATOR(const MapType& dictionary, const UnderscoreTags::ValueOrDefaultTag2Arg<ArgType0, ArgType1>& tag) {
  const auto& it = dictionary.find(tag.arg0);
  return it == dictionary.end() ? tag.arg1 : it->second;
}
template <typename MapType, typename ArgType0, typename ArgType1> // mutable
ArgType1&
PIPE_OPERATOR(MapType& dictionary, UnderscoreTags::ValueOrDefaultTag2Arg<ArgType0, ArgType1>& tag) {
  const auto& it = dictionary.find(tag.arg0);
  return it == dictionary.end() ? tag.arg1 : it->second;
}
template <typename MapType, typename ArgType0, typename ArgType1> // rvalue
ArgType1
PIPE_OPERATOR(MapType&& dictionary, UnderscoreTags::ValueOrDefaultTag2Arg<ArgType0, ArgType1>& tag) {
  const auto& it = dictionary.find(tag.arg0);
  return it == dictionary.end() ? tag.arg1 : it->second;
}

// type_name
CREATE_TAG_0_ARG( TypeNameTag );
template <typename T>
std::string
PIPE_OPERATOR(const T* ptr, const UnderscoreTags::TypeNameTag&) {
  return typeid(*ptr).name();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Strings
//

namespace UnderscoreDetail {
  template <typename StreamType, typename StringType>
  struct make_string_base : public StringType {
    typedef StringType inherited;
    template <typename T>
    make_string_base operator<<(const T& rhs) {
      StreamType stream;
      stream << rhs;
      inherited::append(stream.str());
      return *this;
    }
    template <typename T>
    make_string_base operator<<(const std::basic_string<T>& rhs) {
      StringType str(rhs.begin(), rhs.end());
      inherited::append(str);
      return *this;
    }
  };
  typedef make_string_base<std::stringstream, std::string> make_string;
  typedef make_string_base<std::wstringstream, std::wstring> make_wstring;

  // container_to_string
  template <typename StringType, typename StreamType, typename ContainerType>
  StringType
  container_to_string(const ContainerType& container) {
    auto first = std::begin(container);
    const auto& last = std::end(container);
    const auto& prev_last = std::prev(last);
    StreamType sstr;
    sstr << "[";
    while(first != last) {
      sstr << (*first);
      if (first != prev_last)
        sstr << ", ";
      ++first;
    }
    sstr << "]";
    return sstr.str();
  }
  template <typename ValueType> // is arithmetic, not a container - use std::to_string
  std::string to_string_impl(const ValueType& value, std::true_type, std::false_type) {
    return std::to_string(value);
  }
  template <typename ValueType> // not arithmetic, not a container - use a stream
  std::string to_string_impl(const ValueType& value, std::false_type, std::false_type) {
    std::stringstream sstr;
    sstr << std::boolalpha << value;
    return sstr.str();
  }
  template <typename ContainerType> // not arithmetic, is container - serialize container
  std::string to_string_impl(const ContainerType& container, std::false_type, std::true_type) {
    return container_to_string<std::string, std::stringstream, ContainerType>(container);
  }
  template <typename ValueType>
  std::string to_string(const ValueType& value) {
    return UnderscoreDetail::to_string_impl(value, typename std::is_arithmetic<ValueType>::type(), std::false_type() );
  }
}
CREATE_TAG_0_ARG( ToStringTag );
CREATE_TAG_0_ARG( ToWstringTag );
template <typename ValueType> // std::string
std::string
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::ToStringTag&) {
  return UnderscoreDetail::to_string(value);
}
template <typename ValueType> // std::wstring
std::wstring
PIPE_OPERATOR(const ValueType& value, const UnderscoreTags::ToWstringTag&) {
  return std::wstring(); //UnderscoreDetail::to_wstring(value); todo
}

// to_string/to_wstring - vector
template <typename ValueType, typename AllocType>
std::string
PIPE_OPERATOR(const std::vector<ValueType, AllocType>& container, const UnderscoreTags::ToStringTag&) {
  return UnderscoreDetail::container_to_string<std::string, std::stringstream>(container);
}
template <typename ValueType, typename AllocType>
std::wstring
PIPE_OPERATOR(const std::vector<ValueType, AllocType>& container, const UnderscoreTags::ToWstringTag&) {
  return UnderscoreDetail::container_to_string<std::wstring, std::wstringstream>(container);
}
// to_string - list
template <typename ValueType, typename AllocType>
std::string
PIPE_OPERATOR(const std::list<ValueType, AllocType>& container, const UnderscoreTags::ToStringTag&) {
  return UnderscoreDetail::container_to_string<std::string, std::stringstream>(container);
}
template <typename ValueType, typename AllocType>
std::wstring
PIPE_OPERATOR(const std::list<ValueType, AllocType>& container, const UnderscoreTags::ToWstringTag&) {
  return UnderscoreDetail::container_to_string<std::wstring, std::wstringstream>(container);
}
// to_string - deque
template <typename ValueType, typename AllocType>
std::string
PIPE_OPERATOR(const std::deque<ValueType, AllocType>& container, const UnderscoreTags::ToStringTag&) {
  return UnderscoreDetail::container_to_string<std::string, std::stringstream>(container);
}
template <typename ValueType, typename AllocType>
std::wstring
PIPE_OPERATOR(const std::deque<ValueType, AllocType>& container, const UnderscoreTags::ToWstringTag&) {
  return UnderscoreDetail::container_to_string<std::wstring, std::wstringstream>(container);
}



// replace_substring - todo improve performance
namespace UnderscoreDetail {
  template <typename ContainerType, typename OriginalSequenceIterator, typename ReplacementSequenceIterator>
  size_t
  replace_sequence(
    ContainerType& container,
    const OriginalSequenceIterator& original_begin,
    const OriginalSequenceIterator& original_end,
    const ReplacementSequenceIterator& replacement_begin,
    const ReplacementSequenceIterator& replacement_end
  ) {
    const auto& original_length = std::distance(original_begin, original_end);
    if (original_length == 0)
      return 0;
    const auto& replacement_length = std::distance(replacement_begin, replacement_end);
    // Return 0 if equal
    if (original_length == replacement_length) {
      auto o_it = original_begin;
      auto r_it = replacement_begin;
      bool equal = true;
      for(; o_it != original_end; ++o_it, ++r_it) // Use hand made version of std::equal as Visual Studio gives a warning otherwise.
        if ((*o_it) != (*r_it)) {
          equal = false;
          break;
        }
       if (equal)
         return 0;
    }
    // Replace
    size_t num_replacements = 0;
    for(auto pos = std::search(std::begin(container), std::end(container), original_begin, original_end);
        pos != std::end(container);
        pos = std::search(pos, std::end(container), original_begin, original_end)
       ) {
      const auto& idx = std::distance(std::begin(container), pos);
      // Match size of hole
      if (original_length > replacement_length)
        container.erase(pos, std::next(pos, original_length - replacement_length));
      else if (original_length < replacement_length)
        container.insert(pos, replacement_length - original_length, typename ContainerType::value_type());
      // Copy
      pos = std::next(std::begin(container), idx);
      std::copy(replacement_begin, replacement_end, pos);
      pos = std::next(pos, replacement_length);
      ++num_replacements;
    }
    return num_replacements;
  }
  template <typename ValueType>
  const ValueType* find_zero(const ValueType* first) {
    while(*first != static_cast<ValueType>(0))
      ++first;
    return first;
  }
}
CREATE_TAG_2_ARG( ReplaceSubstringTag );
template <typename ContainerType, typename ArgType0, typename ArgType1>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::ReplaceSubstringTag2Arg<ArgType0, ArgType1>& tag) {
  typedef decltype(*std::begin(tag.arg0)) ValueType;
  const static auto zero = static_cast<ValueType>(0);
  const auto& original_last = std::find(std::begin(tag.arg0), std::end(tag.arg0), zero); // for null-ended strings compatibility
  const auto& replacement_last = std::find(std::begin(tag.arg1), std::end(tag.arg1), zero);
  UnderscoreDetail::replace_sequence(
    container,
    std::begin(tag.arg0),
    original_last,
    std::begin(tag.arg1),
    replacement_last
  );
  return container;
}

// str
CREATE_TAG_0_ARG( StrTag );
template <typename ValueType>
UnderscoreDetail::make_string
operator<<(const UnderscoreTags::StrTag&, const ValueType& value) {
  UnderscoreDetail::make_string mstr;
  mstr << value;
  return mstr;
}

// wstr
CREATE_TAG_0_ARG( WstrTag );
template <typename ValueType>
UnderscoreDetail::make_wstring
operator<<(const UnderscoreTags::WstrTag&, const ValueType& value) {
  UnderscoreDetail::make_string mstr;
  mstr << value;
  return mstr;
}

// tokenize string
CREATE_TAG_1_ARG(TokenizeStringTag);
template <typename ContainerType, typename ArgType0>
std::vector<ContainerType>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::TokenizeStringTag1Arg<ArgType0>& tag) {
	std::vector<ContainerType> tokens;
	tokens.reserve(16);
	const auto& container_begin = std::begin(container);
	const auto& container_end = std::end(container);
	const auto& delimiters = tag.arg0;
	const auto& delimiters_begin = std::begin(delimiters);
	const auto& delimiters_end = std::find(delimiters_begin, std::end(delimiters), 0); // Find null, to be compatible with char literals
	for(auto left = UnderscoreDetail::find_first_not_of(container_begin, container_end, delimiters_begin, delimiters_end);
		left != container_end;
		left = UnderscoreDetail::find_first_not_of(left, container_end, delimiters_begin, delimiters_end)
		) {
		const auto& right = std::find_first_of(std::next(left), container_end, delimiters_begin, delimiters_end);
		ContainerType token;
		token.assign(left, right);
		tokens.emplace_back(std::move(token));
		left = right;
  }
  return tokens;
}

// to_lower
CREATE_TAG_0_ARG( ToLowerTag );
template<typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::ToLowerTag&) {
  for(auto it = std::begin(container); it != std::end(container); ++it)
    *it = ::tolower(*it);
  return container;
}

// to_upper
CREATE_TAG_0_ARG( ToUpperTag );
template<typename ContainerType>
ContainerType
PIPE_OPERATOR(ContainerType container, const UnderscoreTags::ToUpperTag&) {
  for(auto it = std::begin(container); it != std::end(container); ++it)
    *it = ::toupper(*it);
  return container;
}
























/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


struct Underscore {
  Underscore(){}

  // STL Tags for piping
  UnderscoreTags::AccumulateTag accumulate;
  UnderscoreTags::AdressOfTag adressof;
  UnderscoreTags::AllOfTag all_of;
  UnderscoreTags::AllOfEqualTag all_of_equal;
  UnderscoreTags::AnyOfTag any_of;
  UnderscoreTags::AnyOfTag contains_if;
  UnderscoreTags::AnyOfEqualTag any_of_equal;
  UnderscoreTags::AnyOfEqualTag contains;
  UnderscoreTags::BackInserterTag back_inserter;
  UnderscoreTags::BeginTag begin;
  UnderscoreTags::CBeginTag cbegin;
  UnderscoreTags::CEndTag cend;
  UnderscoreTags::CopyIfTag copy_if;
  UnderscoreTags::CountTag count;
  UnderscoreTags::CountIfTag count_if;
  UnderscoreTags::EndTag end;
  UnderscoreTags::EqualTag equal;
  UnderscoreTags::FindTag find;
  UnderscoreTags::FindFirstOfTag find_first_of;
  UnderscoreTags::FindFirstNotOfTag find_first_not_of;
  UnderscoreTags::FindLastOfTag find_last_of;
  UnderscoreTags::FindLastNotOfTag find_last_not_of;
  UnderscoreTags::FindIfTag find_if;
  UnderscoreTags::FindIfNotTag find_if_not;
  UnderscoreTags::FindIndexTag find_index;
  UnderscoreTags::FindIndexIfTag find_index_if;
  UnderscoreTags::FindIndexIfNotTag find_index_if_not;
  UnderscoreTags::ForEachTag for_each;
  UnderscoreTags::FrontInserterTag front_inserter;
  template <size_t Idx> UnderscoreTags::GetTag<Idx> get() const { return UnderscoreTags::GetTag<Idx>(); }
  UnderscoreTags::GetTag<0> get_0, first;
  UnderscoreTags::GetTag<1> get_1, second;
  UnderscoreTags::GetTag<2> get_2, third;
  UnderscoreTags::GetTag<3> get_3, forth;
  UnderscoreTags::GetTag<4> get_4, fifth;
  UnderscoreTags::GetTag<5> get_5, sixth;
  UnderscoreTags::GetTag<6> get_6, seventh;
  UnderscoreTags::HashTag hash;
  UnderscoreTags::InserterTag inserter;
  UnderscoreTags::IsSortedTag is_sorted;
  UnderscoreTags::MeanValueTag mean_value;
  UnderscoreTags::MoveTag move;
  UnderscoreTags::NextTag next;
  UnderscoreTags::NoneOfTag none_of;
  UnderscoreTags::NoneOfEqualTag none_of_equal;
  UnderscoreTags::MaxElementTag max_element;
  UnderscoreTags::MinElementTag min_element;
  UnderscoreTags::MinMaxElementTag minmax_element;
  UnderscoreTags::PrevTag prev;
  UnderscoreTags::RotatedTag rotated;
  UnderscoreTags::ToStringTag to_string;
  UnderscoreTags::ToWstringTag to_wstring;
  UnderscoreTags::BinarySearchTag binary_search;//
  UnderscoreTags::IncludesTag includes;//
  UnderscoreTags::LexicographicalCompareTag lexicographical_compare; //
  UnderscoreTags::LowerBoundTag lower_bound; //
  UnderscoreTags::UpperBoundTag upper_bound; //

  // STL extensions
  UnderscoreTags::FillTag fill;
  UnderscoreTags::GenerateTag generate;
  UnderscoreTags::NextPermutationTag prev_permutation;
  UnderscoreTags::NthElementTag nth_element;
  UnderscoreTags::PartitionTag partition;
  UnderscoreTags::PrevPermutationTag next_permutation;
  UnderscoreTags::RandomShuffleTag random_shuffle;
  UnderscoreTags::ReplaceTag replace;
  UnderscoreTags::ReplaceIfTag replace_if;
  UnderscoreTags::ReverseTag reverse;
  UnderscoreTags::ShuffleTag shuffle;
  UnderscoreTags::SortTag sort;
  UnderscoreTags::StablePartitionTag stable_partition;
  UnderscoreTags::StableSortTag stable_sort;
  UnderscoreTags::TransformTag transform;
  UnderscoreTags::TransformToTag transform_to;
  UnderscoreTags::UniqueTag unique;

  // Underscore immutable tags
  UnderscoreTags::AsConstTag as_const;
  template <typename T> UnderscoreTags::AsTag<T> as() const { return UnderscoreTags::AsTag<T>(); }
  UnderscoreTags::ClampTag clamp;
  UnderscoreTags::DerefTag deref;
  UnderscoreTags::MaxValueTag max_value;
  UnderscoreTags::MinValueTag min_value;
  UnderscoreTags::MinMaxValueTag minmax_value;
  UnderscoreTags::ReintervalTag reinterval;
  UnderscoreTags::ReintervalClampedTag reinterval_clamped;
  template <typename T> UnderscoreTags::ToTag<T> to() const { return UnderscoreTags::ToTag<T>(); }
  UnderscoreTags::ToTag<char>     to_char;
  UnderscoreTags::ToTag<short>    to_short;
  UnderscoreTags::ToTag<double>   to_double;
  UnderscoreTags::ToTag<float>    to_float;
  UnderscoreTags::ToTag<int>      to_int;
  UnderscoreTags::ToTag<int8_t>   to_int8_t;
  UnderscoreTags::ToTag<int16_t>  to_int16_t;
  UnderscoreTags::ToTag<int32_t>  to_int32_t;
  UnderscoreTags::ToTag<int64_t>  to_int64_t;
  UnderscoreTags::ToTag<size_t>   to_size_t;
  UnderscoreTags::ToTag<uint8_t>  to_uint8_t;
  UnderscoreTags::ToTag<uint16_t> to_uint16_t;
  UnderscoreTags::ToTag<uint32_t> to_uint32_t;
  UnderscoreTags::ToTag<uint64_t> to_uint64_t;
  UnderscoreTags::ToTag<void*>    to_voidptr;
  UnderscoreTags::ToTag<const void*> to_constvoidptr;
  UnderscoreTags::ToVectorTag to_vector;
  UnderscoreTags::ToListTag to_list;
  UnderscoreTags::ToDequeTag to_deque;
  UnderscoreTags::ToSetTag to_set;
  UnderscoreTags::ToUnorderedSetTag to_unordered_set;
  template <typename T> UnderscoreTags::ToContainerTag<T> to_container() const { return UnderscoreTags::ToContainerTag<T>(); }
  UnderscoreTags::MutateTag mutate;
  UnderscoreTags::PipeTag pipe;
  UnderscoreTags::TrimLeftTag trim_left;
  UnderscoreTags::TrimRightTag trim_right;
  UnderscoreTags::TrimTag trim;
  UnderscoreTags::ToLowerTag to_lower;
  UnderscoreTags::ToUpperTag to_upper;
  UnderscoreTags::TokenizeStringTag tokenize_string;
  // Views
  //UnderscoreTags::SubViewTag sub_view;
  //UnderscoreTags::WhereTag where;

  // Access tags
  UnderscoreTags::AtTag at;
  UnderscoreTags::BackTag back;
  UnderscoreTags::EmptyTag empty;
  UnderscoreTags::FrontTag front;
  UnderscoreTags::SizeTag size;
  UnderscoreTags::ValueOrDefaultTag value_or_default;

  // Mutating tags
  UnderscoreTags::EraseAllTag erase_all;
  UnderscoreTags::EraseAllIfTag erase_all_if;
  UnderscoreTags::EraseAllUnstableTag erase_all_unstable;
  UnderscoreTags::EraseConsecutiveDuplicatesTag erase_consecutive_duplicates;
  UnderscoreTags::EraseAllIfUnstableTag erase_all_if_unstable;
  UnderscoreTags::EraseDuplicatesUnstableTag erase_duplicates_unstable;
  UnderscoreTags::EraseFirstTag erase_first;
  UnderscoreTags::EraseFirstIfTag erase_first_if;
  UnderscoreTags::EraseLastTag erase_last;
  UnderscoreTags::EraseLastIfTag erase_last_if;
  UnderscoreTags::InsertedFrontTag inserted_front;
  UnderscoreTags::InsertedBackTag inserted_back;
  UnderscoreTags::PopBackTag pop_back;
  UnderscoreTags::PopFrontTag pop_front;
  UnderscoreTags::PushBackTag push_back;
  UnderscoreTags::PushFrontTag push_front;
  UnderscoreTags::ReplaceSubstringTag replace_substring;
  UnderscoreTags::IsAnyOfTag is_any_of;
  UnderscoreTags::TypeNameTag type_name;

  // Math
  UnderscoreTags::SinTag sin;
  UnderscoreTags::CosTag cos;
  UnderscoreTags::TanTag tan;
  UnderscoreTags::AsinTag asin;
  UnderscoreTags::AcosTag acos;
  UnderscoreTags::AtanTag atan;
  UnderscoreTags::SinhTag sinh;
  UnderscoreTags::CoshTag cosh;
  UnderscoreTags::TanhTag tanh;
  UnderscoreTags::AbsTag abs;
  UnderscoreTags::FabsTag fabs;
  UnderscoreTags::ExpTag exp;
  UnderscoreTags::Exp2Tag exp2;
  UnderscoreTags::LogTag log;
  UnderscoreTags::Log10Tag log10;
  UnderscoreTags::FloorTag floor;
  UnderscoreTags::CeilTag ceil;
  UnderscoreTags::RoundTag round;
  UnderscoreTags::TruncTag trunc;
  UnderscoreTags::ModfTag modf;
  UnderscoreTags::PowTag pow;

  // String functionality
  const UnderscoreTags::StrTag str;
  const UnderscoreTags::WstrTag wstr;

  // Dereference with operator*
  const UnderscoreTags::DerefTag operator*() const {
    return UnderscoreTags::DerefTag();
  }

  // Adress of with operator&
  const UnderscoreTags::AdressOfTag operator&() const {
    return UnderscoreTags::AdressOfTag();
  }
  
  // Checked delete
  UnderscoreTags::CheckedDeleteTag checked_delete;
  UnderscoreTags::CheckedDeleteArrayTag checked_delete_array;

  // Create mutable value/container with operator[]
  template <typename ContainerType>
  UnderscoreDetail::mutable_container_base<ContainerType>
  operator[](ContainerType& container) const {
    UNDERSCORE_STATIC_ASSERT(std::is_const<ContainerType>::value == false, "Underscore Library Error: Cannot mutate a const-container");
    UNDERSCORE_STATIC_ASSERT(std::is_rvalue_reference<ContainerType>::value == false, "Underscore Library Error: Cannot mutate an rvalue-container");
    return UnderscoreDetail::mutable_container_base<ContainerType>(&container);
  }

  // Pipe functions directly with operator() // todo needs more work
  template <typename FunctorType>
  UnderscoreTags::PipeTag1Arg <FunctorType>
  operator()(FunctorType functor) const {
    return UnderscoreTags::PipeTag1Arg<FunctorType>(functor);
  }
  template <typename FunctorType, typename ArgType0>
  UnderscoreTags::PipeTag2Arg <FunctorType, ArgType0>
  operator()(FunctorType functor, const ArgType0& arg0) const {
    return UnderscoreTags::PipeTag2Arg<FunctorType, ArgType0>(functor, arg0);
  }

  // Make arrays
  template <typename T>
  std::array<T, 1> array(const T& a0) {
    std::array<T, 1> ret = {a0};
    return ret;
  }
  template <typename T>
  std::array<T, 2> array(const T& a0, const T& a1) {
    std::array<T, 2> ret = {a0, a1};
    return ret;
  }
  template <typename T>
  std::array<T, 3> array(const T& a0, const T& a1, const T& a2) {
    std::array<T, 3> ret = {a0, a1, a2};
    return ret;
  }
  template <typename T>
  std::array<T, 4>array(const T& a0, const T& a1, const T& a2, const T& a3) {
    std::array<T, 4> ret = {a0, a1, a2, a3};
    return ret;
  }
  template <typename T>
  std::array<T, 5> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
    std::array<T, 5> ret = {a0, a1, a2, a3, a4};
    return ret;
  }
  template <typename T>
  std::array<T, 6> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
    std::array<T, 6> ret = {a0, a1, a2, a3, a4, a5};
    return ret;
  }
  template <typename T>
  std::array<T, 7> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6) {
    std::array<T, 7> ret = {a0, a1, a2, a3, a4, a5, a6};
    return ret;
  }
  template <typename T>
  std::array<T, 8> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7) {
    std::array<T, 8> ret = {a0, a1, a2, a3, a4, a5, a6, a7};
    return ret;
  }
  template <typename T>
  std::array<T, 9> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8) {
    std::array<T, 9> ret = {a0, a1, a2, a3, a4, a5, a6, a7, a8};
    return ret;
  }
  template <typename T>
  std::array<T, 10> array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8, const T& a9) {
    std::array<T, 10> ret = {a0, a1, a2, a3, a4, a5, a6, a7, a8, a9};
    return ret;
  }
};
static Underscore _;
