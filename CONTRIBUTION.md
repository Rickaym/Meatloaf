## Index
1. **[Project Layout](#project-layout)**
1. **[Style Guide](#style-guide)**


# Project Layout

`include/` ::= a folder that contains the distributed .h (header files) 

`src` ::= a folder that contains source code or implementation files .cpp

# Style Guide

Google's CPP standards should be followed, here's a gist.

## C++ Version

1. Code should target C++17

## Header Files

1. Every `.cpp` code should have an associated `.h` file.

2. Header files should be self-contained (compiles on their own).

3. Use `#pragma once` at the beginning of the header files to prevent multiple inclusion.

4. Avoid forward declarations as much as possible.

5. Header files should only contain declarations. Define functions inline only when they are small, 10 lines or less.

## Naming

1. Use names that describe the purpose or intent of the object. 
Do not worry about saving horizontal space as it is far more important to make your code immediately understandable by a new reader. 
Minimize the use of abbreviations that would likely be unknown to someone outside your project (especially acronyms and initialisms). 
Do not abbreviate by deleting letters within a word. As a rule of thumb, an abbreviation is probably OK if it's listed in Wikipedia. 
Generally speaking, descriptiveness should be proportional to the name's scope of visibility. 
For example, n may be a fine name within a 5-line function, but within the scope of a class, it's likely too vague.

2. Type names start with a capital letter and have a capital letter for each new word, with no underscores: MyExcitingClass, MyExcitingEnum.
The names of all types — classes, structs, type aliases, enums, and type template parameters — have the same naming convention. 
Type names should start with a capital letter and have a capital letter for each new word.

3.The names of variables (including function parameters) and data members are all lowercase, 
with underscores between words. Data members of classes (but not structs) additionally have trailing underscores. 
For instance: a_local_variable, a_struct_data_member, a_class_data_member_.

4. Private static and non-static, are named like ordinary nonmember variables, but with a trailing underscore whereas public static and non-static members
are named exactly like ordinary nonmember variables.

5. Variables declared constexpr or const, and whose value is fixed for the duration of the program, are named with a leading "k" followed by mixed case. 
Underscores can be used as separators in the rare cases where capitalization cannot be used for separation.
