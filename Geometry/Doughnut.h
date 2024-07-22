#pragma once

#include "Cluster.h"
#include <cmath>
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

using namespace std;
using namespace Clipper2Lib;

class AFX_EXT_CLASS CDoughnut sealed : public CCluster
{
protected:
	DECLARE_SERIAL(CDoughnut)
	CDoughnut();

public:
	Gdiplus::FillMode fillmode=Gdiplus::FillMode::FillModeAlternate;

public:
	bool IsReal() const;
	void RecalCentroid() override;

public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const override;	

public:
	bool PickIn(const CPoint& point) const override;
public:
	void Normalize() override;
public:
	~CDoughnut() override;	
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;

private:
	CPath* GetOuter() const;
	unsigned long long Distance2(const CPoint& point) const;
	CPoint FindCentroid() const;

public:
	static bool ComparePair(std::pair<CPath*, double>& pair1, std::pair<CPath*, double>& pair2);
};