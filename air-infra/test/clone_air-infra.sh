# Clone Repos for AIR-INFRA

```
#!/bin/sh

WORKSPACE=$(cd $(dirname $0); pwd)
DIR_AIR="$WORKSPACE/air-infra"

echo "clone air-infra ..."
if [ ! -d "$DIR_AIR" ]; then
    git clone https://code.alipay.com/air-infra/air-infra.git --recurse
else
    cd $DIR_AIR
    git submodule update --init --recursive
    cd ..
fi
```