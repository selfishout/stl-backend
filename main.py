from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from routers import upload

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "https://unrivaled-paletas-19b54d.netlify.app",
        "https://www.unrivaled-paletas-19b54d.netlify.app"
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(upload.router)

@app.get("/")
def root():
    return {"message": "Backend is running."}
