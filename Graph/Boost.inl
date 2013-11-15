namespace Graph {

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline Boost<Type, NodeProp, EdgeProp, GraphProp>::Boost() : Base() {
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline Boost<Type, NodeProp, EdgeProp, GraphProp>::~Boost() {
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::NodeHandle Boost<Type, NodeProp, EdgeProp, GraphProp>::addNode() {
	return ::boost::add_vertex(*this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::EdgeAddResult  Boost<Type, NodeProp, EdgeProp, GraphProp>::addEdge(NodeHandle src, NodeHandle tgt) {
	return ::boost::add_edge(src, tgt, *this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::NodeRange  Boost<Type, NodeProp, EdgeProp, GraphProp>::nodes() const {
	return NodeRange(::boost::vertices(*this));
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::EdgeRange  Boost<Type, NodeProp, EdgeProp, GraphProp>::edges() const {
	return EdgeRange(::boost::edges(*this));
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::OutEdgeRange  Boost<Type, NodeProp, EdgeProp, GraphProp>::outEdges(NodeHandle node) const {
	return OutEdgeRange(::boost::out_edges(node, *this));
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::EdgeHandles  Boost<Type, NodeProp, EdgeProp, GraphProp>::outEdges(NodeHandles nodes) const {
	EdgeHandles result;
	for (auto n : nodes) {
		auto range = outEdges(n);
		result.insert(result.end(), range.begin(), range.end());
	}
	::Algorithm::uniqueSubset(result);
	return result;
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::NodeHandle Boost<Type, NodeProp, EdgeProp, GraphProp>::source(EdgeHandle edge) const {
	return ::boost::source(edge, *this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::NodeHandle Boost<Type, NodeProp, EdgeProp, GraphProp>::target(EdgeHandle edge) const {
	return ::boost::target(edge, *this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline typename Boost<Type, NodeProp, EdgeProp, GraphProp>::EdgeNodes  Boost<Type, NodeProp, EdgeProp, GraphProp>::nodes(EdgeHandle edge) const {
	return {source(edge), target(edge)};
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline unsigned int Boost<Type, NodeProp, EdgeProp, GraphProp>::numNodes() const {
	return ::boost::num_vertices(*this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline unsigned int Boost<Type, NodeProp, EdgeProp, GraphProp>::numEdges() const {
	return ::boost::num_edges(*this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::forallNodes(NodeVisitor visitor) {
	for (auto n : nodes()) visitor(*this, n);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::forallEdges(EdgeVisitor visitor) {
	for (auto e : edges()) visitor(*this, e);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::forallOutEdges(NodeHandle node, EdgeVisitor visitor) {
	for (auto e : outEdges(node)) visitor(*this, e);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::pruneEdges(EdgePredicate pred) {
	::boost::remove_edge_if(std::bind(pred, std::ref(*this), std::placeholders::_1), *this);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
unsigned int Boost<Type, NodeProp, EdgeProp, GraphProp>::connectedComponents(Coloring& components) const {
	components.resize(numNodes());
	return ::boost::connected_components(*this, &components[0]);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
typename Boost<Type,NodeProp,EdgeProp,GraphProp>::Components Boost<Type,NodeProp,EdgeProp,GraphProp>::connectedComponents() {
	Coloring coloring;
	auto count = connectedComponents(coloring);
	Components components(count);
	unsigned int v = 0;
	for (auto vertex : nodes()) {
		components[coloring[v++]].push_back(vertex);
	}
	return components;
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
typename Boost<Type,NodeProp,EdgeProp,GraphProp>::EdgeHandles Boost<Type,NodeProp,EdgeProp,GraphProp>::edges(Component& component) {
	EdgeHandles result;
	forallEdges([&] (Boost& g, EdgeHandle e) {
		NodeHandle src, tgt;
		std::tie(src, tgt) = nodes(e);
		if (std::find(component.begin(), component.end(), src) != component.end()
		&&  std::find(component.begin(), component.end(), tgt) != component.end()) result.push_back(e);
	});
	return result;
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
void Boost<Type,NodeProp,EdgeProp,GraphProp>::bfsSearch(NodeHandle start, NodeVisitor nodeVisitor, NodePairPredicate edgeVisitor) {
	BFSVisitor vis(nodeVisitor, edgeVisitor, this);
	::boost::breadth_first_search(*this, start, visitor(vis));
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
template <class Archive>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::serialize(Archive& ar, const unsigned int version) {
	Base* base = dynamic_cast<Base*>(this);
	ar & (*base);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline Boost<Type, NodeProp, EdgeProp, GraphProp>::BFSVisitor::BFSVisitor(NodeVisitor nodeVisitor, NodePairPredicate edgeVisitor, Boost* graph) : ::boost::default_bfs_visitor(), m_nodeVisitor(nodeVisitor), m_edgeVisitor(edgeVisitor), m_graph(graph) {
	for (auto n : graph->nodes()) {
		m_valid[n] = true;
		m_visited[n] = false;
	}
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::BFSVisitor::examine_vertex(NodeHandle n, Boost) {
	m_lastNode = n;
	m_visited[n] = true;
	if (m_valid[n] && m_nodeVisitor) m_nodeVisitor(*m_graph, n);
}

template <class Type, class NodeProp, class EdgeProp, class GraphProp>
inline void Boost<Type, NodeProp, EdgeProp, GraphProp>::BFSVisitor::examine_edge(EdgeHandle e, Boost g) {
	NodeHandle src = g.source(e), tgt;
	if (src != m_lastNode) {
		src = g.target(e);
		tgt = g.source(e);
	} else {
		tgt = g.target(e);
	}
	if (!m_valid[src]) {
		m_valid[tgt] = false;
		return;
	}
	if (m_edgeVisitor) {
		bool cont = !m_visited[tgt] && m_edgeVisitor(*m_graph, src, tgt);
		if (!cont) m_valid[tgt] = false;
	}
}

} // Graph
