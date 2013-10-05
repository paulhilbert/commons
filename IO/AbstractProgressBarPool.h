#ifndef ABSTRACTPROGRESSBARPOOL_H_
#define ABSTRACTPROGRESSBARPOOL_H_

#include <mutex>

#include "AbstractProgressBar.h"

namespace IO {

class AbstractProgressBarPool {
	public:
		typedef std::shared_ptr<AbstractProgressBarPool> Ptr;
		typedef std::weak_ptr<AbstractProgressBarPool>   WPtr;

	public:
		AbstractProgressBarPool();
		virtual ~AbstractProgressBarPool();

		AbstractProgressBar::Ptr create(std::string label, int steps = 1);
		void remove(int index);

		void  setBarCountChangeCallback(std::function<void (int)> callback);

	protected:
		virtual AbstractProgressBar::Ptr createProgressBar(std::string label, int steps) = 0;
		virtual void removeProgressBar(int index) = 0;

	protected:
		unsigned int               m_count;
		std::mutex                 m_mutex;
		std::function<void (int)>  m_callback;
};

#include "AbstractProgressBarPool.inl"

} // IO

#endif /* ABSTRACTPROGRESSBARPOOL_H_ */
