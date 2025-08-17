import os
import idaapi
import idc
import idautils

def ask_output_folder():
    import tkinter as tk
    from tkinter import filedialog
    root = tk.Tk()
    root.withdraw()
    folder = filedialog.askdirectory(title="Select output folder for ApplicationFramework functions")
    return folder

def find_scene_functions():
    scene_functions = set()
    print("Searching through all functions...")
    
    for func_ea in idautils.Functions():
        func_name = idc.get_func_name(func_ea)
        if not func_name:
            continue
            
        demangled_name = idc.demangle_name(func_name, 0)
        
        if "ITF::ApplicationFramework::" in func_name or (demangled_name and "ITF::ApplicationFramework::" in demangled_name):
            scene_functions.add(func_name)
            print(f"Found Scene function: {func_name}")
            if demangled_name and demangled_name != func_name:
                print(f"  Demangled: {demangled_name}")
    
    return scene_functions

def get_called_functions(ea):
    called = set()
    func = idaapi.get_func(ea)
    if not func:
        return called
    for head in idautils.Heads(func.start_ea, func.end_ea):
        if idc.print_insn_mnem(head).startswith("call"):
            op = idc.get_operand_value(head, 0)
            callee_name = idc.get_func_name(op)
            if callee_name:
                called.add(callee_name)
    return called

def collect_dependency_closure(scene_functions):
    to_visit = set(scene_functions)
    visited = set()
    all_funcs = set()
    
    while to_visit:
        name = to_visit.pop()
        if name in visited:
            continue
        visited.add(name)
        ea = idc.get_name_ea_simple(name)
        if ea == idc.BADADDR:
            continue
        all_funcs.add(name)
        for callee in get_called_functions(ea):
            if callee not in visited:
                to_visit.add(callee)
    
    return all_funcs

def decompile_and_dump(function_names, output_dir):
    if not idaapi.init_hexrays_plugin():
        print("Hex-Rays decompiler is not available!")
        return

    count = 0
    for func_name in sorted(function_names):
        ea = idc.get_name_ea_simple(func_name)
        if ea == idc.BADADDR:
            print(f"Function {func_name} not found!")
            continue
        try:
            cfunc = idaapi.decompile(ea)
            if cfunc is None:
                print(f"Failed to decompile {func_name} at {hex(ea)}")
                continue
            pseudocode = cfunc.get_pseudocode()
            code_str = f"// Decompiled by IDA\n// Function: {func_name}\n"
            code_str += "\n".join([idaapi.tag_remove(line.line) for line in pseudocode])
            
            safe_name = func_name.replace("::", "_").replace(":", "_").replace("/", "_").replace("\\", "_")
            filename = os.path.join(output_dir, f"{safe_name}.cpp")
            with open(filename, "w", encoding="utf-8") as f:
                f.write(code_str)
            print(f"Decompiled {func_name} to {filename}")
            count += 1
        except Exception as e:
            print(f"Error decompiling {func_name}: {e}")
    print(f"Done! Decompiled {count} functions.")

def main():
    print("Searching for Scene functions...")
    scene_functions = find_scene_functions()
    
    if not scene_functions:
        print("No Scene functions found!")
        return
    
    print(f"Found {len(scene_functions)} Scene functions:")
    for func in sorted(scene_functions):
        print(f"  {func}")
    
    output_dir = ask_output_folder()
    if not output_dir:
        print("No output folder selected. Aborting.")
        return
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    print("Collecting all dependencies...")
    all_needed = collect_dependency_closure(scene_functions)
    print(f"Total functions to decompile: {len(all_needed)}")
    
    decompile_and_dump(all_needed, output_dir)

if __name__ == "__main__":
    main()
