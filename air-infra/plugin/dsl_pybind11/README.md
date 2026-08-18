# How to use dsl plugin

## dependence

- install with pip
Why does the pip install package not have pybind lib?
```
pip install pybind11==2.12.0

pybind11-2.12.0-py3-none-any.whl
```

- install with build src

```
git clone https://github.com/pybind/pybind11.git
cd pybind11
mkdir build
cd build
cmake ..
make install
```

## build

```
mkdir build
cd build
cmake ..
make
make install
```

## Test

- DSL
```
python3 test.py
```

- TRAVISOR

```
python3 dsl.py  --pyfile "script.py"
```

