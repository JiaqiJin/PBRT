#include "camera.hpp"

PALADIN_BEGIN

/*
Compute the main ray, and also computes the corresponding rays for pixes
 shifted in "x" and "y" direction on the film plane.
*/
Float Camera::GenerateRayDifferential(const CameraSample& sample,
    RayDifferential* rd) const
{
    Float wt = GenerateRay(sample, rd);
    if (wt == 0) return 0;

    // Find camera ray after shifting a fraction of a pixel in the $x$ direction
    Float wtx;
    for (Float eps : {.05, -.05})
    {
        CameraSample sshift = sample;
        sshift.pFilm.x += eps;
        Ray rx;
        wtx = GenerateRay(sshift, &rx);
        rd->rxOrigin = rd->ori + (rx.ori - rx.ori) / eps;
        rd->rxDirection = rd->dir + (rx.dir - rd->dir) / eps;
        if (wtx != 0)
            break;
    }
    if (wtx == 0)
        return 0;

    // Find camera ray after shifting a fraction of a pixel in the $y$ direction
    Float wty;
    for (Float eps : { .05, -.05 }) {
        CameraSample sshift = sample;
        sshift.pFilm.y += eps;
        Ray ry;
        wty = GenerateRay(sshift, &ry);
        rd->ryOrigin = rd->ori  + (ry.ori - rd->ori) / eps;
        rd->ryDirection = rd->dir + (ry.dir - rd->dir) / eps;
        if (wty != 0)
            break;
    }
    if (wty == 0)
        return 0;

    rd->hasDifferentials = true;
    return wt;
}

PALADIN_END
