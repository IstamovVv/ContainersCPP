#pragma once
#include <iostream>
#include <sstream>

inline std::ostream& print() {
	return std::cout << '\n';
}

template<typename T>
std::ostream& print(const T& what)
{
	return std::cout << what;
}

template<typename T, typename... Types>
std::ostream& print(const T& what, const Types& ... other)
{
	std::cout << what << " ";
	return print(other...);
}

inline std::ostream& printLine()
{
	return std::cout << '\n';
}

template<typename T>
std::ostream& printLine(const T& what) {
	return std::cout << what << '\n';
}

template<typename T, typename... Types>
std::ostream& printLine(const T& what, const Types& ... other)
{
	std::cout << what << " ";
	return printLine(other...);
}

template<class T>
void input(T& value)
{
	std::cin >> value;
}

template<class T, class... Args>
void input(T& value, Args&&... args)
{
	input(value);
	input(args...);
}

template<class T>
void inputLine(T& value)
{
	std::getline(std::cin, value);
}

template<class T, class... Args>
void inputLine(T& value, Args&&... args)
{
	inputLine(value);
	inputLine(args...);
}
