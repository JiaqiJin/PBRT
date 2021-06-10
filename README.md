# PBRT

## About
This repository is my personal offline Ray Tracing renderer, the code basically come from [PBRT](https://github.com/mmp/pbrt-v3).
### Build
The main program is in the folder KawaiiMiao. 

  ```sh
   git clone https://github.com/Jiaqidesune/PBRTStudy.git
  ```
  
Open KawaiiMiao.sln in KawaiiMiao folder with Visual Studio (2019)

This project is built with following third parties :
- [GLM](https://github.com/g-truc/glm) for fast algebra and math calculation.
- [stb_image](https://github.com/nothings/stb) for image I/O
- [oneTBB](https://github.com/oneapi-src/oneTBB) for multi-Thread
- [Assimp](https://github.com/assimp/assimp) for model import
- [nlohmann_json](https://github.com/nlohmann/json) for parsing json file of the scene
## Examples 

![Image](https://github.com/Jiaqidesune/PBRTStudy/blob/master/example/result.jpg)

# Feature

- BSDF, material related
  - [ ] microfacet BRDF
  - [x] lambertian Reflection and transmission
  - [x] Specular Reflection and transmission
  - [ ] Oren-Nayer
  - [ ] Fresnel Blend
  - [ ] bump map
  - [ ] normal map

- Camera
   - [x] Perspective camera
   - [ ] Ortho Camera
   - [ ] Real camera
   - [ ] Environmental Camera

- Sampler
   - [x] Random sampler
   - [ ] halton sampler
   - [ ] Stratified sampler
   - [ ] sobol sampler
   - [ ] MaxMinDistSampler
   - [ ] ZeroTwoSequenceSampler

- Filter
   - [x] Gaussian filter
   - [x] box filter
   - [ ] sinc filter
   - [ ] mitchell filter

- Light source
   - [x] point light
   - [ ] Diffuse area light
   - [ ] Spot light (spot light)
   - [ ] Ambient light (environment)
   - [ ] Distant light (distant light))
   - [ ] Projector (projection light)
   - [ ] Texture area light 
