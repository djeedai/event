event
=====

Simple event system inspired from C#

The goal was to support the syntax
  event += handler;
with the minimum explicit typecast or other conversion.

A drawback is that if the event is a member of a class:
- either it is public, and the syntax works but the event can be fired from outside the class;
- or it is private/protected, and then the syntax does not work anymore outside the class.
Your choice.

There are 2 versions:
- The old-style C++98 version with a single EventArgs object containing all arguments.
- The new-style C++11 version which uses variadic templates to support a random number of arguments of any type.

Compilation:
g++ -o main-cxx98 main_cxx98.cpp
g++ -o main-cxx11 -std=c++11 main_cxx11.cpp

