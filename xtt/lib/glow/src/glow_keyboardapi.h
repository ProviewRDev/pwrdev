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

#ifndef glow_keyboardapi_h
#define glow_keyboardapi_h

#ifndef __cplusplus
typedef void* KeyboardCtx;
#ifndef glow_api_h
typedef void* GlowCtx;
#endif
#else
#include "glow_keyboardctx.h"
#endif

#include "glow.h"

#if defined __cplusplus
extern "C" {
#endif

/*! \file glow_keyboardapi.h
    \brief Contains c API for the color palette, i.e. the KeyboardCtx class. */
/*! \addtogroup GlowKeyboardApi */
/*@{*/

typedef KeyboardCtx* keyboard_tCtx;

//! Enable an event an register a callback function for the event.
/*!
  \param ctx		Colorpalette context.
  \param event	Event to enable.
  \param event_type	Eventtype of the event.
  \param event_cb	Callback function for the event.
*/
void keyboard_EnableEvent(KeyboardCtx* ctx, glow_eEvent event,
    glow_eEventType event_type,
    int (*event_cb)(GlowCtx* ctx, glow_tEvent event));

//! Disable an event.
/*!
  \param ctx		Colorpalette context.
  \param event	Event to disable.
*/
void keyboard_DisableEvent(KeyboardCtx* ctx, glow_eEvent event);

//! Disable all events.
/*!
  \param ctx		Colorpalette context.
*/
void keyboard_DisableEventAll(KeyboardCtx* ctx);

//! Get user data.
/*!
  \param ctx		Colorpalette context.
  \param user_data	User data.
*/
void keyboard_GetCtxUserData(keyboard_tCtx ctx, void** user_data);

//! Set user data.
/*!
  \param ctx		Colorpalette context.
  \param user_data	User data.
*/
void keyboard_SetCtxUserData(keyboard_tCtx ctx, void* user_data);

void keyboard_SetKeymap(keyboard_tCtx ctx, keyboard_eKeymap keymap);
void keyboard_SetType(keyboard_tCtx ctx, keyboard_eType type);
void keyboard_SetDefaultConfig(keyboard_eKeymap keymap, keyboard_eType type);
void keyboard_GetSize(keyboard_tCtx ctx, int* width, int* height);
void keyboard_SetSize(keyboard_tCtx ctx, int width, int height);
void keyboard_SetShift(keyboard_tCtx ctx, int shift);

/*@}*/
#if defined __cplusplus
}
#endif
#endif
