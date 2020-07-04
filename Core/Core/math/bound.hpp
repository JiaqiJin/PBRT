#ifndef bounds_hpp
#define bounds_hpp

template <typename T>
class Bounds2 {

public:
    // Bounds2 Public Methods
    Bounds2() {
        T minNum = std::numeric_limits<T>::lowest();
        T maxNum = std::numeric_limits<T>::max();
        pMin = Point2<T>(maxNum, maxNum);
        pMax = Point2<T>(minNum, minNum);
    }
    explicit Bounds2(const Point2<T>& p) : pMin(p), pMax(p) {}
    Bounds2(const Point2<T>& p1, const Point2<T>& p2) {
        pMin = Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
        pMax = Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
    }
    template <typename U>
    explicit operator Bounds2<U>() const {
        return Bounds2<U>((Point2<U>)pMin, (Point2<U>)pMax);
    }

    Vector2<T> diagonal() const { return pMax - pMin; }
    T Area() const {
        Vector2<T> d = pMax - pMin;
        return (d.x * d.y);
    }
    int maximumExtent() const {
        Vector2<T> diag = diagonal();
        if (diag.x > diag.y)
            return 0;
        else
            return 1;
    }
    inline const Point2<T>& operator[](int i) const {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }
    inline Point2<T>& operator[](int i) {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }
    bool operator==(const Bounds2<T>& b) const {
        return b.pMin == pMin && b.pMax == pMax;
    }
    bool operator!=(const Bounds2<T>& b) const {
        return b.pMin != pMin || b.pMax != pMax;
    }
    //    Point2<T> Lerp(const Point2f &t) const {
    //        return Point2<T>(pbrt::Lerp(t.x, pMin.x, pMax.x),
    //                         pbrt::Lerp(t.y, pMin.y, pMax.y));
    //    }
    Vector2<T> offset(const Point2<T>& p) const {
        Vector2<T> o = p - pMin;
        if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
        return o;
    }
    void boundingSphere(Point2<T>* c, Float* rad) const {
        *c = (pMin + pMax) / 2;
        *rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
    }
    friend std::ostream& operator<<(std::ostream& os, const Bounds2<T>& b) {
        os << "[ " << b.pMin << " - " << b.pMax << " ]";
        return os;
    }

    // Bounds2 Public Data
    Point2<T> pMin, pMax;
};

template <typename T>
class Bounds3 {

public:


	// Bounds3 Public Data
	Point3<T> pMin, pMax;
};


#endif /* bound_h */
