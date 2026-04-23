import zipfile
import json
import os
import sys

mod_id = sys.argv[1] if len(sys.argv) > 1 else "your-mod-id"

# Load mod.json
with open("mod.json", "r") as f:
    mod_json = json.load(f)

qmod_name = f"{mod_id}.qmod"

# Find the built .so
so_name = f"lib{mod_id}.so"
so_path = os.path.join("build", so_name)

if not os.path.exists(so_path):
    # Try finding any .so in build/
    for f in os.listdir("build"):
        if f.endswith(".so") and "debug" not in f.lower():
            so_path = os.path.join("build", f)
            so_name = f
            break

if not os.path.exists(so_path):
    print(f"Error: Could not find .so file in build/")
    sys.exit(1)

print(f"Packaging {so_path} into {qmod_name}")

# Update mod.json modFiles to match actual .so name
mod_json["modFiles"] = [so_name]

with zipfile.ZipFile(qmod_name, "w", zipfile.ZIP_DEFLATED) as zf:
    zf.writestr("mod.json", json.dumps(mod_json, indent=2))
    zf.write(so_path, so_name)

print(f"Created {qmod_name}")
