#ifndef __random__
#define __random__

#include <random>

template <class RealType, class IntType>
class Random {
public:
	Random() {
		std::random_device dev;
		_mt = std::mt19937(dev());
	}
	Random( unsigned int seed ) {
		_mt = std::mt19937(seed);
	}
	~Random() {
	}

	RealType nextReal() {
		std::uniform_real_distribution<RealType> dist;
		return dist(_mt);
	}

	RealType nextReal( const RealType min, const RealType max ) {
		std::uniform_real_distribution<RealType> dist(min, max);
		return dist(_mt);
	}

	IntType nextInt() {
		std::uniform_int_distribution<IntType> dist;
		return dist(_mt);
	}

	IntType nextInt( const IntType min, const IntType max ) {
		std::uniform_int_distribution<IntType> dist(min, max);
		return dist(_mt);
	}

private:
	std::mt19937 _mt;
};

#endif /* __random__ */