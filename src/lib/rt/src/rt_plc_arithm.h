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

#ifndef rt_plc_arithm_h
#define rt_plc_arithm_h

void sum_exec(plc_sThread* tp, pwr_sClass_sum* object);
void maxmin_exec(plc_sThread* tp, pwr_sClass_maxmin* object);
void limit_exec(plc_sThread* tp, pwr_sClass_limit* object);
void comph_exec(plc_sThread* tp, pwr_sClass_comph* object);
void compl_exec(plc_sThread* tp, pwr_sClass_compl* object);
void select_exec(plc_sThread* tp, pwr_sClass_select* object);
void ramp_init(pwr_sClass_ramp* object);
void ramp_exec(plc_sThread* tp, pwr_sClass_ramp* object);
void filter_init(pwr_sClass_filter* object);
void filter_exec(plc_sThread* tp, pwr_sClass_filter* object);
void speed_exec(plc_sThread* tp, pwr_sClass_speed* object);
void timint_exec(plc_sThread* tp, pwr_sClass_timint* object);
void timemean_exec(plc_sThread* tp, pwr_sClass_timemean* o);
void curve_exec(plc_sThread* tp, pwr_sClass_curve* object);
void adelay_init(pwr_sClass_adelay* object);
void adelay_exec(plc_sThread* tp, pwr_sClass_adelay* object);
void pispeed_init(pwr_sClass_pispeed* object);
void pispeed_exec(plc_sThread* tp, pwr_sClass_pispeed* object);
void DtoMask_exec(plc_sThread* tp, pwr_sClass_DtoMask* object);
void MaskToD_exec(plc_sThread* tp, pwr_sClass_MaskToD* object);
void DtoEnum_exec(plc_sThread* tp, pwr_sClass_DtoEnum* object);
void EnumToD_exec(plc_sThread* tp, pwr_sClass_EnumToD* object);
void Mod_exec(plc_sThread* tp, pwr_sClass_Mod* object);
void Equal_exec(plc_sThread* tp, pwr_sClass_Equal* object);
void NotEqual_exec(plc_sThread* tp, pwr_sClass_NotEqual* object);
void GreaterEqual_exec(plc_sThread* tp, pwr_sClass_GreaterEqual* object);
void GreaterThan_exec(plc_sThread* tp, pwr_sClass_GreaterThan* object);
void LessEqual_exec(plc_sThread* tp, pwr_sClass_LessEqual* object);
void LessThan_exec(plc_sThread* tp, pwr_sClass_LessThan* object);
void IEqual_exec(plc_sThread* tp, pwr_sClass_IEqual* object);
void INotEqual_exec(plc_sThread* tp, pwr_sClass_INotEqual* object);
void IGreaterEqual_exec(plc_sThread* tp, pwr_sClass_IGreaterEqual* object);
void IGreaterThan_exec(plc_sThread* tp, pwr_sClass_IGreaterThan* object);
void ILessEqual_exec(plc_sThread* tp, pwr_sClass_ILessEqual* object);
void ILessThan_exec(plc_sThread* tp, pwr_sClass_ILessThan* object);
void IAdd_exec(plc_sThread* tp, pwr_sClass_IAdd* object);
void IMul_exec(plc_sThread* tp, pwr_sClass_IMul* object);
void ISub_exec(plc_sThread* tp, pwr_sClass_ISub* object);
void IDiv_exec(plc_sThread* tp, pwr_sClass_IDiv* object);
void IMax_exec(plc_sThread* tp, pwr_sClass_IMax* object);
void IMin_exec(plc_sThread* tp, pwr_sClass_IMin* object);
void ISel_exec(plc_sThread* tp, pwr_sClass_ISel* object);
void ILimit_exec(plc_sThread* tp, pwr_sClass_ILimit* object);
void IMux_exec(plc_sThread* tp, pwr_sClass_IMux* object);
void Mux_exec(plc_sThread* tp, pwr_sClass_Mux* object);
void Demux_exec(plc_sThread* tp, pwr_sClass_Demux* object);
void IDemux_exec(plc_sThread* tp, pwr_sClass_IDemux* object);
void Add_exec(plc_sThread* tp, pwr_sClass_Add* object);
void Mul_exec(plc_sThread* tp, pwr_sClass_Mul* object);
void Sub_exec(plc_sThread* tp, pwr_sClass_Sub* object);
void Div_exec(plc_sThread* tp, pwr_sClass_Div* object);
void Max_exec(plc_sThread* tp, pwr_sClass_Max* object);
void Min_exec(plc_sThread* tp, pwr_sClass_Min* object);
void Random_exec(plc_sThread* tp, pwr_sClass_Random* o);
void BwShiftLeft_exec(plc_sThread* tp, pwr_sClass_BwShiftLeft* object);
void BwShiftRight_exec(plc_sThread* tp, pwr_sClass_BwShiftRight* object);
void BwRotateRight_exec(plc_sThread* tp, pwr_sClass_BwRotateRight* object);
void BwRotateLeft_exec(plc_sThread* tp, pwr_sClass_BwRotateLeft* object);
void AtSel_exec(plc_sThread* tp, pwr_sClass_AtSel* object);
void DtSel_exec(plc_sThread* tp, pwr_sClass_DtSel* object);
void AtMux_exec(plc_sThread* tp, pwr_sClass_AtMux* object);
void DtMux_exec(plc_sThread* tp, pwr_sClass_DtMux* object);
void AtMin_exec(plc_sThread* tp, pwr_sClass_AtMin* object);
void AtMax_exec(plc_sThread* tp, pwr_sClass_AtMax* object);
void DtMin_exec(plc_sThread* tp, pwr_sClass_DtMin* object);
void DtMax_exec(plc_sThread* tp, pwr_sClass_DtMax* object);
void AtLimit_exec(plc_sThread* tp, pwr_sClass_AtLimit* object);
void DtLimit_exec(plc_sThread* tp, pwr_sClass_DtLimit* object);
void AtDemux_exec(plc_sThread* tp, pwr_sClass_AtDemux* object);
void DtDemux_exec(plc_sThread* tp, pwr_sClass_DtDemux* object);
void StrSel_exec(plc_sThread* tp, pwr_sClass_StrSel* object);
void StrMux_exec(plc_sThread* tp, pwr_sClass_StrMux* object);
void StrEqual_exec(plc_sThread* tp, pwr_sClass_StrEqual* object);
void StrNotEqual_exec(plc_sThread* tp, pwr_sClass_StrNotEqual* object);
void StrAdd_exec(plc_sThread* tp, pwr_sClass_StrAdd* object);
void StrTrim_exec(plc_sThread* tp, pwr_sClass_StrTrim* object);
void StrParse_exec(plc_sThread* tp, pwr_sClass_StrParse* object);
void StatusSeverity_exec(plc_sThread* tp, pwr_sClass_StatusSeverity* object);

#endif
