# backend/cpp_bridge.py
import subprocess
import os
from pathlib import Path

# Use absolute paths for deployment
BACKEND_ROOT = Path(__file__).parent.parent
CPP_DIR = BACKEND_ROOT / "cpp"
BIN_DIR = CPP_DIR / "bin"
EXE_PATH = BIN_DIR / "generate_ou"

def compile_cpp_backend():
    """Compile the C++ backend if needed"""
    try:
        # Check if executable already exists and is recent
        if EXE_PATH.exists():
            # Check if source files are newer than executable
            source_files = list(CPP_DIR.glob("*.cpp")) + list(CPP_DIR.glob("*.hpp"))
            exe_time = EXE_PATH.stat().st_mtime
            
            # If executable is newer than all source files, skip compilation
            if all(exe_time > src.stat().st_mtime for src in source_files):
                return True, "Already compiled"
        
        # Compile the C++ code
        BIN_DIR.mkdir(parents=True, exist_ok=True)
        
        # Use g++ to compile
        cmd = [
            "g++", "-std=c++17", "-O2",
            str(CPP_DIR / "generate_ou.cpp"),
            str(CPP_DIR / "SignedDistanceCalculator.cpp"),
            str(CPP_DIR / "PointGenerator.cpp"),
            "-o", str(EXE_PATH)
        ]
        
        result = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            cwd=str(CPP_DIR)
        )
        
        if result.returncode == 0:
            # Make executable
            os.chmod(EXE_PATH, 0o755)
            return True, "Compilation successful"
        else:
            return False, f"Compilation failed: {result.stderr}"
            
    except Exception as e:
        return False, f"Compilation error: {str(e)}"

def run_cpp_backend(stl_path: Path, ou_path: Path):
    """Run the C++ backend to generate .ou file"""
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
    except FileNotFoundError:
        return False, f"Executable not found: {EXE_PATH}"
    except Exception as e:
        return False, f"Execution error: {str(e)}"
