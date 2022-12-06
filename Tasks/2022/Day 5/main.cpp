#include <iostream>
#include <fstream>
#include <sstream>

template <typename T>
class Stack
{
public:
	Stack(size_t capacity = 0) : m_capacity(capacity), m_size(0), m_data(capacity > 0 ? new T[capacity] : nullptr) {}
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
	T peek() const { return ((m_size == 0 || !m_data) ? T() : m_data[m_size-1]); }
	T pop()
	{
		T ret = peek();
		if (m_size != 0)
			m_size--;
		return ret;
	}
	void push(const T& val)
	{
		if (m_size == m_capacity)
			return;
		m_data[m_size] = val;
		m_size++;
	}
	void flip()
	{
		if (m_size == 0 || !m_data)
			return;
		
		for (size_t front = 0, back = m_size-1 ; front < back ; front++, back--)
		{
			T temp = m_data[front];
			m_data[front] = m_data[back];
			m_data[back] = temp;
		}
	}
public:
	bool isEmpty() const { return (m_size == 0); }
	operator bool() const { return !isEmpty(); }
private:
	size_t m_capacity;
	size_t m_size;
	T* m_data;
};

template <typename T>
std::string serializeStack(Stack<T> s) // intended copy, because I'm printing elements by popping them... that sounds wrong...
{
	std::stringstream ss;
	while (s)
	{
		ss << s.pop();
		if (s.size() != 0)
			ss << ", ";
	}
	return ss.str();
}

enum State
{
	STACKING = 0,
	MOVING = 1,
	DONE = 2
};
using SupplyStack = Stack<char>;

static State s_state = STACKING;
static size_t nofStacks = 0;
static SupplyStack* stacks = nullptr;


void extractStacks(const std::string& line)
{
	if (line.length() == 0 || isdigit(line[1]))
		return;
		
	//	std::cerr << " [LINE] - The line is \"" << line << "\" - [LINE]\n";
	
	size_t maxWidth = (nofStacks*4) - 1;

	for (size_t i = 0, pos = 1 ; pos < maxWidth ; i++, pos += 4)
	{
		char supply = line[pos];
		if (supply == ' ')
			continue;
		stacks[i].push(supply);
	}
}

void crateMover9000(size_t from, size_t to, size_t count)
{
	if (!stacks || from >= nofStacks || to >= nofStacks)
		return;
	
	SupplyStack& source = stacks[from];
	SupplyStack& destination = stacks[to];

	for (size_t i = 0 ; i < count ; i++)
		destination.push(source.pop());
}

void crateMover9001(size_t from, size_t to, size_t count)
{
	if (!stacks || from >= nofStacks || to >= nofStacks)
		return;
	
	SupplyStack& source = stacks[from];
	SupplyStack& destination = stacks[to];

	SupplyStack tempStack(count);
	for (size_t i = 0 ; i < count ; i++)
		tempStack.push(source.pop());
	
	for (size_t i = 0 ; i < count ; i++)
		destination.push(tempStack.pop());
}

int main()
{
	std::ifstream file("input.txt");
	if (!file)
		return -1;
	
	std::string line(50,'\0');
	file.getline(line.data(), 50, '\n');
	size_t lineLen = std::strlen(line.c_str());
	
	nofStacks = ((lineLen + 1) / 4);
	if (nofStacks == 0)
	{
		file.close();
		return 0;
	}
	std::cout << "The ship has " << nofStacks << " stacks!\n";
	
	stacks = new SupplyStack[nofStacks];
	for (size_t i = 0 ; i < nofStacks ; i++)
	{
		stacks[i] = SupplyStack(nofStacks * nofStacks);
	}


	
	while (file && s_state != DONE)
	{
		//	std::cerr << "Current line: " << line << " || Length: " << lineLen << '\n';
		if (lineLen == 0)
		{
			((int&)s_state)++;

			if (s_state == MOVING)
			{
				for (size_t i = 0 ; i < nofStacks ; i++)
					stacks[i].flip();
			}
			line = std::string(50, '\0');
			file.getline(line.data(), 50, '\n');
			lineLen = std::strlen(line.c_str());

			continue;
		}

		switch (s_state)
		{
			case STACKING:
			{
				extractStacks(line);
			}
			break;
			case MOVING:
			{
				size_t spaces[5] = {0};
				spaces[0] = line.find_first_of(' ');
				for (size_t i = 1 ; i < 5 ; i++)
					spaces[i] = line.find_first_of(' ', spaces[i-1]+1);

				std::string countStr = line.substr(spaces[0]+1, spaces[1]-spaces[0] - 1);
				std::string fromStr = line.substr(spaces[2]+1, spaces[3]-spaces[2] - 1);
				std::string toStr = line.substr(spaces[4]+1);

				size_t count = std::atoll(countStr.c_str());
				size_t from = std::atoll(fromStr.c_str()) - 1;
				size_t to = std::atoll(toStr.c_str()) - 1;

				//	crateMover9000(from, to, count);
				crateMover9001(from, to, count);
			}
			break;
			default:
			{
			}
			break;
		}

		line = std::string(50, '\0');
		file.getline(line.data(), 50, '\n');
		lineLen = std::strlen(line.c_str());
	}
	file.close();

	std::cout << "After all the moving instructions, all the top crates are:\n";
	for (size_t i = 0 ; i < nofStacks ; i++)
		std::cout << stacks[i].peek();
	std::cout << '\n';

	return 0;
}