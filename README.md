# PBRT

## This repository is my personal offline Ray Tracing renderer, the code basically come from [PBRT](https://github.com/mmp/pbrt-v3).

## Examples 

![Image](https://github.com/Jiaqidesune/PBRTStudy/example/result.jpg)

# Feature

- BSDF, material related

  - [] microfacet BRDF
  - [x] lambertian Reflection and transmission
  - [x] Specular Reflection and transmission
  - [] Oren-Nayer
  - [] Fresnel Blend
  - [] bump map
  - [] normal map

-Camera
   -[x] Perspective camera
   -[] Ortho Camera
   -[] Real camera
   -[] Environmental Camera

- Sampler
   -[x] Random sampler
   -[] halton sampler
   -[] Stratified sampler
   -[] sobol sampler
   -[] MaxMinDistSampler
   -[] ZeroTwoSequenceSampler

-Filter
   -[x] Gaussian filter
   -[x] box filter
   -[] sinc filter
   -[] mitchell filter

-Light source
   -[x] point light
   -[] Diffuse area light
   -[] Spot light (spot light)
   -[] Ambient light (environment)
   -[] Distant light (distant light))
   -[] Projector (projection light)
   -[] Texture area light 
