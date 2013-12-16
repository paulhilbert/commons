#ifndef OPENMESHTRAITS_H
#define OPENMESHTRAITS_H

#include <stdexcept>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <Eigen/Dense>


#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>

namespace Geometry {

template <class OpenMeshType>
struct OpenMeshTraits {
	// types
	typedef OpenMeshType    MeshType;

	typedef float           ScalarType;
	typedef Eigen::Vector3f PositionType;
	typedef Eigen::Vector3f NormalType;
	typedef Eigen::Vector4f ColorType;

	typedef int             VertexId;
	typedef int             FaceId;
	typedef int             Size;

	// mesh IO
	static bool loadFromFile(MeshType& mesh, const std::string& path);
	static bool saveToFile(const MeshType& mesh, const std::string& path);

	static void adjust(MeshType& mesh, const Eigen::Matrix3f& transform, float scale = 1.f, bool recenter = false);
	static void adjust(MeshType& mesh, const Eigen::Vector3f& up, const Eigen::Vector3f& front, float scale = 1.f, bool recenter = false);
	static void adjust(MeshType& mesh, const std::string& up = "Z", const std::string& front = "Y", float scale = 1.f, bool recenter = false);

	// mesh properties
	static Size numVertices(const MeshType& mesh);
	static Size numFaces(const MeshType& mesh);

	// element access
	static std::vector<VertexId> vertices(const MeshType& mesh);
	static std::vector<FaceId> faces(const MeshType& mesh);

	// element properties
	static PositionType vertexPosition(const MeshType& mesh, VertexId id);
	static NormalType vertexNormal(const MeshType& mesh, VertexId id);
	static ColorType vertexColor(const MeshType& mesh, VertexId id);

	static NormalType faceNormal(const MeshType& mesh, FaceId id);

	static std::vector<PositionType> vertexPositions(const MeshType& mesh);
	static std::vector<NormalType> vertexNormals(const MeshType& mesh);
	static std::vector<ColorType> vertexColors(const MeshType& mesh);

	static std::vector<NormalType> faceNormals(const MeshType& mesh);

	// topology
	static std::vector<VertexId> faceVertices(const MeshType& mesh, FaceId face);
	static std::vector<FaceId> adjacentFaces(const MeshType& mesh, FaceId face);

	// operators
	static ScalarType norm(const PositionType& p);
	static PositionType crossP(const PositionType& p0, const PositionType& p1);

	// transformation
	static void transform(MeshType& mesh, const Eigen::Affine3f& transformation);
};


struct InternalOpenMeshTraits : public OpenMesh::DefaultTraits {
	typedef OpenMesh::Vec4f Color;

	VertexAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color );
	FaceAttributes( OpenMesh::Attributes::Normal );
};

typedef OpenMesh::TriMesh_ArrayKernelT<InternalOpenMeshTraits>   TriMesh;
typedef OpenMesh::PolyMesh_ArrayKernelT<InternalOpenMeshTraits>  PolyMesh;



#include "OpenMeshTraits.inl"

} // Geometry

#endif // OPENMESHTRAITS_H
