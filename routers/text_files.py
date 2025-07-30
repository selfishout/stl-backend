import os
import uuid
from datetime import datetime
from fastapi import APIRouter, UploadFile, File, Depends, HTTPException
from fastapi.responses import JSONResponse
from sqlalchemy.orm import Session
from pathlib import Path
from database import get_db
from models import TextFile

router = APIRouter()

# Create upload directory for text files
TEXT_UPLOAD_DIR = Path("uploads/text")
TEXT_UPLOAD_DIR.mkdir(parents=True, exist_ok=True)

@router.post("/upload_text/")
async def upload_text_file(file: UploadFile = File(...), db: Session = Depends(get_db)):
    """Upload a text file and store its metadata and content in database"""
    
    # Validate file type
    if not file.filename.lower().endswith('.txt'):
        raise HTTPException(status_code=400, detail="Only .txt files are allowed")
    
    # Read file content
    content = await file.read()
    content_str = content.decode('utf-8')
    
    # Generate unique filename
    original_filename = file.filename
    unique_filename = f"{uuid.uuid4()}_{original_filename}"
    file_path = TEXT_UPLOAD_DIR / unique_filename
    
    # Save file to disk
    with open(file_path, "wb") as buffer:
        buffer.write(content)
    
    # Create database record
    text_file = TextFile(
        filename=unique_filename,
        original_filename=original_filename,
        file_path=str(file_path),
        content=content_str,
        file_size=len(content),
        upload_date=datetime.now(),
        last_modified=datetime.now()
    )
    
    db.add(text_file)
    db.commit()
    db.refresh(text_file)
    
    return {
        "message": "Text file uploaded successfully",
        "file_id": text_file.id,
        "filename": text_file.filename,
        "original_filename": text_file.original_filename,
        "file_size": text_file.file_size
    }

@router.get("/text_files/")
async def get_all_text_files(db: Session = Depends(get_db)):
    """Get all uploaded text files"""
    text_files = db.query(TextFile).order_by(TextFile.upload_date.desc()).all()
    return [text_file.to_dict() for text_file in text_files]

@router.get("/text_files/{file_id}")
async def get_text_file(file_id: int, db: Session = Depends(get_db)):
    """Get a specific text file by ID"""
    text_file = db.query(TextFile).filter(TextFile.id == file_id).first()
    if not text_file:
        raise HTTPException(status_code=404, detail="Text file not found")
    return text_file.to_dict()

@router.get("/text_files/{file_id}/content")
async def get_text_file_content(file_id: int, db: Session = Depends(get_db)):
    """Get the content of a specific text file"""
    text_file = db.query(TextFile).filter(TextFile.id == file_id).first()
    if not text_file:
        raise HTTPException(status_code=404, detail="Text file not found")
    return {
        "id": text_file.id,
        "filename": text_file.original_filename,
        "content": text_file.content,
        "file_size": text_file.file_size
    }

@router.put("/text_files/{file_id}")
async def update_text_file(file_id: int, file: UploadFile = File(...), db: Session = Depends(get_db)):
    """Update a text file"""
    text_file = db.query(TextFile).filter(TextFile.id == file_id).first()
    if not text_file:
        raise HTTPException(status_code=404, detail="Text file not found")
    
    # Validate file type
    if not file.filename.lower().endswith('.txt'):
        raise HTTPException(status_code=400, detail="Only .txt files are allowed")
    
    # Read new content
    content = await file.read()
    content_str = content.decode('utf-8')
    
    # Update file on disk
    file_path = Path(text_file.file_path)
    with open(file_path, "wb") as buffer:
        buffer.write(content)
    
    # Update database record
    text_file.content = content_str
    text_file.file_size = len(content)
    text_file.last_modified = datetime.now()
    text_file.original_filename = file.filename
    
    db.commit()
    db.refresh(text_file)
    
    return {
        "message": "Text file updated successfully",
        "file_id": text_file.id,
        "filename": text_file.original_filename,
        "file_size": text_file.file_size
    }

@router.delete("/text_files/{file_id}")
async def delete_text_file(file_id: int, db: Session = Depends(get_db)):
    """Delete a text file"""
    text_file = db.query(TextFile).filter(TextFile.id == file_id).first()
    if not text_file:
        raise HTTPException(status_code=404, detail="Text file not found")
    
    # Delete file from disk
    file_path = Path(text_file.file_path)
    if file_path.exists():
        file_path.unlink()
    
    # Delete from database
    db.delete(text_file)
    db.commit()
    
    return {"message": "Text file deleted successfully"} 