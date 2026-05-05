# How to use dsl plugin

## dependence

- install with pip
Why does the pip install package not have pybind lib?
```
# python api
pip install pybind11

# c++ lib
apt install pybind11-dev
```


## build

```
python3 -m pip install --user -e .
mkdir build
cd build
cmake -G Ninja ..
ninja
ninja install
```

## Test

- DSL
```
./add_vector_demo.cxx
```
