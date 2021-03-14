#ifndef transform_hpp
#define transform_hpp

#include "../core/Header.h"

RENDERING_BEGIN

class Matrix4x4 {
public:
    Matrix4x4() {
        _m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1.f;
        _m[0][1] = _m[0][2] = _m[0][3] = _m[1][0] = _m[1][2] = _m[1][3] = _m[2][0] =
            _m[2][1] = _m[2][3] = _m[3][0] = _m[3][1] = _m[3][2] = 0.f;
    }

    Matrix4x4(const Float p[4][4]) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                _m[i][j] = p[i][j];
            }
        }
    }

    Matrix4x4(const Float* value) {
        for (int i = 0; i < 16; ++i) {
            _a[i] = value[i];
        }
    }

    inline void setZero() {
        for (int i = 0; i < 16; ++i) {
            _a[i] = 0;
        }
    }

    // 伴随矩阵除以行列式
    Matrix4x4 getInverseMat() const;

    Matrix4x4 getTransposeMat() const;

    Matrix4x4 getAdjointMat() const;

    Float getAdjointElement(Float a1, Float a2, Float a3,
        Float b1, Float b2, Float b3,
        Float c1, Float c2, Float c3)const;

    Float getDet() const;

    // 返回左上角3x3矩阵的行列式
    inline Float det3x3() const {
        Float det =
            _m[0][0] * (_m[1][1] * _m[2][2] - _m[1][2] * _m[2][1]) -
            _m[0][1] * (_m[1][0] * _m[2][2] - _m[1][2] * _m[2][0]) +
            _m[0][2] * (_m[1][0] * _m[2][1] - _m[1][1] * _m[2][0]);
        return det;
    }

    //判断变量中是否包含nan分量
    bool hasNaNs() const {
        for (int i = 0; i < 16; ++i) {
            if (std::isnan(_a[i])) {
                return true;
            }
        }
        return false;
    }

    bool operator == (const Matrix4x4& other) const;

    Matrix4x4 operator + (const Matrix4x4& other) const;

    Matrix4x4 operator * (const Matrix4x4& other) const;

    Matrix4x4 operator - (const Matrix4x4& other) const;

    bool operator != (const Matrix4x4& other) const;

    Matrix4x4 operator * (Float num) const;

    Matrix4x4 operator / (Float num) const;

    bool operator < (const Matrix4x4& other) const;

    bool isIdentity() const;

    static Matrix4x4 identity();

    friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat) {
        // clang-format off
        os << StringPrintf("[ [ %f, %f, %f, %f ] "
            "[ %f, %f, %f, %f ] "
            "[ %f, %f, %f, %f ] "
            "[ %f, %f, %f, %f ] ]",
            mat._m[0][0], mat._m[0][1], mat._m[0][2], mat._m[0][3],
            mat._m[1][0], mat._m[1][1], mat._m[1][2], mat._m[1][3],
            mat._m[2][0], mat._m[2][1], mat._m[2][2], mat._m[2][3],
            mat._m[3][0], mat._m[3][1], mat._m[3][2], mat._m[3][3]);
        // clang-format on
        return os;
    }

private:

    union {
        Float _m[4][4];
        Float _a[16];
        struct
        {
            Float _11; Float _12; Float _13; Float _14;
            Float _21; Float _22; Float _23; Float _24;
            Float _31; Float _32; Float _33; Float _34;
            Float _41; Float _42; Float _43; Float _44;
        };
    };

    friend class Transform;
    friend struct Quaternion;
    friend class AnimatedTransform;
};


class Transform {
 
public:
    Transform() {}

    Transform(const Float mat[4][4]) {
        _mat = Matrix4x4(mat);
        _matInv = _mat.getInverseMat();
    }

    Transform(const Matrix4x4& mat) : _mat(mat), _matInv(_mat.getInverseMat()) { }

    Transform(const Matrix4x4& mat, const Matrix4x4& matInv) : _mat(mat), _matInv(matInv) { }

    Transform getInverse() const {
        return Transform(_matInv, _mat);
    }

    Transform getTranspose() const {
        return Transform(_mat.getTransposeMat(), _matInv.getTransposeMat());
    }

    bool operator == (const Transform& other) const {
        return other._mat == _mat && other._matInv == _matInv;
    }

    bool operator != (const Transform& other) const {
        return other._mat != _mat || other._matInv != _matInv;
    }

    bool operator < (const Transform& other) const {
        return _mat < other._mat;
    }

    bool isIdentity() const {
        return _mat.isIdentity();
    }

    const Matrix4x4& getMatrix() const {
        return _mat;
    }

    const Matrix4x4& getInverseMatrix() const {
        return _matInv;
    }

    static Transform translate(const Vector3f& delta);

    static Transform scale(Float x, Float y, Float z);

    static Transform scale(Float s);

    static Transform rotateX(Float theta, bool bRadian = false);

    static Transform rotateY(Float theta, bool bRadian = false);

    static Transform rotateZ(Float theta, bool bRadian = false);

    static Transform rotate(Float theta, const Vector3f& axis, bool bRadian = false);

    static Transform lookAt(const Point3f& pos, const Point3f& look, const Vector3f& up);

    static Transform orthographic(Float zNear, Float zFar);

    static Transform perspective(Float fov, Float zNear, Float zFar, bool bRadian = false);

    static Transform identity();

private:
    Matrix4x4 _mat;
    Matrix4x4 _matInv;
};

RENDERING_END

#endif /* transform_hpp */