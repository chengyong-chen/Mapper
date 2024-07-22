#pragma once

using namespace std;

template<class T>
class __declspec(dllexport) CTreeNode
{
public:
	CTreeNode()
		: m_parent(0), m_prevsibling(0), m_nextsibling(0), m_firstchild(0)
	{
	}
	CTreeNode(T* parent)
		: m_parent(parent), m_prevsibling(0), m_nextsibling(0), m_firstchild(0)
	{
	}
	virtual ~CTreeNode()
	{
	}

protected:
	T* m_parent;
	T* m_prevsibling;
	T* m_nextsibling;
	T* m_firstchild;

public:
	T* Parent() const
	{
		return m_parent;
	}

	T* Prevsibling() const
	{
		return m_prevsibling;
	}

	T* Nextsibling() const
	{
		return m_nextsibling;
	}

	T* FirstChild() const
	{
		return m_firstchild;
	}

	void NoChild()
	{
		m_firstchild = nullptr;
	}

	T* LastChild() const
	{
		if(m_firstchild==nullptr)
			return nullptr;

		T* pT = m_firstchild;
		while(pT->m_nextsibling!=nullptr)
		{
			pT = pT->Nextsibling();
		}
		return pT;
	}

	bool IsLeaf() const
	{
		return m_firstchild==nullptr ? true : false;
	}

	T* GetUper() const
	{
		if(m_prevsibling==nullptr)
			return m_parent;
		else
		{
			T* pT = m_prevsibling;
			while(pT->IsLeaf()==false)
			{
				pT = pT->LastChild();
			}
			return pT;
		}
	}

	T* GetDown() const
	{
		if(m_firstchild!=nullptr)
			return m_firstchild;
		else if(m_nextsibling!=nullptr)
			return m_nextsibling;
		else
		{
			T* pT = m_parent;
			while(pT!=nullptr)
			{
				if(pT->m_nextsibling!=nullptr)
					return pT->m_nextsibling;

				pT = pT->m_parent;
			}

			return nullptr;
		}
	}

	bool IsAChild(T* pNode) const
	{
		T* pT = m_firstchild;
		while(pT!=nullptr)
		{
			if(pT==pNode)
				return true;

			pT = pT->Nextsibling();
		}

		return false;
	}

public:
	void AddFirstChild(T* pNode)
	{
		//		assert(pNode != nullptr);

		if(m_firstchild==nullptr)
		{
			pNode->m_prevsibling = nullptr;
			pNode->m_nextsibling = nullptr;
		}
		else
		{
			pNode->m_prevsibling = nullptr;
			pNode->m_nextsibling = m_firstchild;
			m_firstchild->m_prevsibling = pNode;
		}

		m_firstchild = pNode;
		pNode->m_parent = (T*)this;
	}

	void AddLastChild(T* pNode)
	{
		//		assert(pNode != nullptr);

		if(m_firstchild==nullptr)
		{
			pNode->m_prevsibling = nullptr;
			m_firstchild = pNode;
		}
		else
		{
			T* pLast = LastChild();
			//			assert(pLast != nullptr);
			pLast->m_nextsibling = pNode;
			pNode->m_prevsibling = pLast;
		}

		pNode->m_nextsibling = nullptr;
		pNode->m_parent = (T*)this;
	}

	virtual void AddChild(T* pAfter, T* pNode)
	{
		if(pAfter==nullptr)
			AddFirstChild(pNode);
		else if(IsAChild(pAfter)==false)
			return;
		else if(IsAChild(pNode)==true)
			return;
		else if(pAfter->m_nextsibling==nullptr)
			AddLastChild(pNode);
		else
		{
			pAfter->m_nextsibling->m_prevsibling = pNode;
			pNode->m_nextsibling = pAfter->m_nextsibling;
			pAfter->m_nextsibling = pNode;
			pNode->m_prevsibling = pAfter;
			pNode->m_parent = (T*)this;
		}
	}

	void DetachChild(T* pNode)
	{
		if(IsAChild(pNode)==false)
			return;
		else if(pNode==m_firstchild) //first child
		{
			m_firstchild = pNode->m_nextsibling;
			if(m_firstchild!=nullptr)
				m_firstchild->m_prevsibling = nullptr;
		}
		else if(pNode->m_nextsibling==nullptr) //last child
		{
			pNode->m_prevsibling->m_nextsibling = nullptr;
		}
		else
		{
			pNode->m_prevsibling->m_nextsibling = pNode->m_nextsibling;
			pNode->m_nextsibling->m_prevsibling = pNode->m_prevsibling;
		}
		pNode->m_parent = nullptr;
		pNode->m_prevsibling = nullptr;
		pNode->m_nextsibling = nullptr;
	}
	void ReplaceBy(T* pNode)
	{
		pNode->m_parent = m_parent;
		pNode->m_prevsibling = m_prevsibling;
		pNode->m_nextsibling = m_nextsibling;

		if(m_parent->m_firstchild == this)
		{
			m_parent->m_firstchild = pNode;
		}
		if(m_nextsibling != nullptr)
		{
			m_nextsibling->m_prevsibling = pNode;
		}
		if(m_prevsibling != nullptr)
		{
			m_prevsibling->m_nextsibling = pNode;
		}
	}
	T* GetChild(CString name) const
	{
		T* pT = m_firstchild;
		while(pT!=nullptr)
		{
			if(pT->m_strName == name)
				return pT;

			pT = pT->Nextsibling();
		}
		return nullptr;
	}
	int childrensize() const
	{
		int size = 0;
		T* pT = m_firstchild;
		while(pT!=nullptr)
		{
			size++;
			pT = pT->m_nextsibling;
		}
		return size;
	}

	int decendentsize() const
	{
		int size = this->childrensize();
		T* pT = m_firstchild;
		while(pT!=nullptr)
		{
			size += pT->childrensize();
			pT = pT->m_nextsibling;
		}
		return size;
	}
};