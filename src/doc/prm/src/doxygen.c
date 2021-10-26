/*! \file doxygen.c
  \brief Contains group definitions for doxygen.
*/

/*! \defgroup PRM Programmer's Reference Manual

The Programmer's Referens Manual describes the interface to Proview for
applications programs written in c and c++.

The java interface is not yet included in this manual.

The intended audience is
- The application programmer who wants to write a program that attaches to the
Proview realtime database or other Proview facilites.
- Any user who wants to deepen his understanding of the system.
- Proview developers who uses these API's to improve and extend the Proview
functionallity.

Is is assumed that the reader has programming skills and some understanding of
the c programming language.

This manual will give a description of the following API's
- Gdh  The interface to the realtime database with objects, classes,
subscriptions etc.
- Mh   Sending alarms and messages from application programs.
- QCom Using the Proview communication protocol.
- Time Functions to convert and compare time values.
- Cdh  Convert and compare object attributes.
- Errh Print messages on system log.
- Sevcli Storage environment client interface.
- NMps Interface to NMps Cell objects.
- AProc Application process interface.
- Net  Network conversion functions.
- rt_appl A base class for applications i c++.

Python API
- <a href="../pwrrt.html">pwrrt Runtime API.</a>
- <a href="../pwrwb.html">pwrwb Development API.</a>
- <a href="../pwrtest.html">pwrrt Test log API.</a>
*/

////////////////////
// Subgroups in PRM
////////////////////

/*! \defgroup Pwr Pwr Proview Data definitions
  \ingroup PRM
  This module contains Proview basic data definitions.
*/

/*! \defgroup GDH Gdh Global Data Handler
  \ingroup PRM
  The Global Data Handler API contains functions to get information about
  the classes, volumes and objects in the realtime database. There are also
  functions to create, delete and move object in the database.

  <b>Includefiles</b><br>
  The includefile for Gdh is

  rt_gdh.h

  The return status codes are defined in the files

  rt_gdh_msg.h<br>
  rt_hash_msg.h

  <h2>Function overview</h2>
  <h3>Attribute Functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetAttributeCharacteristics()</td>
        <td>Get the characteristics of an attribute given its name and the class
  it belongs to.</td></tr>
     <tr><td>gdh_NameToAttrref()</td>
        <td>Convert from name string to attribute reference.</td></tr>
     <tr><td>gdh_AttrrefToName()</td>
        <td>Translate an attribute reference to name.</td></tr>
     <tr><td>gdh_ClassAttrToAttrref()</td>
        <td>Converts a class and attribute string to attrref format.</td></tr>
     <tr><td>gdh_ClassAttrrefToAttr()</td>
        <td>Get the attribute name from a class and attrref.</td></tr>
     <tr><td>gdh_GetAttributeCharAttrref()</td>
        <td>Get the characteristics of an attribute.</td></tr>
     <tr><td>gdh_AttrArefToObjectAref()</td>
        <td>Get the attriubte reference to an object or attribute object,
         from the attribute reference to an attribute in the object.</td></tr>
     <tr><td>gdh_ArefANameToAref()</td>
        <td>Get the attribute reference for an attribute in an object
        attribute reference, where the attribute is specified by a
  string.</td></tr>
     <tr><td>gdh_AttrValueToString()</td>
        <td>Convert attribute value to string.</td></tr>
     <tr><td>gdh_AttrStringToValue()</td>
        <td>Convert a string to attribute value.</td></tr>
     <tr><td>gdh_ArefDisabled()</td>
        <td>Check if an attribute is disabled.</td></tr>
    </table>

  <h3>Class and Type Functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetClassList()</td>
        <td>Get the object identity for the first object in the list of
        objects of a specified class.</td></tr>
     <tr><td>gdh_GetNextObject()</td>
        <td>Get the object identity of the next object in the list of
        objects of a certain class.</td></tr>
     <tr><td>gdh_GetPreviousObject()</td>
        <td>Get the object identity of the previous object in the list
        of objects of a certain class.</td></tr>
     <tr><td>gdh_GetClassListAttrRef()</td>
        <td>Get the attribute reference of the first object in the list
        of objects of a certain class. The object can be a solitary
        object or an attribute object.</td></tr>
     <tr><td>gdh_GetNextAttrRef()</td>
        <td>Get the attrref of the next object in the list of objects of
        a certain class. The object can be a solitary object or an
        attribute object.</td></tr>
     <tr><td>gdh_GetObjectClassList()</td>
        <td>Get the attribute reference of the first attribute object
        of a specified class in a specified object.</td></tr>
     <tr><td>gdh_GetNextObjectAttrRef()</td>
        <td>Get the attrref of the next attribute object of a
        specified class in a specified object.</td></tr>
     <tr><td>gdh_ClassNameToId()</td>
        <td>Get the class identity corresponding to a class specified
        by name.</td></tr>
     <tr><td>gdh_GetSuperClass()</td>
        <td>Get the superclass for a class.</td></tr>
     <tr><td>gdh_GetEnumValueDef()</td>
        <td>Get definition information for an enumeration type.</td></tr>
     <tr><td>gdh_GetMaskBitDef()</td>
        <td>Get definition information for a mask type.</td></tr>
    </table>

  <h3>Direct link/Subscription Functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_DLRefObjectInfoAttrref()</td>
        <td>Request a direct link to the data denoted by an attribute
        reference.</td></tr>
     <tr><td>gdh_DLUnrefObjectInfo()</td>
        <td>Terminate direct linking of an object or an object
  attribute.</td></tr>
     <tr><td>gdh_DLUnrefObjectInfoAll()</td>
        <td>Removes all direct links set up by this process.</td></tr>
     <tr><td>gdh_GetSubscriptionOldness()</td>
        <td>Checks the oldness of a certain subscription.</td></tr>
     <tr><td>gdh_RefObjectInfo()</td>
        <td>The routine returns an address to the data denoted by name.
        The name can be either an object or an object attribute.</td></tr>
     <tr><td>gdh_UnrefObjectInfo()</td>
        <td>Terminates the subscriptions defined by the subscription
  identity.</td></tr>
     <tr><td>gdh_UnrefObjectInfoAll()</td>
        <td>Terminates all subscriptions set up by this process.</td></tr>
     <tr><td>gdh_RefObjectInfoList()</td>
        <td>This routine returns addresses to teh information that is
        denoted by"objref[].fullname".</td></tr>
     <tr><td>gdh_SetSubscriptionDefaults()</td>
        <td>Sets default subscription timers for future subscription
        requests.</td></tr>
     <tr><td>gdh_SubRefObjectInfoAttrref()</td>
        <td>This routine returns a subid to the information that is
        denoted by 'aref'. The name can be either an object or an object
        attribute.</td></tr>
     <tr><td>gdh_SubRefObjectInfoName()</td>
        <td>Get the adress of the data that is denoted by 'name'.</td></tr>
     <tr><td>gdh_SubUnrefObjectInfoList()</td>
        <td>Terminates a list of subscriptions.</td></tr>
     <tr><td>gdh_SubUnrefObjectInfo()</td>
        <td>Terminates subscription of an object or attribute.</td></tr>
     <tr><td>gdh_SubUnrefObjectInfoAll()</td>
        <td>Terminates all subscriptions set up by this process.</td></tr>
     <tr><td>gdh_SubData()</td>
        <td>Fetch the data associated with a subscription.</td></tr>
     <tr><td>gdh_SubSize()</td>
        <td>Fetch the data size associated with a subscription.</td></tr>
     <tr><td>gdh_SubAssociateBuffer()</td>
        <td>Associate a buffer with the subscription.</td></tr>
     <tr><td>gdh_SubRefObjectInfoList()</td>
        <td>Sets up a list of subscriptions to an object/attribute.</td></tr>
   </table>
  <h3>File handling functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_FWriteObject()</td>
        <td>Write the content of an object to textfile.</td></tr>
     <tr><td>gdh_FReadObject()</td>
        <td>Read a textfile into an object.</td></tr>
    </table>

  <h3>Initializing Gdh</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_Init()</td>
        <td>Initialize the process to the Proview runtime environment.</td></tr>
    </table>

  <h3>Miscellaneous</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_NethandlerRunning()</td>
        <td>Check if the Nethandler has started.</td></tr>
     <tr><td>gdh_GetSecurityInfo()</td>
        <td>Get security information.</td></tr>
     <tr><td>gdh_StoreRtdbPointer()</td>
        <td>Convert pointer to rtdb relative pointer.</td></tr>
     <tr><td>gdh_TranslateRtdbPointer()</td>
        <td>Convert rtdb relative pointer to absolute pointer.</td></tr>
    </table>

  <h3>Node information</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetObjectNodeIndex()</td>
        <td>Get the node identity of the owner of an object.</td></tr>
     <tr><td>gdh_GetNodeIndex()</td>
        <td>Get the node identity of the local node.</td></tr>
     <tr><td>gdh_GetNodeObject()</td>
        <td>Return the objid of the node object for a node identifier
        (of type pwr_tNodeId).</td></tr>
    </table>

  <h3>Object manipulation functions</h3><h4> </h4><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_CreateObject()</td>
        <td>Create an object in a dynamic volume.</td></tr>
     <tr><td>gdh_DeleteObject()</td>
        <td>Remove a local object.</td></tr>
     <tr><td>gdh_RenameObject()</td>
        <td>Rename a local object.</td></tr>
     <tr><td>gdh_MoveObject()</td>
        <td>Move a local object.</td></tr>
    </table>

  <h3>Object tree functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetRootList()</td>
        <td>Get the first root object in the object tree.</td></tr>
     <tr><td>gdh_GetParent()</td>
        <td>Get the object identity of the parent of an object.</td></tr>
     <tr><td>gdh_GetLocalParent()</td>
        <td>Get the object identity of the local parent of an object.</td></tr>
     <tr><td>gdh_GetChild()</td>
        <td>Get the object identity of the first child of an object.</td></tr>
     <tr><td>gdh_GetNextSibling()</td>
        <td>Get the object identity of the next sibling (i.e.
        the next object with the same parent).</td></tr>
     <tr><td>gdh_GetPreviousSibling()</td>
        <td>Get the object identity of the previous sibling (i.e.
        the previous object with the same parent).</td></tr>
    </table>

  <h3>Object properties</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetObjectSize()</td>
        <td>Get the size of the body of an object.</td></tr>
     <tr><td>gdh_GetDynamicAttrSize()</td>
        <td>Get the size of a dynamic attribute.</td></tr>
     <tr><td>gdh_GetObjectClass()</td>
        <td>Get the class identifier of an object.</td></tr>
     <tr><td>gdh_GetAttrRefTid()</td>
        <td>Get the type or class identifier of an attribute
  reference.</td></tr>
     <tr><td>gdh_GetObjectLocation()</td>
        <td>Return the location of an object.</td></tr>
     <tr><td>gdh_GetObjectDLCount()</td>
        <td>Gets the direct link count of an object.</td></tr>
    </table>

  <h3>Object alarm status</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetAlarmInfo()</td>
        <td>Get the alarm status for a certain object.</td></tr>
     <tr><td>gdh_SetAlarmLevel()</td>
        <td>This routine sets the alarm level for a certain object.</td></tr>
     <tr><td>gdh_SetAlarmBlockLevel()</td>
        <td>Sets the alarm block level for a certain object.</td></tr>
    </table>

  <h3>Reading an writing object data</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetObjectInfo()</td>
        <td>Fetch data from an object or an attribute.</td></tr>
     <tr><td>gdh_GetObjectInfoAttrref()</td>
        <td>Fech the data of an attribute or a whole object body.</td></tr>
     <tr><td>gdh_SetObjectInfo()</td>
        <td>Change the value of an attribute of the whole body of an
  object.</td></tr>
     <tr><td>gdh_SetObjectInfoAttrref()</td>
        <td>Change the data of an object or attribute.</td></tr>
    </table>

  <h3>Threadsafe functions for times and strings</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetTimeDL()</td>
        <td>Fetch a direct linked absolute time value.</td></tr>
     <tr><td>gdh_SetTimeDL()</td>
        <td>Store a direct linked absolute time value.</td></tr>
     <tr><td>gdh_GetDeltaTimeDL()</td>
        <td>Fetch a direct linked delta time value.</td></tr>
     <tr><td>gdh_SetDeltaTimeDL()</td>
        <td>Store a direct linked delta time value.</td></tr>
     <tr><td>gdh_GetStrDL()</td>
        <td>Fetch a direct linked string value.</td></tr>
     <tr><td>gdh_SetStrDL()</td>
        <td>Store a direct linked string value.</td></tr>
     <tr><td>gdh_GetObjectInfoTime()</td>
        <td>Fetch an absolute time value from an attribute.</td></tr>
     <tr><td>gdh_SetObjectInfoTime()</td>
        <td>Store a absolute time value from an attribute.</td></tr>
     <tr><td>gdh_GetObjectInfoDeltaTime()</td>
        <td>Fetch a delta time value from an attribute.</td></tr>
     <tr><td>gdh_SetObjectInfoDeltaTime()</td>
        <td>Store a delta time value from an attribute.</td></tr>
     <tr><td>gdh_GetObjectInfoStr()</td>
        <td>Fetch a string value from an attribute.</td></tr>
     <tr><td>gdh_SetObjectInfoStr()</td>
        <td>Store a string value from an attribute.</td></tr>
    </table>

  <h3>Translating between different object representations</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_ObjidToName()</td>
        <td>Get the name of an object by its object identity.</td></tr>
     <tr><td>gdh_ObjidToPointer()</td>
        <td>Gets the address of the data of an object, given its object
  identity.</td></tr>
     <tr><td>gdh_MountObjidToPointer()</td>
        <td>Get a pointer to a mount object.</td></tr>
     <tr><td>gdh_AttrRefToPointer()</td>
        <td>Get the address of the data of an attribute reference.</td></tr>
     <tr><td>gdh_NameToObjid()</td>
        <td>Get the object identity of an object specified by name.</td></tr>
     <tr><td>gdh_NameToPointer()</td>
        <td>Get the address of the data of an object, given its name.</td></tr>
    </table>

  <h3>Volume functions</h3><h4> </h4>
    <table border="1" cellspacing="0">
     <tr><td>gdh_GetVolumeList()</td>
        <td>Get the first volume.</td></tr>
     <tr><td>gdh_GetNextVolume()</td>
        <td>Get the next volume in the volume list.</td></tr>
     <tr><td>gdh_VolumeIdToName()</td>
        <td>Get the name for a volume denoted by volume identity.</td></tr>
     <tr><td>gdh_GetVolumeInfo()</td>
        <td>Get info for a volume.</td></tr>
    </table>

*/

/*! \defgroup MSGH MhAppl MessageHandler API
  \ingroup PRM
  This module contains routines for the MessageHandler application interface.
*/

/*! \defgroup QCOM QCOM Queue Communication
  \ingroup PRM
  This module contains routines for the Queue Communication.
*/

/*! \defgroup Time Time management
  \ingroup PRM
  This module contains utilites for time management.
*/

/*! \defgroup Cdh Cdh Class definition handler
  \ingroup PRM
  This module contains functions for class definitions.
*/

/*! \defgroup Errh Errh System log messages
  \ingroup PRM
  This module contains functions for logging error messages.
*/

/*! \defgroup SEVCLI Sevcli SEV Client interface
  \ingroup PRM
  This module contains for the SEV cilent and is used to fetch historical
  data from the database.
*/

/*! \defgroup NMpsAppl NMpsAppl NMps API
  \ingroup PRM
  This module contains interface functions for application access to NMps.
*/

/*! \defgroup AProc AProc Application process interface
  \ingroup PRM
  This module contains functions to handle application processes.
*/

/*! \defgroup Net Net Network conversion functions
  \ingroup PRM
  This module contains functions for network conversion.
*/

/*! \defgroup rt rt_appl Base class for applications
  \ingroup PRM
  rt_appl is a base class for c++ applications.

  It contains initialization of gdh and qcom, and handles the restart and stop
  events
  when the runtime environment is restarted or stopped.

  A subclass should implement the virtual functions open(), close() and scan().
  <h2>open()</h2>
  open() is called after initialization, and may contain setup of direct links
  to
  objects and attributes in the realtime database.

  The open() function is also called after a soft restart.
  @see rt_appl::open

  <h2>scan()</h2>
  The scan() function is called cyclic with the scantime specified in the
  constuctor.

  @see rt_appl::scan
  <h2>close()</h2>
  The close() function is called when a restart or stop event is received. It
  should
  unlink all direct links.

  @see rt_appl::close
*/
