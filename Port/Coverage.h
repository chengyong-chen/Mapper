#pragma once

#include  <math.h>
#include  <stdlib.h>

#include "Global.h"

class CDatainfo;
class CLayerTree;
class __declspec(dllexport) CCoverage : public CObject
{
//	DECLARE_SERIAL(CCoverage);
public:
	CCoverage(CDatainfo& datainfo);
	CCoverage(CDatainfo& datainfo,LPCTSTR lpszCoveragePath);
	virtual ~CCoverage();

	static CString m_strDb; //

	union revType
	{
		TCHAR str[4];
		long  valLong;
		float valFloat;
	};

	struct LAB
	{
		LONG	ID;		// 标识点序列号
		LONG	polyId;	// 多边形序列号
		CPointF points[3];	// 标识点定位坐标
	};
	typedef CTypedPtrMap<CMapWordToPtr,WORD,LAB*> CMapLab;

	struct ARC
	{
		LONG	ID;		    // 弧段序列号
		LONG	lLength;    // lDataLength = (lPosCount*8L + 6*4L)/2L
		LONG	ID_;	    // 用户标识号 (旧的)
		LONG	lFNode;		// 首结点
		LONG	lTNode;		// 尾结点
		LONG	lLPoly;		// 左多边形
		LONG	lRPoly;		// 右多边形
		LONG	lPosCount;	// 弧段节点个数
		CPointF *points;    // 弧段定位点序列

		ARC()
		{ 
			lPosCount = 0;
			points = nullptr;
		}

		~ARC()
		{
			lPosCount = 0;
			delete []points; 
			points = nullptr; 
		}

	};	// ARC 数据文件结构
	typedef CTypedPtrMap<CMapWordToPtr,WORD,ARC*> CMapARC;

	struct ARX
	{
		LONG	ID;		// 弧段序列号
		LONG	lLength;    // lDataLength = (lPosCount*8L + 6*4L)/2L
		LONG	ID_;	// 用户标识号 (旧的)
		LONG	lFNode;		// 首结点
	};
	typedef CList<ARX, ARX> ARX_LIST;

	struct CNT
	{
		LONG	polyId;		// 多边形序列号
		LONG  lReserved;	    // UNKNOWN
		CPointF point;		    // 多边形几何中心
		LONG	IDCount;    // 多边形标识码个数
		LONG*	IDs;		// 多边形标识码序列

		CNT()
		{
			IDCount = 0;
			IDs = nullptr;
		}
		virtual ~CNT()
		{
			IDCount = 0;
			delete IDs;
			IDs = nullptr;
		}
		CNT(CNT& cnt)
		{
			polyId		= cnt.polyId;
			lReserved	= cnt.lReserved;
			point	= cnt.point;
			IDCount	= cnt.IDCount;
			IDs		= nullptr;
			if (IDCount)
			{
				IDs = new LONG[IDCount];
				CopyMemory(IDs, cnt.IDs, sizeof(LONG)*IDCount);
			}
		}
		CNT operator = (CNT cnt)
		{
			polyId		= cnt.polyId;
			lReserved	= cnt.lReserved;
			point			= cnt.point;
			IDCount	= cnt.IDCount;
			delete IDs; 
			IDs = nullptr;
			if (IDCount)
			{
				IDs = new LONG[IDCount];
				CopyMemory(IDs, cnt.IDs, sizeof(LONG)*IDCount);
			}
			return *this;
		}
	};	// CNT 数据文件结构
	typedef CList<CNT, CNT> CNT_LIST;

	struct TXT{
		LONG	lAnnoID;		// 注记序列号
		LONG	UserID_;	    // 用户标识号
		LONG	lReserved0;	    // UNKNOWN
		LONG	nPosCount;	    // 定位点个数
		// nPosCount = 1:	单点定位
		// nPosCount = 2:	两点定位
		// nPosCount = 4:	曲线定位
		CPointF point0[4];	    // 定位点序列
		LONG	lReserved1;	    // UNKNOWN
		CPointF point1[4];	    //	UNKNOWN
		// point1[3].x 为字大
		LONG	lReserved2;	    // UNKNOWN
		LONG	lCharCount;	    // 注记个数 (CHAR 单位)
		TCHAR* pzAnno;		    // 注记 (填充至 LONG 边界) 
		// 长度为 if ((sizeof(CHAR)*lCharCount) % 4)
		// ((sizeof(CHAR)*lCharCount)/4 + 1)*4
		TXT()
		{
			lCharCount = 0;
			pzAnno = nullptr;
		}
		~TXT()
		{
			lCharCount = 0;
			delete pzAnno;
			pzAnno = nullptr;
		}
		TXT(TXT& txt)
		{
			lAnnoID		= txt.lAnnoID;
			UserID_		= txt.UserID_;
			lReserved0	= txt.lReserved0;
			nPosCount	= txt.nPosCount;
			CopyMemory(point0, txt.point0, sizeof(CPointF)*4);
			lReserved1	= txt.lReserved1;
			CopyMemory(point1, txt.point1, sizeof(CPointF)*4);
			lReserved2	= txt.lReserved2;
			lCharCount	= txt.lCharCount;
			pzAnno		= nullptr;
			if (lCharCount)
			{
				pzAnno = new TCHAR[lCharCount + 1];
				CopyMemory(pzAnno, txt.pzAnno, sizeof(TCHAR)*lCharCount);
				pzAnno[lCharCount] = '\0';
			}
		}
		TXT operator = (TXT txt)
		{
			lAnnoID		= txt.lAnnoID;
			UserID_		= txt.UserID_;
			lReserved0	= txt.lReserved0;
			nPosCount	= txt.nPosCount;
			CopyMemory(point0, txt.point0, sizeof(CPointF)*4);
			lReserved1	= txt.lReserved1;
			CopyMemory(point1, txt.point1, sizeof(CPointF)*4);
			lReserved2	= txt.lReserved2;
			lCharCount	= txt.lCharCount;
			delete pzAnno; 
			pzAnno = nullptr;
			if (lCharCount)
			{
				pzAnno = new TCHAR[lCharCount + 1];
				CopyMemory(pzAnno, txt.pzAnno, sizeof(TCHAR)*lCharCount);
				pzAnno[lCharCount] = '\0';
			}
			return *this;
		}
	};	// TXT 数据文件结构
	typedef CList<TXT, TXT> TXT_LIST;


	struct AAT
	{
		LONG	FNODE_;	// 始结点
		LONG	TNODE_;	// 尾结点
		LONG	LPOLY_;	// 左多边形
		LONG	RPOLY_;	// 右多边形
		FLOAT	LENGTH;	// 长度
		LONG	ID;	// 系统标识码 (其中 XXXX 为 COVERAGE 层名称)
		LONG	UserID;// 用户标识码

		TCHAR	Code[6+1];
		TCHAR	Name[50+1];
		TCHAR	HYDC[6   +1];	// 接口码
		TCHAR   RN[6 +1];	
		TCHAR	RN2[6 +1];	
		TCHAR	GB[5+1];
		TCHAR	TN[6    +1];
		TCHAR   MapTN[11+1];	

		AAT()
		{
			FNODE_ = 0;	
			TNODE_ = 0;	
			LPOLY_ = 0;	
			RPOLY_ = 0;	
			LENGTH = 0.f;
			ID = 0;	
			UserID = 0;

			_tcsnset(Code,0X00,7);
			_tcsnset(RN,0X00,7);
			_tcsnset(RN2,0X00,7);
			_tcsnset(HYDC,0X00,7);
			_tcsnset(Name,0X00,51);
			_tcsnset(TN,0X00,7);
			_tcsnset(MapTN,0X00,12);
		}
	}; 
	typedef CList<AAT, AAT> AAT_LIST;
	
	struct PAT	
	{	
		FLOAT	AREA;			// 面积
		FLOAT	PERIMETER;  	// 周长
		LONG	ID;		        // 系统标识码 (其中 XXXX 为 COVERAGE 层名称)
		LONG	UserID;	        // 用户标识码	

		TCHAR	Code[6+1];
		TCHAR	GB[5+1];
		TCHAR	HYDC[6   +1];	// 接口码
		TCHAR	RN[6 +1];	
		TCHAR	Name[50+1]; 
		TCHAR   CapName[50 +1]; 
		TCHAR	TN[6    +1];
		TCHAR   MapTN[11+1];

		PAT()
		{
			AREA = 0.f;
			PERIMETER = 0.f;
			ID = 0;		        // 系统标识码 (其中 XXXX 为 COVERAGE 层名称)
			UserID = 0;	        // 用户标识码	

			_tcsnset(Code,0X00,7);
			_tcsnset(GB,0X00,6);
			_tcsnset(HYDC,0X00,7);
			_tcsnset(RN,0X00,7);
			_tcsnset(Name,0X00,51); 
			_tcsnset(CapName,0X00,50); 
			_tcsnset(TN,0X00,7);
			_tcsnset(MapTN,0X00,12);
		}
	}; 	
	typedef CList<PAT, PAT> PAT_LIST;


	struct PAL
	{
		LONG	polyId;		  // 多边形序列号
		LONG	lReserved;	      // UNKNOWN
		CPointF point[2];	      // UNKNOWN
		LONG	lRelationArcCount;// 关联弧段条数
		struct ARC_POLY
		{
			LONG	ID;		  // 如果 ID < 0 , 则表示			|		如果 ID > 0 , 则表示	
			// polyId 为 ID 的左多边形		|		polyId 为 ID 的右多边形
			LONG	lNodeId;	  // ID 的尾结点					|		ID 的首结点
			LONG	polyId;	  // ID 的右多边形					|		ID 的左多边形
		} *relationship;

		PAL()
		{
			lRelationArcCount = 0;
			relationship = nullptr;
		}
		~PAL()
		{
			lRelationArcCount = 0;
			delete relationship;
		}
		PAL(PAL& pal)
		{
			polyId		= pal.polyId;
			lReserved	= pal.lReserved;
			CopyMemory(point, pal.point, sizeof(CPointF)*2);
			lRelationArcCount = pal.lRelationArcCount;
			relationship = nullptr;
			if (lRelationArcCount)
			{
				relationship = new ARC_POLY[lRelationArcCount];
				CopyMemory(relationship, pal.relationship, sizeof(ARC_POLY)*lRelationArcCount);
			}
		}
		PAL operator = (PAL pal)
		{
			polyId		= pal.polyId;
			lReserved	= pal.lReserved;
			CopyMemory(point, pal.point, sizeof(CPointF)*2);
			lRelationArcCount = pal.lRelationArcCount;
			delete relationship; 
			relationship = nullptr;
			if (lRelationArcCount)
			{
				relationship = new ARC_POLY[lRelationArcCount];
				CopyMemory(relationship, pal.relationship, sizeof(ARC_POLY)*lRelationArcCount);
			}
			return *this;
		}
	};	// PAL 数据文件结构
	typedef CList<PAL, PAL> PAL_LIST;


	struct SEC
	{
		DWORD	RouteLink;	//临时分类码
		DWORD	ArcLink;  	// 国际标准编码
		float	F_Meas;		// 接口码
		float	T_Meas;		// 接口码
		float	F_Pos;		// 接口码
		float	T_Pos;		// 接口码
		DWORD	Sub;		// 接口码
		DWORD	SubID;		// 接口码
	}; // POINT 数据表基本结构
	typedef CList<SEC, SEC> SEC_LIST;


	// Attributes
protected:
	CString	m_strCoveragePath;		// COVERAGE 数据路径

public:
	CDatainfo& m_datainfo;

public:
	CMapLab  labmap;
	CMapARC  arcmap;
	
	PAT_LIST pat_list;
	AAT_LIST aat_list;
	PAL_LIST pal_list;

	typedef CTypedPtrMap<CMapStringToPtr,CString,CLayer*> CMapLayer;

public:
	virtual long	ReadTic(CPointF* pTicData = nullptr);	// 读取配准点数据
	virtual long	ReadBnd(CPointF* pBndData = nullptr);	// 读取边界范围数据
	virtual long    ReadLAB();	// 读取标识点数据
	virtual long	ReadARC();	// 读取弧段数据
	virtual long	ReadArx(CCoverage::ARX_LIST& arx_list);	
	virtual long	ReadTxt(CCoverage::TXT_LIST& txt_list);	
	virtual long	ReadPAL(CCoverage::PAL_LIST& pal_list,LPCTSTR lpszFile=nullptr);
	virtual long	ReadAAT(){return 0;};
	virtual long	ReadPAT(){return 0;};
	virtual long	ReadSEC(CCoverage::SEC_LIST& sec_list,LPCTSTR lpszFile);

	virtual BOOL Import(CLayerTree& layertree) override {return true;};
	virtual BOOL ImportLab(CLibrary& library,CMapLayer& layermap,CLayer* delLab) override;
	virtual BOOL ImportArc(CMapLayer& layermap,CLayer* delArc) override;
	virtual BOOL ImportSec(CGeomList& polylist,CCoverage::SEC_LIST& sec_list) override;
	virtual BOOL ImportPolygon(CMapLayer& layermap,CLayer* delPoly) override;
	virtual BOOL ImportPolygon(CCoverage::PAL_LIST& pal_list,PAT_LIST& pat_list, CLayer* layer,CLayer* delPoly) override;
	void Reverse(revType& rev);


	CLayer* CreateLayers(CLayerTree& layertree,CMapLayer& layermap,CString strLayer);

public:
	static double m_xOffset;
	static double m_yOffset;
	static double m_fScale;
	
	CPoint Change(const CPointF& Scr) const;
};
