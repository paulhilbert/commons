#ifndef VISCOLORMAP_H_
#define VISCOLORMAP_H_

#include <functional>
#include <Eigen/Util>
#include "ColorConversion.h"

namespace Vis {

template <class ColorType, class Scalar>
using ColorMap = std::function<ColorType (Scalar)>;

template <class Scalar>
ColorMap<RGB, Scalar> rgbJet();

template <class Scalar>
ColorMap<RGBA,Scalar> rgbaJet();

template <class Scalar>
ColorMap<HSV, Scalar> hsvJet();

template <class Scalar>
ColorMap<HSVA, Scalar> hsvaJet();

#include "ColorMap.inl"

} // Vis

#endif /* VISCOLORMAP_H_ */
