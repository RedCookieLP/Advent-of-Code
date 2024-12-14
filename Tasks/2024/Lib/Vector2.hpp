#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <typeinfo>
#include <utility>
#include <cmath>
#include <HashHelper.hpp>

template <typename T>
class Vector2
{
	static_assert(std::is_arithmetic_v<T>, "T must be a numerical type!");
public:
	// construction and destruction
	constexpr Vector2() noexcept
		: m_x{static_cast<T>(0)}, m_y{static_cast<T>(0)}
	{}
	template <typename S, typename R>
	inline Vector2(S x, R y) noexcept
		: m_x{static_cast<T>(x)}, m_y{static_cast<T>(y)}
	{}
	template <typename S, typename R>
	inline Vector2(const std::pair<S,R>& numbers) noexcept
		: m_x{static_cast<T>(numbers.first)}, m_y{static_cast<T>(numbers.second)}
	{}
	~Vector2() = default;
	template <typename S>
	inline Vector2(const Vector2<S>& other) noexcept
		: m_x{static_cast<T>(other.m_x)}, m_y{static_cast<T>(other.m_y)}
	{}
	template <typename S>
	inline Vector2(Vector2<S>&& other) noexcept
		: m_x{static_cast<T>(std::move(other.m_x))}, m_y{static_cast<T>(std::move(other.m_y))}
	{}
	template <typename S>
	inline Vector2& operator=(const Vector2<S>& other) noexcept
	{
		m_x = static_cast<T>(other.m_x);
		m_y = static_cast<T>(other.m_y);
		return *this;
	}
	template <typename S>
	inline Vector2& operator=(Vector2<S>&& other) noexcept
	{
		m_x = static_cast<T>(std::move(other.m_x));
		m_y = static_cast<T>(std::move(other.m_y));
		return *this;
	}
public:
	// comparison operators
	template <typename S>
	inline bool operator==(const Vector2<S>& other) const noexcept
	{
		return (m_x == static_cast<T>(other.m_x) && m_y == static_cast<T>(other.m_y));
	}
	template <typename S>
	inline bool operator!=(const Vector2<S>& other) const noexcept
	{
		return !(*this == other);
	}
	template <typename S>
	inline bool operator>(const Vector2<S>& other) const noexcept
	{
		return (this->len2() > other.len2());
	}
	template <typename S>
	inline bool operator<(const Vector2<S>& other) const noexcept
	{
		return !((*this == other) || (*this > other));
	}
	template <typename S>
	inline bool operator>=(const Vector2<S>& other) const noexcept
	{
		return ((*this == other) || (*this > other));
	}
	template <typename S>
	inline bool operator<=(const Vector2<S>& other) const noexcept
	{
		return ((*this == other) || (*this < other));
	}
public:
	// arithmetic operator
	template <typename S>
	inline Vector2<T> operator+(const Vector2<S>& other) const noexcept
	{
		return Vector2{m_x + static_cast<T>(other.m_x), m_y + static_cast<T>(other.m_y)};
	}
	template <typename S>
	inline Vector2<T> operator-(const Vector2<S>& other) const noexcept
	{
		return Vector2{m_x - static_cast<T>(other.m_x), m_y - static_cast<T>(other.m_y)};
	}
	template <typename S>
	inline Vector2<T>& operator+=(const Vector2<S>& other) noexcept
	{
		return (*this = (*this + other));
	}
	template <typename S>
	inline Vector2<T>& operator-=(const Vector2<S>& other) noexcept
	{
		return (*this = (*this - other));
	}
	inline Vector2 operator-() const noexcept
	{
		return {-m_x, -m_y};
	}
	template <typename S>
	inline Vector2<T> operator*(S scalar) const noexcept
	{
		static_assert(std::is_arithmetic_v<S>, "scalar-type must be arithmetic!");
		return Vector2<T>{m_x * scalar, m_y * scalar};
	}
	template <typename S>
	inline Vector2<T>& operator*=(S scalar) noexcept
	{
		static_assert(std::is_arithmetic_v<S>, "scalar-type must be arithmetic!");
		return (*this = (*this * scalar));
	}
	template <typename S>
	inline Vector2<T> operator/(S scalar) const noexcept
	{
		static_assert(std::is_arithmetic_v<S>, "scalar-type must be arithmetic!");
		return Vector2<T>{m_x / scalar, m_y / scalar};
	}
	template <typename S>
	inline Vector2<T>& operator/=(S scalar) noexcept
	{
		static_assert(std::is_arithmetic_v<S>, "scalar-type must be arithmetic!");
		return (*this = (*this / scalar));
	}
public:
	// access methods
	inline T x() const noexcept { return this->m_x; }
	inline T y() const noexcept { return this->m_y; }
	inline T& x() noexcept { return this->m_x; }
	inline T& y() noexcept { return this->m_y; }
	inline constexpr T operator[](size_t idx) const noexcept
	{
		if (idx >= 2u)
			throw std::invalid_argument("idx out of range (idx: " + std::to_string(idx) + ")");
		else
			return (idx == 0u ? m_x : m_y);
	}
	inline constexpr T& operator[](size_t idx) noexcept
	{
		if (idx >= 2u)
			throw std::invalid_argument("idx out of range (idx: " + std::to_string(idx) + ")");
		else
			return (idx == 0u ? m_x : m_y);
	}
public:
	// data methods
	template <typename S = double>
	inline S len2() const noexcept
	{
		return ((static_cast<S>(m_x)*static_cast<S>(m_x)) + (static_cast<S>(m_y)*static_cast<S>(m_y)));
	}
	template <typename S = double>
	inline S len() const noexcept
	{
		return static_cast<S>(std::sqrt<T>(this->len()));
	}
	/**
	 * @brief Returns whether this vector contains the vector `other` inside a square, spanning from [0,0] to (this).
	 * @tparam S Type of `other`s data
	 * @param other The vector to check for
	 * @return `true` if contained, `false` otherwise
	 */
	template <typename S>
	inline bool contains(const Vector2<S>& other) const noexcept
	{
		return (other >= Vector2{} && other.m_x < m_x && other.m_y < m_y);
	}
protected:
	T m_x;
	T m_y;
};

template<typename T>
struct std::hash<Vector2<T>>
{
	inline size_t operator()(const Vector2<T>& vec) const noexcept
	{
		return Hash::combine<T,T>(vec.x(), vec.y());
	}
};

// Specializations
using Vector2d = Vector2<double>;
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2u8 = Vector2<uint8_t>;
using Vector2u16 = Vector2<uint16_t>;
using Vector2u32 = Vector2<uint32_t>;
using Vector2u64 = Vector2<uint64_t>;
using Vector2i8 = Vector2<int8_t>;
using Vector2i16 = Vector2<int16_t>;
using Vector2i32 = Vector2<int32_t>;
using Vector2i64 = Vector2<int64_t>;

#endif // VECTOR2_HPP
