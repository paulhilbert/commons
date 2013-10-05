#ifndef ABSTRACTPROGRESSBAR_H_
#define ABSTRACTPROGRESSBAR_H_

#include <iostream>
#include <memory>
#include <string>

namespace IO {

class AbstractProgressBar {
	public:
		typedef std::shared_ptr<AbstractProgressBar> Ptr;
		typedef std::weak_ptr<AbstractProgressBar>   WPtr;

	public:
		AbstractProgressBar(std::string label, int steps = 1);
		virtual ~AbstractProgressBar();

		void setSteps(int steps);

		virtual void poll(float progress) = 0;
		void poll(unsigned int done, unsigned int todo);

	protected:
		std::string  m_label;
		int          m_steps;
};

#include "AbstractProgressBar.inl"

} // IO

#endif /* ABSTRACTPROGRESSBAR_H_ */
