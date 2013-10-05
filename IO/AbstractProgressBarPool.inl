inline AbstractProgressBarPool::AbstractProgressBarPool() : m_count(0) {
}

inline AbstractProgressBarPool::~AbstractProgressBarPool() {
}

inline AbstractProgressBar::Ptr AbstractProgressBarPool::create(std::string label, int steps) {
	std::lock_guard<std::mutex> guard(m_mutex);
	if (m_callback) m_callback(++m_count);
	auto bar = createProgressBar(label, steps);
	return bar;
}

inline void AbstractProgressBarPool::remove(int index) {
	std::lock_guard<std::mutex> guard(m_mutex);
	removeProgressBar(index);
	if (m_callback) m_callback(--m_count);
}

inline void AbstractProgressBarPool::setBarCountChangeCallback(std::function<void (int)> callback) {
	m_callback = std::move(callback);
}
