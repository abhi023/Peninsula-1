#pragma once

template <typename t, t upper, t lower>
class ranged
{
protected:
	t _t;
	t _upper;
	t _lower;

	const t _difference = _upper - _lower;

public:
	ranged()
	{}

	ranged(t num)
		: _t(num)
	{}

	ranged(t num, t upper, t lower)
		: _t(num), _upper(upper), _lower(lower)
	{}

	void check_bounds()
	{
		if (_t > _upper)
			_t -= _difference;

		if (_t < _lower)
			_t += _difference;
	}

	t add(const t& right)
	{
		_t += right;
		
		check_bounds();
	}

	t subtract(const t& right)
	{
		_t -= right;

		check_bounds();
	}

	t multiply(const t& right)
	{
		_t *= right;

		check_bounds();
	}

	t divide(const t& right)
	{
		_t /= right;

		check_bounds();
	}
};