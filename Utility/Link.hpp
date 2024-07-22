#pragma once

#include <numbers> 
#include <cmath> 
const double ε=0.000001;
using namespace std;


namespace Link
{
	struct Vertex
	{
		double λ;
		double ψ;
		Vertex* prev=nullptr;
		Vertex* next=nullptr;
		bool operator==(const Vertex& other) const
		{
			return std::abs(other.λ - λ) < ε && std::abs(other.ψ - ψ) < ε;
		}
	};
	static unsigned int GetLength(Link::Vertex* vertex)
	{
		unsigned int count=0;
		while(vertex != nullptr)
		{
			count++;
			vertex=vertex->next;
		}
		return count;
	}
	static Vertex* tail(Vertex* pointer)
	{
		while(pointer->next != nullptr)
			pointer=pointer->next;
		return pointer;
	}
	static Vertex* head(Vertex* pointer)
	{
		while(pointer->prev != nullptr)
			pointer=pointer->prev;
		return pointer;
	}
	static Vertex* reverse(Vertex* pointer)
	{
		Vertex* prev = nullptr;
		Vertex* current = pointer;
		while(current != NULL) {
			auto temp = current->next;
			current->next = prev;			
			current->prev = temp;
			prev = current;
			current = current->prev;
		}
		return prev;
	}
	static void Dispose(Vertex* vertex)
	{
		while(vertex != nullptr)
		{
			Link::Vertex* pointer=vertex;
			vertex=vertex->next;
			delete pointer;
		}
	}
}