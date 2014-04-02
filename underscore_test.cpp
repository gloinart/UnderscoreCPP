/**
UNDERSCORE CPP Project
Unit tests.
*/

#include "underscore_test.h"
#include <iostream>


#define LOG(msg) std::cout << "[" << __LINE__ << "]: " << msg << std::endl;
#define LOGV(msg) LOG( #msg << " = [" << (msg) << "]");

#include "underscore.h"
#include "old_syntax_test.h"



#define TEST(left, right) {\
  const auto l = (left);\
  const auto r = (right);\
  if (l != r) { \
    std::cout << "Fail:   (" << #left << ") == (" << #right << ")" << std::endl; \
  } else { \
  }}
  //std::cout << "Succes: (" << #left << ") == (" << #right << ")" << std::endl; \
  
namespace UTDetail {
  template <typename T>
  std::vector<float> ToFloats(const std::vector<T>& ints) {
    std::vector<float> floats(ints.size());
    std::transform(std::begin(ints), std::end(ints), std::begin(floats), [](const T& val) {
      return (float) val;
    });
    return floats;
  }
}



void run_underscore_test() {

  const auto& array = _.array(4,3,4,5,6,7);
  const auto& vector = array | _.to_vector;
  
  // checked_delete
  {
    auto ptr_a = new int;
    auto ptr_b = new std::vector<int>(1000);
    _.checked_delete >> ptr_a >> ptr_b;
    TEST(ptr_a, nullptr);
    TEST(ptr_b, nullptr);
    LOGV(ptr_a | _.type_name);
    LOGV(ptr_b | _.type_name);
  }

  

  // Misc
  {  
    TEST( array | _.get<2>() , 4 )
    TEST( array | _.third , 4 );
    TEST( array | _.end | _.prev(2) | _.deref , 6 );
    TEST( array | _.begin | _.next | _.deref , 3 );
    TEST( array.data() , array | _.first | _.adressof );
    TEST( array | _.begin | *_ , array | _.begin | _.deref );
    TEST( array | _.front | _.adressof , array | _.front | &_ );
    TEST( vector | _.pushed_back(8) | _.back() , 8 );  
    TEST( 7 | _.is_any_of(1,2,3,4), false);
    TEST( 7 | _.is_any_of(1,2,7,4), true);
  }
  
  // min, max, mean, median, minmax
  {
    TEST( array | _.min_value , 3 );
    TEST( array | _.max_value , 7 );
    TEST( array | _.min_value , array | _.min_element | _.deref );
    TEST( array | _.max_value , array | _.max_element | _.deref );
    TEST( array | _.minmax_value | _.first, 3 );
    TEST( array | _.minmax_value | _.second, 7 );
  }
  
  // algorithms
  {
    TEST( array | _.equal(vector), true);
    TEST( array | _.replaced(4, 8) , _.array(8,3,8,5,6,7) );
    TEST( array | _.for_each([](int& val){val*=2;}) , _.array(8,6,8,10,12,14) );
    TEST( array | _.accumulate , std::accumulate(array.begin(), array.end(), 0) );
    TEST( array | _.sorted | _.is_sorted, true );
    TEST( array | _.rotated(3) , _.array(5,6,7,4,3,4) );
    TEST( array | _.sorted | _.random_shuffled | _.is_sorted , false );
    TEST( array | _.count_if([](int val) {return true;}) , array | _.size );
    TEST( array | _.count(4) , 2 );
  }

  // Pipe
  {
    const auto& ToFloats = [](std::vector<int> ints) -> std::vector<float> { 
      std::vector<float> floats(ints.size());
      std::transform(std::begin(ints), std::end(ints), std::begin(floats), [](int val) {
        return (float) val;
      });
      return floats;
    };
    
    TEST( std::vector<int>(3, 3) | _.pipe(ToFloats), std::vector<float>(3,3));
    TEST( std::vector<int>(3, 3) | _(ToFloats), std::vector<float>(3,3));
    TEST( std::vector<int>(3, 3) | _.pipe(UTDetail::ToFloats<int>), std::vector<float>(3,3));
    TEST( std::vector<int>(3, 3) | _(UTDetail::ToFloats<int>), std::vector<float>(3,3));
  }
  
  // all_of, any_of, none_of
  {
    TEST( _.array(1,2,3) | _.any_of_equal(2), true);
    TEST( _.array(1,2,3) | _.any_of_equal(4), false);
    TEST( _.array(3,3,3) | _.all_of_equal(3), true);
    TEST( _.array(3,3,2) | _.all_of_equal(3), false);
    TEST( _.array(3,3,3) | _.none_of_equal(4), true);
    TEST( _.array(3,3,3) | _.none_of_equal(3), false);
  }
  
  // Find
  {
    TEST( array | _.find_first_of(_.array(6, 5)) | _.deref, 5 );
    TEST( array | _.find_last_of(_.array(6, 5)) | _.deref, 6 );
    TEST( array | _.find_first_not_of(_.array(3, 4)) | _.deref, 5 );
    TEST( array | _.find_last_not_of(_.array(6, 7)) | _.deref, 5 );
    TEST( array | _.find_index(3) , 1 );
    TEST( array | _.find_index_if([](int val){return val >= 6;}) , 4 );
  }
  
  // Erase
  {
    TEST( vector | _.erase_all_unstable(4) | _.contains(4), false);
    TEST( vector | _.erase_all(4) , _.array(3,5,6,7) | _.to_vector);
    TEST( vector | _.erase_all_if([](int val){ return val % 2 == 1;}) , _.array(4,4,6) | _.to_vector);
    TEST( _.array(1,2,2,3,4,4) | _.to_vector | _.erase_consecutive_duplicates, _.array(1,2,3,4) | _.to_vector);
  }
  
  // Move container
  {
    auto v = vector;
    auto adr = v | _.front | _.adressof;
    auto sorted = v | _.move | _.sorted;
    TEST( v | _.empty , true  );
    TEST( sorted | _.empty , false  );
    TEST( sorted | _.front | _.adressof , adr );
  }
  
  // Mutate container
  {
    auto v = vector;
    TEST( v | _.is_sorted , false );
    v | _.mutate  | _.random_shuffled |  _.sorted | _.popped_back;
    TEST( v | _.is_sorted , true );
    TEST( v | _.size , vector.size() - 1);
  }
  
  // Math
  {
    TEST( 1.0 | _.reinterval(0.0, 10.0, 0.0, 100.0) , 10.0 );
    TEST( 0.6 | _.round, 1.0);
    TEST( 1.49 | _.round, 1.0);
  }
  
  // transformed
  {
    std::vector<float> vec_transformed;
    for(const auto& val: vector)
      vec_transformed.push_back(val*val | _.to<float>());
    auto functor = [](int x) -> float { return (float)(x*x); };
    // transformed
    TEST(vector | _.transformed(functor), vec_transformed);
    // transformed_to
    std::vector<float> vvvv(8);
    auto t = UnderscoreTags::TransformedToTag();
    auto t2arg = t(functor, vvvv);
    TEST(array | t2arg, vec_transformed);
    //TEST(array | _.transformed_to( functor, vvvv ), vec_transformed );
    //TEST(array | _.transformed_to( functor, std::vector<float>() ), vec_transformed );
  }
  
  // String handling
  {
    {
      const auto a = _.str << 1 << "2";
      const auto& b = _.str << 1 << "2";
      const std::string c = _.str << 1 << "2";
      const std::string& d = _.str << 1 << "2";
      TEST(a, std::string("12"));
      TEST(b, std::string("12"));
      TEST(c, std::string("12"));
      TEST(d, std::string("12"));
    }
    const std::string& str = _.str << 1 << 2 << 3 << 4 << 5 << 4 << 3 << 2 << 1;
    TEST( str , "123454321" );
    const auto& rep1 = str | _.replace_substring("23", "0");
    TEST( rep1, "10454321" );
    const auto& rep2 = str | _.replace_substring("23", _.str << "023");
    TEST( rep2, "1023454321" );
    const auto& rep3 = str | _.replace_substring(23 | _.to_string, "023");
    TEST( rep3, "1023454321" );
    TEST( std::string("  1 2 3 4  ") | _.trim , std::string("1 2 3 4"));
    TEST( std::string("  1 2 3 4  ") | _.trim_left , std::string("1 2 3 4  "));
    TEST( std::string("  1 2 3 4  ") | _.trim_right , std::string("  1 2 3 4"));
    TEST( 1 | _.to_string, std::string("1"));
    TEST( "hej" | _.to_string, std::string("hej"));
    TEST( _.array(1,2,3) | _.to_vector | _.to_string, std::string("[1, 2, 3]"));
    TEST( "ABC" | _.to_string | _.to_lower, "abc");
    TEST( "abc" | _.to_string | _.to_upper, "ABC");
    
    // Tokenize
    std::string sstr = "123454321";
    const auto& tokens1 = sstr | _.tokenize_string(std::string("43"));
    TEST(tokens1.size(), 3);
    TEST(tokens1.at(0), std::string("12"));
    TEST(tokens1.at(1), std::string("5"));
    TEST(tokens1.at(2), std::string("21"));
    const auto& tokens2 = _.str << "11131" | _.tokenize_string(_.str << "13");
    TEST(tokens2.size(), 0);
    const auto& tokens3 = _.str << "11131" | _.tokenize_string(_.str << "1");
    TEST(tokens3.size(), 1);
    
    // 
    std::wstring wstrTest = L"ABC";
    std::string strTest = "ABC";
    TEST(_.str << wstrTest, _.str << "ABC");
    TEST(_.wstr << strTest, _.wstr << L"abc");
    // TOdo more
  }
  
  

  // wstring handling
  /*
  {
    const std::wstring str = _.wstr() + 1 + 2 + 3 + 4 + 5 + 4 + 3 + 2 + 1;
    TEST( str , L"123454321" );
    //const auto& rep1 = str | _.replace_sequence(23|_.to_string, 0|_.to_string);
    const auto& rep1 = str | _.replace_sequence(L"23", L"0");
    TEST( rep1, L"10454321" );
    //const auto& rep2 = str | _.replace_sequence(23|_.to_string, std::string("023"));
    //const auto& rep2 = str | _.replace_sequence("23", "023");
    //TEST( rep2, "1023454321" );
    TEST( std::wstring(L"  1 2 3 4  ") | _.trim , std::wstring(L"1 2 3 4"));
    TEST( std::wstring(L"  1 2 3 4  ") | _.trim_left , std::wstring(L"1 2 3 4  "));
    TEST( std::wstring(L"  1 2 3 4  ") | _.trim_right , std::wstring(L"  1 2 3 4"));
    TEST( 1 | _.to_wstring, std::wstring(L"1"));
    TEST( L"hej" | _.to_wstring, std::wstring(L"hej"));
    TEST( _.array(1,2,3) | _.to_vector | _.to_wstring, std::wstring(L"[1, 2, 3]"));
    TEST( L"ABC" | _.to_wstring | _.to_lower, L"abc" | _.to_wstring);
    TEST( L"abc" | _.to_wstring | _.to_upper, L"ABC" | _.to_wstring);
  }  
  */
  
  

  const auto& x = vector | _.to_vector | _.at(0);
  const auto y = vector | _.to_vector | _.at(0);
  //auto z = array | _.to_vector | _.min_element;
  //LOG(*z);
  LOGV(x);
  LOGV(y);
  auto arr = array;
  auto z = arr | _.mutate | _.sorted | _.min_element;
  LOG(*z);
  
  auto v = vector;
  //auto w = vector | _.transformed([](int x){ return (float)(x*x); });

  
  // views
  /*
  {
    // Conditional
    auto Filter = [](int val) { return val > 3; };
    //TEST( array | utl::filter_view([](int val) { return val > 3; })  | _.to_vector, _.array(4,4,5,6,7) | _.to_vector);
    //TEST( array | _.to_vector | utl::filter_view([](int val) { return val > 3; }) | _.filled(9) | _.to_vector, _.array(9,9,9,9,9) | _.to_vector);
    
    TEST (
      array 
      | utl::sub_view(1) 
      | utl::filter_view([](int val) {return val > 3;}) 
      | _.to_deque 
      , 
      _.array(4,5,6,7) | _.to_deque );
    
    // Subview
    TEST( array | utl::sub_view(1) | _.to_deque , _.array(3,4,5,6,7) | _.to_deque );
    TEST( vector | _.mutate | utl::sub_view(1) | _.filled(1) | _.to_deque , _.array(1,1,1,1,1) | _.to_deque );
    //TEST( array | utl::sub_view(1) | utl::sub_view(1) | _.to_deque , _.array(4,5,6,7) | _.to_deque );
    //TEST( array | utl::sub_view(2, 3) | _.to_list , _.array(4,5,6) | _.to_list );
  }
  */
  
}
