#ifndef MESHANALYSIS_H
#define MESHANALYSIS_H

#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <memory>
#include <set>

#include "../Random/RNG.h"
using namespace Random;

#include "CloudAnalysis.h"
#include "BoundingBox.h"

#ifdef USE_PCL
#include <pcl/point_cloud.h>
#endif


namespace Geometry {


template <class MeshTraits>
class MeshAnalysis {
	public:
		typedef typename MeshTraits::MeshType    Mesh;
		typedef typename MeshTraits::ScalarType  Scalar;
		typedef typename MeshTraits::PointType   Point;
		typedef typename MeshTraits::FaceId      FId;

		typedef std::set<FId>                    FIdSet;
		typedef FIdSet                           CC;
		typedef std::vector<CC>                  CCs;

	public:
		static Point vertexCOM(Mesh& mesh, unsigned int normalizeAfter = 0);
		static CCs connectedComponents(Mesh& mesh);

		static std::vector<Point> sampleOnSurface(const Mesh& mesh, unsigned int samplesPerSquareUnit);
#ifdef USE_PCL
		template <class PointType>
		static void sampleOnSurface(const Mesh& mesh, unsigned int samplesPerSquareUnit, typename pcl::PointCloud<PointType>::Ptr cloud, std::function<void (int done, int todo)> bar = nullptr);
#endif

		static Scalar faceArea(const Mesh& mesh, FId face);
		static Scalar surfaceArea(const Mesh& mesh);

	protected:
		typedef typename RNG::Traits<Scalar>::Generator  RNG01;

	protected:
		static CC ccTraverse(Mesh& mesh, std::map<FId,bool>& done, FId start);
		static Point randomPointInTriangle(const Mesh& mesh, const FId& face, RNG01& rng);
		static std::vector<Scalar> faceAreas(const Mesh& mesh);
};


#include "MeshAnalysis.inl"

} /* Geometry */

#endif // MESHANALYSIS_H
