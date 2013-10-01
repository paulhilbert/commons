#ifndef VISCOLORS_H_
#define VISCOLORS_H_

#include "Color.h"

namespace Vis {

namespace Colors {

// rgb colors
inline RGB  rgbRed       () { return RGB (1.f, 0.f, 0.f); }
inline RGB  rgbGreen     () { return RGB (0.f, 1.f, 0.f); }
inline RGB  rgbBlue      () { return RGB (0.f, 0.f, 1.f); }
inline RGB  rgbYellow    () { return RGB (1.f, 1.f, 0.f); }
inline RGB  rgbCyan      () { return RGB (0.f, 1.f, 1.f); }
inline RGB  rgbMagenta   () { return RGB (1.f, 0.f, 1.f); }
inline RGB  rgbWhite     () { return RGB (1.f, 1.f, 1.f); }
inline RGB  rgbBlack     () { return RGB (0.f, 0.f, 0.f); }
inline RGB  rgbGrey      () { return RGB (0.5f, 0.5f, 0.5f); }
inline RGB  rgbLightGrey () { return RGB (0.7f, 0.7f, 0.7f); }
inline RGB  rgbDarkGrey  () { return RGB (0.3f, 0.3f, 0.3f); }

// rgba colors
inline RGBA rgbaRed       (float alpha = 1.f) { return RGBA(1.f, 0.f, 0.f, alpha); }
inline RGBA rgbaGreen     (float alpha = 1.f) { return RGBA(0.f, 1.f, 0.f, alpha); }
inline RGBA rgbaBlue      (float alpha = 1.f) { return RGBA(0.f, 0.f, 1.f, alpha); }
inline RGBA rgbaYellow    (float alpha = 1.f) { return RGBA(1.f, 1.f, 0.f, alpha); }
inline RGBA rgbaCyan      (float alpha = 1.f) { return RGBA(0.f, 1.f, 1.f, alpha); }
inline RGBA rgbaMagenta   (float alpha = 1.f) { return RGBA(1.f, 0.f, 1.f, alpha); }
inline RGBA rgbaWhite     (float alpha = 1.f) { return RGBA(1.f, 1.f, 1.f, alpha); }
inline RGBA rgbaBlack     (float alpha = 1.f) { return RGBA(0.f, 0.f, 0.f, alpha); }
inline RGBA rgbaGrey      (float alpha = 1.f) { return RGBA(0.5f, 0.5f, 0.5f, alpha); }
inline RGBA rgbaLightGrey (float alpha = 1.f) { return RGBA(0.7f, 0.7f, 0.7f, alpha); }
inline RGBA rgbaDarkGrey  (float alpha = 1.f) { return RGBA(0.3f, 0.3f, 0.3f, alpha); }

inline RGBA rgbaInvisible ()                  { return RGBA(0.f, 0.f, 0.f, 0.f);; }




} // Vis
} // Colors

#endif /* VISCOLORS_H_ */
