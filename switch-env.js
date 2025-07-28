#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

const configPath = path.join(__dirname, 'frontend/src/config.js');

function switchEnvironment(env) {
  if (!['dev', 'prod'].includes(env)) {
    console.error('❌ Invalid environment. Use "dev" or "prod"');
    process.exit(1);
  }

  try {
    let configContent = fs.readFileSync(configPath, 'utf8');
    
    if (env === 'prod') {
      configContent = configContent.replace(
        /const USE_RENDER = false;/,
        'const USE_RENDER = true;'
      );
      console.log('✅ Switched to PRODUCTION environment');
      console.log('🌐 Backend URL: https://stl-backend-ipt7.onrender.com');
    } else {
      configContent = configContent.replace(
        /const USE_RENDER = true;/,
        'const USE_RENDER = false;'
      );
      console.log('✅ Switched to DEVELOPMENT environment');
      console.log('🌐 Backend URL: http://localhost:8000');
    }
    
    fs.writeFileSync(configPath, configContent);
    console.log('📝 Configuration updated successfully!');
    
  } catch (error) {
    console.error('❌ Error updating configuration:', error.message);
    process.exit(1);
  }
}

const environment = process.argv[2];
if (!environment) {
  console.log('Usage: node switch-env.js [dev|prod]');
  console.log('');
  console.log('Examples:');
  console.log('  node switch-env.js dev   # Switch to development');
  console.log('  node switch-env.js prod  # Switch to production');
  process.exit(1);
}

switchEnvironment(environment); 