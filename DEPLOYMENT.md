# STL Dashboard Deployment Guide

This guide covers deploying the STL Dashboard to production on Render (backend) and Netlify (frontend).

## 🚀 Quick Deployment

### 1. Frontend Deployment (Netlify)

1. **Switch to production environment:**
   ```bash
   node switch-env.js prod
   ```

2. **Build the frontend:**
   ```bash
   cd frontend
   npm run build
   ```

3. **Deploy to Netlify:**
   - Connect your GitHub repository to Netlify
   - Set build command: `npm run build`
   - Set publish directory: `build`
   - Deploy!

### 2. Backend Deployment (Render)

1. **Create a new Web Service on Render:**
   - Connect your GitHub repository
   - Set build command: `cd backend && chmod +x build.sh && ./build.sh`
   - Set start command: `cd backend && chmod +x start.sh && ./start.sh`
   - Set environment: `Python 3`

2. **Environment Variables (optional):**
   - `ENVIRONMENT=production`
   - `PORT=8000`

## 📋 Detailed Steps

### Frontend (Netlify)

1. **Environment Configuration:**
   - The `config.js` file is already set for production
   - Backend URL: `https://stl-backend-ipt7.onrender.com`

2. **Build Process:**
   ```bash
   cd frontend
   npm install
   npm run build
   ```

3. **Netlify Configuration:**
   - The `netlify.toml` file is already configured
   - Handles SPA routing with redirects

### Backend (Render)

1. **Build Process:**
   - Compiles C++ tools for `.ou` file generation
   - Installs Python dependencies
   - Creates necessary directories

2. **Runtime Configuration:**
   - Uses Gunicorn with Uvicorn workers
   - Optimized for production performance
   - Proper CORS settings for Netlify domain

3. **File Structure:**
   ```
   backend/
   ├── build.sh          # Build script
   ├── start.sh          # Production startup
   ├── requirements.txt  # Python dependencies
   ├── main.py          # FastAPI app
   ├── cpp/             # C++ backend tools
   └── uploads/         # File storage
   ```

## 🔧 Environment Switching

Use the provided script to switch between environments:

```bash
# Switch to production
node switch-env.js prod

# Switch to development
node switch-env.js dev
```

## 🌐 URLs

- **Frontend (Netlify):** `https://stl-dashboard-frontend.netlify.app`
- **Backend (Render):** `https://stl-backend-ipt7.onrender.com`
- **API Docs:** `https://stl-backend-ipt7.onrender.com/docs`

## ✅ Verification

After deployment, verify:

1. **Frontend loads correctly**
2. **STL file upload works**
3. **3D viewer displays models**
4. **Cross-section slicing works**
5. **Heatmap generation works**
6. **`.ou` files are generated automatically**

## 🐛 Troubleshooting

### Common Issues:

1. **CORS Errors:**
   - Check that the frontend domain is in the allowed origins
   - Verify the backend URL in `config.js`

2. **C++ Compilation Errors:**
   - Ensure g++ is available on Render
   - Check the build logs for compilation errors

3. **File Upload Issues:**
   - Verify upload directories exist
   - Check file permissions

4. **Heatmap Not Working:**
   - Ensure `.ou` files are being generated
   - Check browser console for errors

### Logs:
- **Render:** Check the deployment logs in the Render dashboard
- **Netlify:** Check the build logs in the Netlify dashboard

## 🔄 Updates

To update the deployment:

1. **Frontend:** Push to GitHub, Netlify auto-deploys
2. **Backend:** Push to GitHub, Render auto-deploys
3. **Environment:** Use `node switch-env.js prod` if needed

## 📞 Support

If you encounter issues:
1. Check the logs in Render/Netlify dashboards
2. Verify environment configuration
3. Test locally first with `node switch-env.js dev` 