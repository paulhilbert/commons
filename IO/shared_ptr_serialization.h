#ifndef SHARED_PTR_SERIALIZATION_H_
#define SHARED_PTR_SERIALIZATION_H_

#include <unordered_map>
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<class Archive, class Type>
void save(Archive & archive, const std::shared_ptr<Type> & value, const unsigned int /*version*/) {
	Type *data = value.get();
	archive << data;
}

template<class Archive, class Type>
void load(Archive & archive, std::shared_ptr<Type> & value, const unsigned int /*version*/) {
	Type *data;
	archive >> data;

	static std::unordered_map<void*, std::weak_ptr<Type>> hash;

	value = hash[data].lock();
	if (!value) {
		value = std::shared_ptr<Type>(data);
		hash[data] = value;
	}
}

template<class Archive, class Type>
inline void serialize(Archive & archive, std::shared_ptr<Type> & value, const unsigned int version) {
	split_free(archive, value, version);
}

} // serialization
} // boost

#endif /* SHARED_PTR_SERIALIZATION_H_ */
