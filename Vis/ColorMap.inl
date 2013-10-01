template <class Scalar>
ColorMap<RGB, Scalar> rgbJet() {
	ColorMap<RGBA, Scalar> rgbaMap = rgbaJet<Scalar>();
	ColorMap<RGB, Scalar> map = [=] (Scalar value) {
		RGBA rgba = rgbaMap(value);
		return RGB(rgba[0], rgba[1], rgba[2]);
	};
	return map;
}

template <class Scalar>
ColorMap<RGBA, Scalar> rgbaJet() {
	ColorMap<RGBA, Scalar> map = [] (Scalar value) {
		Scalar fourValue = 4.f * value;
		Scalar red   = std::min(fourValue - 1.5, -fourValue + 4.5);
		Scalar green = std::min(fourValue - 0.5, -fourValue + 3.5);
		Scalar blue  = std::min(fourValue + 0.5, -fourValue + 2.5);

		RGBA result(red, green, blue, Scalar(1));
		Eigen::clamp(result, Scalar(0), Scalar(1));
		return result;
	};
	return map;
}

template <class Scalar>
ColorMap<HSV, Scalar> hsvJet() {
	ColorMap<RGB, Scalar> rgbMap = rgbJet<Scalar>();
	ColorMap<HSV, Scalar> map = [=] (Scalar value) {
		RGB rgb = rgbMap(value);
		return ColorConversion::rgb2hsv(rgb);
	};
	return map;
}

template <class Scalar>
ColorMap<HSVA, Scalar> hsvaJet() {
	ColorMap<RGBA, Scalar> rgbaMap = rgbaJet<Scalar>();
	ColorMap<HSVA, Scalar> map = [=] (Scalar value) {
		RGBA rgba = rgbaMap(value);
		return ColorConversion::rgba2hsva(rgba);
	};
	return map;
}
