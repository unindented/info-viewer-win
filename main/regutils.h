///////////////////////////////////////////////////////////////////////////////
///
/// iNFO - Utility to view NFO and DIZ files
/// Copyright (C) 2005 Daniel P�rez �lvarez <unindented@gmail.com>
///
///////////////////////////////////////////////////////////////////////////////

#ifndef	_REGUTILS_H_
#define _REGUTILS_H_

#ifndef MAX_KEYLENGTH
#define MAX_KEYLENGTH 80
#endif // MAX_KEYLENGTH

BOOL ReadRegistryBin(HKEY, LPCTSTR, LPVOID, ULONG);
BOOL ReadRegistryInt(HKEY, LPCTSTR, LPLONG, LONG);
BOOL ReadRegistryStr(HKEY, LPCTSTR, LPTSTR, ULONG, LPCTSTR);
BOOL WriteRegistryBin(HKEY, LPCTSTR, LPVOID, ULONG);
BOOL WriteRegistryInt(HKEY, LPCTSTR, LONG);
BOOL WriteRegistryStr(HKEY, LPCTSTR, LPCTSTR);
BOOL IsAssociatedExtToApp(LPCTSTR, LPCTSTR);
VOID AssociateExtToApp(LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR);

#endif // _REGUTILS_H_
