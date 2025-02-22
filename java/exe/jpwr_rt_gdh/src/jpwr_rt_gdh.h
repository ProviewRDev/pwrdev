/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class jpwr_rt_Gdh */

#ifndef _Included_jpwr_rt_Gdh
#define _Included_jpwr_rt_Gdh
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     jpwr_rt_Gdh
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jpwr_rt_Gdh_init
  (JNIEnv *, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    setObjectInfoFloat
 * Signature: (Ljava/lang/String;F)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_setObjectInfoFloat
  (JNIEnv *, jobject, jstring, jfloat);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    setObjectInfoInt
 * Signature: (Ljava/lang/String;I)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_setObjectInfoInt
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    setObjectInfoBoolean
 * Signature: (Ljava/lang/String;Z)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_setObjectInfoBoolean
  (JNIEnv *, jobject, jstring, jboolean);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    setObjectInfoString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_setObjectInfoString
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoFloat
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrFloat;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoFloat
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoInt
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrInt;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoInt
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoBoolean
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrBoolean;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoBoolean
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoString
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoString
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoObjid
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoObjid
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoFloatArray
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrFloatArray;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoFloatArray
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectInfoIntArray
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrIntArray;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectInfoIntArray
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    toggleObjectInfo
 * Signature: (Ljava/lang/String;)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_toggleObjectInfo
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    refObjectInfo
 * Signature: (Ljava/lang/String;)Ljpwr/rt/GdhrRefObjectInfo;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_refObjectInfo
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoFloat
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoFloat
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoBoolean
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoBoolean
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoInt
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoString
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoFloatArray
 * Signature: (II)[F
 */
JNIEXPORT jfloatArray JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoFloatArray
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoBooleanArray
 * Signature: (II)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoBooleanArray
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoIntArray
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoIntArray
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectRefInfoStringArray
 * Signature: (IIII)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_jpwr_rt_Gdh_getObjectRefInfoStringArray
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    unrefObjectInfo
 * Signature: (Ljpwr/rt/PwrtRefId;)Ljpwr/rt/PwrtStatus;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_unrefObjectInfo
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    nameToObjid
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_nameToObjid
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    nameToAttrRef
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrAttrRef;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_nameToAttrRef
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    objidToName
 * Signature: (Ljpwr/rt/PwrtObjid;I)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_objidToName
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    attrRefToName
 * Signature: (Ljpwr/rt/PwrtAttrRef;I)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_attrRefToName
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getRootList
 * Signature: ()Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getRootList
  (JNIEnv *, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getNextObject
 * Signature: (Ljpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getNextObject
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getChild
 * Signature: (Ljpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getChild
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getParent
 * Signature: (Ljpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getParent
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getNextSibling
 * Signature: (Ljpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getNextSibling
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectClass
 * Signature: (Ljpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrClassId;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectClass
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getAttrRefTid
 * Signature: (Ljpwr/rt/PwrtAttrRef;)Ljpwr/rt/CdhrTypeId;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getAttrRefTid
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    classNameToId
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrClassId;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_classNameToId
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getClassList
 * Signature: (I)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getClassList
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    classIdToObjid
 * Signature: (I)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_classIdToObjid
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getNodeObject
 * Signature: (I)Ljpwr/rt/CdhrObjid;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getNodeObject
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getAttributeChar
 * Signature: (Ljava/lang/String;)Ljpwr/rt/GdhrGetAttributeChar;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getAttributeChar
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getAttributeFlags
 * Signature: (Ljava/lang/String;)Ljpwr/rt/GdhrGetAttributeFlags;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getAttributeFlags
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getClassAttribute
 * Signature: (ILjpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrObjAttr;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getClassAttribute
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    translateFilename
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jpwr_rt_Gdh_translateFilename
  (JNIEnv *, jclass, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    crrSignal
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_crrSignal
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    crrObject
 * Signature: (Ljava/lang/String;)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_crrObject
  (JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getMsg
 * Signature: (I)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getMsg
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getMsgText
 * Signature: (I)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getMsgText
  (JNIEnv *, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getSuperClass
 * Signature: (ILjpwr/rt/PwrtObjid;)Ljpwr/rt/CdhrClassId;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getSuperClass
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectBodyDef
 * Signature: (ILjpwr/rt/PwrtAttrRef;)Ljpwr/rt/GdhrsAttrDef;
 */
JNIEXPORT jobjectArray JNICALL Java_jpwr_rt_Gdh_getObjectBodyDef
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getCircBuffInfo
 * Signature: (Ljpwr/rt/CircBuffInfo;)I
 */
JNIEXPORT jint JNICALL Java_jpwr_rt_Gdh_getCircBuffInfo
  (JNIEnv *, jobject, jobject);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    updateCircBuffInfo
 * Signature: (Ljpwr/rt/CircBuffInfo[];I)I
 */
JNIEXPORT jint JNICALL Java_jpwr_rt_Gdh_updateCircBuffInfo
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getDsTrend
 * Signature: (Ljava/lang/String;III)Ljpwr/rt/GdhrGetDsTrend;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getDsTrend
(JNIEnv *, jobject, jstring, jint, jint, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getSevItemInfo
 * Signature: (Ljava/lang/String;)Ljpwr/rt/GdhrSevItemInfo;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getSevItemInfo
(JNIEnv *, jobject, jstring);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getSevItemData
 * Signature: (Ljava/lang/String;Ljpwr/rt/PwrtObjid;Ljava/lang/String;FI)Ljpwr/rt/GdhrSevItemData;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getSevItemData
(JNIEnv *, jobject, jstring, jobject, jstring, jfloat, jint);

/*
 * Class:     jpwr_rt_Gdh
 * Method:    getObjectEnumText
 * Signature: (Ljava/lang/String;I)Ljpwr/rt/CdhrString;
 */
JNIEXPORT jobject JNICALL Java_jpwr_rt_Gdh_getObjectEnumText
  (JNIEnv *, jobject, jstring, jint);

#ifdef __cplusplus
}
#endif
#endif
