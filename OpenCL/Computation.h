#ifndef OPENCLCOMPUTATION_H_
#define OPENCLCOMPUTATION_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include <boost/optional.hpp>
#include <boost/none.hpp>
using boost::optional;
using boost::none;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "../IO/Log.h"
using namespace IO;

#include <CL/cl.hpp>

#include "MatrixBuffer.h"
#include "Error.h"

namespace OpenCL {

const cl_mem_flags DEV_R   = CL_MEM_READ_ONLY;
const cl_mem_flags DEV_W   = CL_MEM_WRITE_ONLY;
const cl_mem_flags DEV_RW  = CL_MEM_READ_WRITE;
const cl_mem_flags HOST_R  = 0;
const cl_mem_flags HOST_W  = 0;
const cl_mem_flags HOST_N  = 0;

class Computation {
	public:
		typedef std::shared_ptr<Computation>        Ptr;
		typedef std::weak_ptr<Computation>          WPtr;
		typedef std::shared_ptr<cl::Context>        ContextPtr;
		typedef std::shared_ptr<cl::Program>        ProgramPtr;
		typedef std::vector<cl::Device>             Devices;
		typedef std::shared_ptr<Devices>            DevicesPtr;
		typedef std::shared_ptr<cl::KernelFunctor>  FunctorPtr;
		typedef std::shared_ptr<cl::CommandQueue>   QueuePtr;

	public:
		virtual ~Computation() {}

		static Ptr fromSourceFiles(const std::vector<std::string>& paths, optional<std::string> sourceRoot = none, std::string additionalOptions = "") {
			Computation::Ptr self(new Computation());

			cl_int err;

			// platform
			std::vector<cl::Platform> platforms;
			err = cl::Platform::get(&platforms);
			if (!checkGetPlatform(err)) return Ptr();

			// device
			self->m_devices = DevicesPtr(new Devices());
			err = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, self->m_devices.get());
			if (!checkGetDevices(err, self->m_devices->size())) return Ptr();
			
			// context
			self->m_context = ContextPtr(new cl::Context(*(self->m_devices), NULL, NULL, NULL, &err));
			if (!checkGetContext(err)) return Ptr();

			// load sources
			if (!paths.size()) {
				Log::error("Empty source path set given.");
				return Ptr();
			}
			std::vector<std::string> codes;
			cl::Program::Sources sources;
			std::ifstream in;
			for (const auto& p : paths) {
				fs::path fullPath(sourceRoot ? sourceRoot.get() : p);
				if (sourceRoot) fullPath /= p;
				in.open(fullPath.string().c_str());
				if (!in.good()) {
					Log::error("Could not load file \""+fullPath.string()+"\".");
					return Ptr();
				}
				std::istreambuf_iterator<char> begin(in), end;
				std::string code(begin, end);
				in.close();
				codes.push_back(code);
				sources.push_back({code.c_str(), code.length()+1});
			}

			// program
			self->m_program = ProgramPtr(new cl::Program(*(self->m_context), sources, &err));
			if (!checkCreateProgram(err))	return Ptr();
			
			std::string buildOptions = sourceRoot ? "-I" + sourceRoot.get() : "";
			if (additionalOptions != "") buildOptions += " " + additionalOptions;
			err = self->m_program->build(*(self->m_devices), buildOptions != "" ? buildOptions.c_str() : nullptr);
			if (!checkBuildProgram(err, self->m_devices, self->m_program)) return Ptr();

			self->m_queue = QueuePtr(new cl::CommandQueue(*(self->m_context), (*(self->m_devices))[0], 0, &err));
			if (!checkCreateQueue(err)) return Ptr();

			self->m_allocated = std::shared_ptr<unsigned long>(new unsigned long(0));

			return self;
		}

		ContextPtr  getContext();
		ProgramPtr  getProgram();
		DevicesPtr  getDevices();
		QueuePtr    getQueue();
		FunctorPtr  getFunctor(std::string functionName, const cl::NDRange& global, const cl::NDRange& local = cl::NullRange, const cl::NDRange& offset = cl::NullRange);

		cl_ulong    memInBytesGlobal() const;
		cl_ulong    memInBytesPerAlloc() const;
		cl_ulong    memInBytesAvailable() const;
		double      memInKBytesGlobal() const;
		double      memInKBytesPerAlloc() const;
		double      memInKBytesAvailable() const;
		double      memInMBytesGlobal() const;
		double      memInMBytesPerAlloc() const;
		double      memInMBytesAvailable() const;

		template <class T>
		unsigned long numValuesPerAlloc() const;
		template <class T>
		unsigned long numValuesGlobal() const;
		template <class T>
		unsigned long numValuesAvailable() const;

		template <class... Args>
		bool run(FunctorPtr functor, Args... args);

		template <int blockSize, class Scalar, class... Args>
		bool runBlock1D(unsigned int dim, Eigen::VectorXf& result, std::string funcName, Args... args);

		template <class Scalar, class... Args>
		bool runBlock1DDyn(unsigned int dim, Eigen::VectorXf& result, std::string funcName, Args... args);

		template <int rowCount, class Scalar, class... Args>
		bool runBlock2D(unsigned int dim, Eigen::MatrixXf& result, std::string funcName, Args... args);

		template <class Scalar, class... Args>
		bool runBlock2DDyn(unsigned int dim, Eigen::MatrixXf& result, std::string funcName, Args... args);

#ifdef OPENCL_USE_EIGEN
		template <class Scalar, int Dim>
		typename MatrixBuffer<Scalar>::Ptr getMatrixBuffer(const std::vector<Eigen::Matrix<Scalar, Dim, 1>>& vectors, cl_int mode);
		template <class Scalar>
		typename MatrixBuffer<Scalar>::Ptr getMatrixBuffer(const Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>& matrix, cl_int mode);
#endif
#ifdef OPENCL_USE_PCL
		typename MatrixBuffer<cl_float>::Ptr getMatrixBuffer(const pcl::PointCloud<pcl::PointNormal>& cloud, cl_int mode, const std::vector<int>& subset = std::vector<int>());
#endif
		template <class Scalar>
		typename MatrixBuffer<Scalar>::Ptr getMatrixBuffer(int rows, int cols, cl_int mode);

	protected:
		template <class Scalar>
		void        checkAvailableSpace(unsigned long count);
		static bool checkGetPlatform(cl_int err);
		static bool checkGetDevices(cl_int err, unsigned int deviceCount);
		static bool checkGetContext(cl_int err);
		static bool checkCreateProgram(cl_int err);
		static bool checkBuildProgram(cl_int err);
		static bool checkBuildProgram(cl_int err, DevicesPtr devices, ProgramPtr program);
		static bool checkCreateQueue(cl_int err);
		static bool checkRunKernel(cl_int err);
		static bool checkGetFunctor(cl_int err);

	private:
		Computation() {}

	protected:
		ContextPtr m_context;
		ProgramPtr m_program;
		DevicesPtr m_devices;
		QueuePtr   m_queue;
		std::shared_ptr<unsigned long> m_allocated;
};

#include "Computation.inl"

} // OpenCL

#endif /* OPENCLCOMPUTATION_H_ */
