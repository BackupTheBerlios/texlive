// -*- related-file-name: "../../liblcdf/transform.cc" -*-
#ifndef LCDF_TRANSFORM_HH
#define LCDF_TRANSFORM_HH
#include <lcdf/bezier.hh>
#include <lcdf/string.hh>
#include <assert.h>

class Transform { public:

    Transform();
    Transform(const double[6]);
    Transform(double scalex, double sheary, double shearx, double scaley, double shiftx, double shifty);
    // Transform(const Transform &)	generated by compiler
    // ~Transform()			generated by compiler

    double operator[](int i) const	{ assert(i>=0&&i<6); return _m[i]; }
    bool null() const			{ return _null; }
    void check_null(double tolerance);
  
    void scale(double, double);
    void scale(const Point &p)			{ scale(p.x, p.y); }
    void scale(double d)			{ scale(d, d); }
    void rotate(double);
    void translate(double, double);
    void translate(const Point &p)		{ translate(p.x, p.y); }
    void shear(double);
    inline void transform(const Transform &);

    inline Transform scaled(double, double) const;
    Transform scaled(const Point &p) const	{ return scaled(p.x, p.y); }
    Transform scaled(double d) const		{ return scaled(d, d); }
    inline Transform rotated(double) const;
    inline Transform translated(double, double) const;
    inline Transform translated(const Point &p) const;
    inline Transform sheared(double) const;
    Transform transformed(const Transform &) const;

    // Transform operator+(Transform, const Point &);
    // Transform &operator+=(Transform &, const Point &);
    // Transform operator*(Transform, double);
    // Transform &operator*=(Transform &, double);
    // Transform operator*(Transform, const Transform &);
    // Transform &operator*=(Transform &, const Transform &);
    friend Point operator*(const Point &, const Transform &);
    friend Point &operator*=(Point &, const Transform &);
    friend Bezier operator*(const Bezier &, const Transform &);
    friend Bezier &operator*=(Bezier &, const Transform &);

    String unparse() const;

  private:

    // stored in PostScript order (along columns)
    double _m[6];
    bool _null;

    void real_apply_to(Point &) const;
    Point real_apply(const Point &) const;

};


inline Transform
Transform::scaled(double x, double y) const
{
    Transform t(*this);
    t.scale(x, y);
    return t;
}

inline Transform
Transform::rotated(double r) const
{
    Transform t(*this);
    t.rotate(r);
    return t;
}

inline Transform
Transform::translated(double x, double y) const
{
    Transform t(*this);
    t.translate(x, y);
    return t;
}

inline Transform
Transform::translated(const Point &p) const
{
    return translated(p.x, p.y);
}

inline Transform
Transform::sheared(double s) const
{
    Transform t(*this);
    t.shear(s);
    return t;
}


inline Transform &
operator+=(Transform &t, const Point &p)
{
    t.translate(p);
    return t;
}

inline Transform
operator+(Transform t, const Point &p)
{
    return t += p;
}

inline Transform &
operator*=(Transform &t, double scale)
{
    t.scale(scale);
    return t;
}

inline Transform
operator*(Transform t, double scale)
{
    return t *= scale;
}

inline Transform
operator*(const Transform &t, const Transform &tt)
{
    return t.transformed(tt);
}

inline Transform &
operator*=(Transform &t, const Transform &tt)
{
    t = t.transformed(tt);
    return t;
}

inline void
Transform::transform(const Transform &t)
{
    *this *= t;
}


inline Point &
operator*=(Point &p, const Transform &t)
{
    if (!t.null())
	t.real_apply_to(p);
    return p;
}

inline Point
operator*(const Point &p, const Transform &t)
{
    return (t.null() ? p : t.real_apply(p));
}

#endif
