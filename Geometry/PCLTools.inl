/*
template <class PointT>
typename PCLTools<PointT>::CloudType::Ptr PCLTools<PointT>::loadPointCloud(fs::path cloudPath, std::string upAxis, double scale) {
	if (cloudPath.extension() != ".pcd") return CloudType::Ptr();
	CloudType::Ptr cloud(new CloudType());
	if (pcl::io::loadPCDFile<Point>(cloudPath.string(), *cloud) == -1) {
		return CloudType::Ptr();
	}

	if (scale == 1.f && upAxis == "Z") return;
	// compute center
	float cx = 0.f, cy = 0.f, cz = 0.f;	int i=0;
	std::for_each(cloud->begin(), cloud->end(), [&] (Point& p) { 
		cx *= i; cy *= i; cz *= i++;
		cx += p.x; cy += p.y; cz += p.z;
		cx /= static_cast<float>(i); cy /= static_cast<float>(i); cz /= static_cast<float>(i);
	});

	Eigen::Matrix4f tC;
	tC << 1.f, 0.f, 0.f, -cx,
	      0.f, 1.f, 0.f, -cy,
	      0.f, 0.f, 1.f, -cz,
			0.f, 0.f, 0.f, 1.f;

	//std::for_each(cloud->begin(), cloud->end(), [&] (PointType& p) { p.x -= cx; p.y -= cy; p.z -= cz; });
	Eigen::Matrix4f tR;
	if (upAxis == "Y") {
		tR << 1.f, 0.f,  0.f, 0.f,
			   0.f, 0.f, -1.f, 0.f,
				0.f, 1.f,  0.f, 0.f,
				0.f, 0.f,  0.f, 1.f;
		//std::for_each(cloud->begin(), cloud->end(), [&] (PointType& p) { float tmp = p.y; p.y = -p.z; p.z = tmp; });
	} else if (upAxis == "X") {
		tR << 0.f, 0.f, -1.f, 0.f,
			   0.f, 1.f,  0.f, 0.f,
				1.f, 0.f,  0.f, 0.f,
				0.f, 0.f,  0.f, 1.f;
		//std::for_each(cloud->begin(), cloud->end(), [&] (PointType& p) { float tmp = p.x; p.x = -p.z; p.z = tmp; });
	} else if (upAxis == "Z") {
		tR = Eigen::Matrix4f::Identity();
	} else {
		Log::warn("Unrecognized mesh orientation given, assuming Z axis.");
		return;
	}

	pcl::transformPointCloudWithNormals(*cloud, *cloud, tR*tC);

	if (scale != 1.f) {
		Eigen::Matrix4f tS = Eigen::Matrix4f::Identity();
		tS.block<3,3>(0,0) *= scale;
		pcl::transformPointCloud(*cloud, *cloud, tS);
	}
	return cloud;
}
*/

template <class PointT>
typename PCLTools<PointT>::QuadricType::Ptr PCLTools<PointT>::fitQuadric(typename CloudType::ConstPtr cloud, const PointT& pos, NeighborQuery<PointT>& nq, Eigen::Matrix<float,3,3>* localBase) {
	Matrix3f transform;
	if (localBase) {
		transform = *localBase;
	} else {
		Eigen::Matrix<float, 3, 1> normal = pos.getNormalVector3fMap();
		transform.col(2) = normal;
		transform.col(0) = (Eigen::Matrix<float, 3, 3>::Identity() - normal*normal.transpose()).col(0).normalized();
		transform.col(1) = transform.col(2).cross(transform.col(1));
	}
	Vector3f params = localQuadricParams(cloud, nq, pos, transform);
	typename QuadricType::AMat Q;
	Q << params[0], 0.5f*params[2], 0.f, 
	0.5f*params[2], params[1], 0.f, 
	0.f, 0.f, 0.f;
	typename QuadricType::Ptr quadric(new QuadricType(Q, QuadricType::AVec::Zero(), 0.f));
	return quadric;
}

template <class PointT>
float PCLTools<PointT>::meanCurvature(typename CloudType::Ptr cloud, const NQ& nq, const PointT& pos, const IdxSet& subset) {
	// get nearest neighbors
	std::vector<int>  neighbors;
	std::vector<float> sqrDists;
	NQSearchVisitor nqVisitor(pos, nq.search, neighbors, sqrDists);
	boost::apply_visitor(nqVisitor, nq.param);
	if (subset.size()) {
		Algorithm::remove(neighbors, [&](int idx) { return !std::binary_search(subset.begin(), subset.end(), idx); });
	}
	if (!neighbors.size()) {
		std::cout << "Empty neighbor query" << "\n";
		return 0.f;
	}

	pcl::PrincipalCurvaturesEstimation<PointT, PointT> pce;
	auto pIt = cloud->insert(cloud->end(), pos);
	float x,y,z,p1,p2;
	pce.computePointPrincipalCurvatures(*cloud, cloud->size()-1, neighbors, x, y, z, p1, p2);
	cloud->erase(pIt);
	return 0.5f * (p1+p2);
	//Vector3f params = localQuadricParams(cloud, nq, pos, localBase);
	//return 0.5f * (params[0] + params[1]);
}

template <class PointT>
typename PCLTools<PointT>::Points PCLTools<PointT>::getNeighbors(typename CloudType::ConstPtr cloud, const PointT& pos, const NQ& nq) {
	IdxSet  neighbors = getNeighborIndices(cloud, pos, nq);
	std::vector<PointT> result(neighbors.size());
	std::transform(neighbors.begin(), neighbors.end(), result.begin(), [&] (unsigned int idx) { return cloud->points[idx]; });
	return result;
}

template <class PointT>
typename PCLTools<PointT>::IdxSet PCLTools<PointT>::getNeighborIndices(typename CloudType::ConstPtr cloud, const PointT& pos, const NQ& nq) {
	std::vector<int>  neighbors;
	std::vector<float> sqrDists;
	NQSearchVisitor nqVisitor(pos, nq.search, neighbors, sqrDists);
	boost::apply_visitor(nqVisitor, nq.param);
	return neighbors;
}

template <class PointT>
Vector3f PCLTools<PointT>::localQuadricParams(typename CloudType::ConstPtr cloud, NeighborQuery<PointT>& nq, const PointT& pos, const Eigen::Matrix3f& localBase) {
	auto neighbors = getNeighborIndices(cloud, pos, nq);
	Vector3f vec = pos.getVector3fMap();
	// remove points too near the input position
	Algorithm::remove(neighbors, [&] (Idx idx) {
		Vector3f pnt = cloud->points[idx].getVector3fMap();
		return (std::isnan(pnt[0]) || std::isnan(pnt[1]) || std::isnan(pnt[2]) || (pnt - vec).norm() < 0.00001f);
	});
	if (!neighbors.size()) return Vector3f::Zero();

	// setup problem
	Eigen::MatrixXf A(neighbors.size(), 3);
	Eigen::VectorXf b(neighbors.size());
	unsigned int row = 0;
	for (const auto& p : neighbors) {
		Vector3f point = cloud->points[p].getVector3fMap();
		Vector3f proj = localBase*(point-vec);
		Vector3f r(proj[0]*proj[0], proj[1]*proj[1], proj[0]*proj[1]);
		if (std::isinf(r[0]) || std::isinf(r[1]) || std::isinf(r[2])) std::cout << "inf" << "\n";
		A.row(row) = r;
		b[row++] = proj[2];
	}
	// return solution
	auto solution = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
	return solution;
}

template <class PointT>
typename PCLTools<PointT>::IdxSet PCLTools<PointT>::cloudIndices(typename CloudType::ConstPtr cloud) {
	IdxSet all(cloud->size());
	std::iota(all.begin(), all.end(), 0);
	return all;
}


/// PCLTools::NQSearchVisitor ///

template <class PointT>
PCLTools<PointT>::NQSearchVisitor::NQSearchVisitor(const PointT& query, typename SearchType::Ptr search, std::vector<int>& indices, std::vector<float>& sqrDists) : boost::static_visitor<>(), m_query(query), m_search(search), m_indices(indices), m_sqrDists(sqrDists) {
}

template <class PointT>
PCLTools<PointT>::NQSearchVisitor::~NQSearchVisitor() {
}

template <class PointT>
void PCLTools<PointT>::NQSearchVisitor::operator()(float param) {
	m_search->radiusSearch(m_query, param, m_indices, m_sqrDists);
}

template <class PointT>
void PCLTools<PointT>::NQSearchVisitor::operator()(unsigned int param) {
	m_indices.resize(param);
	m_sqrDists.resize(param);
	m_search->nearestKSearch(m_query, param, m_indices, m_sqrDists);
}

/// PCLTools::NQSetVisitor ///

template <class PointT>
template <class Algo>
PCLTools<PointT>::NQSetVisitor<Algo>::NQSetVisitor(typename SearchType::Ptr search, Algo& algo) : boost::static_visitor<>(), m_search(search), m_algo(algo) {
}

template <class PointT>
template <class Algo>
PCLTools<PointT>::NQSetVisitor<Algo>::~NQSetVisitor() {
}

template <class PointT>
template <class Algo>
void PCLTools<PointT>::NQSetVisitor<Algo>::operator()(float param) {
	m_algo.setSearchMethod(m_search);
	m_algo.setRadiusSearch(param);
}

template <class PointT>
template <class Algo>
void PCLTools<PointT>::NQSetVisitor<Algo>::operator()(unsigned int param) {
	m_algo.setSearchMethod(m_search);
	m_algo.setKSearch(param);
}
