// objidlib.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "objidlib.h"

OBJIDLIB_API BOOL ObjectIdExists(LPCWSTR sFileName)
{
  HANDLE hFile = OpenFileForRead(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));
  OBJECTID_ATTRIBUTE Obj;
  DWORD dwRet;

  if (hFile == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  if (!DeviceIoControl(hFile, FSCTL_GET_OBJECT_ID,
     NULL, 0, &Obj, sizeof(OBJECTID_ATTRIBUTE), &dwRet, NULL))
  {
    CloseHandle(hFile);
    return FALSE;
  }
  
  CloseHandle(hFile);
  return TRUE;
}

OBJIDLIB_API BOOL CreateObjectId(LPCWSTR sFileName, POBJECTID_ATTRIBUTE pObjId)
{
  HANDLE hNew = OpenFileForRead(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));
  OBJECTID_ATTRIBUTE Obj;
  DWORD dwRet;

  if (hNew == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  } 

  ZeroMemory(&Obj, sizeof(OBJECTID_ATTRIBUTE));

  if (!DeviceIoControl(hNew,
     FSCTL_CREATE_OR_GET_OBJECT_ID,
     NULL, 0, &Obj, sizeof(OBJECTID_ATTRIBUTE), &dwRet, NULL))
  {
    CloseHandle(hNew);
    return FALSE;
  }

  if (NULL != pObjId)
  {
    memcpy(pObjId, &Obj, sizeof(OBJECTID_ATTRIBUTE));
  }

  CloseHandle(hNew);
  return TRUE;
}

OBJIDLIB_API BOOL GetObjectId(LPCWSTR sFileName, POBJECTID_ATTRIBUTE pObjId)
{
  if (NULL != pObjId)
  {
    return FALSE; 
  }

  HANDLE hNew = OpenFileForRead(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));
  
  DWORD dwRet;

  if (hNew == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  } 

  if (!DeviceIoControl(hNew,
     FSCTL_GET_OBJECT_ID,
     NULL, 0, pObjId, sizeof(OBJECTID_ATTRIBUTE), &dwRet, NULL))
  {
    CloseHandle(hNew);
    return FALSE;
  }

  CloseHandle(hNew);
  return TRUE;
}

OBJIDLIB_API BOOL DeleteObjectId(LPCWSTR sFileName)
{
  HANDLE hDel = OpenFileForWrite(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));
  DWORD dwRet;

  if (hDel == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }
  
  if (!DeviceIoControl(hDel,
    FSCTL_DELETE_OBJECT_ID,
    NULL, 0, NULL, 0, &dwRet, NULL))
  {
    CloseHandle(hDel);
    return FALSE;
  }

  CloseHandle(hDel);
  return TRUE;
}
