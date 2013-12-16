#ifndef GRAPHBOOST_H_
#define GRAPHBOOST_H_

#include <memory>
#include <tuple>
#include <functional>
#include <type_traits>

#include <Generic/Range.h>
#include <Algorithm/Sets.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <boost/serialization/access.hpp>

namespace Graph {

// use these as Type param below
struct DirTag   { typedef ::boost::directedS      type; };
struct UnDirTag { typedef ::boost::undirectedS    type; };
struct BiDirTag { typedef ::boost::bidirectionalS type; };

typedef ::boost::no_property NullProp;

// shortener t'defs
template <bool multiEdge>
using OutEdgeListType = typename std::conditional<multiEdge, boost::vecS, boost::setS>::type;
template <class Type, bool multiEdge, class NodeProp, class EdgeProp, class GraphProp>
using GraphBase = ::boost::adjacency_list<OutEdgeListType<multiEdge>, boost::vecS, typename Type::type, NodeProp, EdgeProp, GraphProp>;
template <class Base>
using GraphTraits = ::boost::graph_traits<Base>;



template <class Type, bool multiEdge, class NodeProp = NullProp, class EdgeProp = NullProp, class GraphProp = NullProp>
class Boost : public GraphBase<Type, multiEdge, NodeProp, EdgeProp, GraphProp> {
	public:
		typedef std::shared_ptr<Boost> Ptr;
		typedef std::weak_ptr<Boost>   WPtr;

		typedef GraphBase<Type, multiEdge, NodeProp, EdgeProp, GraphProp>   Base;
		typedef GraphTraits<Base>                                           Traits;

		typedef typename Traits::vertex_descriptor                          NodeHandle;
		typedef typename Traits::edge_descriptor                            EdgeHandle;

		typedef std::vector<NodeHandle>                                     NodeHandles;
		typedef std::vector<EdgeHandle>                                     EdgeHandles;

		typedef typename Traits::vertex_iterator                            NodeIter;
		typedef typename Traits::edge_iterator                              EdgeIter;
		typedef typename Traits::out_edge_iterator                          OutEdgeIter;
		typedef Generic::Range<NodeIter>                                    NodeRange;
		typedef Generic::Range<EdgeIter>                                    EdgeRange;
		typedef Generic::Range<OutEdgeIter>                                 OutEdgeRange;

		typedef std::pair<EdgeHandle, bool>                                 EdgeAddResult;
		typedef std::pair<NodeHandle, NodeHandle>                           EdgeNodes;

		typedef std::function<void (Boost&, NodeHandle)>                    NodeVisitor;
		typedef std::function<void (Boost&, EdgeHandle)>                    EdgeVisitor;
		typedef std::function<bool (Boost&, EdgeHandle)>                    EdgePredicate;
		typedef std::function<bool (Boost&, NodeHandle, NodeHandle)>        NodePairPredicate;

		typedef std::vector<unsigned int>                                   Coloring;
		typedef std::vector<NodeHandle>                                     Component;
		typedef std::vector<Component>                                      Components;

	public:
		Boost();
		virtual ~Boost();

		NodeHandle     addNode();
		EdgeAddResult  addEdge(NodeHandle src, NodeHandle tgt);

		NodeRange      nodes() const;
		EdgeRange      edges() const;

		OutEdgeRange   outEdges(NodeHandle node) const;
		EdgeHandles    outEdges(NodeHandles nodes) const;
		NodeHandle     source(EdgeHandle edge) const;
		NodeHandle     target(EdgeHandle edge) const;
		EdgeNodes      nodes(EdgeHandle edge) const;

		unsigned int   numNodes() const;
		unsigned int   numEdges() const;

		void           forallNodes(NodeVisitor visitor);
		void           forallEdges(EdgeVisitor visitor);
		void           forallOutEdges(NodeHandle node, EdgeVisitor visitor);

		void           pruneEdges(EdgePredicate pred);

		// connected components
		unsigned int   connectedComponents(Coloring& components) const;
		Components     connectedComponents();
		EdgeHandles    edges(Component& component);

		/**
		 *  Performs a breadth-first-search on this graph.
		 *
		 *  @param start Node handle used as starting point.
		 *  @param nodeVisitor Visitor that gets called everytime a node is discovered.
		 *  @param edgeVisitor Predicate that gets called everytime an edge is discovered. Returning false cancels the corresponding path.
		 */
		void bfsSearch(NodeHandle start, NodeVisitor nodeVisitor = nullptr, NodePairPredicate edgeVisitor = nullptr);

	protected:
		// serialization
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version);

		struct BFSVisitor : public ::boost::default_bfs_visitor {
			BFSVisitor(NodeVisitor nodeVisitor, NodePairPredicate edgeVisitor, Boost* graph);
			void examine_vertex(NodeHandle n, Boost g);
			void examine_edge(EdgeHandle e, Boost g);

			NodeVisitor                 m_nodeVisitor;
			NodePairPredicate           m_edgeVisitor;

			Boost*                      m_graph;

			NodeHandle                  m_lastNode;
			std::map<NodeHandle, bool>  m_valid;
			std::map<NodeHandle, bool>  m_visited;

		};
};

} // Graph

#include "Boost.inl"


#endif /* GRAPHBOOST_H_ */
