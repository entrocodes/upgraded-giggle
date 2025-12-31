import os
from pathlib import Path

# Configuration
ROOT_DIR = Path(r"C:/PixelPong/PixelPong")
CHAR_LIMIT = 30000  # Conservative limit for message length
IGNORE_DIRS = {".git", ".vs", "dependencies", "__pycache__", "build", "bin"}
EXTENSIONS = {".hpp", ".cpp", ".h", ".c"}

class FileSplitter:
    def __init__(self, base_name="summary"):
        self.base_name = base_name
        self.part_num = 1
        self.current_chars = 0
        self.file_handle = None

    def _open_new_file(self):
        if self.file_handle:
            self.file_handle.close()
        filename = f"{self.base_name}_part_{self.part_num}.txt"
        self.file_handle = open(filename, "w", encoding="utf-8")
        print(f"Creating {filename}...")
        self.part_num += 1
        self.current_chars = 0

    def write(self, text):
        if self.file_handle is None or (self.current_chars + len(text) > CHAR_LIMIT):
            self._open_new_file()
        
        self.file_handle.write(text)
        self.current_chars += len(text)

    def close(self):
        if self.file_handle:
            self.file_handle.close()

def generate_tree(path, indent=""):
    tree_str = ""
    items = sorted([item for item in path.iterdir() if item.name not in IGNORE_DIRS])
    for i, item in enumerate(items):
        is_last = (i == len(items) - 1)
        connector = "└── " if is_last else "├── "
        tree_str += f"{indent}{connector}{item.name}\n"
        if item.is_dir():
            new_indent = indent + ("    " if is_last else "│   ")
            tree_str += generate_tree(item, new_indent)
    return tree_str

def main():
    if not ROOT_DIR.exists():
        print(f"Error: Path {ROOT_DIR} not found.")
        return

    splitter = FileSplitter("summary")

    # 1. Write Tree to the first file
    tree_header = "=== PROJECT FILE TREE ===\n" + f"{ROOT_DIR.name}/\n" + generate_tree(ROOT_DIR) + "\n" + "="*30 + "\n\n"
    splitter.write(tree_header)

    # 2. Process Source Files
    for root, dirs, files in os.walk(ROOT_DIR):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        for file in sorted(files):
            file_path = Path(root) / file
            if file_path.suffix.lower() in EXTENSIONS:
                relative_path = file_path.relative_to(ROOT_DIR)
                
                content = f"--- FILE: {relative_path} ---\n"
                try:
                    with open(file_path, "r", encoding="utf-8", errors="replace") as f:
                        content += f.read() + "\n\n"
                except Exception as e:
                    content += f"[Error reading: {e}]\n\n"
                
                splitter.write(content)

    splitter.close()
    print("Done! All parts generated.")

if __name__ == "__main__":
    main()