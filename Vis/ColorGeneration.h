#ifndef COLORGENERATION_H
#define COLORGENERATION_H

#include <vector>
#include <algorithm>

#include "../Random/RNG.h"
using namespace Random;

#include "ColorConversion.h"

namespace Vis {

struct ColorGeneration {
	static RGB  randomHueRGB();
	static RGBA randomHueRGBA();
	static std::vector<RGB>  randomHuesRGB(unsigned int count);
	static std::vector<RGBA> randomHuesRGBA(unsigned int count);
	static std::vector<RGB>  uniformHuesRGB(unsigned int count);
	static std::vector<RGBA> uniformHuesRGBA(unsigned int count);
};


RGB ColorGeneration::randomHueRGB() {
	RNG* rng = RNG::instance();
	float hue = rng->uniformAB<float>(0.f, 2.f*M_PI);
	return ColorConversion::hsv2rgb(HSV(hue, 1.f, 1.f));
}

RGBA ColorGeneration::randomHueRGBA() {
	RGBA rgba;
	rgba << randomHueRGB(), 1.f;
	return rgba;
}

std::vector<RGB> ColorGeneration::randomHuesRGB(unsigned int count) {
	std::vector<RGB> result;
	std::generate_n(std::back_inserter(result), count, &ColorGeneration::randomHueRGB);
	return result;
}

std::vector<RGBA> ColorGeneration::randomHuesRGBA(unsigned int count) {
	std::vector<RGBA> result;
	std::generate_n(std::back_inserter(result), count, &ColorGeneration::randomHueRGBA);
	return result;
}

std::vector<RGB> ColorGeneration::uniformHuesRGB(unsigned int count) {
	float hueStep = static_cast<float>(2.0*M_PI / count);
	std::vector<RGB> result;
	for (unsigned int i=0; i<count; ++i) result.push_back(ColorConversion::hsv2rgb(HSV(hueStep*i, 1.f, 1.f)));
	return result;
}

std::vector<RGBA> ColorGeneration::uniformHuesRGBA(unsigned int count) {
	float hueStep = static_cast<float>(2.0*M_PI / count);
	std::vector<RGBA> result;
	for (unsigned int i=0; i<count; ++i) result.push_back(ColorConversion::hsva2rgba(HSVA(hueStep*i, 1.f, 1.f, 1.f)));
	return result;
}

} /* Vis */

#endif // COLORGENERATION_H
