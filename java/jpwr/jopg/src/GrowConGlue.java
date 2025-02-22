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

package jpwr.jopg;
import jpwr.rt.*;
import java.io.*;
import java.util.*;

public class GrowConGlue extends GrowNode {

    int line_width_up;
    int line_width_down;
    int line_width_left;
    int line_width_right;
    int border;
    
    public GrowConGlue(GrowCmn cmn) {
	super(cmn);
    }

    public int type() {
	return Glow.eObjectType_GrowConGlue;
    }

    public void open(BufferedReader reader) {
	String line;
	StringTokenizer token;
	boolean end_found = false;

	try {
	    while( (line = reader.readLine()) != null) {
		token = new StringTokenizer(line);
		int key = Integer.valueOf(token.nextToken());
		if ( cmn.debug) System.out.println( "GrowConGlue : " + line);

		switch ( key) {
		case Glow.eSave_GrowConGlue: 
		    break;
		case Glow.eSave_GrowConGlue_line_width_up: 
		    line_width_up = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowConGlue_line_width_down: 
		    line_width_down = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowConGlue_line_width_left: 
		    line_width_left = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowConGlue_line_width_right: 
		    line_width_right = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowConGlue_border: 
		    border = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowConGlue_node_part: 
		    super.open(reader); 
		    break;
		case Glow.eSave_End:
		    end_found = true;
		    break;
		default:
		    System.out.println( "Syntax error in GrowConGlue");
		    break;
		}
		if ( end_found)
		    break;
	    }
		
	} catch ( Exception e) {
	    System.out.println( "IOException GrowConGlue");
	}
    }

    public void draw() {
	draw(null, highlight, hot, null, null);
    }

    public void draw( GlowTransform t, int highlight, int hot, Object node, Object colornode)
    {
	if ( cmn.nodraw != 0)
	    return;
	int idx, idx_up, idx_down, idx_left, idx_right;
	int drawtype;
	int shift_drawtype;

	idx = (int)( cmn.mw.zoom_factor_y / cmn.mw.base_zoom_factor * line_width - 1);
	idx += hot;
	double x1, y1, x2, y2; 
	int ll_x, ll_y, ur_x, ur_y, m_x, m_y;

	if (t == null) {
	    x1 = x_left * cmn.mw.zoom_factor_x - cmn.mw.offset_x;
	    y1 = y_low * cmn.mw.zoom_factor_y - cmn.mw.offset_y;
	    x2 = x_right * cmn.mw.zoom_factor_x - cmn.mw.offset_x;
	    y2 = y_high * cmn.mw.zoom_factor_y - cmn.mw.offset_y;
	}
	else {
	    x1 = t.x( x_left, y_low) * cmn.mw.zoom_factor_x - cmn.mw.offset_x;
	    y1 = t.y( x_left, y_low) * cmn.mw.zoom_factor_y - cmn.mw.offset_y;
	    x2 = t.x( x_right, y_high) * cmn.mw.zoom_factor_x - cmn.mw.offset_x;
	    y2 = t.y( x_right, y_high) * cmn.mw.zoom_factor_y - cmn.mw.offset_y;
	}

	ll_x = (int)( Math.min( x1, x2) + 0.5);
	ur_x = (int)( Math.max( x1, x2) + 0.5);
	ll_y = (int)( Math.min( y1, y2) + 0.5);
	ur_y = (int)( Math.max( y1, y2) + 0.5);
	m_x = (int)((x1 + x2) / 2 + 0.5);
	m_y = (int)((y1 + y2) / 2 + 0.5);

	drawtype = GlowColor.get_drawtype( draw_type, Glow.eDrawType_LineHighlight,
					   highlight, (GrowNode)colornode, 0, 0);

	int lw_up, lw_down, lw_left, lw_right;

	lw_down = line_width_down;
	lw_right = line_width_right;
	lw_up = line_width_up;
	lw_left = line_width_left;

	idx_up = (int)( cmn.mw.zoom_factor_y / cmn.mw.base_zoom_factor * lw_up - 1);
	idx_up += hot;
	idx_down = (int)( cmn.mw.zoom_factor_y / cmn.mw.base_zoom_factor * lw_down - 1);
	idx_down += hot;
	idx_left = (int)( cmn.mw.zoom_factor_x / cmn.mw.base_zoom_factor * lw_left - 1);
	idx_left += hot;
	idx_right = (int)( cmn.mw.zoom_factor_x / cmn.mw.base_zoom_factor * lw_right - 1);
	idx_right += hot;

	idx = Math.max( 0, idx);
	idx = Math.min( idx, Glow.DRAW_TYPE_SIZE-1);
	idx_up = Math.max( 0, idx_up);
	idx_up = Math.min( idx_up, Glow.DRAW_TYPE_SIZE-1);
	idx_down = Math.max( 0, idx_down);
	idx_down = Math.min( idx_down, Glow.DRAW_TYPE_SIZE-1);
	idx_left = Math.max( 0, idx_left);
	idx_left = Math.min( idx_left, Glow.DRAW_TYPE_SIZE-1);
	idx_right = Math.max( 0, idx_right);
	idx_right = Math.min( idx_right, Glow.DRAW_TYPE_SIZE-1);

	if ( lw_up != -1 && lw_down == -1 &&
	     lw_right == -1 && lw_left == -1) {
	    // Up termination
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y, idx_up + 1, ur_y - m_y, drawtype);
	    if ( shadow != 0 && idx_up > 2) {
		if ( border != 0) {
		    idx_up -= 2;
		    m_y++;
		}

		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_up/2, m_y, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x + idx_up/2, m_y, shift_drawtype, 0, 0);

		if ( border != 0) {
		    idx_up += 2;
		    m_y--;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x + idx_up/2, m_y, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_up == -1 && lw_down != -1 &&
		  lw_right == -1 && lw_left == -1) {
	    // Down termination
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y - ll_y, drawtype);
	    if ( shadow != 0 && idx_down > 2) {
		if ( border != 0) {
		    idx_down -= 2;
		    m_y--;
		}

		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y, m_x + idx_down/2, m_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y, shift_drawtype, 0, 0);

		if ( border != 0) {
		    idx_down += 2;
		    m_y++;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y, m_x + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_up == -1 && lw_down == -1 &&
		  lw_right != -1 && lw_left == -1) {
	    // Right termination
	    cmn.gdraw.fill_rect( m_x, m_y - idx_right + idx_right/2, ur_x - m_x, idx_right + 1, drawtype);
	    if ( shadow != 0 && idx_right > 2) {
		if ( border != 0) {
		    idx_right -= 2;
		    m_x++;
		}

		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, m_x, m_y + idx_right/2, shift_drawtype, 0, 0);

		if ( border != 0) {
		    idx_right += 2;
		    m_x--;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, m_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_up == -1 && lw_down == -1 &&
		  lw_right == -1 && lw_left != -1) {
	    // Left termination
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x - ll_x, idx_left + 1, drawtype);
	    if ( shadow != 0 && idx_left > 2) {
		if ( border != 0) {
		    idx_left -= 2;
		    m_x--;
		}

		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_left + idx_left/2, m_x, m_y + idx_left/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);

		if ( border != 0) {
		    idx_left += 2;
		    m_x++;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_left + idx_left/2, m_x, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_up != -1 && lw_down != -1 &&
		  lw_right == -1 && lw_left == -1) {
	    // Vertical glue
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y, idx_up + 1, ur_y - m_y, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y - ll_y, drawtype);

	    if ( shadow != 0 && idx_down > 2) {
		if ( border != 0) {
		    idx_up -= 2;
		    idx_down -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_up += 2;
		    idx_down += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		if ( idx_down != idx_up) {
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y, m_x - idx_down + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y, m_x + idx_down/2, m_y, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left != -1 && lw_right != -1 &&
		  lw_up == -1 && lw_down == -1) {
	    // Horizontal glue
	    cmn.gdraw.fill_rect( m_x, m_y - idx_right + idx_right/2, ur_x - m_x, idx_right + 1, drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x - ll_x, idx_left + 1, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_right > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_right -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_left += 2;
		    idx_right += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		if ( idx_left != idx_right) {
		    cmn.gdraw.line( m_x, m_y - idx_right + idx_right/2, m_x, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x, m_y + idx_right/2, m_x, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left != -1 && lw_right == -1 &&
		  lw_up != -1 && lw_down == -1) {
	    // Left up corner
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y + idx_left/2, idx_up + 1, ur_y -  (m_y + idx_left/2), drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2 - ll_x + 1, idx_left + 1, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_left += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_left == -1 && lw_right != -1 &&
		  lw_up != -1 && lw_down == -1) {
	    // Right up corner
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, idx_up + 1, ur_y -  (m_y - idx_right + idx_right/2), drawtype);
	    cmn.gdraw.fill_rect( m_x + idx_up/2, m_y - idx_right + idx_right/2, ur_x - (m_x + idx_up/2), idx_right + 1, drawtype);
	    if ( shadow != 0 && (idx_right > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_right -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_right += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_left == -1 && lw_right != -1 &&
		  lw_up == -1 && lw_down != -1) {
	    // Right down corner
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y - idx_right + idx_right/2 - ll_y, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, ur_x - (m_x - idx_down + idx_down/2), idx_right + 1, drawtype);
	    if ( shadow != 0 && (idx_right > 2 || idx_down > 2)) {
		if ( border != 0) {
		    idx_right -= 2;
		    idx_down -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y + idx_right/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_right += 2;
		    idx_down += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_left != -1 && lw_right == -1 &&
		  lw_up == -1 && lw_down != -1) {
	    // Left down corner
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y + idx_left/2 - ll_y + 1, drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2 - ll_x, idx_left + 1, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_down > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_down -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x , m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x,  m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_left += 2;
		    idx_down += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x , m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x,  m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else if ( lw_left != -1 && lw_right != -1 &&
		  lw_up == -1 && lw_down != -1) {
	    // Left right down threeway
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y - ll_y + 1, drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x + idx_down/2 - ll_x, idx_left + 1, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, ur_x - (m_x - idx_down + idx_down/2), idx_right + 1, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_right > 2 || idx_down > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_right -= 2;
		    idx_down -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x , m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		if ( idx_right == idx_left)
		    cmn.gdraw.line( ll_x,  m_y + idx_left/2, ur_x, m_y + idx_left/2, shift_drawtype, 0, 0);
		else if ( idx_right > idx_left) {
		    cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_down + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		}
		else {
		    cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		}
		if ( border != 0) {
		    idx_left += 2;
		    idx_right += 2;
		    idx_down += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x , m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		if ( idx_right == idx_left)
		    cmn.gdraw.line( ll_x,  m_y + idx_left/2, ur_x, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		else if ( idx_right > idx_left) {
		    cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_down + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, m_y + idx_right/2, m_x - idx_down + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
		else {
		    cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_down/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_down/2, m_y + idx_right/2, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left != -1 && lw_right != -1 &&
		  lw_up != -1 && lw_down == -1) {
	    // Left right up threeway
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y, idx_up + 1, ur_y - m_y, drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2 - ll_x, idx_left + 1, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, ur_x - (m_x - idx_up + idx_up/2), idx_right + 1, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_right > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_right -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( idx_right == idx_left)
		    cmn.gdraw.line( ll_x,  m_y - idx_left + idx_left/2, ur_x, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		else if ( idx_right > idx_left) {
		    cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_up + idx_up/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		}
		else {
		    cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_right + idx_right/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x , m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_left += 2;
		    idx_right += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x , m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		if ( idx_right == idx_left)
		    cmn.gdraw.line( ll_x,  m_y - idx_left + idx_left/2, ur_x, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		else if ( idx_right > idx_left) {
		    cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_up + idx_up/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
		else {
		    cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_right + idx_right/2, m_x + idx_up/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left != -1 && lw_right == -1 &&
		  lw_up != -1 && lw_down != -1) {
	    // Left up down threeway
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x - ll_x, idx_left + 1, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y - idx_left + idx_left/2, idx_up + 1, ur_y - (m_y - idx_left + idx_left/2), drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y + idx_left/2 - ll_y, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_down > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_left -= 2;
		    idx_down -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, shift_drawtype, 0, 0);

		if ( idx_up == idx_down)
		    cmn.gdraw.line( m_x + idx_down/2,  ll_y, m_x + idx_down/2, ur_y, shift_drawtype, 0, 0);
		else if ( idx_down > idx_up) {
		    cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y + idx_left/2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		}
		else {
		    cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		}
		if ( border != 0) {
		    idx_left += 2;
		    idx_down += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);

		if ( idx_up == idx_down)
		    cmn.gdraw.line( m_x + idx_down/2,  ll_y, m_x + idx_down/2, ur_y, Glow.eDrawType_Line, 0, 0);
		else if ( idx_down > idx_up) {
		    cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y + idx_left/2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y + idx_left/2, m_x + idx_down/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
		else {
		    cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x + idx_up/2, m_y - idx_left + idx_left/2, m_x + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left == -1 && lw_right != -1 &&
		  lw_up != -1 && lw_down != -1) {
	    // Right up down threeway
	    cmn.gdraw.fill_rect( m_x, m_y - idx_right + idx_right/2, ur_x - m_x, idx_right + 1, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, idx_up + 1, ur_y - (m_y - idx_right + idx_right/2), drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y + idx_right/2 - ll_y, drawtype);
	    if ( shadow != 0 && (idx_right > 2 || idx_down > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_right -= 2;
		    idx_down -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);

		if ( idx_up == idx_down)
		    cmn.gdraw.line( m_x - idx_down + idx_down/2,  ll_y, m_x - idx_down + idx_down/2, ur_y, shift_drawtype, 0, 0);
		else if ( idx_down > idx_up) {
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y + idx_right/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_right/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		    // cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_right/2, m_x - idx_down + idx_down/2, m_y + idx_right/2, shift_drawtype, 0, 0);
		}
		else {
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right /2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right /2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_right += 2;
		    idx_down += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right /2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right /2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);

		if ( idx_up == idx_down)
		    cmn.gdraw.line( m_x - idx_down + idx_down/2,  ll_y, m_x - idx_down + idx_down/2, ur_y, Glow.eDrawType_Line, 0, 0);
		else if ( idx_down > idx_up) {
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_right/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_right/2, m_x - idx_down + idx_down/2, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		}
		else {
		    cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		    cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y - idx_right + idx_right/2, m_x - idx_down + idx_down/2, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		}
	    }
	}
	else if ( lw_left != -1 && lw_right != -1 &&
		  lw_up != -1 && lw_down != -1) {
	    // Right left up down fourway
	    cmn.gdraw.fill_rect( m_x, m_y - idx_right + idx_right/2, ur_x - m_x, idx_right + 1, drawtype);
	    cmn.gdraw.fill_rect( ll_x, m_y - idx_left + idx_left/2, m_x - ll_x, idx_left + 1, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_up + idx_up/2, m_y, idx_up + 1, ur_y - m_y, drawtype);
	    cmn.gdraw.fill_rect( m_x - idx_down + idx_down/2, ll_y, idx_down + 1, m_y - ll_y, drawtype);
	    if ( shadow != 0 && (idx_left > 2 || idx_right > 2 || idx_down > 2 || idx_up > 2)) {
		if ( border != 0) {
		    idx_right -= 2;
		    idx_left -= 2;
		    idx_down -= 2;
		    idx_up -= 2;
		}
		shift_drawtype = GlowColor.shift_drawtype( draw_type, -2, null); // Light
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, shift_drawtype, 0, 0);
		shift_drawtype = GlowColor.shift_drawtype( draw_type, 2, null); // Dark
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right /2, m_x + idx_up/2, ur_y, shift_drawtype, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right /2, shift_drawtype, 0, 0);
		if ( border != 0) {
		    idx_right += 2;
		    idx_left += 2;
		    idx_down += 2;
		    idx_up += 2;
		}
	    }
	    if ( border != 0) {
		cmn.gdraw.line( m_x + idx_down/2, m_y - idx_right + idx_right/2, ur_x, m_y - idx_right + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_down/2, ll_y, m_x + idx_down/2, m_y - idx_right + idx_right /2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right/2, ur_x, m_y + idx_right/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x + idx_up/2, m_y + idx_right /2, m_x + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y - idx_left + idx_left/2, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_down + idx_down/2, ll_y, m_x - idx_down + idx_down/2, m_y - idx_left + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( ll_x, m_y + idx_left/2, m_x - idx_up + idx_up/2, m_y + idx_left/2, Glow.eDrawType_Line, 0, 0);
		cmn.gdraw.line( m_x - idx_up + idx_up/2, m_y + idx_left/2, m_x - idx_up + idx_up/2, ur_y, Glow.eDrawType_Line, 0, 0);
	    }
	}
	else {
	    cmn.gdraw.line( m_x, ll_y, m_x, m_y, drawtype, idx, 0);
	    cmn.gdraw.line( m_x, m_y, m_x, ur_y, drawtype, idx, 0);
	    cmn.gdraw.line( ll_x, m_y, m_x, m_y, drawtype, idx, 0);
	    cmn.gdraw.line( m_x, m_y, ur_x, m_y, drawtype, idx, 0);
	}
    }

}
