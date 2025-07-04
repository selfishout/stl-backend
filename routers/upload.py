# backend/routers/upload.py
import os
from fastapi import APIRouter, UploadFile, File
from fastapi.responses import JSONResponse
from pathlib import Path
from cpp_bridge import compile_cpp_backend, run_cpp_backend  # 👈 import helper functions

router = APIRouter()

UPLOAD_DIR = Path("uploads/stl")
OU_DIR = Path("uploads/ou")
UPLOAD_DIR.mkdir(parents=True, exist_ok=True)
OU_DIR.mkdir(parents=True, exist_ok=True)

@router.post("/upload_stl/")
async def upload_stl(file: UploadFile = File(...)):
    stl_path = UPLOAD_DIR / file.filename
    with stl_path.open("wb") as buffer:
        buffer.write(await file.read())

    # 👇 compile the backend
    success, message = compile_cpp_backend()
    if not success:
        return JSONResponse(
            status_code=500,
            content={"error": "Compilation failed", "details": message}
        )

    # 👇 run the backend
    ou_path = OU_DIR / "generated_points.ou"
    success, message = run_cpp_backend(stl_path, ou_path)
    if not success:
        return JSONResponse(
            status_code=500,
            content={"error": "Execution failed", "details": message}
        )

    return {
        "stl_url": f"/uploads/stl/{file.filename}",
        "ou_url": f"/uploads/ou/generated_points.ou"
    }
