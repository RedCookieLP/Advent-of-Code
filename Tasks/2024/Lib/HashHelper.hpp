#ifndef HASHHELPER_HPP
#define HASHHELPER_HPP

namespace Hash
{
	template <typename T, typename... Ts>
	inline size_t combine(T&& arg, Ts&&... args) noexcept
	{
		using TValue = std::remove_reference_t<T>;
		size_t hash = std::hash<TValue>{}(arg);
		if constexpr(sizeof...(Ts) > 0u)
		{
			hash ^= combine<Ts...>(std::forward<Ts>(args)...) + 0x9e3779b9ull + (hash << 6ull) + (hash >> 2u);
		}
		return hash;
	}
	template <typename T, typename... Ts>
	inline size_t combine(const T& arg, const Ts&... args) noexcept
	{
		using TValue = std::remove_reference_t<T>;
		size_t hash = std::hash<TValue>{}(arg);
		if constexpr(sizeof...(Ts) > 0u)
		{
			hash ^= combine<Ts...>(args...) + 0x9e3779b9ull + (hash << 6ull) + (hash >> 2u);
		}
		return hash;
	}
}

#endif // HASHHELPER_HPP