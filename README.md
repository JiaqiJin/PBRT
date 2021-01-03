# PBRTStudy

# 反射模型

光线散射模型描述光在表面上的散射方式，什么方向进行折射和反射。光的反射用BRDF（bidirectional reflectance distribution function）函数描述，BTDF (bidirectional transmission distribution function) 描述表面上的透射，而BSDF (bidirectional scattering distribution function) 包含这俩种效果。

以下的内容大部分整理自pbrt第八章 —— REFLECTION MODELS。
表面反射模型来自多种来源：
- 测量数据模型：通过真实世界的实验测量得到，这样的数据可以直接以表格形式使用或用于计算一组基本函数的系数。
- 现象学模型：试图描述真实世界表面的定性特性的方程式在模仿它们方面非常有效。
- 模拟：有时候表面组成的底层信息可以获取到，例如我们知道颜料由带颜色的附着于某些介质的粒子组成，每种粒子的反射特性我们可以知道。这种情况下我们可以模拟微观层面的光散射来模拟产生反射数据。这个过程既可以在渲染过程完成也可以作为预处理。
- 物理（波）光学模型：将光当作波来看待，并计算麦克斯韦方程组的解。
- 几何光学模型：如果已知表面的低水平散射和几何特性，反射模型可以直接从描述中构造出来。几何光学使建模光与表面的相互作用更容易处理。

## 基本术语

表面反射可以分成为四大类 : 
- 漫光反射（diffuse）: 光的所有反射方向的强度都是一样的, 虽然现实中不存在理想漫反射，但许多表面，如粗糙的黑板，无光泽的油漆，都近似漫反射。。
- 光泽镜面反射（glossy specular）: 比如塑料或高光油漆，在某些方向会反射光，并且模糊的显示反射的物体。
- 完美镜面反射（perfect specular）只有一个方向有反射光。镜子和玻璃很近似理想镜面反射。
- 回归反射（retro-reflective）: 反射光会沿着入射光的相反方向射回来。比如天鹅绒，月球。
- 
大部分真实表面都是这四种反射类型的混合。这几种反射的区别主要在于它们的反射波瓣分布，下图中的网格描述的范围。

![image](https://cdn.jsdelivr.net/gh/ZeusYang/CDN-for-yangwc.com@1.1.35/blog/Scattering/1.png)

给定一类特定的反射，反射的分布函数还可以分成各向同性（isotropic）和各向异性（anisotropic）。大多数对象都是各向同性的：如果您选择表面上的一个点并在该点处绕其法线轴旋转，则反射光的分布不会改变。 相反，以这种方式旋转各向异性材料时，它们反射的光量不同。 各向异性表面的示例包括拉丝金属，许多类型的布和光盘。

## 几何设置

由于描述散射现象通常是特点于对象表面的点，因此我们的BRFD和BTDF计算基本上是在对象表面的局部坐标系(local coordinate system) 中进行的。这个局部坐标系我们称之为着色坐标系。该坐标系如下图所示，在该坐标系中，以切线(tangent)、副切线(subtangent)和法线向量(normal vectors)分别作为 x, y, 和z轴。

![image](https://cdn.jsdelivr.net/gh/ZeusYang/CDN-for-yangwc.com@1.1.35/blog/Scattering/2.png)

所有BRDF和BTDF中的方向向量都是在这个坐标系中定义的。也可以用球面坐标（θ , φ）；θ表示给定的向量和z轴的角度，φ向量表示投影到xy平面后，和x轴的角度。给定在该坐标系的一个方向向量ω，可以很容易计算它和法线形成的角度的余弦值。在BRDF和BTDF函数中，用 wi表示入射方向向量，wo表示出射方向向量，默认均已经归一化为单位向量。

![ima22e](https://cdn.jsdelivr.net/gh/ZeusYang/CDN-for-yangwc.com@1.1.35/blog/Scattering/3.png)

## 基本接口

BRDFs和BTDFs共享一个通用的基类，BxDF。

``` c++
class BxDF {
public:
//BxDF Interface
    .....

//BxDF Public Data
const BxDFType tye;
}; 
```

该接口中的type用于子类指明当前是反射还是透射，同时又属于哪类反射模型：

``` c++
enum BxDFType {
    BSDF_REFLECTION = 1 << 0,
    BSDF_TRANSMISSION = 1 << 1,
    BSDF_DIFFUSE = 1 << 2,
    BSDF_GLOSSY = 1 << 3,
    BSDF_SPECULAR = 1 << 4,
    BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
               BSDF_TRANSMISSION,
};

//BxDF interface 
BxDF(BxDFType type) : type(type) {}
```

BxDF提供的关键方法是 BxDF::f(), 它要求输入入射和反射方向，返回相应的BSDF函数值。

``` c++
virtual Spectrum f(const Vector2f& wo, const Vector2f& wi) const = 0;
``` 

并非所有的BxFD可以使用f（）方法来进行评估。例如，像镜子，玻璃或水这样的完美镜面物体只能将光从单个入射方向散射到单个出射方向。
因为此时它们的反射波瓣很窄，只占整个半球方向很小的一部分，我们根据向量的反射特性获取入射方向(除了单一的反射方向外，其他方向的光都为0，时的BRDF是一个狄拉克函数delta distributions)。为此，亦声明了如下的接口：

``` c++
virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi,
       const Point2f &sample, Float *pdf,
       BxDFType *sampledType = nullptr) const;
``` 

BxDF::Sample_f()是，调用方给定一个输出方向ωo，BxDF::Sample_f()会计算出一个输入方向wi(输入函数是wo，返回值是wi)。函数的其他参数我们暂时先忽略。用球面坐标（θ , φ）表示方向向量， BSDF函数本质是关于入射方向和散射方向的4D函数，在某些特殊情况下，我们可以简化会在单个方向上的2D函数。一种特殊情况就是半球-方向反射率（hemispherical-directional reflectance），由于在半球的常量照明(constant illumination)，即入射幅射率是一个常量函数，因此可以将反射方程中的入射辐射率（incident radiance）Li提取到半球积分外面，剩下的就是半球-方向反射率。对于给定的出射方向 wo， 可以直接计算入射辐照度该方向上的反射比率 :

$$ \rho_{hd}(\omega_o)=\int_{H^2(n)} f_r(p,\omega_o, \omega_i)|cos\theta_i|d\omega_i \tag $$

上述的公式也可以被解读为: 由于来自给定方向的光会在半球上产生全反射(此时wo就变成了入射方向而非反射方向）。我们定义下面的rho函数接口用于计算半球-方向反射率，参数nSample和sample用于辅助计算公式的积分。

``` c++
virtual Spectrum rho(const Vector3f &wo, int nSamples,
                     const Point2f *samples) const;
                     
``` 

除了半球-方向反射率之外， 还有一种反射率叫做半球-半球反射率（hemispherical-hemispherical reflectance）。它计算的是一个光谱值，当所有方向的入射光辐射率都相同时，改值给出了表面反射的入射光值，再向整个半球射出去的照度比例(表面出射的总能量/表面接收ID总能量)。

 $$ \rho_{hh}=\frac{1}{\pi}\int_{H^2(n)} \int_{H^2(n)}f_r(p,\omega_o,\omega_i)|cos\theta_o cos\theta_i| d\omega_o d\omega_i \tag $$
 
 可以理解为再半球-方向反射率(hemispherical-directional reflectance)公式上消减出射方向的维度。为此，我们可以计算 $$rho_{hh}$$ 公式在没有方向wo的情况下，因此，定义下面的接口用以计算公式 :
 
 ``` c++
 virtual Spectrum rho(int nSamples, const Point2f *samples1,
                         const Point2f *samples2) const;
``` 

在此基础上，我们在创建ScaledBxDF类，它的本质就是将BxDF的函数返回值在乘上一个光谱值（RGB或者基于采样的光谱向量）。

 ``` c++
class ScaleBxDF : public BxDF {
public:
       ScaledBxDF(BxDF *bxdf, const Spectrum &scale) 
           : BxDF(BxDFType(bxdf->type)), bxdf(bxdf), scale(scale) {
        }
       Spectrum rho(const Vector3f &w, int nSamples, const Point2f *samples) const {
           return scale * bxdf->rho(w, nSamples, samples); 
       }
       Spectrum rho(int nSamples, const Point2f *samples1,
               const Point2f *samples2) const {
           return scale * bxdf->rho(nSamples, samples1, samples2); 
       }
       Spectrum f(const Vector3f &wo, const Vector3f &wi) const{
           return scale * bxdf->f(wo, wi);
       }
       Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
           Float *pdf, BxDFType *sampledType) const;

private:
    BxDF *bxdf;
    Spectrum scale;
}
``` 

## 镜面反射和透射

使用物理和几何光学模型，在相对光滑的表面上的光特性相对容易分析。这些表面表现出完美的镜面反射和入射光透射率的行为。完美的镜面反射和透射非常特殊，给定一个入射方向 $$w_i$$,我们可以很容易地求出相应的完美镜面反射方向$$w_o$$，而透射方向的计算则稍微要麻烦一点。斯涅尔定律(Snell`s law)给出介质（medium）透射指数和透射角度的关系，透射指数述了特定介质中的光传播比真空中的传播慢得多。用$$\eta$$标记：

$$\eta_i sin\theta_i = \eta_t sin \theta_t$$

通常，折射率随着光的波长而变化。因此，入射光通常在俩个不同的介质之间的边界沿多个方向进行散射，这种现象被称为色散现象（dispersion）。当入射的白光被棱镜（prism）分成光谱成分时，可以看到这种效果。计算机图形中的常见做法是忽略这种波长依赖性，因为这种效果通常对视觉准确性不是至关重要的，而忽略它会大大简化光传输计算。

除了散射（scattering）方向的计算，我们还要计算光能的反射（reflection）比例和折射（refraction）比例，这通过菲涅尔方程联系起来。

## 菲涅耳反方程 Fresnel Reflectance

除了反射和透射的方向外，还必须要计算反射和透射的入射光的比例，而菲涅尔方程（Fresnel equations）描述了光照射到表面上的比例。给定折射系数（index of refraction）和入射光线与表面法线（normal）所成的角度，菲涅耳方程式指定了入射照明的两种不同[偏振](https://www.jianshu.com/p/9027da93e2e7)态（polarization ）下材料的相应反射率，两种偏振状态分别是平行偏振光（parallel polarized light）和垂直偏振光（vertically polarized light.）。由于偏振的视觉效果在大多数环境中是有限的，因此在通常情况下，我们将假设光是非偏振的。因此菲涅耳反射率应该是俩种偏振状态的平均值。

根据是否存在能够导电的自由电子，可以将物体的材质分成以下的三大类：

- 绝缘体（dielectric）：此类材质不导电，其介质的折射指数均为实数值，会透射一定量的入射光，此类材质有玻璃、矿物油、水和空气等；
- 导电体（conductor）：此类材质能够导电，因其具有自由移动的电子，此类材质不透明（一般透射进入内部的光被转换成了热能，厚度很薄的极端情况不考虑），仅发生反射。其介质的折射指数是复数值 $$\overline \eta=\eta + ik$$
- 半导体（semiconductor）：例如硅或锗都属于此类，在这里我们不考虑。

导体和电介质都由同一组菲涅尔方程控制。尽管如此，我们还是希望为电介质创建一个特殊的评估函数，以受益于在保证折射率为实数时这些方程采用的特别简单的形式。为了方便起见，我们定义一个抽象的Fresnel类，该类提供了一个用于计算Fresnel反射系数的一个接口，Evaluate接口输入$$\cos \theta_i$$, 返回菲涅尔反射率（Fresnel Reflectance）。

```c++
class Fresnel 
{
public:
  // Fresnel Interface
    virtual ~Fresnel();
    virtual Spectrum Evaluate(Float cosI) const = 0;
};
```
下图列出了一些常见的绝缘体的折射系数：
![fresnelTable](https://cdn.jsdelivr.net/gh/ZeusYang/CDN-for-yangwc.com@1.1.35/blog/Scattering/4.png)

要计算俩种介电(dielectric)介质(media)界面处的菲涅耳反射率，我们需要知道俩种介电质的折射率。电介质(dielectric)的菲涅耳反射公式为 :

$$r_{||}=\frac{\eta_t cos\theta_i-\eta_icos\theta_t}{\eta_tcos\theta_i+\eta_icos\theta_t}$$

$$r_{\perp}=\frac{\eta_i cos\theta_i-\eta_tcos\theta_t}{\eta_icos\theta_i+\eta_tcos\theta_t}$$


其中$$r_{||}$$ 是平行偏振光(polarized light)的的菲涅耳反射率，$$r_{\perp}$$是垂直偏振光的反射率。$$\eta_i$$和$$\eta_t$$是入射介质和透射介质的反射率，$$\theta_i$$和$$\theta_t$$分别是入射方向$$\omega_i$$和折射方向$$\omega_t$$与法线的夹角。

对于非偏振光，菲涅尔反射率是两者的综合平均 : 

$$F_r=\frac12(r_{||}^2+r_{\perp}^2)$$

因此，根据能量守恒定律，绝缘体的透射率为 $$1 - F_r$$。

根据以上讨论，FrDielectric（）函数计算介电材料(dielectric materials)和非偏振光(unpolarized light)的菲涅耳反射公式。输入参数分别为$$\eta_i$$、$$\eta_t$$和$$cos\theta_i$$，返回绝缘体的反射率 :

```c++
Float frDielectric(Float cosThetaI, Float etaI, Float etaT) {
    cosThetaI = clamp(cosThetaI, -1, 1);

    bool entering = cosThetaI > 0.f;
    // 如果如果入射角大于90° 
    // 则法线方向反了，cosThetaI取绝对值，对换两个折射率
    if (!entering) {
        std::swap(etaI, cosThetaI);
    }

    // 用斯涅耳定律计算sinThetaI
    Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
    Float sinThetaT = etaI / etaT * sinThetaI;

    // 全内部反射情况 Handle total internal reflection
    if (sinThetaT >= 1) {
        return 1;
    }
    Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));
    Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT))
        / ((etaT * cosThetaI) + (etaI * cosThetaT));
    Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT))
        / ((etaI * cosThetaI) + (etaT * cosThetaT));
    return (Rparl * Rparl + Rperp * Rperp) / 2;
}
};
```

为了找到透视角的余弦cosThetaT，先要判断入射方向的介质是在外边还是在介质的内部，以便可以正确地解释两个折射率。在计算前我们可以根据$$cos\theta_i$$ 的符号判断入射光在那一面的介质。如果余弦在0到1之间，则射线在外侧；如果余弦在-1和0之间，则射线在内侧。如果是由内部向外透射，则应该交换一下折射系数。此外，还需要考虑全面反射的情况，根据Snell定律计算出来的$$sin\theta_t$$是否大于等于1来进行判断，如果大于1，则返回1.0的反射率（即全部反射了）。因此，绝缘体Fresnel类实现如下，直接调用frDielectric函数:

```c++
class FresnelDielectric : public Fresnel{
public :
    FresnelDielectric(Float etaI, Float etaT) : etaI(etaI), etaT(etaT) {}

    virtual Spectrum evaluate(Float cosThetaI){
        return frDielectric(cosThetaI, etaI,etaT);
    }
    virtual std::string toString() const {
        return StringPrintf("[ FrenselDielectric etaI: %f etaT: %f ]", _etaI, _etaT);
    }

private:
    Float etaI, etaT;
};
```

接着要考虑到电体的介质的反射率。导电体的折射指数(indices of refraction)为复数值$$\overline \eta=\eta+ik$$。有时候光会被材质吸收并转化成热能，虚数部分$$k$$被称为吸收系数（absorption coefficient)。导体和电介质之间的边界处的菲涅耳反射率由下式给出：

$$r_\perp=\frac{a^2+b^2-2a cos\theta + cos^2\theta}{a^2+b^2+2acos\theta+cos^2\theta}$$

$$r_{||}=r_{\perp}\frac{cos^2\theta(a^2+b^2)-2acos\theta sin^2\theta + sin^4\theta}{cos^2\theta(a^2+b^2)+2acos\theta sin^2\theta+sin^4\theta}$$

其中 :

$$ a^2+b^2 =\sqrt{(\eta^2-k^2-sin^2\theta)^2+4\eta^2k^2}$$

而上式中的$$\eta+ik=\overline \eta_t/\overline \eta_i$$是虚数部分计算后的相对折射系数。由此实现导电体的frConductor，输入$$cos\theta_i$$、$$\overline\eta_i$$、$$\overline \eta_t = \eta_t+ik$$,返回导电体的菲涅尔反射率。入射介质折射率是一个光谱值，因为导体的折射与长波有关 :
 
```c++
Spectrum frConductor(Float cosThetaI, const Spectrum& etaI,
    const Spectrum& etaT, const Spectrum& kt)
{
    cosThetaI = clamp(cosThetaI, -1, 1);
    Spectrum eta = etat / etai;
    Spectrum etak = kt / etai;
    
    Float cosThetaI2 = cosThetaI * cosThetaI;
    Float sinThetaI2 = 1. - cosThetaI2;
    Spectrum eta2 = eta * eta;
    Spectrum etak2 = etak * etak;
    
    Spectrum t0 = eta2 - etak2 - sinThetaI2;
    Spectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
    Spectrum t1 = a2plusb2 + cosThetaI2;
    Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
    Spectrum t2 = (Float)2 * cosThetaI * a;
    Spectrum Rs = (t1 - t2) / (t1 + t2);
    
    Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    Spectrum t4 = t2 * sinThetaI2;
    Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);
    
    return 0.5 * (Rp + Rs);
}
```

因此，导电体的菲涅尔反射类FresnelConductor实现如下 :

```c++
class FresnelConductor : public Fresnel {

public:
     FresnelConductor(const Spectrum& etaI, const Spectrum& etaT,
        const Spectrum& kt)
        : _etaI(etaI), _etaT(etaT), _kt(kt) {}

     virtual Spectrum evaluate(Float cosThetaI) const {
        return frConductor(std::abs(cosThetaI), _etaI, _etaT, _kt);
    }

     virtual std::string toString() const {
        return std::string("[ FresnelConductor etaI: ") + _etaI.ToString() +
            std::string(" etaT: ") + _etaT.ToString() + std::string(" k: ") +
            _kt.ToString() + std::string(" ]");
    }

private:
    Spectrum _etaI, _etaT, _kt;
};
```




