//////////////////////////////////////////////////////////////////////////
//
// String View Creation Benchmark -- by Giovanni Dicanio
//
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------

#include <windows.h>            // Windows SDK
#include <atlstr.h>             // CString

#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <vector>


//------------------------------------------------------------------------
// Performance Counter Helpers
//------------------------------------------------------------------------

inline long long Counter() noexcept
{
    LARGE_INTEGER perfCounter;
    ::QueryPerformanceCounter(&perfCounter);
    return perfCounter.QuadPart;
}

inline long long Frequency() noexcept
{
    LARGE_INTEGER perfFrequency;
    ::QueryPerformanceFrequency(&perfFrequency);
    return perfFrequency.QuadPart;
}

void PrintTime(const long long start, const long long finish, const char* const s)
{
    std::cout << s << ": " << (finish - start) * 1000.0 / Frequency() << " ms \n";
}


//------------------------------------------------------------------------
// Helper function that *efficiently* creates a wstring_view to a CString
//------------------------------------------------------------------------
inline std::wstring_view AsView(const CString& str)
{
    return { str.GetString(), static_cast<size_t>(str.GetLength()) };
}


//------------------------------------------------------------------------
// Benchmark code
//------------------------------------------------------------------------
int main()
{
    using std::cout;
    using std::vector;
    using std::wstring_view;

    cout << "\n*** String View Creation Benchmark -- by Giovanni Dicanio *** \n\n";

    const auto shuffled = []() -> vector<CString> {
        const CString lorem[] = {
            L"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.",
            L"Maecenas porttitor congue massa. Fusce posuere, magna sed",
            L"pulvinar ultricies, purus lectus malesuada libero,",
            L"sit amet commodo magna eros quis urna.",
            L"Nunc viverra imperdiet enim. Fusce est. Vivamus a tellus.",
            L"Pellentesque habitant morbi tristique senectus et netus et",
            L"malesuada fames ac turpis egestas. Proin pharetra nonummy pede.",
            L"Mauris et orci. [*** add more chars to prevent SSO ***]"
        };

        vector<CString> v;

        for (int i = 0; i < (400 * 1000); ++i)
        {
            for (const auto& s : lorem)
            {
#ifdef TEST_TINY_STRINGS
                // Tiny strings
                UNREFERENCED_PARAMETER(s);
                v.push_back(CString(L"#") + std::to_wstring(i).c_str());
#else
                v.push_back(s + L" (#" + std::to_wstring(i).c_str() + L")");
#endif
            }
        }

        std::mt19937 prng(1980);
        std::shuffle(v.begin(), v.end(), prng);

        return v;
    }();

    cout << "String count: " << (shuffled.size() / 1000) << "k \n\n";

    long long start = 0;
    long long finish = 0;


    //
    // Measure creation times
    //

    start = Counter();
    vector<wstring_view> v1;
    for (const auto& s : shuffled)
    {
        // Invoke the wstring_view one-parameter constructor
        // that takes a C-style null-terminated string
        v1.push_back(s.GetString());
    }
    finish = Counter();
    PrintTime(start, finish, "wstring_view(GetString())");

    start = Counter();
    vector<wstring_view> v2;
    for (const auto& s : shuffled)
    {
        // Use the AsView() O(1)-length helper function
        v2.push_back(AsView(s));
    }
    finish = Counter();
    PrintTime(start, finish, "AsView");
}
