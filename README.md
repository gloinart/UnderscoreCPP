UnderscoreCPP
=============

Underscore.CPP

Elegant C++

Code examples:

Create containers<br/>
  auto array = _.array(1,2,3,4,5);
  auto vector = _.array(1,2,3,4,5) | _.to_vector;
  auto deque = _.array(1,2,3,4,5) | _.to_deque;

Algorithms
  // vector is unchanged, result is placed in modified
  auto modified = vector | _.random_shuffle | _.sort | _.erase_all(3);
  // vector is changed
  _[vector] | _.random_shuffle | _.sort | _.erase_all(3);
  
Operator direct

String handling
  std::string str = _.str << "Test" << std::vector<int>(3,3) << ":" << 7.8;
  std::string str = _.str << "10101" | _.replace_substring("0", "00");
