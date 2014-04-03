#pragma once


#define VALUE_TYPE(IteratorType) typename std::decay<decltype(*BaseIteratorType())>::type

namespace UnderscoreDetail {


template <typename BaseIteratorType, typename PredicateType>
class ConstFilterIterator : public std::iterator<std::forward_iterator_tag, VALUE_TYPE(BaseIteratorType)>  {
  typedef ConstFilterIterator<BaseIteratorType, PredicateType> my_type;
public:
  typedef VALUE_TYPE(BaseIteratorType) value_type;
  typedef typename std::forward_iterator_tag iterator_category;
  typedef typename std::ptrdiff_t difference_type;
  typedef PredicateType predicate_type;
  typedef BaseIteratorType base_iterator_type;
  ConstFilterIterator(BaseIteratorType begin, BaseIteratorType end, PredicateType predicate) 
  : base_iterator_(std::move(begin))
  , end_(std::move(end))
  , predicate_(std::move(predicate))
  {}
  ConstFilterIterator(const ConstFilterIterator& other)  
  : base_iterator_(other.base_iterator_)
  , end_(other.end_)
  , predicate_(other.predicate_)
  {}
  bool operator==(const ConstFilterIterator& other) const { return base_iterator_ == other.base_iterator_; }
  bool operator!=(const ConstFilterIterator& other) const { return !(*this == other); }
  bool operator<(const ConstFilterIterator& other) const { return base_iterator_ < other.base_iterator_; }
  const value_type& operator*() const { return *base_iterator_; }
  const value_type& operator->() const { return *base_iterator_; }
  void operator++() {
    for(++base_iterator_; base_iterator_ != end_ && false == predicate_(*base_iterator_); )
      ++base_iterator_;
  }
  void first_valid() {
    while( base_iterator_ != end_ && false == predicate_(*base_iterator_))
      ++(base_iterator_);
  }
protected:
  BaseIteratorType base_iterator_;
  BaseIteratorType end_;
  PredicateType predicate_; 
};

template <typename BaseIteratorType, typename PredicateType>
class MutableFilterIterator : public std::iterator<std::forward_iterator_tag, VALUE_TYPE(BaseIteratorType)>  {
  typedef MutableFilterIterator<BaseIteratorType, PredicateType> my_type;
public:
  typedef VALUE_TYPE(BaseIteratorType) value_type;
  typedef typename std::forward_iterator_tag iterator_category;
  typedef typename std::ptrdiff_t difference_type;
  typedef PredicateType predicate_type;
  typedef BaseIteratorType base_iterator_type;
  MutableFilterIterator(const BaseIteratorType& begin, const BaseIteratorType& end, const PredicateType& predicate) 
  : base_iterator_(begin)
  , end_(end)
  , predicate_(predicate)
  {}
  MutableFilterIterator(const MutableFilterIterator& other)  
  : base_iterator_(other.base_iterator_)
  , end_(other.end_)
  , predicate_(other.predicate_)
  {}
  bool operator==(const MutableFilterIterator& other) const { return base_iterator_ == other.base_iterator_; }
  bool operator!=(const MutableFilterIterator& other) const { return !(*this == other); }
  bool operator<(const MutableFilterIterator& other) const { return base_iterator_ < other.base_iterator_; }
  const value_type& operator*() const { return *base_iterator_; }
  const value_type& operator->() const { return *base_iterator_; }
  value_type& operator*() { return *base_iterator_; }
  value_type& operator->() { return *base_iterator_; }
  void operator++() {
    for(++base_iterator_; base_iterator_ != end_ && false == predicate_(*base_iterator_); )
      ++base_iterator_;
  }
  void first_valid() {
    while( base_iterator_ != end_ && false == predicate_(*base_iterator_))
      ++base_iterator_;
  }
protected:
  BaseIteratorType base_iterator_;
  BaseIteratorType end_;
  PredicateType predicate_; 
};


template <typename BaseIteratorType, typename PredicateType>
struct ConstFilterRange {
  typedef VALUE_TYPE(BaseIteratorType) value_type;
  typedef ConstFilterIterator<BaseIteratorType, PredicateType> ConstFilterIteratorType;
  typedef MutableFilterIterator<BaseIteratorType, PredicateType> MutableFilterIteratorType;
  typedef MutableFilterIteratorType iterator;
  typedef ConstFilterIteratorType const_iterator;
  ConstFilterRange(BaseIteratorType first, BaseIteratorType last, PredicateType predicate) 
  : first_(std::move(first))
  , last_(std::move(last))
  , predicate_(std::move(predicate))
  {}
  iterator begin() {
    iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  const_iterator begin() const {
    const_iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  const_iterator cbegin() const {
    const_iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  iterator end() {
    return iterator(last_, last_, predicate_);
  }
  const_iterator end() const {
    return const_iterator(last_, last_, predicate_);
  }
  const_iterator cend() const {
    return const_iterator(last_, last_, predicate_);
  }
private:
  BaseIteratorType first_;
  BaseIteratorType last_;
  PredicateType predicate_;
};

template <typename BaseIteratorType, typename PredicateType>
struct MutableFilterRange {
  typedef VALUE_TYPE(BaseIteratorType) value_type;
  typedef ConstFilterIterator<BaseIteratorType, PredicateType> ConstFilterIteratorType;
  typedef MutableFilterIterator<BaseIteratorType, PredicateType> MutableFilterIteratorType;
  typedef MutableFilterIteratorType iterator;
  typedef ConstFilterIteratorType const_iterator;
  MutableFilterRange(const BaseIteratorType& first, const BaseIteratorType& last, const PredicateType& predicate) 
  : first_(first)
  , last_(last)
  , predicate_(predicate)
  {}
  iterator begin() {
    iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  const_iterator begin() const {
    const_iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  const_iterator cbegin() const {
    const_iterator it(first_, last_, predicate_);
    it.first_valid();
    return it;
  }
  iterator end() {
    return iterator(last_, last_, predicate_);
  }
  const_iterator end() const {
    return const_iterator(last_, last_, predicate_);
  }
  const_iterator cend() const {
    return const_iterator(last_, last_, predicate_);
  }
private:
  BaseIteratorType first_;
  BaseIteratorType last_;
  PredicateType predicate_;
};

// sub_view_base
template <typename ConstIteratorType, typename IteratorType>
class sub_view_base {
public:
  typedef ConstIteratorType const_iterator;
  typedef IteratorType iterator;
  typedef typename std::iterator_traits<IteratorType>::value_type value_type;
  typedef typename std::iterator_traits<IteratorType>::reference reference;
  sub_view_base(iterator first, iterator last) 
  : first_(std::move(first))
  , last_(std::move(last)) 
  {}
  size_t size() const { return std::distance(first_, last_); }
  bool empty() const { return first_ == last_; }
  iterator begin() { return first_; }
  iterator end() { return last_; }
  const_iterator begin() const { return first_; }
  const_iterator end() const { return last_; }
  const_iterator cbegin() const { return first_; }
  const_iterator cend() const { return last_; }
  iterator rbegin() { return std::reverse_iterator<iterator>(std::prev(last_)); }
  iterator rend() { return std::reverse_iterator<iterator>(std::prev(first_)); }
  const_iterator rbegin() const { return std::reverse_iterator<const_iterator>(std::prev(last_)); }
  const_iterator rend() const { return std::reverse_iterator<const_iterator>(std::prev(first_)); }
  const_iterator crbegin() const { return std::reverse_iterator<const_iterator>(std::prev(last_)); }
  const_iterator crend() const { return std::reverse_iterator<const_iterator>(std::prev(first_)); }
  void pop_back() { if (first_ != last_) --last_; }
  void pop_front() { if (first_ != last_) ++first_; }
  void erase(iterator position) {
    UNDERSCORE_ASSERT(position == begin());
    pop_front();
  }
  void erase(iterator first, iterator last) {
    UNDERSCORE_ASSERT(first == begin());
    std::advance(first_, std::distance(first, last));
  }
  value_type& at(size_t idx) { return *std::next(first_, idx); }
  const value_type& at(size_t idx) const { return *std::next(first_, idx); }
  value_type& operator[](size_t idx) { return *std::next(first_, idx); }
  const value_type& operator[](size_t idx) const { return *std::next(first_, idx); }
  value_type& front() { return *first_; }
  const value_type& front() const { return *first_; }
  value_type& back() { return *std::prev(last_); }
  const value_type& back() const { return *std::prev(last_); }
  void resize(size_t new_size) {
    UNDERSCORE_ASSERT( new_size <= std::distance(first_, last_) );
    last_ = std::next(first_, new_size);
  }
private:
  iterator first_;
  iterator last_;
};

} // UnderscoreDetail

// filter_view
CREATE_TAG_1_ARG( FilterViewTag );
template <typename Functor>
struct FilterViewTag1Arg {
  FilterViewTag1Arg(Functor functor) : functor(std::move(functor)) {}
  Functor functor;
};
struct FilterViewTag {
  template <typename Functor>
  FilterViewTag1Arg<Functor>
  operator()(Functor functor) const {
    return FilterViewTag1Arg<Functor>(std::move(functor));  
  }
};
template <typename ContainerType, typename ArgType0>
UnderscoreDetail::ConstFilterRange<typename ContainerType::const_iterator, typename std::decay<ArgType0>::type>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::FilterViewTag1Arg<ArgType0>& tag) {
  typedef UnderscoreDetail::ConstFilterRange<typename ContainerType::const_iterator, typename std::decay<ArgType0>::type> Range;
  return Range(container.begin(), container.end(), std::move(tag.arg0));
}
template <typename ContainerType, typename ArgType0>
UnderscoreDetail::MutableFilterRange<typename ContainerType::iterator, typename std::decay<ArgType0>::type>
PIPE_OPERATOR(ContainerType& container, const UnderscoreTags::FilterViewTag1Arg<ArgType0>& tag) {
  typedef UnderscoreDetail::MutableFilterRange<typename ContainerType::iterator, typename std::decay<ArgType0>::type> Range;
  return Range(container.begin(), container.end(), std::move(tag.arg0));
}

// sub_view
CREATE_TAG_0_1_2_ARG_SAME_TYPE( SubViewTag );
template <typename ContainerType, typename ArgType0> // immutable - one argument
UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::const_iterator>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::SubViewTag1Arg<ArgType0>& tag) {
  const auto& left = std::next(container.cbegin(), tag.arg0);
  const auto& right = std::end(container);
  typedef UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::const_iterator> Range;
  return Range(left, right);
}
/*
template <typename ContainerType, typename ArgType0> // immutable
UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::iterator>
PIPE_OPERATOR(const ContainerType& container, const UnderscoreTags::SubViewTag2Arg<ArgType0>& tag) {
  const auto& left = std::next(container.cbegin(), tag.arg0);
  const auto& right = std::next(left, tag.arg1);
  typedef UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::iterator> Range;
  return Range(left, right);
}
*/
template <typename BaseContainerType, typename ArgType0> // mutable - requires a mutable_container
UnderscoreDetail::sub_view_base <typename ContainerType::const_iterator, typename ContainerType::iterator>
PIPE_OPERATOR(UnderscoreDetail::mutable_container_base<BaseContainerType> mutable_container, const UnderscoreTags::SubViewTag1Arg<ArgType0>& tag) {
  const auto& left = std::next(mutable_container.begin(), tag.arg0);
  const auto& right = std::end(mutable_container);
  typedef UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::iterator> Range;
  return Range(left, right);
}
/*
template <typename BaseContainerType, typename ArgType0> // mutable - requires a mutable_container
UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::iterator>
PIPE_OPERATOR(UnderscoreDetail::mutable_container_base<BaseContainerType> mutable_container, const UnderscoreTags::SubViewTag2Arg<ArgType0>& tag) {
  const auto& left = std::next(mutable_container.begin(), tag.arg0);
  const auto& right = std::next(left, tag.arg1);
  typedef UnderscoreDetail::sub_view_base<typename ContainerType::const_iterator, typename ContainerType::iterator> Range;
  return Range(left, right);
}
*/
/*
template <typename BaseIteratorType, typename ArgType0> // mutable - requires a mutable_container
UnderscoreDetail::sub_view_base<BaseIteratorType>
  PIPE_OPERATOR(UnderscoreDetail::sub_view_base<BaseIteratorType> sub_view, const UnderscoreTags::SubViewTag1Arg<ArgType0>& tag) {
  const auto& left = std::next(sub_view.begin(), tag.arg0);
  return UnderscoreDetail::sub_view_base<BaseIteratorType>(left, std::end(sub_view));
}
template <typename BaseIteratorType, typename ArgType0> // create a new sub_view from previous sub_view
UnderscoreDetail::sub_view_base<BaseIteratorType>
PIPE_OPERATOR(UnderscoreDetail::sub_view_base<BaseIteratorType> sub_view, const UnderscoreTags::SubViewTag2Arg<ArgType0>& tag) {
  const auto& left = std::next(sub_view.begin(), tag.arg0);
  const auto& right = std::next(left, tag.arg1);
  return UnderscoreDetail::sub_view_base<BaseIteratorType>(left, right);
}
*/

namespace utl {
  UnderscoreTags::SubViewTag sub_view;
  UnderscoreTags::FilterViewTag filter_view;
}