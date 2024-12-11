#ifndef HASHHELPER_HPP
#define HASHHELPER_HPP

namespace Hash
{
	template <typename T, typename... Ts>
	inline size_t combine(const T& arg, Ts&&... args) noexcept
	{
		size_t hash = std::hash<T>{}(arg);
		if constexpr(sizeof...(Ts) > 0u)
		{
			hash ^= combine<Ts...>(std::forward<Ts>(args)...) + 0x9e3779b9ull + (hash << 6ull) + (hash >> 2u);
		}
		return hash;
	}
}

#endif // HASHHELPER_HPP