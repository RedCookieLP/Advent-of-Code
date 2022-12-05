#include <iostream>
#include <fstream>

template <typename T>
class Stack
{
public:
	Stack(size_t capacity = 0) : m_capacity(capacity), m_size(0), m_data(capacity > 0 ? new T[capacity] : nullptr) { memset(m_data, 0, sizeof(T) * capacity); }
	Stack(const Stack& o) : m_capacity(o.m_capacity), m_size(o.m_size), m_data(o.m_capacity > 0 ? new T[o.m_capacity] : nullptr)
	{
		if (o.m_data && o.m_size > 0)
			memcpy_s(m_data, sizeof(T) * m_size, o.m_data, sizeof(T) * o.m_size);
	}
	Stack(Stack&& o) : m_capacity(o.m_capacity), m_size(o.m_size), m_data(o.m_data)
	{
		o.m_data = nullptr;
		o.m_size = 0;
		o.m_capacity = 0;
	}
	Stack& operator=(const Stack& o)
	{
		delete[] m_data;
		m_data = nullptr;
		
		m_capacity = o.m_capacity;
		m_size = o.m_size;
		if (m_capacity > 0)
			m_data = new T[m_capacity];
		
		if (o.m_data && o.m_size > 0)
			memcpy_s(m_data, sizeof(T) * m_size, o.m_data, sizeof(T) * o.m_size);
		
		return *this;
	}
	Stack& operator=(Stack&& o)
	{
		delete[] m_data;
		m_data = nullptr;
		
		m_capacity = o.m_capacity;
		m_size = o.m_size;
		m_data = o.m_data;
		
		o.m_data = nullptr;
		o.m_size = 0;
		o.m_capacity = 0;
		
		return *this;
	}
	~Stack() { delete[] m_data; }
public:
	size_t size() const { return m_size; }
	size_t capacity() const { return m_capacity; }
public:
	void push(const T& val)
	{
		if (m_size == m_capacity)
			return;
		m_data[m_size++] = std::move(val);
	}
	const T& peek() const { return (m_size == 0 ? T() : m_data[m_size-1]); }
	T pop()
	{
		T ret = peek();
		if (m_size == 0)
			m_size--;
		return val;
	}
private:
	size_t m_capacity;
	size_t m_size;
	T* m_data;
};

int main()
{
	std::ifstream file("input.txt");
	if (!file)
		return -1;
	
	std::string line(50,'\0');
	file.getline(line.data(), 50, '\n');
	size_t lineLen = std::strlen(line.c_str());
	
	size_t nofStacks = (lineLen >= 3 ? (1 + ((lineLen-3)/4)) : 0);
	if (nofStacks == 0)
	{
		file.close();
		return 0;
	}
	std::cout << "The ship has " << nofStacks << " stacks!\n";
	
	Stack<int>* stacks = new Stack<int>[nofStacks];
	for (size_t i = 0 ; i < nofStacks ; i++)
	{
		stacks[i] = Stack<int>(nofStacks * nofStacks);
	}
	
	while (file)
	{
		std::string line(50,'\0');
		file.getline(line.data(), 50, '\n');
	}
	file.close();

	return 0;
}