#!/bin/bash
set -e

echo "=== ACE Compiler GitHub Migration Script ==="
echo ""

# Configuration
REPO_ROOT="/Users/shaw/Documents/workspace/01-Projects/ACE/ace-compiler"
SUBDIRS=("compiler/air-infra" "compiler/nn-addon" "compiler/fhe-cmplr")

cd "$REPO_ROOT"

echo "Step 1: Removing .aci files and directories from root..."
rm -rf .aci.yml .aci/
echo "✓ Root .aci files removed"

echo ""
echo "Step 2: Processing subdirectories..."
for subdir in "${SUBDIRS[@]}"; do
    if [ -d "$subdir" ]; then
        echo "  Processing: $subdir"
        
        # Remove .aci files
        rm -rf "$subdir/.aci.yml" "$subdir/.aci/"
        
        # Remove .git directory (detach from submodule)
        rm -rf "$subdir/.git"
        
        echo "  ✓ Cleaned: $subdir"
    else
        echo "  ⚠ Warning: $subdir not found, skipping"
    fi
done

echo ""
echo "Step 3: Creating .gitignore for GitHub..."
cat > .gitignore << 'EOF'
# Build artifacts
build/
dist/
*.so
*.dylib
*.dll
*.exe

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# Python cache
__pycache__/
*.py[cod]
*$py.class
.Python

# Node modules (if any)
node_modules/
npm-debug.log
yarn-error.log

# Temporary files
tmp/
temp/
*.tmp
*.bak

# Log files
*.log

# Coverage reports
coverage/
*.gcno
*.gcda

# CMake
CMakeFiles/
CMakeCache.txt
cmake_install.cmake

# Compiled objects
*.o
*.obj

# Archives
*.tar.gz
*.zip
*.7z

# Test results
test_results/
pytest_cache/
.pytest_cache/

# Environment files
.env
.env.local
*.env

# Documentation build
docs/_build/
site/

# Misc
*.orig
*.rej
EOF

echo "✓ .gitignore created"

echo ""
echo "Step 4: Initializing new Git repository..."
if [ -d ".git" ]; then
    echo "⚠ Removing existing .git directory..."
    rm -rf .git
    git init
    echo "✓ Git repository reinitialized"
else
    git init
    echo "✓ Git repository initialized"
fi

echo ""
echo "Step 5: Adding all files..."
git add .
echo "✓ Files staged"

echo ""
echo "Step 6: Creating initial commit..."
git commit -m "Initial commit: ACE Compiler framework for FHE inference

- Multi-level IR architecture (NN IR, Vector IR, Crypto IR)
- Automated parameter derivation engine
- Optimizations: FHEFusion, MetaKernel (IMRA), ReSBM
- Support for CPU/GPU/FPGA backends
- Publications: CGO'26, OOPSLA'25, ASPLOS'25, CGO'25"
echo "✓ Initial commit created"

echo ""
echo "=== Migration Complete ==="
echo ""
echo "Next steps:"
echo "1. Create a new repository on GitHub"
echo "2. Run the following commands to push:"
echo ""
echo "   git remote add origin https://github.com/YOUR_USERNAME/ace-compiler.git"
echo "   git branch -M main"
echo "   git push -u origin main"
echo ""
echo "Or if you already have a remote:"
echo "   git remote set-url origin https://github.com/YOUR_USERNAME/ace-compiler.git"
echo "   git push -u origin main"
echo ""
