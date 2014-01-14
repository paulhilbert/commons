#ifndef EIGEN_SERIALIZATION_HPP_
#define EIGEN_SERIALIZATION_HPP_

#include <boost/serialization/array.hpp>
#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace boost {
namespace serialization {

template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void serialize(Archive& ar, ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& m, const unsigned int file_version) {
	int rows = m.rows(), cols = m.cols();
	ar & BOOST_SERIALIZATION_NVP(rows);
	ar & BOOST_SERIALIZATION_NVP(cols);
	if (rows * cols != m.size()) m.resize(rows, cols);
	ar & make_array(m.data(), m.size());
	/*
	for (unsigned int i=0; i<m.size(); ++i) {
		ar & BOOST_SERIALIZATION_NVP(m.data()[i]);
	}
	*/
}

template <class Archive, class _Scalar, int _AmbientDim>
inline void serialize(Archive& ar, ::Eigen::AlignedBox<_Scalar, _AmbientDim>& bb, const unsigned int file_version) {
	ar & make_nvp("min", bb.min());
	ar & make_nvp("max", bb.max());
	//ar & BOOST_SERIALIZATION_NVP(bb.min());
	//ar & BOOST_SERIALIZATION_NVP(bb.max());
}

template <class Archive, class _Scalar, int _AmbientDim, int _Options>
inline void serialize(Archive& ar, ::Eigen::Hyperplane<_Scalar, _AmbientDim, _Options>& plane, const unsigned int file_version) {
	ar & make_nvp("coeffs", plane.coeffs());
	//ar & BOOST_SERIALIZATION_NVP(plane.coeffs());
}

} // serialization
} // boost

#endif /* EIGEN_SERIALIZATION_HPP_ */
