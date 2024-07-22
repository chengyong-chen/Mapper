#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

class COutputList : public CListBox
{
public:
	COutputList();

	// Implementation
public:
	~COutputList() override;

protected:
	DECLARE_MESSAGE_MAP()
};
