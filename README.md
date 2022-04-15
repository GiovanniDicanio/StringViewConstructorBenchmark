# StringViewConstructorBenchmark
Benchmark comparing the performance of different string_view constructors

This benchmark compares two `std::wstring_view` overloaded constructors: the one that takes a simple C-style null-terminated string pointer vs. the one that takes a string character pointer and a size.

Benchmark C++ source code available [here](StringViewCreationBenchmark/StringViewCreationBenchmark/StringViewCreationBenchmark.cpp).

Code built with Visual Studio 2019 with C++17 features enabled.

The related blog post can be found [here](https://blogs.msmvps.com/gdicanio/2022/04/15/adventures-with-string_view-optimizing-code-with-o1-operations/).
