#include <iostream>
#include <assert.h>
#include <vector>

#include "qformat.hpp"

class FixedPointArithmeticTests
{
public:
    FixedPointArithmeticTests() = default;
    ~FixedPointArithmeticTests() = default;

    void Run()
    {
        TestFloatCast();
        TestAddition();
        TestSubtraction();
        TestMultiplication();
        TestDivision();
    }

    void TestFloatCast()
    {
        std::cout << "Run Float casting tests" << std::endl;

        constexpr auto test_size = 200;
        const float Resolution = std::powf(2, -QFormat<Q15>::FraqBitN);

        std::vector<float> stim2;
        stim2.reserve(test_size);
        std::generate_n(std::back_inserter(stim2), test_size, [i = -1.0f]() mutable
                        { const auto ret = i; i += 0.01f; return ret; });

        for (const auto &i : stim2)
        {
            auto fpx = QFormat<Q15>(i);
            auto fx = fpx.GetFloat();
            auto diff = fx - i;
            assert((diff < Resolution) && (-diff < Resolution));
        }
    }

    void TestAddition()
    {
        std::cout << "Run Addition tests" << std::endl;

        constexpr auto test_size = 8;
        constexpr auto scale = 0.125f;

        std::vector<int> stim, stim2;
        stim.reserve(test_size);
        stim2.reserve(test_size * 2);
        std::generate_n(std::back_inserter(stim), test_size + 1, [i = -4]() mutable
                        { return i++; });
        std::generate_n(std::back_inserter(stim2), test_size * 2 + 1, [i = -8]() mutable
                        { return i++; });

        for (const auto &x : stim)
        {
            QFormat<Q15> fpx{x * scale};
            for (const auto &y : stim)
            {
                QFormat<Q15> fpy{y * scale};
                QFormat<Q15> fpa{(x + y) * scale};

                assert(fpa == (fpx + fpy));
                assert(fpa == (fpy + fpx));

                auto tmp = fpx;
                tmp += fpy;
                assert(fpa == tmp);
            }
        }
    }

    void TestSubtraction()
    {
        std::cout << "Run Subtraction tests" << std::endl;

        constexpr auto test_size = 25;
        constexpr auto scale = 1.0f / 32.0f;

        std::vector<int> stim, stim2;
        stim.reserve(test_size);
        stim2.reserve(test_size);
        // range(-16, 8)
        std::generate_n(std::back_inserter(stim), test_size, [i = -16]() mutable
                        { return i++; });
        // range(-8, 16)
        std::generate_n(std::back_inserter(stim2), test_size, [i = -8]() mutable
                        { return i++; });

        for (const auto &x : stim)
        {
            QFormat<Q15> fpx{x * scale};
            for (const auto &y : stim2)
            {
                QFormat<Q15> fpy{y * scale};
                QFormat<Q15> fpa{(x - y) * scale};

                assert(fpa == (fpx - fpy));
                assert(-fpa == (fpy - fpx));
                assert(((x - y) * scale) == (float(fpx - fpy)));

                auto tmp = fpx;
                tmp -= fpy;
                assert(fpa == tmp);
            }
        }
    }

    void TestMultiplication()
    {
        std::cout << "Run Multiplication tests" << std::endl;

        constexpr auto test_size = 25;
        constexpr auto scale = 1.0f / 32.0f;
        auto scale2 = scale * scale;

        std::vector<int> stim, stim2;
        stim.reserve(test_size);
        stim2.reserve(test_size);
        // range(-16, 8)
        std::generate_n(std::back_inserter(stim), test_size, [i = -16]() mutable
                        { return i++; });
        // range(-8, 16)
        std::generate_n(std::back_inserter(stim2), test_size, [i = -8]() mutable
                        { return i++; });

        for (const auto &x : stim)
        {
            QFormat<Q15> fpx{x * scale};
            for (const auto &y : stim2)
            {
                QFormat<Q15> fpy{y * scale};
                QFormat<Q15> fpa{(x * y) * scale2};

                assert(fpa == (fpx * fpy));
                assert(fpa == (fpy * fpx));
                assert(((x * y) * scale2) == (float(fpx * fpy)));

                auto tmp = fpx;
                tmp *= fpy;
                assert(fpa == tmp);
            }
        }
    }

    void TestDivision()
    {
        std::cout << "Run Division tests" << std::endl;

        constexpr auto test_size = 32;
        constexpr auto scale = 1.0f / 32.0f;
        auto scale2 = scale * scale;
        const float Resolution = std::powf(2, -QFormat<Q15>::FraqBitN);

        std::vector<int> stim, stim2;
        stim.reserve(test_size * 2 + 1);
        stim2.reserve(test_size + 1);
        // range(-16, 8)
        std::generate_n(std::back_inserter(stim), test_size * 2 + 1, [i = -32]() mutable
                        { return i++; });
        // range(-8, 16)
        std::generate_n(std::back_inserter(stim2), test_size + 1, [i = -16]() mutable
                        { return i++; });

        for (const auto &a : stim)
        {
            if (a == 0)
                continue;

            QFormat<Q15> fpa{a * scale};
            for (const auto &y : stim2)
            {
                if (y == 0)
                    continue;

                QFormat<Q15> fpy{y * scale};
                QFormat<Q15> fpx{(y * a) * scale2};

                // assert(fpa == (fpx / fpy));
                {
                    auto temp = fpx / fpy;
                    auto diff = fpa.GetFloat() - temp.GetFloat();
                    assert((diff < Resolution) && (-diff < Resolution));
                }

                // assert((QFormat<Q15>{1.0f} / fpa) == (fpy / fpx));
                {
                    auto temp = QFormat<Q15>{1.0f} / fpa;
                    auto temp1 = fpy / fpx;
                    auto diff = temp.GetFloat() - temp1.GetFloat();
                    assert((diff < Resolution) && (-diff < Resolution));
                }

                // assert((a * scale) == float(fpx / fpy));
                {
                    auto temp = fpx / fpy;
                    auto diff = (a * scale) - temp.GetFloat();
                    assert((diff < Resolution) && (-diff < Resolution));
                }

                auto tmp = fpx;
                tmp /= fpy;
                // assert(fpa == tmp);
                {
                    auto diff = tmp.GetFloat() - fpa.GetFloat();
                    assert((diff < Resolution) && (-diff < Resolution));
                }
            }
        }
    }
};

int main(int, char **)
{
    FixedPointArithmeticTests tests;
    tests.Run();
    std::cout << "All tests passed" << std::endl;
}
