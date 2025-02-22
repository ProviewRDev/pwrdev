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

int rttsys_start_grafcet_monitor(menu_ctx ctx, pwr_tObjid plc_objid);
int rttsys_start_system_picture(menu_ctx ctx, char* picture_name);
int rttsys_get_current_logg_entry(int* entry);

int rttsys_get_login_picture(rtt_t_backgr** picture);

/*
int RTTSYS_SHOW_SUBSRV( menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_SHOW_SUBCLI( menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_SHOW_NODES( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_OBJECT_PID( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_OBJECT_AV( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_GRAFCET( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_GRAFCET_PLC( 	menu_ctx	ctx,
                                int		event,
                                char		*parameter_ptr,
                                char		*objectname,
                                char		**picture);
int RTTSYS_PLCPGM( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_ERROR( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_DEVICE( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_WATCHDOG(	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_PID( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_LOGGING( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_SHOW_SYS( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_OBJECT_CELL( menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);
int RTTSYS_NMPSCELL( 	menu_ctx	ctx,
                        int		event,
                        char		*parameter_ptr,
                        char		*objectname,
                        char		**picture);

*/
