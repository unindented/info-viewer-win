///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel Pérez Álvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_REGUTILS_H_
#define _REGUTILS_H_

#define MAX_KEYLENGTH 80

BOOL ReadRegistryBin(HKEY, LPCTSTR, LPVOID, ULONG);
BOOL ReadRegistryInt(HKEY, LPCTSTR, LPLONG, LONG);
BOOL ReadRegistryStr(HKEY, LPCTSTR, LPTSTR, ULONG, LPCTSTR);
BOOL WriteRegistryBin(HKEY, LPCTSTR, LPVOID, ULONG);
BOOL WriteRegistryInt(HKEY, LPCTSTR, LONG);
BOOL WriteRegistryStr(HKEY, LPCTSTR, LPCTSTR);
BOOL IsAssociatedExtToApp(LPCTSTR, LPCTSTR);
void AssociateExtToApp(LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR);

#endif // _REGUTILS_H_
