#include "stdafx.h"
#include "Fill.h"
#include "FillCompact.h"
#include "FillComplex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFill::CFill()
{
}

CFill* CFill::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CFill* pClone = (CFill*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}
void CFill::Sha1(boost::uuids::detail::sha1::digest_type& hash) const
{
	boost::uuids::detail::sha1 sha1;
	this->Sha1(sha1);
	sha1.get_digest(hash);
}
void CFill::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_byte(Gettype());
	
}
BOOL CFill::operator !=(const CFill& fill) const
{
	return !(*this==fill);
}

CFill* CFill::Apply(BYTE type)
{
	CFill* fill = CFillCompact::Apply(type);
	fill = fill==nullptr ? CFillComplex::Apply(type) : fill;
	return fill;
}
