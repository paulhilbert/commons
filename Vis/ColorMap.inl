template <class Scalar>
std::function<RGB (Scalar)> rgbJet() {
	std::function<RGBA (Scalar)> rgbaMap = rgbaJet<Scalar>();
	std::function<RGB (Scalar)> map = [=] (Scalar value) {
		RGBA rgba = rgbaMap(value);
		return RGB(rgba[0], rgba[1], rgba[2]);
	};
	return map;
}

template <class Scalar>
std::function<RGBA (Scalar)> rgbaJet() {
	std::function<RGBA (Scalar)> map = [] (Scalar value) {
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
std::function<HSV (Scalar)> hsvJet() {
	std::function<RGB (Scalar)> rgbMap = rgbJet<Scalar>();
	std::function<HSV (Scalar)> map = [=] (Scalar value) {
		RGB rgb = rgbMap(value);
		return ColorConversion::rgb2hsv(rgb);
	};
	return map;
}

template <class Scalar>
std::function<HSVA (Scalar)> hsvaJet() {
	std::function<RGBA (Scalar)> rgbaMap = rgbaJet<Scalar>();
	std::function<HSVA (Scalar)> map = [=] (Scalar value) {
		RGBA rgba = rgbaMap(value);
		return ColorConversion::rgba2hsva(rgba);
	};
	return map;
}
