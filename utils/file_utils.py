# backend/utils/file_utils.py
from pathlib import Path

def ensure_directories():
    """
    Create required upload directories if they don't exist.
    """
    (Path("uploads/stl")).mkdir(parents=True, exist_ok=True)
    (Path("uploads/ou")).mkdir(parents=True, exist_ok=True)
    (Path("bin")).mkdir(parents=True, exist_ok=True)

def get_stl_path(filename: str) -> Path:
    return Path("uploads/stl") / filename

def get_ou_path() -> Path:
    return Path("uploads/ou") / "generated_points.ou"

def get_cpp_executable_path() -> Path:
    return Path("bin") / "generate_ou"
