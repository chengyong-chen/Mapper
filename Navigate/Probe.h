#pragma once

#include <queue>

using namespace std;

struct Probe
{
	DWORD dwNodeIndex;
	bool m_bUsed; //是否为红点集要素
	float m_dShortestLength; //至此点的最短路径长度
	DWORD m_dwPreShortestEdge;//最短路径的上一条边
	//	double m_edifyToHead;
	//	double m_edifyToTail;
};

class CProbeVect : public vector<Probe*>
{
	public:
	virtual ~CProbeVect();

	public:
	DWORD FetchMini();
	void Insert(Probe* pProbe);

	static void Changed(Probe* pProbe);
};
