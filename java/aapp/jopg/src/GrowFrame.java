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
import java.net.*;

public class GrowFrame implements GraphApplIfc {
    String instance;
    GrowFrameApplIfc appl;

    public GrowFrame( String file, Gdh gdh, String instance, GrowFrameApplIfc appl) {
    }

    private void init( String file, Gdh gdh) {
    }


    public int getWidth() {
	//return localPanel.getWidth() + 5;
    	return 100;
    }
    public int getHeight() {
	//return localPanel.getHeight() + 40;
    	return 100;
    }

    void setSize() {
    }

    public void closeGrow() {
    }

    public int command(String cmd) {
	if ( appl != null)
	    return appl.command(cmd, this);
	return 0;
    }

    public int script(String script) {
	if ( appl != null)
	    return appl.script(script);
	return 0;
    }

    public void confirmNo() {}
    public void confirmYes() {}    
    public void openConfirmDialog( Object dyn, String text, Object object) {}

    public void openValueInputDialog( Object dyn, String text, Object object) {}
    public String getObject() {
	return instance;
    }
    public Object loadGrowCtx( String fname, String owner) {
	return null;
    }
    public int loadSubgraph( String fname) {
	return 0;
    }
    public void closeGraph() {}
    public boolean isAuthorized(int access) {
	return false;
    }
    public void openPopupMenu( String object, double x, double y) {}
}





