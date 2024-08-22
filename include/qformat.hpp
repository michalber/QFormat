#ifndef __QFORMAT_HPP__
#define __QFORMAT_HPP__

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <type_traits>

#if ((defined(_MSVC_LANG) && _MSVC_LANG < 201703L) || __cplusplus < 201703L)
#warning "QFormat is working only on C++17 and above"
#endif

template<typename Integer,
         typename IntermedType,
         std::size_t Q,
         std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
class QType {
  public:
    typedef Integer IntType;
    typedef IntermedType IntermediateType;

    inline static constexpr uint32_t N              = std::numeric_limits<std::make_unsigned_t<Integer>>::digits;
    inline static constexpr uint32_t SignCorrection = std::is_signed_v<Integer>;
    inline static constexpr uint32_t Roundup        = 1 << (N - 1);
    inline static constexpr uint32_t FraqBitN       = N - Q;
    inline static constexpr uint32_t IntBitN        = Q;
    inline static constexpr uint32_t Scale          = IntermediateType(1) << FraqBitN;

  public:
    QType()  = default;
    ~QType() = default;

    bool Validate(Integer scaledVal) {
        constexpr auto threshold = 1 << (N - 1);
        return scaledVal >= threshold or scaledVal < -threshold;
    }

    template<class T1, class T2, std::size_t Q2> constexpr inline bool operator==(const QType<T1, T2, Q2> &rhs) {
        return (this->FraqBitN == rhs.FraqBitN) and (this->IntBitN == rhs.IntBitN);
    }

    template<class T1, class T2, std::size_t Q2> constexpr inline bool operator!=(const QType<T1, T2, Q2> &rhs) {
        return !(*this == rhs);
    }
};

using Q15 = QType<int16_t, int32_t, 1>;
using Q29 = QType<int32_t, int64_t, 3>;
using Q31 = QType<int32_t, int64_t, 1>;

using Q15U  = QType<uint16_t, uint32_t, 1>;
using Q29U  = QType<uint32_t, uint64_t, 3>;
using Q31SU = QType<uint32_t, uint64_t, 1>;


template<class Type, bool EnableRounding = true> class QFormat : public Type {
  public:
    constexpr QFormat() : mQformatInt{ 0 } {
#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = 0.0f;
#endif
    }

    template<typename T, typename std::enable_if_t<std::is_integral<T>::value, bool> = true>
    constexpr inline explicit QFormat(T val) noexcept
        : mQformatInt(static_cast<typename Type::IntType>(val * Type::Scale)) {
#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(val);
#endif
    }

    //     constexpr QFormat(typename Type::IntType val) : mQformatInt{ val } {
    // #ifdef QFORMAT_USE_FLOAT_VAL
    //         mFloatVal = ToFloat(val);
    // #endif
    //     }

    template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    constexpr inline explicit QFormat(T val) noexcept
        : mQformatInt(static_cast<typename Type::IntType>(
              (EnableRounding) ? (val >= 0.0) ? (val * Type::Scale + T{ 0.5 }) : (val * Type::Scale - T{ 0.5 })
                               : (val * Type::Scale))) {
#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif
    }


    //     constexpr QFormat(float val) : mQformatInt{ ToQFormat(val) } {
    // #ifdef QFORMAT_USE_FLOAT_VAL
    //         mFloatVal = ToFloat(mQformatInt);
    // #endif
    //     }

    // Constructor for converting between Q formats
    template<class TypeT2> constexpr QFormat(const QFormat<TypeT2> &source) {
        const int32_t bitInc = this->FraqBitN - source.FraqBitN;
        if (bitInc == 0) {
            mQformatInt = source.mQformatInt;
        } else if (bitInc > 0) {
            auto newVal = source.mQformatInt << bitInc;
            if (source.mQformatInt > 0) {
                newVal |= 1 << (bitInc - 1);
            } else {
                newVal |= ((1 << (bitInc - 1)) - 1);
            }

            mQformatInt = newVal;
        } else {
            mQformatInt = source.mQformatInt >> -bitInc;
        }

#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif
    }

    ~QFormat() = default;

    constexpr inline float GetFloat() { return ToFloat(mQformatInt); }

    template<class T1> constexpr inline bool operator==(const QFormat<T1> &rhs) {
        if (std::is_same<Type, T1>::value) { return (this->mQformatInt == rhs.mQformatInt); }

        return false;
    }

    template<class T1> constexpr inline bool operator!=(const QFormat<T1> &rhs) { return !(*this == rhs); }

    //=============================================================================
    constexpr inline QFormat &operator+=(const QFormat &rhs) {
        mQformatInt += rhs.mQformatInt;
#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif// QFORMAT_USE_FLOAT_VAL

        return *this;
    }

    friend constexpr inline QFormat operator+(QFormat lhs, const QFormat &rhs) {
        lhs += rhs;
        return lhs;
    }

    template<class T2> friend constexpr inline QFormat operator+(QFormat lhs, const QFormat<T2> &rhs) {
        lhs += QFormat{ rhs };
        return lhs;
    }
    //=============================================================================
    constexpr inline QFormat<Type> operator-() {
        QFormat<Type> ret = *this;
        ret.mQformatInt   = -mQformatInt;
        return ret;
    }

    constexpr inline QFormat &operator-=(const QFormat &rhs) {
        mQformatInt -= rhs.mQformatInt;
#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif// QFORMAT_USE_FLOAT_VAL

        return *this;
    }

    friend constexpr inline QFormat operator-(QFormat lhs, const QFormat &rhs) {
        lhs -= rhs;
        return lhs;
    }

    template<class T2> friend constexpr inline QFormat operator-(QFormat lhs, const QFormat<T2> &rhs) {
        lhs -= QFormat{ rhs };
        return lhs;
    }
    //=============================================================================
    constexpr inline QFormat &operator*=(const QFormat &rhs) {
        // typename Type::IntType    result;
        // decltype(result * result) temp;

        // temp = static_cast<decltype(temp)>(mQformatInt) * static_cast<decltype(temp)>(rhs.mQformatInt);

        // temp += (1 << (Type::FraqBitN - 1));

        // result = Saturate(temp >> Type::FraqBitN);

        // mQformatInt = result;

        // if (EnableRounding) {
        //     // Normal fixed-point multiplication is: x * y / 2**FractionBits.
        //     // To correctly round the last bit in the result, we need one more bit of information.
        //     // We do this by multiplying by two before dividing and adding the LSB to the real result.
        //     auto value = (static_cast<decltype(mQformatInt * mQformatInt)>(mQformatInt) * rhs.mQformatInt) / (Type::Scale / 2);
        //     mQformatInt    = static_cast<typename Type::IntType>((value / 2) + (value % 2));
        // } else {
            auto value = (static_cast<decltype(mQformatInt * mQformatInt)>(mQformatInt) * rhs.mQformatInt) / Type::Scale;
            mQformatInt    = Saturate(static_cast<typename Type::IntType>(value));
        // }

#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif// QFORMAT_USE_FLOAT_VAL

        return *this;
    }

    friend constexpr inline QFormat operator*(QFormat lhs, const QFormat &rhs) {
        lhs *= rhs;
        return lhs;
    }

    template<class T2> friend constexpr inline QFormat operator*(QFormat lhs, const QFormat<T2> &rhs) {
        lhs *= QFormat{ rhs };
        return lhs;
    }
    //=============================================================================
    constexpr inline QFormat &operator/=(const QFormat &rhs) {
        /* pre-multiply by the base (Upscale to Q16 so that the result will be in Q8 format) */
        decltype(mQformatInt * mQformatInt) temp = mQformatInt << Type::FraqBitN;
        /* Rounding: mid values are rounded up (down for negative values). */
        /* OR compare most significant bits i.e. if (((temp >> 31) & 1) == ((b >> 15) & 1)) */
        if ((temp >= 0 && rhs.mQformatInt >= 0) || (temp < 0 && rhs.mQformatInt < 0)) {
            temp += rhs.mQformatInt >> 1; /* OR shift 1 bit i.e. temp += (b >> 1); */
        } else {
            temp -= rhs.mQformatInt >> 1; /* OR shift 1 bit i.e. temp -= (b >> 1); */
        }
        mQformatInt = static_cast<int16_t>(temp / rhs.mQformatInt);

#ifdef QFORMAT_USE_FLOAT_VAL
        mFloatVal = ToFloat(mQformatInt);
#endif// QFORMAT_USE_FLOAT_VAL

        return *this;
    }

    friend constexpr inline QFormat operator/(QFormat lhs, const QFormat &rhs) {
        lhs /= rhs;
        return lhs;
    }

    template<class T2> friend constexpr inline QFormat operator/(QFormat lhs, const QFormat<T2> &rhs) {
        lhs /= QFormat{ rhs };
        return lhs;
    }

    operator float() { return ToFloat(mQformatInt); }

    operator const float() const { return ToFloat(mQformatInt); }

    //=============================================================
    constexpr inline typename Type::IntType GetLsbQBitsSet() {
        return (Type::N == 32) ? 0xFFFFFFFF
                               : ((static_cast<std::make_unsigned_t<typename Type::IntType>>(1) << Type::FraqBitN) - 1);
    }

    constexpr inline typename Type::IntType GetIntPart() { return mQformatInt >> Type::FraqBitN; }

    constexpr inline typename Type::IntType GetFraqPart() { return mQformatInt & GetLsbQBitsSet(); }

    constexpr inline typename Type::IntType Saturate(typename Type::IntermediateType n) {
        if (n > std::numeric_limits<typename Type::IntType>::max()) {
            return std::numeric_limits<typename Type::IntType>::max();
        } else if (n < std::numeric_limits<typename Type::IntType>::min()) {
            return std::numeric_limits<typename Type::IntType>::min();
        }

        return static_cast<typename Type::IntType>(n);
    }

  private:
    constexpr inline typename Type::IntType ToQFormat(float n) const {
        return static_cast<typename Type::IntType>(std::floor(n * Type::Scale));
    }

    constexpr inline float ToFloat(typename Type::IntType n) const { return static_cast<float>(n) / Type::Scale; }

  public:
    typename Type::IntType mQformatInt{};
#ifdef QFORMAT_USE_FLOAT_VAL
    float mFloatVal{};
#endif
};

#endif// !__QFORMAT_HPP__