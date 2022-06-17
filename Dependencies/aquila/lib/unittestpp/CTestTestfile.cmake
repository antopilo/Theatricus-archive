# CMake generated Testfile for 
# Source directory: C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp
# Build directory: C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(TestUnitTest++ "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/Debug/TestUnitTest++.exe")
  set_tests_properties(TestUnitTest++ PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;48;add_test;C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(TestUnitTest++ "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/Release/TestUnitTest++.exe")
  set_tests_properties(TestUnitTest++ PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;48;add_test;C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(TestUnitTest++ "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/MinSizeRel/TestUnitTest++.exe")
  set_tests_properties(TestUnitTest++ PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;48;add_test;C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(TestUnitTest++ "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/RelWithDebInfo/TestUnitTest++.exe")
  set_tests_properties(TestUnitTest++ PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;48;add_test;C:/Users/Antoine/Desktop/Theatricus/Dependencies/aquila/lib/unittestpp/CMakeLists.txt;0;")
else()
  add_test(TestUnitTest++ NOT_AVAILABLE)
endif()
