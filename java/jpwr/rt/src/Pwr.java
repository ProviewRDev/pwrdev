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


package jpwr.rt;

/**
 * Proview basic declarations.
 *
 * @author	cs
 */
public class Pwr {
  public static final int eTix_Boolean 		= 1;
  public static final int eTix_Float32 		= 2;
  public static final int eTix_Float64 		= 3;
  public static final int eTix_Char 		= 4;
  public static final int eTix_Int8 		= 5;
  public static final int eTix_Int16 		= 6;
  public static final int eTix_Int32 		= 7;
  public static final int eTix_UInt8 		= 8;
  public static final int eTix_UInt16 		= 9;
  public static final int eTix_UInt32 		= 10;
  public static final int eTix_Objid 		= 11;
  public static final int eTix_Buffer 		= 12;
  public static final int eTix_String 		= 13;
  public static final int eTix_Enum		= 14;
  public static final int eTix_Struct 		= 15;
  public static final int eTix_Mask 		= 16;
  public static final int eTix_Array 		= 17;
  public static final int eTix_Time 		= 18;
  public static final int eTix_Text 		= 19;
  public static final int eTix_AttrRef 		= 20;
  public static final int eTix_UInt64 		= 21;
  public static final int eTix_Int64 		= 22;
  public static final int eTix_ClassId 		= 23;
  public static final int eTix_TypeId 		= 24;
  public static final int eTix_VolumeId 	= 25;
  public static final int eTix_ObjectIx 	= 26;
  public static final int eTix_RefId 		= 27;
  public static final int eTix_DeltaTime 	= 28;
  public static final int eTix_Status	 	= 29;
  public static final int eTix_NetStatus 	= 30;
  public static final int eTix_CastId 		= 31;
  public static final int eTix_ProString 	= 32;
  public static final int eTix_DisableAttr 	= 33;
  public static final int eTix_DataRef	 	= 34;
  public static final int eType_Unknown		= 0;
  public static final int eType_Boolean		= (1 << 16) + (1 << 15) + eTix_Boolean;
  public static final int eType_Float32		= (1 << 16) + (1 << 15) + eTix_Float32;
  public static final int eType_Float64		= (1 << 16) + (1 << 15) + eTix_Float64;
  public static final int eType_Char		= (1 << 16) + (1 << 15) + eTix_Char;
  public static final int eType_Int8		= (1 << 16) + (1 << 15) + eTix_Int8;
  public static final int eType_Int16		= (1 << 16) + (1 << 15) + eTix_Int16;
  public static final int eType_Int32		= (1 << 16) + (1 << 15) + eTix_Int32;
  public static final int eType_UInt8		= (1 << 16) + (1 << 15) + eTix_UInt8;
  public static final int eType_UInt16		= (1 << 16) + (1 << 15) + eTix_UInt16;
  public static final int eType_UInt32		= (1 << 16) + (1 << 15) + eTix_UInt32;
  public static final int eType_Objid		= (1 << 16) + (1 << 15) + eTix_Objid;
  public static final int eType_Buffer		= (1 << 16) + (1 << 15) + eTix_Buffer;
  public static final int eType_String		= (1 << 16) + (1 << 15) + eTix_String;
  public static final int eType_Enum		= (1 << 16) + (1 << 15) + eTix_Enum;
  public static final int eType_Struct		= (1 << 16) + (1 << 15) + eTix_Struct;
  public static final int eType_Mask		= (1 << 16) + (1 << 15) + eTix_Mask;
  public static final int eType_Array		= (1 << 16) + (1 << 15) + eTix_Array;
  public static final int eType_Time		= (1 << 16) + (1 << 15) + eTix_Time;
  public static final int eType_Text		= (1 << 16) + (1 << 15) + eTix_Text;
  public static final int eType_AttrRef		= (1 << 16) + (1 << 15) + eTix_AttrRef;
  public static final int eType_UInt64		= (1 << 16) + (1 << 15) + eTix_UInt64;
  public static final int eType_Int64		= (1 << 16) + (1 << 15) + eTix_Int64;
  public static final int eType_ClassId		= (1 << 16) + (1 << 15) + eTix_ClassId;
  public static final int eType_TypeId		= (1 << 16) + (1 << 15) + eTix_TypeId;
  public static final int eType_VolumeId	= (1 << 16) + (1 << 15) + eTix_VolumeId;
  public static final int eType_ObjectIx	= (1 << 16) + (1 << 15) + eTix_ObjectIx;
  public static final int eType_RefId		= (1 << 16) + (1 << 15) + eTix_RefId;
  public static final int eType_DeltaTime	= (1 << 16) + (1 << 15) + eTix_DeltaTime;
  public static final int eType_Status		= (1 << 16) + (1 << 15) + eTix_Status;
  public static final int eType_NetStatus	= (1 << 16) + (1 << 15) + eTix_NetStatus;
  public static final int eType_CastId		= (1 << 16) + (1 << 15) + eTix_CastId;
  public static final int eType_ProString	= (1 << 16) + (1 << 15) + eTix_ProString;
  public static final int eType_DisableAttr	= (1 << 16) + (1 << 15) + eTix_DisableAttr;
  public static final int eType_DataRef		= (1 << 16) + (1 << 15) + eTix_DataRef;

  public static final int mPrv_RtRead		= 1 << 0;
  public static final int mPrv_RtWrite		= 1 << 1;
  public static final int mPrv_System		= 1 << 2;
  public static final int mPrv_Maintenance	= 1 << 3;
  public static final int mPrv_Process		= 1 << 4;
  public static final int mPrv_Instrument	= 1 << 5;
  public static final int mPrv_Operator1	= 1 << 6;
  public static final int mPrv_Operator2	= 1 << 7;
  public static final int mPrv_Operator3	= 1 << 8;
  public static final int mPrv_Operator4	= 1 << 9;
  public static final int mPrv_Operator5	= 1 << 10;
  public static final int mPrv_Operator6	= 1 << 11;
  public static final int mPrv_Operator7	= 1 << 12;
  public static final int mPrv_Operator8	= 1 << 13;
  public static final int mPrv_Operator9	= 1 << 14;
  public static final int mPrv_Operator10	= 1 << 15;
  public static final int mPrv_RtEventsAck     	= 1 << 18;
  public static final int mPrv_RtPlc		= 1 << 19;
  public static final int mPrv_RtNavigator     	= 1 << 20;
  public static final int mPrv_DevRead		= 1 << 21;
  public static final int mPrv_DevPlc		= 1 << 22;
  public static final int mPrv_DevConfig	= 1 << 23;
  public static final int mPrv_DevClass		= 1 << 24;
  public static final int mPrv_RtEventsBlock   	= 1 << 25;
  public static final int mPrv_Administrator   	= 1 << 26;
  public static final int mPrv_SevRead		= 1 << 27;
  public static final int mPrv_SevAdmin		= 1 << 28;

  public static final int mAccess_RtRead       	= mPrv_RtRead;
  public static final int mAccess_RtWrite      	= mPrv_RtWrite;
  public static final int mAccess_System       	= mPrv_System;
  public static final int mAccess_Process      	= mPrv_Process;
  public static final int mAccess_Maintenance  	= mPrv_Maintenance;
  public static final int mAccess_Instrument    = mPrv_Instrument;
  public static final int mAccess_RtEventsBlock = mPrv_RtEventsBlock;
  public static final int mAccess_RtEventsAck 	= mPrv_RtEventsAck;
  public static final int mAccess_RtPlc 	= mPrv_RtPlc;
  public static final int mAccess_RtNavigator 	= mPrv_RtNavigator;
  public static final int mAccess_AllRt		= mPrv_System |
				  		  mPrv_Maintenance |
				  		  mPrv_Process |
                                                  mPrv_Instrument |
				                  mPrv_RtRead |
				                  mPrv_RtWrite |
				                  mPrv_RtEventsBlock |
				                  mPrv_RtEventsAck |
				                  mPrv_RtPlc |
				                  mPrv_RtNavigator |
						  mPrv_Operator1 |
						  mPrv_Operator2 |
						  mPrv_Operator3 |
						  mPrv_Operator4 |
						  mPrv_Operator5 |
						  mPrv_Operator6 |
						  mPrv_Operator7 |
						  mPrv_Operator8 |
						  mPrv_Operator9 |
						  mPrv_Operator10;
  public static final int mAccess_AllOperators	= mPrv_Operator1 |
						  mPrv_Operator2 |
						  mPrv_Operator3 |
						  mPrv_Operator4 |
						  mPrv_Operator5 |
						  mPrv_Operator6 |
						  mPrv_Operator7 |
						  mPrv_Operator8 |
						  mPrv_Operator9 |
						  mPrv_Operator10;
  public static final int mAccess_AllPwr	= ~0;
  
  public static final int mAdef_pointer	        = 1 << 0;		/*  1	  */
  public static final int mAdef_array	        = 1 << 1;		/*  2	  */
  public static final int mAdef_backup	        = 1 << 2;		/*  4	  */
  public static final int mAdef_changelog	= 1 << 3;		/*  8	  */
  public static final int mAdef_state	        = 1 << 4;		/*  16	  */
  public static final int mAdef_const	        = 1 << 5;		/*  32	  */
  public static final int mAdef_rtvirtual	= 1 << 6;		/*  64	  */
  public static final int mAdef_devbodyref	= 1 << 7;		/*  128	  */
  public static final int mAdef_dynamic	        = 1 << 8;		/*  256	  */
  public static final int mAdef_publicwrite	= 1 << 9;		/*  512	  */
  public static final int mAdef_noedit	        = 1 << 10;		/*  1024  */
  public static final int mAdef_invisible	= 1 << 11;		/*  2048  */
  public static final int mAdef_refdirect	= 1 << 12;		/*  4096  If an AttrRef should not give alue pointed to  */
  public static final int mAdef_noinvert	= 1 << 13;		/*  8192  */
  public static final int mAdef_noremove	= 1 << 14;		/*  16384 Cannot be removed, no used chkbx */
  public static final int mAdef_rtdbref	        = 1 << 15;		/*  32768 */
  public static final int mAdef_private	        = 1 << 16;		/*  65536 Private pointer, not to be displayed */
  public static final int mAdef_class	        = 1 << 17;
  public static final int mAdef_superclass      = 1 << 18;
  public static final int mAdef_buffer	        = 1 << 19;
  public static final int mAdef_nowbl       	= 1 << 20;		/* Don't print to wbl file */
  public static final int mAdef_alwayswbl       = 1 << 21;		/* Always print to wbl file */
  public static final int mAdef_disableattr     = 1 << 22;		/* Can be disabled */
  public static final int mAdef_rthide	        = 1 << 23;		/* Hide in runtime */


}





