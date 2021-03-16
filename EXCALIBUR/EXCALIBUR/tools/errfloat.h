#pragma once

#include "../core/Header.h"

RENDERING_BEGIN

class EFloat {
public:
    EFloat() {}
    EFloat(float _val, float err = 0.f) : _val(_val) {
        if (err == 0.)
            _low = _high = _val;
        else {
            _low = nextFloatDown(_val - err);
            _high = nextFloatUp(_val + err);
        }

#ifndef NDEBUG
        _vPrecise = _val;
        check();
#endif  // NDEBUG
    }

#ifndef NDEBUG
    EFloat(float _val, long double lD, float err) : EFloat(_val, err) {
        _vPrecise = lD;
        check();
    }
#endif  // DEBUG

    /*
    书中介绍的方法为(a ± δa) ⊕ (b ± δb)
    = ((a ± δa) + (b ± δb))(1± γ1)
    = a + b + [±δa ± δb ± (a + b)γ1 ± γ1 * δa ± γ1 * δb].
    但看起来不直观，所以改用以下方式
    */
    EFloat operator + (const EFloat& ef) const {
        EFloat r;
        r._val = _val + ef._val;
#ifndef NDEBUG
        r._vPrecise = _vPrecise + ef._vPrecise;
#endif  // DEBUG
        // 使用next的原因是获取更加保守的边界
        r._low = nextFloatDown(lowerBound() + ef.lowerBound());
        r._high = nextFloatUp(upperBound() + ef.upperBound());
        r.check();
        return r;
    }

    // 强制类型转换
    explicit operator float() const {
        return _val;
    }

    // 强制类型转换
    explicit operator double() const {
        return _val;
    }

    float getAbsoluteError() const {
        return _high - _low;
    }

    float upperBound() const {
        return _high;
    }

    float lowerBound() const {
        return _low;
    }

#ifndef NDEBUG
    float getRelativeError() const {
        return std::abs((_vPrecise - _val) / _vPrecise);
    }
    long double preciseValue() const {
        return _vPrecise;
    }
#endif

    // 减法，要注意的是误差要保守估计
    EFloat operator - (const EFloat& ef) const {
        EFloat r;
        r._val = _val - ef._val;
#ifndef NDEBUG
        r._vPrecise = _vPrecise - ef._vPrecise;
#endif
        // 新的下界尽可能低
        r._low = nextFloatDown(lowerBound() - ef.upperBound());
        // 新的上界尽可能高
        r._high = nextFloatUp(upperBound() - ef.lowerBound());
        r.check();
        return r;
    }

    EFloat operator * (const EFloat& ef) const {
        EFloat r;
        r._val = _val * ef._val;
#ifndef NDEBUG
        r._vPrecise = _vPrecise * ef._vPrecise;
#endif
        Float prod[4] = {
            lowerBound() * ef.lowerBound(),
            upperBound() * ef.lowerBound(),
            lowerBound() * ef.upperBound(),
            upperBound() * ef.upperBound() };

        // 新的下界尽可能低
        r._low = nextFloatDown(std::min(std::min(prod[0], prod[1]), std::min(prod[2], prod[3])));
        // 新的上界尽可能高
        r._high = nextFloatUp(std::max(std::max(prod[0], prod[1]), std::max(prod[2], prod[3])));
        r.check();
        return r;
    }

    EFloat operator / (const EFloat& ef) const {
        EFloat r;
        r._val = _val / ef._val;
#ifndef NDEBUG
        r._vPrecise = _vPrecise / ef._vPrecise;
#endif
        if (ef._low < 0 && ef._high > 0) {
            r._low = -Infinity;
            r._high = Infinity;
        }
        else {
            Float di_val[4] = {
                lowerBound() / ef.lowerBound(),
                upperBound() / ef.lowerBound(),
                lowerBound() / ef.upperBound(),
                upperBound() / ef.upperBound() };
            // 新的下界尽可能低
            r._low = nextFloatDown(std::min(std::min(di_val[0], di_val[1]), std::min(di_val[2], di_val[3])));
            // 新的上界尽可能高
            r._high = nextFloatUp(std::max(std::max(di_val[0], di_val[1]), std::max(di_val[2], di_val[3])));
        }
        r.check();
        return r;
    }

    EFloat operator - () const {
        EFloat r;
        r._val = -_val;
#ifndef NDEBUG
        r._vPrecise = -_vPrecise;
#endif
        r._low = -_high;
        r._high = -_low;
        r.check();
        return r;
    }

    inline bool operator == (const EFloat& fe) const {
        return _val == fe._val;
    }

    inline void check() const {
        if (!std::isinf(_low) && !std::isnan(_low) && !std::isinf(_high) &&
            !std::isnan(_high))
            CHECK_LE(_low, _high);
#ifndef NDEBUG
        if (!std::isinf(_val) && !std::isnan(_val)) {
            CHECK_LE(lowerBound(), _vPrecise);
            CHECK_LE(_vPrecise, upperBound());
        }
#endif
    }

    EFloat(const EFloat& ef) {
        ef.check();
        _val = ef._val;
        _low = ef._low;
        _high = ef._high;
#ifndef NDEBUG
        _vPrecise = ef._vPrecise;
#endif
    }

    EFloat& operator = (const EFloat& ef) {
        ef.check();
        if (&ef != this) {
            _val = ef._val;
            _low = ef._low;
            _high = ef._high;
#ifndef NDEBUG
            _vPrecise = ef._vPrecise;
#endif
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const EFloat& ef) {
        os << StringPrintf("_val=%f (%a) - [%f, %f]",
            ef._val, ef._val, ef._low, ef._high);
#ifndef NDEBUG
        os << StringPrintf(", precise=%.30Lf", ef._vPrecise);
#endif // !NDEBUG
        return os;
    }

private:
    // 浮点数round之后的值
    float _val;

    // round之后减去绝对误差
    float _low;

    // round之后加上绝对误差
    float _high;
#ifndef NDEBUG
    // debug模式下的高精度数据
    long double _vPrecise;
#endif  // NDEBUG
    friend inline EFloat sqrt(const EFloat& fe);
    friend inline EFloat abs(const EFloat& fe);
    friend inline bool quadratic(const EFloat& A, const EFloat& B, const EFloat& C, EFloat* t0, EFloat* t1);
};

inline EFloat operator*(float f, EFloat fe) {
    return EFloat(f) * fe;
}

inline EFloat operator/(float f, EFloat fe) {
    return EFloat(f) / fe;
}

inline EFloat operator+(float f, EFloat fe) {
    return EFloat(f) + fe;
}

inline EFloat operator-(float f, EFloat fe) {
    return EFloat(f) - fe;
}

inline EFloat sqrt(const EFloat& fe) {
    EFloat r;
    r._val = std::sqrt(fe._val);
#ifndef NDEBUG
    r._vPrecise = std::sqrt(fe._vPrecise);
#endif
    r._low = nextFloatDown(std::sqrt(fe._low));
    r._high = nextFloatUp(std::sqrt(fe._high));
    r.check();
    return r;
}

inline EFloat abs(const EFloat& fe) {
    if (fe._low >= 0)
        // 如果下界大于零，直接返回
        return fe;
    else if (fe._high <= 0) {
        // 如果上界小于零，则每个成员都乘以-1
        EFloat r;
        r._val = -fe._val;
#ifndef NDEBUG
        r._vPrecise = -fe._vPrecise;
#endif
        r._low = -fe._high;
        r._high = -fe._low;
        r.check();
        return r;
    }
    else {
        EFloat r;
        r._val = std::abs(fe._val);
#ifndef NDEBUG
        r._vPrecise = std::abs(fe._vPrecise);
#endif
        r._low = 0;
        r._high = std::max(-fe._low, fe._high);
        r.check();
        return r;
    }
}

inline bool quadratic(const EFloat& A, const EFloat& B, const EFloat& C, EFloat* t0, EFloat* t1) {
    // ax^2 + bx + c = 0
    // 先计算判别式
    double discrim = (double)B._val * (double)B._val - 4. * (double)A._val * (double)C._val;
    if (discrim < 0.) {
        return false;
    }
    double rootDiscrim = std::sqrt(discrim);

    EFloat floatRootDiscrim(rootDiscrim, MachineEpsilon * rootDiscrim);

    EFloat q;
    if ((float)B < 0) {
        q = -.5 * (B - floatRootDiscrim);
    }
    else {
        q = -.5 * (B + floatRootDiscrim);
    }
    *t0 = q / A;
    *t1 = C / q;
    if ((float)*t0 > (float)*t1) {
        std::swap(*t0, *t1);
    }
    return true;
}


RENDERING_END