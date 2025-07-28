#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

console.log('🔍 Verifying deployment configuration...\n');

let allGood = true;

// Check frontend config
console.log('📱 Frontend Configuration:');
const configPath = path.join(__dirname, 'frontend/src/config.js');
if (fs.existsSync(configPath)) {
  const configContent = fs.readFileSync(configPath, 'utf8');
  if (configContent.includes('USE_RENDER = true')) {
    console.log('✅ Production mode enabled');
  } else {
    console.log('❌ Still in development mode');
    allGood = false;
  }
} else {
  console.log('❌ Config file not found');
  allGood = false;
}

// Check backend files
console.log('\n🔧 Backend Configuration:');
const backendFiles = [
  'backend/build.sh',
  'backend/start.sh',
  'backend/requirements.txt',
  'backend/main.py'
];

backendFiles.forEach(file => {
  if (fs.existsSync(file)) {
    console.log(`✅ ${file}`);
  } else {
    console.log(`❌ ${file} missing`);
    allGood = false;
  }
});

// Check C++ files
console.log('\n⚙️  C++ Backend:');
const cppFiles = [
  'backend/cpp/generate_ou.cpp',
  'backend/cpp/SignedDistanceCalculator.cpp',
  'backend/cpp/PointGenerator.cpp'
];

cppFiles.forEach(file => {
  if (fs.existsSync(file)) {
    console.log(`✅ ${file}`);
  } else {
    console.log(`❌ ${file} missing`);
    allGood = false;
  }
});

// Check frontend build files
console.log('\n🌐 Frontend Build:');
const frontendFiles = [
  'frontend/package.json',
  'frontend/netlify.toml',
  'frontend/public/index.html'
];

frontendFiles.forEach(file => {
  if (fs.existsSync(file)) {
    console.log(`✅ ${file}`);
  } else {
    console.log(`❌ ${file} missing`);
    allGood = false;
  }
});

// Check scripts
console.log('\n📜 Scripts:');
const scripts = [
  'switch-env.js',
  'DEPLOYMENT.md'
];

scripts.forEach(file => {
  if (fs.existsSync(file)) {
    console.log(`✅ ${file}`);
  } else {
    console.log(`❌ ${file} missing`);
    allGood = false;
  }
});

console.log('\n' + '='.repeat(50));

if (allGood) {
  console.log('🎉 All systems ready for deployment!');
  console.log('\n📋 Next steps:');
  console.log('1. Push code to GitHub');
  console.log('2. Deploy backend to Render');
  console.log('3. Deploy frontend to Netlify');
  console.log('4. Test the deployed application');
} else {
  console.log('❌ Some issues found. Please fix them before deploying.');
}

console.log('\n📖 See DEPLOYMENT.md for detailed instructions.'); 