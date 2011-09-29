/** @file     objidlib.h
 *  @brief    A header file of a library for
 *            working with NTFS Object Idenitifers
 *  @author   amdf
 *  @version  0.2
 *  @date     September 2011
 */

#ifdef OBJIDLIB_EXPORTS
//! Dll export definition
#define OBJIDLIB_API extern "C" __declspec(dllexport)
#else
#define OBJIDLIB_API extern "C" __declspec(dllimport)
#endif

#ifndef OBJECTID_ATTRIBUTE
//! Object id file stream
typedef struct 
{
  /** The identifier that uniquely identifies the file or directory
   *  within the volume on which it resides. This is the portion of
   *  the object id that is indexed.
   */
  GUID ObjectId;
  union
  {
    struct
    {
      /** The identifier of the volume on which the object resided
       * when the object identifier was created, or zero if the
       * volume had no object identifier at that time. After copy
       * operations, move operations, or other file operations,
       * this may not be the same as the object identifier of the
       * volume on which the object presently resides.
       */
      GUID BirthVolumeId;
      /** The object identifier of the object at the time it was created.
       * After copy operations, move operations, or other file operations,
       * this may not be the same as the ObjectId member at present.
       */
      GUID BirthObjectId;
      /**
       * Reserved; should be zero.
       */
      GUID DomainId;
    };
    /** This portion of the object id is not indexed,
      * it's just some metadata for the user's benefit.
      */
    UCHAR ExtendedInfo[48];
  };
} OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;
#endif

OBJIDLIB_API BOOL CreateObjectId(IN LPCWSTR sFileName, OUT POBJECTID_ATTRIBUTE pObjId);
OBJIDLIB_API BOOL GetObjectId(IN LPCWSTR sFileName, OUT POBJECTID_ATTRIBUTE pObjId);
OBJIDLIB_API BOOL ObjectIdExists(IN LPCWSTR sFileName);
OBJIDLIB_API BOOL DeleteObjectId(IN LPCWSTR sFileName);
OBJIDLIB_API BOOL SetObjectId(IN LPCWSTR sFileName, IN POBJECTID_ATTRIBUTE pObjId);
OBJIDLIB_API BOOL SetObjectIdExt(IN LPCWSTR sFileName, IN POBJECTID_ATTRIBUTE pObjId);
