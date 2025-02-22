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
import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.geom.*;
import javax.swing.*;


/**
   Flow annotation.
   Place in a node for an instance specific text.
*/
public class FlowAnnot implements FlowArrayElem {
  FlowPoint p;
  int draw_type;
  int text_size;
  int display_level;
  int annot_type;
  int number;
  FlowCmn cmn;

  public FlowAnnot( FlowCmn cmn) {
    this.cmn = cmn;
    p = new FlowPoint(cmn);
  }

  public void open( BufferedReader reader) {
    String line;
    StringTokenizer token;
    boolean end = false;

    try {
      while( (line = reader.readLine()) != null) {
	token = new StringTokenizer(line);
	int key = new Integer(token.nextToken()).intValue();
	if ( cmn.debug) System.out.println( "line : " + key);

	switch ( key) {
	case Flow.eSave_Annot:
	  break;
	case Flow.eSave_Annot_number:
	  number = new Integer(token.nextToken()).intValue();
	  break;
	case Flow.eSave_Annot_draw_type:
	  draw_type = new Integer(token.nextToken()).intValue();
	  break;
	case Flow.eSave_Annot_text_size:
	  text_size = new Integer(token.nextToken()).intValue();
	  break;
	case Flow.eSave_Annot_display_level:
	  display_level = new Integer(token.nextToken()).intValue();
	  break;
	case Flow.eSave_Annot_p:
	  p.open( reader);
	  break;
	case Flow.eSave_Annot_annot_type:
	  annot_type = new Integer(token.nextToken()).intValue();
	  break;
	case Flow.eSave_End:
	  end = true;
	  break;
	default:
	  System.out.println( "Syntax error in FlowAnnot");
	  break;
	}
	if ( end)
	  break;
      }
    } catch ( Exception e) {
      System.out.println( "IOExeption FlowAnnot");
    }
  }

  public void draw( Graphics2D g, FlowPoint p0, FlowNodeIfc node, boolean highlight) { 
    if ( node == null)
	return;
    String[] annotv = node.getAnnotv();

    if ( annotv[number] == null)
      return;
    if ( (display_level & FlowCmn.display_level) == 0)
      return;

    int tsize;
    int idx = (int)(cmn.zoom_factor / cmn.base_zoom_factor * (text_size +4) - 4);
    if ( idx < 0) return;
    // if ( idx > 8)
    //   idx = 8;

    switch( idx) {
    case 0: tsize = 8; break;
    case 1: tsize = 10; break;
    case 2: tsize = 12; break;
    case 3: tsize = 14; break;
    case 4: tsize = 14; break;
    case 5: tsize = 18; break;
    case 6: tsize = 18; break;
    case 7: tsize = 18; break;
    default: tsize = idx * 3;
    }
    tsize -= tsize/5;

    Font f;
    switch ( draw_type) {
    case Flow.eDrawType_TextHelveticaBold:
      f = new Font("Helvetica", Font.BOLD, tsize);
      break;
    default:
      f = new Font("Helvetica", Font.PLAIN, tsize);
    }
    char[] c = new char[] { 10 };
    StringTokenizer token = new StringTokenizer( annotv[number], new String(c));
    g.setColor( Color.black);
    if ( highlight)
      g.setColor( Color.red);
    g.setFont( f);

    float x = (float)((p.x + p0.x) * cmn.zoom_factor);
    float y = (float)((p.y + p0.y) * cmn.zoom_factor - tsize/4);
    while ( token.hasMoreTokens()) {
      g.drawString( token.nextToken(), x, y);
      y += f.getSize2D() * 1.4;
    }
  }

}







