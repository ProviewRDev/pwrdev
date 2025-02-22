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
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableColumn;
import java.io.*;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.StringSelection;
import jpwr.rt.*;

/** 
 * Event log table.
 * HistTable is an extended JScrollPane containing the 
 * a JTable alarmTable. The HistTable also contains methods for exporting
 * the alarmTable to an excel file, and for performing a search in the historic
 * event list using a HistSender 
 */
 
public class HistTable extends JScrollPane {
     static final long serialVersionUID = 8840562919946236178L;   
    JopSession session;
    boolean DEBUG=false;
    public EventTableModel atModel;
    public JTable alarmTable;
    String[] columnNamesEventTable = {"Prio","Type","Time","Event text","Event name"};
    
    String[] excelMess= {JopLang.transl("Export to Excel"),
			 JopLang.transl("The result is put into the Clipboard. Open a new Excel document and use Paste to insert it.")};

    // Constructor
    public HistTable(JopSession s){
	session = s;
	setup();            
    }
    
    //Layout the HistTable. Add a mouse Listener to the JTable alarmTable
    //and set it up to support JopMethodsMenu.
    private void setup(){
               
	atModel = new EventTableModel();
	alarmTable = new JTable(atModel);
	alarmTable.setCellEditor(null);
	alarmTable.setIntercellSpacing(new Dimension(8, 2));
	this.initColumnSizes(alarmTable, atModel);
	alarmTable.getTableHeader().setReorderingAllowed(false);
	alarmTable.getColumn((Object)columnNamesEventTable[0]).setCellRenderer(new EventTableCellRender());
    
    
	this.setViewportView(alarmTable);
	this.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
	this.getViewport().setBackground(Color.white);
	
	((EventTableModel)alarmTable.getModel()).updateTable(); 
    
    }
    
    /*Get a pointer to the local Clipboard, format a string with all cell
      elements with \t between columns and \n between rows and put it on
      the clipboard. Display a message telling how to paste the string to
      excel*/
    
    public void exportExcel() {
	StringBuffer copybuffer = new StringBuffer("");
	for( int i = 0; i < alarmTable.getRowCount(); i++){
	    for ( int j = 0; j < alarmTable.getColumnCount(); j++){
		copybuffer.append((String) alarmTable.getValueAt(i,j));
		copybuffer.append("\t");
	    }
	    copybuffer.append("\n");
	}  
	Clipboard cb= Toolkit.getDefaultToolkit().getSystemClipboard();
	StringSelection output = new StringSelection(copybuffer.toString());
	cb.setContents(output, output);
	JOptionPane.showMessageDialog(this.getParent(),excelMess[1],excelMess[0],JOptionPane.INFORMATION_MESSAGE);
    
        
    }
    
    //XLS
    //Creates a String for sending to a javascript which can save to a file.
    public String exportExcelFromApplet() {
	String copyString = "";
	for( int i = 0; i < alarmTable.getRowCount(); i++){
		for ( int j = 0; j < alarmTable.getColumnCount(); j++){
		    copyString = copyString +(String) alarmTable.getValueAt(i,j);
		    copyString = copyString+"\t";
		}
		copyString = copyString + "\n" ;
	}  
	return copyString;
    }
    
    
    //Set column sizes to fit the table contents...
    private void initColumnSizes(JTable table, AbstractTableModel model) {
	TableColumn column = null;
	Component comp = null;
	int headerWidth = 0;
	int cellWidth = 0;
	Object[] longValues;
    
    
	longValues = ((EventTableModel)(model)).longValues;
    
    
	for( int i = 0; i < longValues.length; i++) {
	    column = table.getColumnModel().getColumn(i);
        
	    comp = table.getDefaultRenderer(model.getColumnClass(i)).
		getTableCellRendererComponent(
					      table, longValues[i],
					      false, false, 0, i);
	    cellWidth = comp.getPreferredSize().width;
        
	    if(DEBUG) {
		System.out.println("Initializing width of column "
				   + i + ". "
				   + "headerWidth = " + headerWidth
				   + "; cellWidth = " + cellWidth);
            }
        
	    //XXX: Before Swing 1.1 Beta 2, use setMinWidth instead.
	    column.setPreferredWidth(Math.max(headerWidth, cellWidth));
	    if(i == 0) {
		column.setPreferredWidth(30);
		column.setMaxWidth(30);
		column.setResizable(false);
            }
        
        }
    }
    
    public void presentStat() {        
	// Derive interesting statistics from the current searchresult and
        //display it in a JFrame...
	HistStatistics statistics = new HistStatistics(atModel.mhData,session);
	JFrame frame = new JFrame();
        JPanel panel = (JPanel)frame.getContentPane();
        panel.setLayout(new FlowLayout());
        panel.add(statistics);
        frame.pack();
        frame.setVisible(true);

    }

    public int getNrOfResults() {
	return alarmTable.getRowCount();
    }
    // Make a search in the Historical Event list based on the HistQuery search
    public void performSearch(Object root, HistQuery search) {
        HistSender sender = new HistSender(root);
        atModel.setMhData(sender.SearchRequest(search));
    }
    
    
}
