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
import jpwr.rt.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
   Ge action type PulldownMenu.
*/
public class GeDynPulldownMenu extends GeDynElem {
  String[] itemsText;
  GeDyn[] itemsDyn;

  public GeDynPulldownMenu( GeDyn dyn, String itemsText[], GeDyn[] itemsDyn) {
    super( dyn, GeDyn.mDynType_No, GeDyn.mActionType_PulldownMenu);
    this.itemsText = itemsText;
    this.itemsDyn = itemsDyn;
  }
  public void action( int eventType, MouseEvent e) {
    switch ( eventType) {
    case GeDyn.eEvent_MB1Down:
      break;
    case GeDyn.eEvent_MB1Up:
      break;
    case GeDyn.eEvent_MB1Click:
      new PulldownMenu((Component)dyn.comp, 
		       // ((Component)dyn.comp).getX() - ((Component)dyn.comp).getWidth()/2, 
		       // ((Component)dyn.comp).getY() + ((Component)dyn.comp).getHeight() - 2
		       0, ((Component)dyn.comp).getHeight() - 2
		       );
      break;
    }
  }

  class PulldownMenu implements ActionListener, PopupMenuListener {
    Component invoker;

    PulldownMenu( Component invoker, int x, int y) {
      this.invoker = invoker;
      JMenuItem item;
      JPopupMenu popup = new JPopupMenu();

      for ( int i = 0; i < 32; i++) {
        if ( itemsText[i] != null && itemsDyn[i] != null) {
	  if ( (itemsDyn[i].actionType & GeDyn.mActionType_PulldownMenu) != 0) {
	      System.out.println( itemsText[i] + " " + i + " " + itemsDyn[i].actionType + " " +
				  GeDyn.mActionType_PulldownMenu);
	    if ( itemsDyn[i].elements.length == 1 &&
		 itemsDyn[i].elements[0] instanceof GeDynPulldownMenu)
	      popup.add( subMenu( itemsText[i], (GeDynPulldownMenu) itemsDyn[i].elements[0])); // Check which element...
	  }
	  else {
	    popup.add( item = new JMenuItem( itemsText[i]));
	    item.addActionListener( this);
	  }
        }
      }
      popup.addPopupMenuListener( this);
      popup.show( invoker, x, y);
    }

    JMenu subMenu( String text, GeDynPulldownMenu pulldown) {
      JMenu menu = new JMenu( text);
      JMenuItem item;

      for ( int i = 0; i < 32; i++) {
        if ( pulldown.itemsText[i] != null && pulldown.itemsDyn[i] != null) {
	  if ( (pulldown.itemsDyn[i].actionType & GeDyn.mActionType_PulldownMenu) != 0) {
	    if ( pulldown.itemsDyn[i].elements.length == 1 &&
		 pulldown.itemsDyn[i].elements[0] instanceof GeDynPulldownMenu)
	      menu.add( subMenu( pulldown.itemsText[i], (GeDynPulldownMenu) pulldown.itemsDyn[i].elements[0]));
	  }
	  else {
	    menu.add( item = new JMenuItem( pulldown.itemsText[i]));
	    item.addActionListener( this);
	  }
        }
      }
      return menu;
    }
    public void popupMenuWillBecomeVisible( PopupMenuEvent e) {}
    public void popupMenuWillBecomeInvisible( PopupMenuEvent e) {}
    public void popupMenuCanceled( PopupMenuEvent e) {}

    public void actionPerformed( ActionEvent event) {

      for ( int i = 0; i < 32; i++) {
        if ( itemsText[i] != null && itemsText[i].equals( event.getActionCommand())) {
	  if ( itemsDyn[i] != null) {
	    MouseEvent e = new MouseEvent( invoker, 0, 0, 0, invoker.getX(), invoker.getY(), 1, false);

	    System.out.println("Action " + event.getActionCommand());
	    itemsDyn[i].action( GeDyn.eEvent_MB1Click, e);
	  }
	  break;
        }
	else if ( itemsDyn[i] != null &&
		  (itemsDyn[i].actionType & GeDyn.mActionType_PulldownMenu) != 0) {
	  if ( itemsDyn[i].elements.length == 1 && 
	       itemsDyn[i].elements[0] instanceof GeDynPulldownMenu)
	    subMenuAction( event, (GeDynPulldownMenu) itemsDyn[i].elements[0]);
	}
      }
    }  

    public void subMenuAction( ActionEvent event, GeDynPulldownMenu pulldown) {
      for ( int i = 0; i < 32; i++) {
        if ( pulldown.itemsText[i] != null && pulldown.itemsText[i].equals( event.getActionCommand())) {
	  if ( pulldown.itemsDyn[i] != null) {
	    MouseEvent e = new MouseEvent( invoker, 0, 0, 0, invoker.getX(), invoker.getY(), 1, false);

	    System.out.println("Action " + event.getActionCommand());
	    pulldown.itemsDyn[i].action( GeDyn.eEvent_MB1Click, e);
	  }
	  break;
        }
	else if ( pulldown.itemsDyn[i] != null && 
		  (pulldown.itemsDyn[i].actionType & GeDyn.mActionType_PulldownMenu) != 0) {
	  if ( pulldown.itemsDyn[i].elements.length == 1)
	    subMenuAction( event, (GeDynPulldownMenu) pulldown.itemsDyn[i].elements[0]);
	}
      }
    }
  }
}













