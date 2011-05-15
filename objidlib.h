#ifdef OBJIDLIB_EXPORTS
#define OBJIDLIB_API extern "C" __declspec(dllexport)
#else
#define OBJIDLIB_API extern "C" __declspec(dllimport)
#endif

OBJIDLIB_API BOOL CreateObjectId(LPCWSTR sFileName, POBJECTID_ATTRIBUTE pObjId);
OBJIDLIB_API BOOL GetObjectId(LPCWSTR sFileName, POBJECTID_ATTRIBUTE pObjId);
OBJIDLIB_API BOOL ObjectIdExists(LPCWSTR sFileName);
OBJIDLIB_API BOOL DeleteObjectId(LPCWSTR sFileName);
