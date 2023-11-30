from tkinter import *
from tkinter import ttk
import matplotlib.pyplot as plt
import os

class Tooltip:
    def __init__(self, widget,
                 bg='#FFFFEA',
                 pad=(5, 3, 5, 3),
                 text='widget info',
                 waittime=400,
                 wraplength=250):

        self.waittime = waittime  # in miliseconds, originally 500
        self.wraplength = wraplength  # in pixels, originally 180
        self.widget = widget
        self.text = text
        self.widget.bind("<Enter>", self.onEnter)
        self.widget.bind("<Leave>", self.onLeave)
        self.widget.bind("<ButtonPress>", self.onLeave)
        self.bg = bg
        self.pad = pad
        self.id = None
        self.tw = None

    def onEnter(self, event=None):
        self.schedule()

    def onLeave(self, event=None):
        self.unschedule()
        self.hide()

    def schedule(self):
        self.unschedule()
        self.id = self.widget.after(self.waittime, self.show)

    def unschedule(self):
        id_ = self.id
        self.id = None
        if id_:
            self.widget.after_cancel(id_)

    def show(self):
        def tip_pos_calculator(widget, label,
                               tip_delta=(10, 5), pad=(5, 3, 5, 3)):

            w = widget

            s_width, s_height = w.winfo_screenwidth(), w.winfo_screenheight()

            width, height = (pad[0] + label.winfo_reqwidth() + pad[2],
                             pad[1] + label.winfo_reqheight() + pad[3])

            mouse_x, mouse_y = w.winfo_pointerxy()

            x1, y1 = mouse_x + tip_delta[0], mouse_y + tip_delta[1]
            x2, y2 = x1 + width, y1 + height

            x_delta = x2 - s_width
            if x_delta < 0:
                x_delta = 0
            y_delta = y2 - s_height
            if y_delta < 0:
                y_delta = 0

            offscreen = (x_delta, y_delta) != (0, 0)

            if offscreen:

                if x_delta:
                    x1 = mouse_x - tip_delta[0] - width

                if y_delta:
                    y1 = mouse_y - tip_delta[1] - height

            offscreen_again = y1 < 0  # out on the top

            if offscreen_again:
                # No further checks will be done.

                # TIP:
                # A further mod might automagically augment the
                # wraplength when the tooltip is too high to be
                # kept inside the screen.
                y1 = 0

            return x1, y1

        bg = self.bg
        pad = self.pad
        widget = self.widget

        # creates a toplevel window
        self.tw = Toplevel(widget)

        # Leaves only the label and removes the app window
        self.tw.wm_overrideredirect(True)

        win = Frame(self.tw,
                       background=bg,
                       borderwidth=0)
        label = ttk.Label(win,
                          text=self.text,
                          justify=LEFT,
                          background=bg,
                          relief=SOLID,
                          borderwidth=0,
                          wraplength=self.wraplength)

        label.grid(padx=(pad[0], pad[2]),
                   pady=(pad[1], pad[3]),
                   sticky=NSEW)
        win.grid()

        x, y = tip_pos_calculator(widget, label)

        self.tw.wm_geometry("+%d+%d" % (x, y))

    def hide(self):
        tw = self.tw
        if tw:
            tw.destroy()
        self.tw = None

def colortheme(ct):
    if ct < 1 or ct > 16:
        return 15
    return ct

def plotstyle(colortheme):
    plt.style.use(os.environ.get('pwr_load') + '/pwr_colortheme' + str(colortheme) + '.mplstyle')

def style(colortheme):
    if colortheme == 1:
        # Sand
        bg='#FFF2E4'
        fg='#491E02'
        activebg='#FFECD7'
        activefg='#491E02'
        entrybg='#F3F3F3'
        entryfg='#000000'
        entryborder='#491E02'
        menubg='#E5CDA7'
        menufg='#491E02'
        menuactive='#F3E3CB'
    elif colortheme == 2:
        # Maroon
        bg='#DBC3C3'
        fg='#000000'
        activebg='#F8DEDE'
        activefg='#000000'
        entrybg='#E5D6D6'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#650003'
        menufg='#FFFFFF'
        menuactive='#3D0203'
    elif colortheme == 3:
        # Sienna
        bg='#642400'
        fg='#FFC9A9'
        activebg='#4D1C00'
        activefg='#FFC9A9'
        entrybg='#341503'
        entryfg='#FFFFFF'
        entryborder='#000000'
        menubg='#FC8A06'
        menufg='#000000'
        menuactive='#FBAE56'
    elif colortheme == 4:
        # DarkBlue
        bg='#233A51'
        fg='#FFFFFF'
        activebg='#121D28'
        activefg='#FFFFFF'
        entrybg='#31475E'
        entryfg='#FFFFFF'
        entryborder='#000000'
        menubg='#2D547A'
        menufg='#FFFFFF'
        menuactive='#203B55'
    elif colortheme == 5:
        # Classic
        bg='#E3E3E3'
        fg='#000000'
        activebg='#FBF8F8'
        activefg='#000000'
        entrybg='#F3F3F3'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#D8D8D8'
        menufg='#000000'
        menuactive='#ECECEC'
    elif colortheme == 6:
        # Midnight
        bg='#20213E'
        fg='#FBB16C'
        activebg='#2F3294'
        activefg='#FBB16C'
        entrybg='#02021E'
        entryfg='#FBB26C'
        entryborder='#3C3E64'
        menubg='#000027'
        menufg='#FBB16C'
        menuactive='#202068'
    elif colortheme == 7:
        # Playroom
        bg='#FFE3FC'
        fg='#000000'
        activebg='#E5B4E0'
        activefg='#000000'
        entrybg='#FFFFFF'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#1563C2'
        menufg='#FFFFFF'
        menuactive='#134078'
    elif colortheme == 8:
        # NordicLight
        bg='#E8F1FF'
        fg='#000000'
        activebg='#D4E5FD'
        activefg='#000000'
        entrybg='#FFF7E6'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#D5DAEE'
        menufg='#000000'
        menuactive='#C7CFEF'
    elif colortheme == 9:
        # Contrast
        bg='#FFFFFF'
        fg='#000000'
        activebg='#F0F0F0'
        activefg='#000000'
        entrybg='#FFFFFF'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#2B2B2B'
        menufg='#FFFFFF'
        menuactive='#424242'
    elif colortheme == 10:
        # AzurContrast
        bg='#A5D0FB'
        fg='#000000'
        activebg='#C3E0FC'
        activefg='#000000'
        entrybg='#D5DBE3'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#2B2B2B'
        menufg='#FFFFFF'
        menuactive='#424242'
    elif colortheme == 11:
        # OchreContrast
        bg='#FCD17A'
        fg='#000000'
        activebg='#FDE4AF'
        activefg='#000000'
        entrybg='#E6E3DD'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#1F0400'
        menufg='#FFFFFF'
        menuactive='#472D29'
    elif colortheme == 12:
        # Chesterfield
        bg='#350100'
        fg='#F1D3B9'
        activebg='#501B1A'
        activefg='#F1D3B9'
        entrybg='#DAC5AC'
        entryfg='#441C00'
        entryborder='#000000'
        menubg='#452406'
        menufg='#FFDCBB'
        menuactive='#23170C'
    elif colortheme == 13:
        # TerraVerte
        bg='#F1F8E8'
        fg='#000000'
        activebg='#D1E3B7'
        activefg='#000000'
        entrybg='#F2F3DD'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#385023'
        menufg='#FFFFFF'
        menuactive='#283F13'
    elif colortheme == 14:
        # Polar
        bg='#E8EEF8'
        fg='#000000'
        activebg='#C8DAF4'
        activefg='#000000'
        entrybg='#DDEBF3'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#283A76'
        menufg='#FFFFFF'
        menuactive='#031244'
    elif colortheme == 16:
        # StandardDark
        bg='#393939'
        fg='#F0F1F0'
        activebg='#222222'
        activefg='#F0F1F0'
        entrybg='#1F1F1F'
        entryfg='#EAEAEA'
        entryborder='#412004'
        menubg='#1C1B1B'
        menufg='#EBEBEB'
        menuactive='#313131'
    else:
        # StandardLight
        bg='#FDFDFD'
        fg='#000000'
        activebg='#E7E7E7'
        activefg='#000000'
        entrybg='#F3F3F3'
        entryfg='#000000'
        entryborder='#000000'
        menubg='#F3F3F3'
        menufg='#000000'
        menuactive='#E7E7E7'

    style = ttk.Style()
    #print(ttk.Style().theme_names())
    #style.theme_use('classic')
    style.configure('.', background=bg, foreground=fg)
    style.configure('TEntry', fieldbackground=entrybg, foreground=entryfg,
                        bordercolor=entryborder, insertcolor=entryfg)
    style.map('TButton', foreground=[('active',activefg)],
                  background=[('active', activebg)])
    style.map('TMenubutton', foreground=[('active', menufg)],
                  background=[('active', menuactive)])
    style.configure('PwrTool.TFrame', background=bg, border=1, relief=RAISED)
    style.configure('Pwr.TFrame', background=bg)
    style.configure('Pwr.TLabel', background=bg, foreground=fg)
    style.configure('Pwr.TCheckbutton', highlightthickness=0, background=bg,
                        foreground=fg, indicatorbackground=bg, indicatorforeground=fg)
    style.map('Pwr.TCheckbutton', foreground=[('active',activefg)],
                  background=[('active', activebg)])
    style.configure('Pwr.TButton', background=bg, foreground=fg)
    style.map('Pwr.TButton', foreground=[('active',activefg)],
                  background=[('active', activebg)])
    style.configure('Pwr.TEntry', fieldbackground=entrybg, foreground=entryfg,
                        bordercolor=entryborder, insertcolor=entryfg)
    style.configure('Pwr.TOptionmenu', background=menubg, foreground=menufg)
    style.map('Pwr.TOptionmenu', foreground=[('active', menufg)],
                  background=[('active', menuactive)])
    style.configure('Pwr.Treeview', background=bg, foreground=fg, fieldbackground=menubg)
    style.configure('Vertical.Pwr.TScrollbar', background='orange', arrowcolor='red', throughcolor='yellow')
    return bg, fg, menubg, menufg, menuactive;
