#pragma once
#include "underscore.h"
#include <tuple>

static void old_syntax_test() {

  auto data = _.array(1,2,3,4,5,6); 
  const auto& tpl = std::make_tuple(1,2,3); 

  std::string("1235687") | _.hash; 

  data | _.to_vector | _.to_list | _.to_deque | _.to_set | _.to_unordered_set;
  data | _.accumulate; 
  data | _.accumulate(0); 
  data | _.lexicographical_compare(_.array(1,4,5,6)); 
  data | _.adressof; 
  data | _.all_of([](int val) { return true;}); 
  data | _.all_of_equal(8); 
  data | _.any_of([](int val) { return true;}); 
  data | _.contains_if([](int val) { return true;}); 
  data | _.any_of([](int val) { return true;}); 
  data | _.any_of_equal(8); 
  data | _.contains(8); 
  data | _.back_inserter; 
  data | _.begin; 
  data | _.cbegin; 
  data | _.cend; 
  data | _.count(8); 
  data | _.count_if([](int val) { return true;}); 
  data | _.end; 
  data | _.find(8); 
  data | _.find_if_not([](int val) { return true;}); 
  data | _.find_if([](int val) { return true; }); 
  data | _.find_index(8); 
  data | _.find_index_if_not([](int val) { return true;}); 
  data | _.find_index_if([](int val) { return true; }); 
  //data | _.forward; 
  data | _.front_inserter; 
  const auto& val = tpl | _.get<1>(); 
  //data | _.inserter; 
  auto b = data | _.is_sorted; 
  data | _.move; 
  data.begin() | _.next; 
  data | _.none_of([](int val) { return true;}); 
  data | _.none_of_equal(8); 
  data | _.max_element; 
  data | _.min_element; 
  data | _.minmax_element | _.first; 
  data | _.end | _.prev | *_; 
  std::string str = 7.8 | _.to_string; 
  std::wstring wstr = 0.8 | _.to_wstring; 

  data | _.as_const; 
  data.front() | _.to<double>() | _.clamp(0.0, 1.0); 
  data.begin() | _.deref; 
  data.begin() | *_; 
  data | _.max_value; 
  data | _.mean_value; 
  data | _.min_value; 
  data | _.minmax_value; 
  data | _.front | _.reinterval(0, 1, 2, 3); 
  data | _.back | _.reinterval_clamped(0, 1, 2, 3); 

  100.0 | _.sin | _.cos | _.tan| _.asin | _.acos | _.atan | _.sinh | _.cosh | _.tanh | _.abs| _.fabs | _.ceil | _.floor| _.round; 

  std::vector<int> veccc; 
  auto asdf = data | _.to_vector; 

  //CREATE_TAG_2_ARG( ValueOrDefaultTag ); 

}