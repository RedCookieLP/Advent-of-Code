#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <typeinfo>
#include <utility>
#include <cmath>
#include <HashHelper.hpp>

template <typename T>
class Vector3
{
	static_assert(std::is_arithmetic_v<T>, "T must be a numerical type!");
public:
	// construction and destruction
	constexpr Vector3() noexcept
		: x{0}, y{0}, z{0u}
	{}
	template <typename S, typename R, typename Q>
	constexpr Vector3(S x, R y, Q z) noexcept
		: x{static_cast<T>(x)}, y{static_cast<T>(y)}, z{static_cast<T>{z}}
	{}
	template <typename S, typename R, typename Q>
	inline Vector3(S x, R y, Q z) noexcept
		: x{static_cast<T>(x)}, y{static_cast<T>(y)}, z{static_cast<T>{z}}
	{}
	template <typename S, typename R, typename Q>
	inline Vector3(const std::tuple<S,R,Q>& numbers) noexcept
		: x{static_cast<T>(std::get<0>(numbers))}, y{static_cast<T>(std::get<1>(numbers))}, z{static_cast<T>(std::get<2>(numbers))}
	{}
	~Vector3() = default;
	template <typename S>
	inline Vector3(const Vector3<S>& other) noexcept
		: x{static_cast<T>(other.x)}, y{static_cast<T>(other.y)}, z{static_cast<T>(other.z)}
	{}
	template <typename S>
	inline Vector3(Vector3<S>&& other) noexcept
		: x{static_cast<T>(std::move(other.x))}, y{static_cast<T>(std::move(other.y))}, z{static_cast<T>(std::move(other.z))}
	{}
	template <typename S>
	inline Vector3& operator=(const Vector3<S>& other) noexcept
	{
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
		z = static_cast<T>(other.z);
	}
	template <typename S>
	inline Vector3& operator=(Vector3<S>&& other) noexcept
	{
		x = static_cast<T>(std::move(other.x));
		y = static_cast<T>(std::move(other.y));
		z = static_cast<T>(std::move(other.z));
	}
public:
	// comparison operators
	template <typename S>
	inline bool operator==(const Vector3<S>& other) const noexcept
	{
		return (x == static_cast<T>(other.x) && y == static_cast<T>(other.y) && z == static_cast<T>(other.z));
	}
	template <typename S>
	inline bool operator!=(const Vector3<S>& other) const noexcept
	{
		return !(*this == other);
	}
	template <typename S>
	inline bool operator>(const Vector3<S>& other) const noexcept
	{
		return (this->len2() > other.len2());
	}
	template <typename S>
	inline bool operator<(const Vector3<S>& other) const noexcept
	{
		return !((*this == other) || (*this > other));
	}
	template <typename S>
	inline bool operator>=(const Vector3<S>& other) const noexcept
	{
		return ((*this == other) || (*this > other));
	}
	template <typename S>
	inline bool operator<=(const Vector3<S>& other) const noexcept
	{
		return ((*this == other) || (*this < other));
	}
public:
	// arithmetic operator
	template <typename S>
	inline Vector3<T> operator+(const Vector3<S>& other) const noexcept
	{
		return Vector3{x + static_cast<T>(other.x), y + static_cast<T>(other.y)};
	}
	template <typename S>
	inline Vector3<T> operator-(const Vector3<S>& other) const noexcept
	{
		return Vector3{x - static_cast<T>(other.x), y - static_cast<T>(other.y)};
	}
	template <typename S>
	inline Vector3<T>& operator+=(const Vector3<S>& other) const noexcept
	{
		return (*this = (*this + other));
	}
	template <typename S>
	inline Vector3<T>& operator-=(const Vector3<S>& other) const noexcept
	{
		return (*this = (*this - other));
	}
public:
	// access methods
	inline T x() const noexcept { return this->x; }
	inline T y() const noexcept { return this->x; }
	inline T z() const noexcept { return this->z; }
	inline T& x() noexcept { return this->x; }
	inline T& y() noexcept { return this->x; }
	inline T& z() noexcept { return this->z; }
	inline constexpr T operator[](size_t idx) const noexcept
	{
		static_assert(idx < 3u, "Index must be in range [0,2]!");
		return (idx == 0u ? x : (index == 1u ? y : z));
	}
	inline T operator[](size_t idx) const noexcept
	{
		static_assert(idx < 3u, "Index must be in range [0,2]!");
		return (idx == 0u ? x : (index == 1u ? y : z));
	}
	inline constexpr T& operator[](size_t idx) noexcept
	{
		static_assert(idx < 3u, "Index must be in range [0,2]!");
		return (idx == 0u ? x : (index == 1u ? y : z));
	}
	inline T& operator[](size_t idx) noexcept
	{
		static_assert(idx < 3u, "Index must be in range [0,2]!");
		return (idx == 0u ? x : (index == 1u ? y : z));
	}
public:
	// data methods
	template <typename S = double>
	inline S len2() const noexcept
	{
		return ((static_cast<S>(x)*static_cast<S>(x)) + (static_cast<S>(y)*static_cast<S>(y)) + (static_cast<S>(z)*static_cast<S>(z)));
	}
	template <typename S = double>
	inline S len() const noexcept
	{
		return static_cast<S>(std::sqrt<S>(this->len()));
	}
	/**
	 * @brief Returns whether this vector contains the vector `other` inside a square, spanning from [0,0,0] to (this).
	 * @tparam S Type of `other`s data
	 * @param other The vector to check for
	 * @return `true` if contained, `false` otherwise
	 */
	template <typename S>
	inline bool contains(const Vector3D<S>& other) const noexcept
	{
		return (other >= Vector3D{} && other < *this);
	}
private:
	T x;
	T y;
	T z;
};

template<typename T>
struct std::hash<Vector3<T>>
{
	inline size_t operator()(const Vector3<T>& vec) const noexcept
	{
		return Hash::combine<T,T>(vec.x(), vec.y(), vec.z());
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& ostream, const Vector3<T>& vec)
{
	return (ostream << '[' << vec.x() << ',' << vec.y() << ',' << vec.z() << ']');
}

// Specializations
using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3u8 = Vector3<uint8_t>;
using Vector3u16 = Vector3<uint16_t>;
using Vector3u32 = Vector3<uint32_t>;
using Vector3u64 = Vector3<uint64_t>;
using Vector3i8 = Vector3<int8_t>;
using Vector3i16 = Vector3<int16_t>;
using Vector3i32 = Vector3<int32_t>;
using Vector3i64 = Vector3<int64_t>;

#endif // VECTOR3_HPP