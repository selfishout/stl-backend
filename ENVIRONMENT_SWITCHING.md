# Environment Switching Guide

This project now includes a simple configuration system that allows you to switch between localhost and Render environments with just one command or variable change.

## Quick Switch Commands

### Switch to Localhost (Development)
```bash
cd frontend
node switch-env.js local
```

### Switch to Render (Production)
```bash
cd frontend
node switch-env.js render
```

## Manual Configuration

If you prefer to manually edit the configuration, you can modify `frontend/src/config.js`:

```javascript
// Change this one variable to switch environments
const USE_RENDER = false; // false = localhost, true = Render
```

## How It Works

The system uses a central configuration file (`frontend/src/config.js`) that:

1. **Defines environment URLs**: 
   - Localhost: `http://localhost:8000`
   - Render:    `https://stl-dashboard-backend.onrender.com` (replace with your actual Render URL)
2. **Exports a helper function** to build full API URLs.
3. **All API calls** in the frontend use this config, so you only need to change the environment in one place.

## Benefits

- ✅ **Single point of control** - Change one variable to switch environments
- ✅ **No hardcoded URLs** - All API calls use the configuration
- ✅ **Easy deployment** - Switch to Render before deploying
- ✅ **Simple development** - Switch to localhost for local development
- ✅ **Script automation** - Use the provided script for quick switching

## Usage Examples

### Development Workflow
1. Start local development: `node switch-env.js local`
2. Start backend: `cd backend && uvicorn main:app --host 0.0.0.0 --port 8000 --reload`
3. Start frontend: `cd frontend && npm start`

### Production Deployment
1. Switch to production: `node switch-env.js render`
2. Deploy to your hosting platform
3. The frontend will automatically use the Render backend

## Updating Render URL

If your Render URL changes, update it in `frontend/src/config.js`:

```javascript
const RENDER_BASE_URL = 'https://your-new-render-url.onrender.com';
```

## Troubleshooting

- **Frontend not connecting**: Check that the environment is set correctly
- **CORS errors**: Ensure the backend CORS settings match the frontend URL
- **404 errors**: Verify the Render URL is correct and the backend is deployed
- If you see errors about API endpoints, make sure you have switched to the correct environment for your deployment.
- If you add new API endpoints, use the helper functions from `config.js` for consistency. 