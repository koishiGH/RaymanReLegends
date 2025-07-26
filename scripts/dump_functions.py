import os
import idaapi
import idc
import idautils

def ask_output_folder():
    import tkinter as tk
    from tkinter import filedialog
    root = tk.Tk()
    root.withdraw()
    folder = filedialog.askdirectory(title="Select output folder for decompiled functions")
    return folder

def decompile_and_dump_subs(output_dir):
    if not idaapi.init_hexrays_plugin():
        print("Hex-Rays decompiler is not available!")
        return

    count = 0
    for ea in idautils.Functions():
        name = idc.get_func_name(ea)
        if name.startswith("sub_"):
            try:
                cfunc = idaapi.decompile(ea)
                if cfunc is None:
                    print(f"Failed to decompile {name} at {hex(ea)}")
                    continue
                pseudocode = cfunc.get_pseudocode()
                code_str = "// Decompiled by IDA\n"
                code_str += "\n".join([idaapi.tag_remove(line.line) for line in pseudocode])
                filename = os.path.join(output_dir, f"{name}.cpp")
                with open(filename, "w", encoding="utf-8") as f:
                    f.write(code_str)
                print(f"Decompiled {name} to {filename}")
                count += 1
            except Exception as e:
                print(f"Error decompiling {name}: {e}")
    print(f"Done! Decompiled {count} functions.")

def main():
    output_dir = ask_output_folder()
    if not output_dir:
        print("No output folder selected. Aborting.")
        return
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    decompile_and_dump_subs(output_dir)

if __name__ == "__main__":
    main()
