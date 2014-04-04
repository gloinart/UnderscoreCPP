UnderscoreCPP
=============

Underscore.CPP - C++ and STL the elegant way!<br/>
<br/>
<br/>
Code examples:<br/>
<br/>
Create containers<br/>
    auto array = _.array(1,2,3,4,5);<br/>
    auto vector = _.array(1,2,3,4,5) | _.to_vector;<br/>
    auto deque = _.array(1,2,3,4,5) | _.to_deque;<br/>
<br/>
Algorithms<br/>
  // vector is unchanged, result is placed in modified<br/>
  auto modified = vector | _.random_shuffle | _.sort | _.erase_all(3);<br/>
  // vector is changed<br/>
  _[vector] | _.random_shuffle | _.sort | _.erase_all(3);<br/>
  <br/>
Operator direct<br/>
<br/>
String handling<br/>
  std::string str = _.str << "Test" << std::vector<int>(3,3) << ":" << 7.8;<br/>
  std::string str = _.str << "10101" | _.replace_substring("0", "00");<br/>
