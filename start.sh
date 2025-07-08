#!/bin/bash

# Production startup script for STL Dashboard Backend
echo "Starting STL Dashboard Backend in production mode..."

# Navigate to backend directory
cd "$(dirname "$0")"

# Ensure build is complete
if [ ! -f "cpp/bin/generate_ou" ]; then
    echo "Building backend first..."
    chmod +x build.sh
    ./build.sh
fi

# Set production environment variables
export ENVIRONMENT=production
export PORT=${PORT:-8000}

# Start the application with gunicorn for production
echo "Starting server on port $PORT..."
gunicorn main:app \
    --bind 0.0.0.0:$PORT \
    --workers 2 \
    --worker-class uvicorn.workers.UvicornWorker \
    --timeout 120 \
    --keep-alive 5 \
    --max-requests 1000 \
    --max-requests-jitter 100 