#pragma once

#include "Header.h"
#include "../math/rng.h"

RENDERING_BEGIN

static __forceinline Point2f uniformSampleSector(const Point2f& u, Float thetaMax) {
    Float r = std::sqrt(u[0]);
    Float theta = thetaMax * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

static __forceinline Point2f uniformSampleDisk(const Point2f& u) {
    Float r = std::sqrt(u[0]);
    Float theta = _2Pi * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

static __forceinline Point2f concentricSampleDisk(const Point2f& u) {
    // 把[0,1]映射到[-1,1]
    Point2f uOffset = 2.f * u - Vector2f(1, 1);

    // 退化到原点的情况
    if (uOffset.x == 0 && uOffset.y == 0) {
        return Point2f(0, 0);
    }

    // r = x
    // θ = y / x * π / 4
    Float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
        r = uOffset.x;
        theta = PiOver4 * (uOffset.y / uOffset.x);
    }
    else {
        r = uOffset.y;
        theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
    }
    return r * Point2f(std::cos(theta), std::sin(theta));
}

static __forceinline Vector3f uniformSampleSphere(const Point2f& u) {
    Float z = 1 - 2 * u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = _2Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

static __forceinline Float uniformConePdf(Float cosThetaMax) {
    return 1 / (_2Pi * (1 - cosThetaMax));
}

static __forceinline Vector3f UniformSampleCone(const Point2f& u, Float cosThetaMax) {
    Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = std::sqrt((Float)1 - cosTheta * cosTheta);
    Float phi = u[1] * _2Pi;
    return Vector3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
        cosTheta);
}

static __forceinline Point2f uniformSampleTriangle(const Point2f& u) {
    Float su0 = std::sqrt(u[0]);
    return Point2f(1 - su0, u[1] * su0);
}

inline Vector3f cosineSampleHemisphere(const Point2f& u) {
    Point2f d = uniformSampleDisk(u);
    Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
    return Vector3f(d.x, d.y, z);
}

inline Vector3f cosineSampleHemisphere(const Point2f& u) {
    Point2f d = uniformSampleDisk(u);
    Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
    return Vector3f(d.x, d.y, z);
}

inline Float cosineHemispherePdf(Float cosTheta) {
    return cosTheta * InvPi;
}

inline Float uniformHemispherePdf() {
    return Inv2Pi;
}


template <typename T>
void shuffle(T* samp, int count, int nDimensions, RNG& rng) {
    for (int i = 0; i < count; ++i) {
        // 随机选择一个i右侧的索引
        int other = i + rng.uniformUInt32(count - i);
        for (int j = 0; j < nDimensions; ++j) {
            std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
        }
    }
}

__forceinline Float uniformSpherePdf() {
    return Inv4Pi;
}

__forceinline Float balanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

__forceinline Float balanceHeuristic(Float fPdf, Float gPdf) {
    return balanceHeuristic(1, fPdf, 1, gPdf);
}

__forceinline Float powerHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
    Float f = nf * fPdf, g = ng * gPdf;
    return f == 0 ? 0 : (f * f) / (f * f + g * g);
}

__forceinline Float powerHeuristic(Float fPdf, Float gPdf) {
    return powerHeuristic(1, fPdf, 1, gPdf);
}


struct Distribution1D {

    Distribution1D() {}
    
    Distribution1D(const Float* f, int num)
    : _func(f, f + num), 
      _cdf(num + 1) {
        _cdf[0] = 0;
        for (int i = 1; i < num + 1; ++i) {
            _cdf[i] = _cdf[i - 1] + _func[i - 1] / num;
        }

        // 由于func积分值不一定为1
        // 所以需要将_cdf归一化
        _funcInt = _cdf[num];
        if (_funcInt == 0) {
            // 如果_func全为零，则均匀分布
            for (int i = 1; i < num + 1; ++i) {
                _cdf[i] = Float(i) / Float(num);
            }
        }
        else {
            for (int i = 1; i < num + 1; ++i) {
                _cdf[i] = _cdf[i] / _funcInt;
            }
        }
    }

    int count() const {
        return (int)_func.size();
    }

    Float sampleContinuous(Float u, Float* pdf = nullptr, int* off = nullptr) const {
        auto predicate = [&](int index) {
            return _cdf[index] <= u;
        };
        int offset = findInterval((int)_cdf.size(), predicate);
        if (off) {
            *off = offset;
        }
        Float du = u - _cdf[offset];
        if ((_cdf[offset + 1] - _cdf[offset]) > 0) {
            CHECK_GT(_cdf[offset + 1], _cdf[offset]);
            du /= (_cdf[offset + 1] - _cdf[offset]);
        }
        DCHECK(!std::isnan(du));

        if (pdf) {
            *pdf = (_funcInt > 0) ? _func[offset] / _funcInt : 0;
        }
        return (offset + du) / count();
    }

    int sampleDiscrete(Float u, Float* pdf = nullptr, Float* uRemapped = nullptr) const {
        auto predicate = [&](int index) {
            return _cdf[index] <= u;
        };
        int offset = findInterval((int)_cdf.size(), predicate);
        if (pdf) {
            // 保证pdf积分为1，所以比连续形式的pdf多除了一个count
            *pdf = (_funcInt > 0) ? _func[offset] / (_funcInt * count()) : 0;
        }
        if (uRemapped) {
            *uRemapped = (u - _cdf[offset]) / (_cdf[offset + 1] - _cdf[offset]);
            DCHECK(*uRemapped >= 0.f && *uRemapped <= 1.f);
        }
        return offset;
    }

    inline Float getFuncInt() const {
        return _funcInt;
    }

    inline Float funcAt(size_t idx) const {
        return _func[idx];
    }

    Float discretePDF(int index) const {
        DCHECK(index >= 0 && index < count());
        return _func[index] / (_funcInt * count());
    }

private:
    // 指定分布的函数
    std::vector<Float> _func;
    // 指定函数的累积分布函数
    std::vector<Float> _cdf;
    // func函数的积分值
    Float _funcInt;

    friend struct Distribution2D;
};

struct Distribution2D {
public:
    Distribution2D() {

    }

    void init(const Float* data, int nu, int nv) {
        _pConditionalV.reserve(nv);
        // 创建v个长度为u的一维分布，存入列表中
        for (int v = 0; v < nv; ++v) {
            _pConditionalV.emplace_back(new Distribution1D(&data[v * nu], nu));
        }
        std::vector<Float> _marginalFunc;
        _marginalFunc.reserve(nv);
        // 将每个一维分布的积分值存入_pMarginal中作为边缘概率密度
        for (int v = 0; v < nv; ++v) {
            _marginalFunc.push_back(_pConditionalV[v]->_funcInt);
        }
        _pMarginal.reset(new Distribution1D(&_marginalFunc[0], nv));
    }

    Distribution2D(const Float* data, int nu, int nv) {
        init(data, nu, nv);
    }

    Point2f sampleContinuous(const Point2f& u, Float* pdf = nullptr) const {
        Float pdfs[2];
        int v;
        Float d1 = _pMarginal->sampleContinuous(u[1], &pdfs[1], &v);
        Float d0 = _pConditionalV[v]->sampleContinuous(u[0], &pdfs[0]);
        if (pdf) {
            *pdf = pdfs[0] * pdfs[1];
        }
        return Point2f(d0, d1);
    }

    Float pdf(const Point2f& p) const {
        int iu = clamp(int(p[0] * _pConditionalV[0]->count()), 0, _pConditionalV[0]->count() - 1);
        int iv = clamp(int(p[1] * _pMarginal->count()), 0, _pMarginal->count() - 1);
        return _pConditionalV[iv]->_func[iu] / _pMarginal->_funcInt;
    }

private:
    // 长度为v的列表，每个列表储存一个样本数量为u的一维分布对象
    std::vector<std::unique_ptr<Distribution1D>> _pConditionalV;
    // u趋向于1时，v的边缘概率密度函数
    std::unique_ptr<Distribution1D> _pMarginal;
};


RENDERING_END