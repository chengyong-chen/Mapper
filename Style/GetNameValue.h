// GetNameValue.h

// Copyright (C) 2001 Hans Dietrich

// This software is released into the public domain.  
// You are free to use it in any way you like.

// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 

///////////////////////////////////////////////////////////////////////////////

#pragma once

LONG GetNameValue(HKEY key, LPCTSTR subkey, LPCTSTR valuename, LPTSTR retdata);
LONG GetNextNameValue(HKEY key, LPCTSTR subkey, LPTSTR szName, LPTSTR szData);
