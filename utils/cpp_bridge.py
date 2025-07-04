# backend/cpp_bridge.py
import subprocess
from pathlib import Path

CPP_DIR = Path(__file__).parent
BIN_DIR = CPP_DIR / "bin"
EXE_PATH = BIN_DIR / "generate_ou"

def compile_cpp_backend():
    if EXE_PATH.exists():
        return True, "Already compiled"

    BIN_DIR.mkdir(parents=True, exist_ok=True)
    compile_cmd = [
        "g++",
        str(CPP_DIR / "PointGenerator.cpp"),
        str(CPP_DIR / "SignedDistanceCalculator.cpp"),
        "-o",
        str(EXE_PATH),
        "-std=c++17"
    ]
    result = subprocess.run(compile_cmd, capture_output=True, text=True)
    if result.returncode != 0:
        return False, result.stderr
    return True, "Compilation successful"

def run_cpp_backend(stl_path: Path, ou_path: Path):
    if not EXE_PATH.exists():
        return False, "Executable missing"
    run_cmd = [str(EXE_PATH), str(stl_path), str(ou_path)]
    result = subprocess.run(run_cmd, capture_output=True, text=True)
    if result.returncode != 0:
        return False, result.stderr
    return True, "Execution successful"
