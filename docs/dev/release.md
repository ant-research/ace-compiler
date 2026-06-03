# Release Process

This document describes the release workflow for maintainers.

## Version Management

### Version Format

Follows [Semantic Versioning](https://semver.org/): `MAJOR.MINOR.PATCH`

| Change Type | Version Bump | Example |
|-------------|--------------|---------|
| Breaking API change | MAJOR | 0.1.0 → 1.0.0 |
| New feature | MINOR | 0.1.0 → 0.2.0 |
| Bug fix | PATCH | 0.1.0 → 0.1.1 |

### Update Version

Edit `fhe_dsl/python/_version.py`:

```python
__version__ = "0.2.0"
```

## Pre-Release Checklist

1. **Update Version** in `fhe_dsl/python/_version.py`

2. **Update Changelog** - Edit `CHANGELOG.md`:
   - Change `[Unreleased]` to `[x.x.x] - YYYY-MM-DD`
   - Add changes under Added/Fixed/Changed sections
   - Create new `[Unreleased]` section for next release

3. **Run Tests**
   ```bash
   export LD_LIBRARY_PATH=$(python -c "import sysconfig; print(sysconfig.get_path('platlib'))")/ace/lib:$LD_LIBRARY_PATH
   pytest tests/ -v
   ```

4. **Verify Build**
   ```bash
   rm -rf build/ dist/ _skbuild/
   ./scripts/dev-build.sh --clean
   python -c "from ace import fhe; print('OK')"
   ```

## Build Distribution

### Clean Build

```bash
rm -rf build/ dist/ _skbuild/
```

### Build Wheel

```bash
# CPU-only (default)
pip wheel . -w dist/

# GPU backends
ENABLE_LIB="phantom" pip wheel . -w dist/
ENABLE_LIB="acelib" pip wheel . -w dist/
ENABLE_LIB="acelib;phantom" pip wheel . -w dist/
```

### Verify Wheel

```bash
# Install and verify
pip install dist/ace-*.whl
python -c "import ace; print(ace.__version__)"
```

## Publish to PyPI (TBD)

### Using Twine

```bash
# Install twine
pip install twine

# Upload to PyPI
twine upload dist/ace-*.whl

# Or test PyPI first
twine upload --repository testpypi dist/ace-*.whl
```

## Create GitHub Release

1. **Create Tag**
   ```bash
   git tag -a v0.2.0 -m "Release v0.2.0"
   git push origin v0.2.0
   ```

2. **Create Release on GitHub**
   - Go to: GitHub → Releases → Draft new release
   - Select tag `v0.2.0`
   - Add release notes
   - Upload wheel files from `dist/`

## CI/CD Example

### GitHub Actions

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Set up Python
        uses: actions/setup-python@v5
        with:
          python-version: '3.10'

      - name: Build wheel
        run: pip wheel . -w dist/

      - name: Upload wheel
        uses: actions/upload-artifact@v4
        with:
          name: wheels
          path: dist/*.whl

  publish:
    needs: build
    runs-on: ubuntu-latest
    steps:
      - uses: actions/download-artifact@v4
        with:
          name: wheels
          path: dist/

      - name: Publish to PyPI
        uses: pypa/gh-action-pypi-publish@release/v1
        with:
          password: ${{ secrets.PYPI_API_TOKEN }}
```

## Rollback Procedure

### Yank from PyPI

```bash
pip yank ace 0.2.0
pip yank ace 0.2.0 --reason="Critical bug found"
```

### Hotfix Release

```bash
# Create hotfix branch
git checkout -b hotfix/v0.2.1 v0.2.0

# Fix issue and update version
# fhe_dsl/python/_version.py: __version__ = "0.2.1"

# Build and release
pip wheel . -w dist/
twine upload dist/ace-*.whl
```

## Related Documentation

- [Package Management](package.md) - Installation and usage
- [Developer Guide](develop.md) - Build from source
- [Testing Guide](testing/index.md) - Test guidelines