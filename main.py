from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from routers import upload, text_files
from fastapi.staticfiles import StaticFiles
import os

app = FastAPI()

# Ensure upload directories exist
os.makedirs("uploads/stl", exist_ok=True)
os.makedirs("uploads/ou", exist_ok=True)
os.makedirs("uploads/text", exist_ok=True)

app.mount("/uploads", StaticFiles(directory="uploads"), name="uploads")

# Production CORS settings
allowed_origins = [
    "https://unrivaled-paletas-19b54d.netlify.app",  # Your actual Netlify domain
    "https://stl-dashboard-frontend.netlify.app",     # Alternative Netlify domain
    "https://stl-dashboard.netlify.app",              # Alternative Netlify domain
    "http://localhost:3000",                          # Local development
    "http://localhost:3001",                          # Alternative local port
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=allowed_origins,
    allow_origin_regex=r"https://.*\.netlify\.app",  # Allow any Netlify domain
    allow_credentials=True,
    allow_methods=["GET", "POST", "PUT", "DELETE", "OPTIONS"],
    allow_headers=["*"],
)

app.include_router(upload.router, prefix="")
app.include_router(text_files.router, prefix="")

@app.get("/")
def root():
    return {"message": "Backend is running."}

@app.get("/health")
def health_check():
    return {"status": "healthy", "environment": "development"}
