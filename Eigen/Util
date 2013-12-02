#ifndef EIGENUTIL_H_
#define EIGENUTIL_H_

#include <Eigen/Dense>

namespace Eigen {

template <class T>
Matrix<T,3,1> up(const Matrix<T,3,1>& ref);

template <class T>
Matrix<T,3,1> right(const Matrix<T,3,1>& ref);

template <class Scalar, int Rows, int Cols>
void clamp(Matrix<Scalar, Rows, Cols>& m, Scalar min, Scalar max);

template <class Scalar, int Rows, int Cols>
void gramSchmidt(Matrix<Scalar, Rows, Cols>& m, bool normalize = false);

#include "Util.inl"

} // Eigen

#endif /* EIGENUTIL_H_ */
