#!/usr/bin/env python3
# Tiny Bitmap Editor (8x8 / 16x16 / 32x32) with SH1107-friendly export
# No external dependencies (tkinter only).

import tkinter as tk
from tkinter import ttk, messagebox

class BitmapEditor(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master.title("Tiny Bitmap Editor (8x8 / 16x16 / 32x32)")
        self.pack(fill="both", expand=True)

        # State
        self.sizes = [8, 16, 32]
        self.cell_count = 16  # default; will be set by size menu
        self.zoom = 20        # pixels per cell
        self.grid_color = "#999"
        self.on_color = "#111"
        self.off_color = "#fff"
        self.paint_val = 1
        self.dragging = False

        # Build UI
        self._build_toolbar()
        self._build_canvas()
        self._build_export_panel()

        # Initialize to 16x16
        self.size_var.set("16")
        self._change_size()

    # ---------- UI ----------
    def _build_toolbar(self):
        toolbar = ttk.Frame(self)
        toolbar.pack(side="top", fill="x", padx=8, pady=6)

        ttk.Label(toolbar, text="Size:").pack(side="left")

        self.size_var = tk.StringVar()
        size_menu = ttk.Combobox(toolbar, textvariable=self.size_var, values=[str(s) for s in self.sizes], width=5, state="readonly")
        size_menu.pack(side="left", padx=6)
        size_menu.bind("<<ComboboxSelected>>", lambda e: self._change_size())

        ttk.Button(toolbar, text="Clear", command=self.clear).pack(side="left", padx=6)
        ttk.Button(toolbar, text="Invert", command=self.invert).pack(side="left", padx=6)

        ttk.Separator(toolbar, orient="vertical").pack(side="left", fill="y", padx=8)

        ttk.Button(toolbar, text="Export C (Row-major)", command=self.export_row_major).pack(side="left", padx=6)
        ttk.Button(toolbar, text="Export C (SH1107 Page-major)", command=self.export_page_major).pack(side="left", padx=6)

        ttk.Separator(toolbar, orient="vertical").pack(side="left", fill="y", padx=8)
        ttk.Button(toolbar, text="Copy Output", command=self.copy_output).pack(side="left")

        ttk.Label(toolbar, text="   Tip: LMB=draw, RMB=erase, drag to paint").pack(side="left", padx=12)

    def _build_canvas(self):
        wrap = ttk.Frame(self)
        wrap.pack(side="top", fill="both", expand=False, padx=8, pady=4)

        self.canvas = tk.Canvas(wrap, width=34*self.zoom, height=34*self.zoom, bg="#ddd", highlightthickness=0)
        self.canvas.pack(side="left", padx=0, pady=0)

        self.canvas.bind("<Button-1>", self._on_left_down)
        self.canvas.bind("<B1-Motion>", self._on_left_drag)
        self.canvas.bind("<ButtonRelease-1>", self._on_left_up)

        self.canvas.bind("<Button-3>", self._on_right_down)
        self.canvas.bind("<B3-Motion>", self._on_right_drag)
        self.canvas.bind("<ButtonRelease-3>", self._on_right_up)

    def _build_export_panel(self):
        box = ttk.LabelFrame(self, text="Export")
        box.pack(side="top", fill="both", expand=True, padx=8, pady=6)

        self.output = tk.Text(box, height=10, wrap="none", font=("Courier New", 10))
        self.output.pack(side="left", fill="both", expand=True)
        sb_y = ttk.Scrollbar(box, command=self.output.yview)
        sb_y.pack(side="right", fill="y")
        self.output.configure(yscrollcommand=sb_y.set)

    # ---------- bitmap data ----------
    def _change_size(self):
        try:
            n = int(self.size_var.get())
        except Exception:
            n = 16
        self.cell_count = n
        # allocate 2D bitmap: rows x cols, 0/1
        self.bitmap = [[0 for _ in range(n)] for _ in range(n)]
        # resize canvas
        w = n * self.zoom
        h = n * self.zoom
        self.canvas.config(width=w+1, height=h+1)
        self._redraw()

    def clear(self):
        for y in range(self.cell_count):
            for x in range(self.cell_count):
                self.bitmap[y][x] = 0
        self._redraw()

    def invert(self):
        for y in range(self.cell_count):
            for x in range(self.cell_count):
                self.bitmap[y][x] ^= 1
        self._redraw()

    # ---------- drawing ----------
    def _redraw(self):
        self.canvas.delete("all")
        n = self.cell_count
        z = self.zoom
        # cells
        for y in range(n):
            for x in range(n):
                x0, y0 = x*z, y*z
                x1, y1 = x0+z, y0+z
                fill = self.on_color if self.bitmap[y][x] else self.off_color
                self.canvas.create_rectangle(x0, y0, x1, y1, outline=self.grid_color, fill=fill)
        # border
        self.canvas.create_rectangle(0, 0, n*z, n*z, outline="#444", width=2)

    def _evt_to_cell(self, event):
        z = self.zoom
        x = event.x // z
        y = event.y // z
        if 0 <= x < self.cell_count and 0 <= y < self.cell_count:
            return int(x), int(y)
        return None

    def _paint(self, x, y, val):
        if 0 <= x < self.cell_count and 0 <= y < self.cell_count:
            if self.bitmap[y][x] != val:
                self.bitmap[y][x] = val
                # draw single cell for speed
                z = self.zoom
                x0, y0 = x*z, y*z
                x1, y1 = x0+z, y0+z
                fill = self.on_color if val else self.off_color
                self.canvas.create_rectangle(x0, y0, x1, y1, outline=self.grid_color, fill=fill)

    def _on_left_down(self, e):
        self.dragging = True
        p = self._evt_to_cell(e)
        if p: self._paint(p[0], p[1], 1)

    def _on_left_drag(self, e):
        if not self.dragging: return
        p = self._evt_to_cell(e)
        if p: self._paint(p[0], p[1], 1)

    def _on_left_up(self, e):
        self.dragging = False

    def _on_right_down(self, e):
        self.dragging = True
        p = self._evt_to_cell(e)
        if p: self._paint(p[0], p[1], 0)

    def _on_right_drag(self, e):
        if not self.dragging: return
        p = self._evt_to_cell(e)
        if p: self._paint(p[0], p[1], 0)

    def _on_right_up(self, e):
        self.dragging = False

    # ---------- exports ----------
    def copy_output(self):
        text = self.output.get("1.0", "end-1c")
        if not text.strip():
            messagebox.showinfo("Copy Output", "Nothing to copy.")
            return
        self.master.clipboard_clear()
        self.master.clipboard_append(text)
        messagebox.showinfo("Copy Output", "Copied to clipboard.")

    def export_row_major(self):
        """
        Row-major (C array): each byte packs 8 horizontal pixels.
        Bit 7 = leftmost pixel of the 8-pack, Bit 0 = rightmost.
        Output order: top row to bottom row, left to right.
        """
        n = self.cell_count
        lines = []
        bytes_out = []

        for y in range(n):
            for xpack in range(0, n, 8):
                b = 0
                for i in range(8):
                    x = xpack + i
                    if x < n and self.bitmap[y][x]:
                        b |= (1 << (7 - i))  # MSB = leftmost
                bytes_out.append(b)

        lines.append(self._c_array(bytes_out, name=f"bitmap_{n}x{n}_rowmajor"))
        self._set_output("\n".join(lines))

    def export_page_major(self):
        """
        SH1107-style page-major: each byte = 8 vertical pixels in one column.
        Page 0 covers rows 0..7, page 1: 8..15, etc. Bit 0 = top row of the page.
        Output order: page 0..P-1, column 0..n-1 (exactly how SH1107 wants data).
        """
        n = self.cell_count
        pages = n // 8
        bytes_out = []

        for page in range(pages):
            y0 = page * 8
            for x in range(n):
                b = 0
                for bit in range(8):
                    y = y0 + bit
                    if y < n and self.bitmap[y][x]:
                        b |= (1 << bit)  # bit0 = top of page
                bytes_out.append(b)

        text = self._c_array(bytes_out, name=f"bitmap_{n}x{n}_sh1107_page")
        self._set_output(text)

    # ---------- helpers ----------
    def _c_array(self, data, name="bitmap"):
        # Format as hex list with 16 bytes per line
        hexes = [f"0x{b:02X}" for b in data]
        lines = []
        lines.append(f"// {name}: {len(data)} bytes")
        lines.append(f"static const unsigned char {name}[] = {{")
        row = []
        for i, h in enumerate(hexes, 1):
            row.append(h)
            if (i % 16) == 0:
                lines.append("  " + ", ".join(row) + ",")
                row = []
        if row:
            lines.append("  " + ", ".join(row))
        lines.append("};")
        return "\n".join(lines)

    def _set_output(self, s):
        self.output.delete("1.0", "end")
        self.output.insert("1.0", s)

def main():
    root = tk.Tk()
    # nicer default ttk theme if available
    try:
        style = ttk.Style(root)
        if "clam" in style.theme_names():
            style.theme_use("clam")
    except Exception:
        pass
    app = BitmapEditor(root)
    root.minsize(520, 420)
    root.mainloop()

if __name__ == "__main__":
    main()

