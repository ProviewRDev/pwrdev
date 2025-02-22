/* 
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2025 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 */

package jpwr.jop;
//package navigator;
import java.util.Enumeration;
import java.util.Vector;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
//import jpwr.jop.JopDynamic;
//import jpwr.jop.JopEngine;
import jpwr.rt.*;

/**
 *  Xtt object class.
 *
 *  @author     JN
 */
public class XttObj extends DynamicObj implements JopDynamic
{
  public Vector attrVector = null;
  public CdhrClassId classId = null;
  public String className = null;
  public boolean debug = false;
  public String description = " ";
  public JopEngine en;
//  public String fullName = null;
  public Gdh gdh;
  public boolean hasBeenReferenced = false;
  public boolean hasChildren = false;
  public int lengthToSecondCol = 15;
//  public String name = null;
  public CdhrObjid objId = null;

  public CdhrAttrRef aref = null;

  public XttRefObj refObj = null;
  public int sts = 2;
  DefaultMutableTreeNode treeNode;
//  public String showableClassName = "Dv";
  private static String isXttObjStr = "XttObj";


  /**
   *  Constructor for the XttObj object
   */
  public XttObj(Gdh gdh, JopEngine en, String fullName, String name, String className, CdhrObjid objId, CdhrClassId classId)
  {
    this.gdh = gdh;
    this.en = en;
    this.fullName = fullName;
    this.name = name;
    this.className = className;
    this.objId = objId;
    this.classId = classId;
    CdhrString ret = en.gdh.getObjectInfoString(this.fullName + "." + "Description");
    if(ret.oddSts())
    {
      this.description = ret.str;
    }
  }


  /**
   *  Constructor for the XttObj object
   *
   *@param  dummystring  Description of the Parameter
   */
  public XttObj(String dummystring)
  {
    this.name = dummystring;
  }


  /**
   *  Constructor for the XttObj object
   *
   */
  public XttObj(GdhrGetXttObj obj, Gdh gdh, JopEngine en)
  {
    this.name = obj.name;
    this.fullName = obj.fullName;
    this.description = obj.description;
    this.className = obj.className;
    this.objId = obj.cdhrObjId;
    this.classId = obj.cdhrClassId;
    this.sts = obj.sts;
    this.hasChildren = obj.hasChildren;
    this.gdh = gdh;
    this.en = en;
  }


  /**
   *  Adds a feature to the AttrVector attribute of the XttObj object
   *
   *  @param  attrVector  The feature to be added to the AttrVector attribute
   */
  public void addAttrVector(Vector attrVector)
  {
    if(!this.hasBeenReferenced)
    {
      this.attrVector = attrVector;
      en.add(this);
      this.hasBeenReferenced = true;
    }
  }



  public Object dynamicGetRoot() 
  {
    return null;
  }

  public void dynamicClose()
  {
    Logg.logg("XttObj: dynamic close", 6);
  }



  public void dynamicOpen()
  {
    Logg.logg("XttObj: Dynamic open", 6);
    String s = null;
    XttObjAttr obj;
    String suffix;
    GdhrRefObjectInfo ret;
    Vector<String> refVec = new Vector<String>();
    //Vector refVec = new Vector();
    for(int i = 0; i < attrVector.size(); i++)
    {
      obj = ((XttObjAttr)attrVector.get(i));
      suffix = this.setTypeIdString(obj.type, obj.size);
      s = this.fullName + "." + obj.name + suffix;
      obj.fullName = s;
      Logg.logg("XttObj:  Name " + s + "Type " + this.className, 6);
      if( ((obj.flags & Pwr.mAdef_array) > 0) &&
          ((obj.flags & Pwr.mAdef_class) <= 0) )
      {
        Logg.logg("XttObj:  Found array, what to do now?? " + s, 6);
        XttArrayAttr arrayAttr = new XttArrayAttr(s);
        DefaultMutableTreeNode arrayChildNode = new DefaultMutableTreeNode(arrayAttr);
        obj.treeNode.add(arrayChildNode);
      }
      else if((obj.flags & Pwr.mAdef_class) > 0)
      {
        Logg.logg("XttObj:  Found class, what to do now?? " + this.fullName + " " + obj.name , 1);

	//	CdhrObjid cdhrObjid = gdh.nameToObjid(this.fullName + obj.name);
	//	System.out.println("nameToObjid " + cdhrObjid.getSts());

	//	CdhrClassId cdhrClassId = gdh.getObjectClass(cdhrObjid.objid);
	/*       
       	CdhrAttrRef aref = gdh.nameToAttrRef(this.fullName + "." + obj.name);
	System.out.println("nameToAttrRef " + aref.getSts());

      	CdhrTypeId tid = gdh.getAttrRefTid(aref.aref);
	String className = gdh.attrRefToName(aref.aref, Cdh.mName_object).str;

        XttObj classObj = new XttObj(this.en.gdh, 
                                     this.en, 
                                     this.fullName + "." + obj.name, 
                                     obj.name,
				     className,
				     new CdhrObjid(aref.aref.getObjid(),aref.getSts()),
				     new CdhrClassId(tid.typeId, tid.sts)); 
	System.out.println("classObj: " + classObj.hasChildren + " " + className);
	*/
	/*
        XttObj classObj = new XttObj(this.en.gdh, 
                                     this.en, 
                                     this.fullName + obj.name, 
                                     obj.name,
				     "Test",
				     cdhrObjid,
				     cdhrClassId); 
	*/
	//        DefaultMutableTreeNode classChildNode = new DefaultMutableTreeNode(classObj);
	//        obj.treeNode.add(classChildNode);
      }
      // Data pointer should be PRIVATE but id not?
      else if(obj.name.compareTo("DataPointer") != 0)
      {
        refVec.add(s);
      }
    }
    Logg.logg("Innan refObjectInfo_Vector", 6);
    Vector retVec = gdh.refObjectInfo_Vector(refVec);
    Logg.logg("Efter refObjectInfo_Vector", 6);
    if(retVec == null)
    {
      Logg.logg("XttObj: refObjectInfo_Vector(" + s + ") returnerar null", 1);
      return;
    }
    else
    {
      int j = 0;
      for(int i = 0; i < retVec.size(); i++)
      {
        ret = (GdhrRefObjectInfo)retVec.get(i);
        if(ret.evenSts())
        {
          Logg.logg("XttObj:  refObjectInfo_Vector(" + s + ") Error ", 1);
        }

        // DataPointer should be PRIVATE but is not?
        while(j < attrVector.size() &&
            (((XttObjAttr)attrVector.get(j)).name.compareTo("DataPointer") == 0 ||
            ((((XttObjAttr)attrVector.get(j)).flags & Pwr.mAdef_array) > 0) ||
	     ((((XttObjAttr)attrVector.get(j)).flags & Pwr.mAdef_class) > 0)) )
        {
          Logg.logg("Hittat datapointer", 1);
          j++;
        }
        if(j < attrVector.size())
        {
          obj = ((XttObjAttr)attrVector.get(j));
          obj.refObj = ret;
        }
        j++;
      }
      en.setFrameReady();
    }
    Logg.logg("refVec=" + refVec.size() + " retVec=" + retVec.size() + "attrVector=" + attrVector.size(), 6);
  }


  public void dynamicUpdate(boolean animationOnly)
  {
    if(animationOnly)
    {
      Logg.logg("XttObj: animationOnly", 6);
      return;
    }
    Logg.logg("XttObj:  dynamicUpdate", 6);
    if(this.attrVector == null)
    {
      Logg.logg("XttObj:  null-attrVector hittad i XttObj dynamicUpdate()", 1);
      return;
    }
    for(int i = 0; i < this.attrVector.size(); i++)
    {
      XttObjAttr obj = (XttObjAttr)this.attrVector.get(i);
      if( ((obj.flags & Pwr.mAdef_array) > 0) &&
          ((obj.flags & Pwr.mAdef_class) <= 0) )
      {
        Enumeration enm = obj.treeNode.children();
        while(enm.hasMoreElements())
        {
          DefaultMutableTreeNode child = (DefaultMutableTreeNode)enm.nextElement();
          XttArrayAttr arrayObj = (XttArrayAttr)child.getUserObject();
          if(arrayObj.elements == obj.elements)
          {
            this.setObjectAttributeValue(arrayObj);
          }
        }

      }
      else if((obj.flags & Pwr.mAdef_class) > 0)
      {
	  /*
        Enumeration enm = obj.treeNode.children();
        while(enm.hasMoreElements())
        {
          DefaultMutableTreeNode child = (DefaultMutableTreeNode)enm.nextElement();
          XttArrayAttr arrayObj = (XttArrayAttr)child.getUserObject();
          if(arrayObj.elements == obj.elements)
          {
            this.setObjectAttributeValue(arrayObj);
          }
        }
	  */

      }
      else if(obj.name.compareTo("DataPointer") != 0)
      {
        Logg.logg("obj.type " + obj.type + " i= " + i, 6);
        this.setObjectAttributeValue(obj);
      }
    }
  }


  public void init(boolean debug)
  {
    if(!debug)
    {
      return;
    }
    if(!XttRefObj.initDone)
    {
      XttRefObj.init(this.en);
    }
    int index = XttRefObj.checkTypeName(this.className);
    if(index >= 0)
    {
      this.refObj = new XttRefObj(this.fullName, this.en, this.classId.classId, this.treeNode, index);
    }
  }


  public void localDynamicClose()
  {
    Logg.logg("XttObj: localDynamic close_vector", 6);
    Vector<PwrtRefId> unref_vec = new Vector<PwrtRefId>();
    //Vector unref_vec = new Vector();
    for(int i = 0; i < attrVector.size(); i++)
    {
      XttObjAttr obj = (XttObjAttr)attrVector.get(i);
      if ( obj.refObj == null || obj.refObj.id == 0)
	continue;

      if( ((obj.flags & Pwr.mAdef_array) > 0) &&
          ((obj.flags & Pwr.mAdef_class) <= 0) )
      {
        Enumeration enm = obj.treeNode.children();
        while(enm.hasMoreElements())
        {
          DefaultMutableTreeNode child = (DefaultMutableTreeNode)enm.nextElement();
          XttArrayAttr arrayObj = (XttArrayAttr)child.getUserObject();
          if(arrayObj.elements == obj.elements)
          {
            unref_vec.add(arrayObj.refObj.refid);
          }
        }
      }
      else if((obj.flags & Pwr.mAdef_class) > 0)
      {
	  /*
        Enumeration enm = obj.treeNode.children();
        while(enm.hasMoreElements())
        {
          DefaultMutableTreeNode child = (DefaultMutableTreeNode)enm.nextElement();
          XttArrayAttr arrayObj = (XttArrayAttr)child.getUserObject();
          if(arrayObj.elements == obj.elements)
          {
            unref_vec.add(arrayObj.refObj.refid);
          }
        }
	  */
      }
      // DataPointer should be PRIVATE but is not?
      else if(obj.name.compareTo("DataPointer") != 0)
      {
        Logg.logg(" " + obj.refObj.refid.rix, 6);
        unref_vec.add(obj.refObj.refid);
      }
    }
    gdh.unrefObjectInfo_Vector(unref_vec);

  }


  public void removeAttrVector()
  {
    this.debug = false;
    if(this.hasBeenReferenced)
    {
      en.remove(this);
      this.localDynamicClose();
      this.attrVector = null;
      this.hasBeenReferenced = false;
    }
    else if(this.refObj != null)
    {
      this.refObj.localDynamicClose();
    }

  }


  /**
   *  A unit test for JUnit
   */
  public String test()
  {
    return "Det gick ju bra";
  }


  public String toString()
  {
    int spaceLength = this.lengthToSecondCol - this.name.length();
    String space = " ";
    while(spaceLength > 1)
    {
      spaceLength--;
      space += " ";
    }
    if(this.refObj != null)
    {
      return this.name + space + this.className + "  " + this.refObj.toString();
    }
    else
    {
      return this.name + space + this.className + "    " + this.description;
    }
  }


  /**
   *  Gets the xttObj attribute of the XttObj class
   *@return    The xttObj value
   */
  public static boolean isXttObj(Object o)
  {
    return o.getClass().getName() == isXttObjStr;
  }
}

