# backend/cpp_bridge.py
import subprocess
from pathlib import Path

CPP_DIR = Path(__file__).parent.parent / "cpp"
BIN_DIR = CPP_DIR / "bin"
EXE_PATH = BIN_DIR / "generate_ou"

def compile_cpp_backend():
    # Assume already compiled for now
    return True, "Already compiled"

def run_cpp_backend(stl_path: Path, ou_path: Path):
    ou_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        result = subprocess.run(
            [str(EXE_PATH), str(stl_path), str(ou_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            check=True
        )
        return True, "Execution successful"
    except subprocess.CalledProcessError as e:
        return False, f"Execution failed: {e.stderr}"
