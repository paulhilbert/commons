template <class T>
inline T RNG::uniform01() {
	auto gen = uniform01Gen<T>();
	return gen();
}

template <class T>
inline T RNG::uniformAB(T a, T b) {
	auto gen = uniformABGen<T>(a, b);
	return gen();
}

template <class T>
inline T RNG::geometric(float p) {
	auto gen = geometricGen<T>(p);
	return gen();
}

template <class T>
inline T RNG::normal(T mean, T stdDev) {
	auto gen = normalGen<T>(mean, stdDev);
	return gen();
}

template <class T>
inline typename RNG::Traits<T>::Generator RNG::uniform01Gen(bool deterministic) {
	auto gen = generator(deterministic);
	typename Traits<T>::Dist01 dist(T(0), T(1));
	return std::bind(dist, gen);
}

template <class T>
inline typename RNG::Traits<T>::Generator RNG::uniformABGen(T a, T b, bool deterministic) {
	// uniform_real gives [a,b) and uniform_int [a,b];
	// getUpperBound(b, ...) is b-1 for is_floating_point<T> == false_type
	auto gen = generator(deterministic);
	typename Traits<T>::DistAB dist( a, Traits<T>::getUpperBound(b, std::is_floating_point<T>()) );
	return std::bind(dist, gen);
}

template <class T>
inline typename RNG::Traits<T>::Generator RNG::geometricGen(float p, bool deterministic) {
	auto gen = generator(deterministic);
	typename Traits<T>::DistGeom dist(p);
	return std::bind(dist, gen);
}

template <class T>
inline typename RNG::Traits<T>::Generator RNG::normalGen(T mean, T stdDev, bool deterministic) {
	auto gen = generator(deterministic);
	typename Traits<T>::DistNormal dist(mean, stdDev);
	return std::bind(dist, gen);
}

//template <class T>
//inline typename RNG::Traits<T>::Generator RNG::normalGen(const T& mean, const T& funcX, const T& funcY, bool deterministic) {
//	typedef Minimize<T,1,1> Minimizer;
//	typedef typename Minimizer::Func Func;
//	typedef typename Minimizer::Deriv Deriv;
//	typedef typename Minimizer::Inputs Inputs;
//	typedef typename Minimizer::Values Values;
//	typedef typename Minimizer::Jacobian Jacobian;
//
//	T y_times_sqrt_2pi = funcY * sqrt(2*M_PI);
//	T shifted_x_sqr = (funcX - mean)*(funcX - mean);
//	Func func = [&] (Inputs& x, Values& y) {
//		y(0,0) = shifted_x_sqr + 2.f*x(0,0)*x(0,0) * log(x(0,0)*y_times_sqrt_2pi);
//	};
//	Deriv deriv = [&] (Inputs& x, Jacobian& y) {
//		y(0,0) = x(0,0) * (T(4.0) * log(x(0,0) * y_times_sqrt_2pi) + T(2.0));
//	};
//	// start value
//	Inputs sigmaStart(1); // 1 is size here
//	sigmaStart(0,0) = mean + T(1.0);
//	// solve
//	T sigma = Minimizer::solve(func, deriv, sigmaStart)(0,0);
//	return normalGen(mean, std::sqrt(sigma), deterministic);
//}

inline RNG::InternalGenerator RNG::generator(bool deterministic) {
	InternalGenerator gen;
	if (!deterministic) {
		unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
		gen.seed(seed);
	}
	return gen;
}
