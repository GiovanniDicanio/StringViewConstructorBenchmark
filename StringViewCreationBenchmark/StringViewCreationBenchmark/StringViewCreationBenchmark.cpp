//////////////////////////////////////////////////////////////////////////
//
// String View Creation Benchmark -- by Giovanni Dicanio
//                                   https://giodicanio.com/
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

inline [[nodiscard]] long long Counter() noexcept
{
    LARGE_INTEGER perfCounter;
    ::QueryPerformanceCounter(&perfCounter);
    return perfCounter.QuadPart;
}

inline [[nodiscard]] long long Frequency() noexcept
{
    LARGE_INTEGER perfFrequency;
    ::QueryPerformanceFrequency(&perfFrequency);
    return perfFrequency.QuadPart;
}

inline void PrintTime(const long long start, const long long finish, const char* const s)
{
    std::cout << s << ": " << (finish - start) * 1000.0 / Frequency() << " ms \n";
}


//------------------------------------------------------------------------
// Helper function that *efficiently* creates a wstring_view to a CString
//------------------------------------------------------------------------
inline [[nodiscard]] std::wstring_view AsView(const CString& str)
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

#ifdef _DEBUG
        static const int kIterCount = 20;
#else
        static const int kIterCount = 400 * 1000;
#endif // _DEBUG

        for (int i = 0; i < kIterCount; ++i)
        {
            for (const auto& s : lorem)
            {
#ifdef TEST_TINY_STRINGS
                // Tiny strings (SSO enabled for standard strings)
                UNREFERENCED_PARAMETER(s);
                v.push_back(CString(L"#") + std::to_wstring(i).c_str());
#else
                v.push_back(s + L" (#" + std::to_wstring(i).c_str() + L")");
#endif
            }
        }

#define USE_RANDOM_DEVICE_FOR_SEED_INIT
#ifdef USE_RANDOM_DEVICE_FOR_SEED_INIT
        std::random_device rd;
        std::mt19937 prng(rd());
#else
        std::mt19937 prng(1980);
#endif // USE_RANDOM_DEVICE_FOR_SEED_INIT

        std::shuffle(v.begin(), v.end(), prng);

        return v;
    }();

#ifdef _DEBUG
    // A few strings are tested in debug mode
    cout << "String count: " << shuffled.size() << "\n\n";
#else
    cout << "String count: " << (shuffled.size() / 1000) << "k \n\n";
#endif // _DEBUG

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
    PrintTime(start, finish, "wstring_view(CString::GetString())");

    start = Counter();
    vector<wstring_view> v2;
    for (const auto& s : shuffled)
    {
        // Use the AsView() O(1)-length helper function
        v2.push_back(AsView(s));
    }
    finish = Counter();
    PrintTime(start, finish, "AsView(efficient with O(1) length)");
}
