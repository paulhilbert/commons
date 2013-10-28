template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::Point MeshAnalysis<MeshTraits>::vertexCOM(Mesh& mesh, unsigned int normalizeAfter) {
	auto ps = MeshTraits::pointSet(mesh);
	BoundingBox<MeshTraits> bb(ps);
	Scalar invScale = MeshTraits::norm(bb.getRange());
	Point  invTrans = bb.getCenter();
	std::for_each(ps.begin(), ps.end(), [=](Point& p) { p = (1.f/invScale) * (p-invTrans); });
	Point com = CloudAnalysis<std::vector<Point>>::centerOfMass(ps);
	return com*invScale + invTrans;
}

template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::CCs MeshAnalysis<MeshTraits>::connectedComponents(Mesh& mesh) {
	FIdSet faceIds = MeshTraits::faceIdSet(mesh);
	if (!faceIds.size()) return CCs();

	std::map<FId,bool> done;
	std::transform(faceIds.begin(), faceIds.end(), std::inserter(done,done.begin()), [&](FId id) { return std::make_pair(id, false); });

	CCs result;
	while (true) {
		auto next = std::find_if(done.begin(), done.end(), [](std::pair<FId,bool> v) { return !v.second; });
		if (next == done.end()) break;
		result.push_back(ccTraverse(mesh, done, next->first));
	}

	return result;
}


template <class MeshTraits>
std::vector<typename MeshAnalysis<MeshTraits>::Point> MeshAnalysis<MeshTraits>::sampleOnSurface(const Mesh& mesh, unsigned int samplesPerSquareUnit) {
	// get cumulative face area histogram
	auto hist = faceAreas(mesh);
	std::partial_sum(hist.begin(), hist.end(), hist.begin());
	// compute sample count
	Scalar surfaceArea = hist.back();
	unsigned int numSamples = static_cast<unsigned int>(surfaceArea * static_cast<float>(samplesPerSquareUnit));

	// sample
	std::vector<Point> samples(numSamples);
	auto rng01 = RNG::uniform01Gen<Scalar>();
	auto faceIds = MeshTraits::faceIdSet(mesh);
	for (auto it = samples.begin(); it != samples.end(); ++it) {
		// get random triangle using face areas as probability distribution
		auto areaIt = std::lower_bound(hist.begin(), hist.end(), surfaceArea * rng01());
		// draw sample for drawn face
		auto faceIter = faceIds.begin();
		std::advance(faceIter, std::distance(hist.begin(), areaIt));
		auto faceId = *faceIter;
		*it = randomPointInTriangle(mesh, faceId, rng01);
	}

	return samples;
}

#ifdef USE_PCL
template <class MeshTraits>
template <class PointType>
inline void MeshAnalysis<MeshTraits>::sampleOnSurface(const Mesh& mesh, unsigned int samplesPerSquareUnit, typename pcl::PointCloud<PointType>::Ptr cloud, std::function<void (int done, int todo)> bar) {
	// get cumulative face area histogram
	auto hist = faceAreas(mesh);
	std::partial_sum(hist.begin(), hist.end(), hist.begin());
	// compute sample count
	Scalar surfaceArea = hist.back();
	unsigned int numSamples = static_cast<unsigned int>(surfaceArea * static_cast<float>(samplesPerSquareUnit));

	// sample
	cloud->resize(numSamples);
	auto rng01 = RNG::uniform01Gen<Scalar>();
	auto faceIds = MeshTraits::faceIdSet(mesh);
	unsigned int done = 0;
	for (auto& p : *cloud) {
		// get random triangle using face areas as probability distribution
		auto areaIt = std::lower_bound(hist.begin(), hist.end(), surfaceArea * rng01());
		// draw sample for drawn face
		auto faceIter = faceIds.begin();
		std::advance(faceIter, std::distance(hist.begin(), areaIt));
		auto faceId = *faceIter;
		auto point = randomPointInTriangle(mesh, faceId, rng01);
		p.getVector3fMap() = Vector3f(point[0], point[1], point[2]);
		p.getNormalVector3fMap() = MeshTraits::faceNormal(mesh, faceId);
		if (bar) bar(++done, cloud->size());
	}
}
#endif

template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::Scalar MeshAnalysis<MeshTraits>::faceArea(const Mesh& mesh, FId face) {
	std::vector<Point> vertices = MeshTraits::facePoints(mesh, face);
	return Scalar(0.5) * MeshTraits::norm(MeshTraits::crossP(vertices[1] - vertices[0], vertices[2] - vertices[0]));
}

template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::Scalar MeshAnalysis<MeshTraits>::surfaceArea(const Mesh& mesh) {
	auto areas = faceAreas(mesh);
	return std::accumulate(areas.begin(), areas.end(), Scalar(0));
}

template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::CC MeshAnalysis<MeshTraits>::ccTraverse(Mesh& mesh, std::map<FId,bool>& done, FId start) {
	CC result;
	std::vector<FId> stack;
	stack.push_back(start);
	while (!stack.empty()) {
		FId crt = stack.back();
		stack.pop_back();
		done[crt] = true;
		result.insert(crt);
		FIdSet adj = MeshTraits::adjacentFaces(mesh, crt);
		for (auto it = adj.begin(); it != adj.end(); ++it) {
			if (!done[*it]) stack.push_back(*it);
		}
	}
	return result;
}

template <class MeshTraits>
typename MeshAnalysis<MeshTraits>::Point MeshAnalysis<MeshTraits>::randomPointInTriangle(const Mesh& mesh, const FId& face, RNG01& rng) {
	Scalar s = sqrt(rng());
	Scalar t = rng();
	Scalar st = s*t;
	std::vector<Point> vertices = MeshTraits::facePoints(mesh, face);
	return (Scalar(1) - s) * vertices[0] + (s - st) * vertices[1] + st * vertices[2];
}

template <class MeshTraits>
std::vector<typename MeshAnalysis<MeshTraits>::Scalar> MeshAnalysis<MeshTraits>::faceAreas(const Mesh& mesh) {
	FIdSet faceIds = MeshTraits::faceIdSet(mesh);
	std::vector<Scalar> areas(faceIds.size());
	std::transform(faceIds.begin(), faceIds.end(), areas.begin(), [&] (FId id) { return MeshAnalysis<MeshTraits>::faceArea(mesh, id); });
	return areas;
}
