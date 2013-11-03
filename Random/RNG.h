#ifndef RNG_H_
#define RNG_H_

#include <random>
#include <chrono> // for time seeding
#include <functional>
#include <type_traits>

//#include "../Optimization/Minimize.h"
//using namespace Optimization;

namespace Random {

class RNG {
	public:
		typedef std::mt19937 InternalGenerator;

		template <class T>
		struct Traits {
			// uniform_01
			typedef std::uniform_real_distribution<T> Dist01;
			typedef typename std::conditional< std::is_floating_point<T>::value, std::uniform_real_distribution<T>, std::uniform_int_distribution<T> >::type DistAB;
			typedef std::geometric_distribution<T> DistGeom;
			typedef std::normal_distribution<T> DistNormal;

			static T getUpperBound(T original, std::true_type) { return original; }
			static T getUpperBound(T original, std::false_type) { return original-1; }

			typedef std::function<T ()> Generator;
		};

	public:
		// Direct
		template <class T>
		static T uniform01();
		template <class T>
		static T uniformAB(T a, T b);
		template <class T>
		static T geometric(float p);
		template <class T>
		static T normal(T mean = 0.f, T stdDev = 1.f);

		// Generators
		template <class T>
		static typename Traits<T>::Generator uniform01Gen(bool deterministic = false);
		template <class T>
		static typename Traits<T>::Generator uniformABGen(T a, T b, bool deterministic = false);
		template <class T>
		static typename Traits<T>::Generator geometricGen(float p, bool deterministic = false);
		template <class T>
		static typename Traits<T>::Generator normalGen(T mean = 0.f, T stdDev = 1.f, bool deterministic = false);
		//template <class T>
		//static typename Traits<T>::Generator normalGen(const T& mean, const T& funcX, const T& funcY, bool deterministic = false);

	protected:
		static InternalGenerator generator(bool deterministic);
};

#include "RNG.inl"

} // Random



#endif /* RNG_H_ */
