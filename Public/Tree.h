#pragma once

template<class T>
class  CTree
{
public:
	class baseiterator
	{
	protected:
		T* m_pCurrentNode = nullptr;

	public:
		baseiterator()
		{
			m_pCurrentNode = nullptr;
		}
		baseiterator(T* pT)
		{
			m_pCurrentNode = pT;
		}
		T* operator*() const
		{
			return (T*)m_pCurrentNode;
		}
		baseiterator& operator=(const baseiterator& other)
		{
			m_pCurrentNode = other.m_pCurrentNode;
			return *this;
		}
		bool operator==(const baseiterator& other) const
		{
			if(other.m_pCurrentNode == m_pCurrentNode)
				return true;
			else
				return false;
		}
		bool operator!=(const baseiterator& other) const
		{
			if(other.m_pCurrentNode != m_pCurrentNode)
				return true;
			else
				return false;
		}
	};
	class const_baseiterator
	{
	protected:
		T* m_pCurrentNode = nullptr;

	public:
		const_baseiterator()
		{
			m_pCurrentNode = nullptr;
		}
		const_baseiterator(T* pT)
		{
			m_pCurrentNode = pT;
		}
		const T* operator*() const
		{
			return (T*)m_pCurrentNode;
		}
		const_baseiterator& operator=(const const_baseiterator& other)
		{
			m_pCurrentNode = other.m_pCurrentNode;
			return *this;
		}
		bool operator==(const const_baseiterator& other) const
		{
			if(other.m_pCurrentNode == m_pCurrentNode)
				return true;
			else
				return false;
		}
		bool operator!=(const const_baseiterator& other) const
		{
			if(other.m_pCurrentNode != m_pCurrentNode)
				return true;
			else
				return false;
		}
	};
	class forwiterator : public baseiterator
	{
	public:
		forwiterator(T* pT)
			:baseiterator(pT)
		{
		}
		forwiterator& operator=(const forwiterator& other)
		{
			baseiterator::m_pCurrentNode = other.m_pCurrentNode;
			return *this;
		}
		forwiterator& operator++()
		{
			baseiterator::m_pCurrentNode = baseiterator::m_pCurrentNode->GetDown();
			return *this;
		}
	};
	class const_forwiterator : public const_baseiterator
	{
	public:
		const_forwiterator(T* pT)
			: const_baseiterator(pT)
		{
		}
		const_forwiterator& operator=(const const_forwiterator& other)
		{
			const_baseiterator::m_pCurrentNode = other.m_pCurrentNode;
			return *this;
		}
		const_forwiterator& operator++()
		{
			const_baseiterator::m_pCurrentNode = const_baseiterator::m_pCurrentNode->GetDown();
			return *this;
		}
	};
	class postiterator : public baseiterator
	{
	public:
		postiterator(T* pT)
			:baseiterator(pT)
		{
		}
		postiterator& operator++()
		{
			baseiterator::m_pCurrentNode = baseiterator::m_pCurrentNode->GetUper();
			return *this;
		}
	};
	class const_postiterator : public const_baseiterator
	{
	public:
		const_postiterator(T* pT)
			:const_baseiterator(pT)
		{
		}
		const_postiterator& operator++()
		{
			const_baseiterator::m_pCurrentNode = baseiterator::m_pCurrentNode->GetUper();
			return *this;
		}
	};
	CTree()
		: m_root(*this)
	{
	}

	T m_root;

	int nodesize()
	{
		return 1 + m_root.decendentsize();
	}

	forwiterator forwbegin() const
	{
		return forwiterator((T*)&m_root);
	}
	const_forwiterator cforwbegin() const
	{
		return const_forwiterator((T*)&m_root);
	}
	forwiterator forwend() const
	{
		return forwiterator(nullptr);
	}
	const_forwiterator cforwend() const
	{
		return const_forwiterator(nullptr);
	}

	postiterator postbegin()
	{
		T* pT = &m_root;
		while(pT != nullptr)
		{
			if(pT->FirstChild() == nullptr)
				break;
			else
				pT = pT->LastChild();
		}
		return postiterator(pT);
	}
	const_postiterator cpostbegin()
	{
		T* pT = &m_root;
		while(pT != nullptr)
		{
			if(pT->FirstChild() == nullptr)
				break;
			else
				pT = pT->LastChild();
		}
		return const_postiterator(pT);
	}
	postiterator postend() const
	{
		return postiterator(nullptr);
	}
	const_postiterator cpostend() const
	{
		return const_postiterator(nullptr);
	}
};
