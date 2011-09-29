/** @file     objidlib.cpp
 *  @brief    A library for working with NTFS Object Idenitifers
 *  @author   amdf
 *  @version  0.2
 *  @date     September 2011
 */

#include "stdafx.h"
#include "objidlib.h"

static HANDLE OpenFileForWrite(IN LPCWSTR sFileName, IN BOOL bBackup)
{
  return CreateFile(
    sFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
    (bBackup)
    ? FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS
    : FILE_FLAG_OPEN_REPARSE_POINT, 0);
}

static HANDLE OpenFileForRead(IN LPCWSTR sFileName, IN BOOL bBackup)
{
  return CreateFile(
    sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
    (bBackup)
    ? FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS
    : FILE_FLAG_OPEN_REPARSE_POINT, 0);
}

/**
 *  @brief      Checks an existence of an object id of a specified file
 *  @param[in]  sFileName File name
 *  @return     TRUE if exists, FALSE otherwise
 */
OBJIDLIB_API BOOL ObjectIdExists(IN LPCWSTR sFileName)
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

/**
 *  @brief      Request a system to create a new object id in a specified file
 *  @param[in]  sFileName File name
 *  @param[out] pObjId Pointer to a buffer receiving a content of a new object id.
 *              If this parameter is NULL, it is ignored.
 *  @return     TRUE if success, FALSE otherwise
 */
OBJIDLIB_API BOOL CreateObjectId(IN LPCWSTR sFileName, OUT POBJECTID_ATTRIBUTE pObjId)
{
  OBJECTID_ATTRIBUTE Obj;
  DWORD dwRet;
  HANDLE hNew = OpenFileForRead(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));

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

/**
 *  @brief      Get a contents of an object id of a specified file
 *  @param[in]  sFileName File name
 *  @param[out] pObjId Pointer to a buffer receiving a content of a new object id.
 *              If this parameter is NULL, it is ignored.
 *  @return     TRUE if success, FALSE otherwise
 */
OBJIDLIB_API BOOL GetObjectId(IN LPCWSTR sFileName, OUT POBJECTID_ATTRIBUTE pObjId)
{
  if (NULL == pObjId)
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

/**
 *  @brief      Delete an object id of a specified file
 *  @param[in]  sFileName File name
 *  @return     TRUE if success, FALSE otherwise
 */
OBJIDLIB_API BOOL DeleteObjectId(IN LPCWSTR sFileName)
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

/**
 *  @brief      Set an object id of a specified file. SE_RESTORE_PRIVILEGE required.
 *  @param[in]  sFileName File name
 *  @param[in]  pObjId An OBJECTID_ATTRIBUTE structure.
 *  @return     TRUE if success, FALSE otherwise
 */
OBJIDLIB_API BOOL SetObjectId(IN LPCWSTR sFileName, IN POBJECTID_ATTRIBUTE pObjId)
{
  if (NULL == pObjId)
  {
    return FALSE;
  }

  DWORD dwRet;
  HANDLE hNew = CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 
    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

  if (hNew == INVALID_HANDLE_VALUE)
  {    
    return FALSE;
  }  

  if (!DeviceIoControl(hNew,
     FSCTL_SET_OBJECT_ID,
     pObjId, sizeof(OBJECTID_ATTRIBUTE), NULL, 0, &dwRet, NULL))
  {
    CloseHandle(hNew);
    return FALSE;
  }

  CloseHandle(hNew);
  return TRUE;
}

/**
 *  @brief      Set an extended part of an object id of a specified file
 *  @param[in]  sFileName File name
 *  @param[in]  pObjId An OBJECTID_ATTRIBUTE structure containing an extended part.
 *              ObjectId member is ignored.
 *  @return     TRUE if success, FALSE otherwise
 */
OBJIDLIB_API BOOL SetObjectIdExt(IN LPCWSTR sFileName, IN POBJECTID_ATTRIBUTE pObjId)
{
  if (NULL == pObjId)
  {
    return FALSE;
  }

  DWORD dwRet;
  HANDLE hNew = OpenFileForWrite(sFileName,
    (GetFileAttributes(sFileName) & FILE_ATTRIBUTE_DIRECTORY));

  if (hNew == INVALID_HANDLE_VALUE)
  {    
    return FALSE;
  }  

  if (!DeviceIoControl(hNew,
     FSCTL_SET_OBJECT_ID_EXTENDED,
     pObjId->ExtendedInfo, sizeof(pObjId->ExtendedInfo), NULL, 0, &dwRet, NULL))
  {
    CloseHandle(hNew);
    return FALSE;
  }

  CloseHandle(hNew);
  return TRUE;
}
