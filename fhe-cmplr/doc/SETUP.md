# Local Development Environment

## docker/docker-install

The purpose of the install method is for a convenience for quickly installing the latest Docker-CE releases on the supported distros. For more thorough instructions for installing on the supported distros, see the install instructions.
- [Install Docker Engine](https://docs.docker.com/engine/install/)
- [Install Docker Desktop on Linux](https://docs.docker.com/desktop/install/linux-install/)
- [Install Docker Desktop on Mac](https://docs.docker.com/desktop/install/mac-install/)
- [Install Docker Desktop on Windows](https://docs.docker.com/desktop/install/windows-install/)

## Setup Local Docker Environment

- Pull Docker Image for local develop

```
# X86_64
docker pull reg.docker.alibaba-inc.com/avh/avh:dev
# AARCH64
docker pull reg.docker.alibaba-inc.com/avh/avh:dev_aarch64
```
- Run the Local Docker Develop Environment, Select the local development mount mode as required

```
# Run Docker Container
# Mount ssh key & workspace folder
docker run -it --name dev -v ~/.ssh/:/root/.ssh/ -v "workspace":/app reg.docker.alibaba-inc.com/avh/avh:dev /bin/bash
```

## Clone SourceCode

- Clone repos by script

After entering Docker, Run the **clone_avhc.sh** script to automatically clone AVHC repos。
```
clone_avhc.sh
```

- Clone repos by manually

```
mkdir workarea && cd workarea
git clone git@code.alipay.com:fhe-cmplr/fhe-cmplr.git --recurse
```
or
```
git clone git@code.alipay.com:fhe-cmplr/fhe-cmplr.git
cd fhe-cmplr
git submodule update --init --recursive
cd ..
```

Note: Because ACI's git submodule can only support https, clone code to local, you need to enter the account password once

## Install libs with air-infra & nn-addon

Daily builds of dependence libraries on ACI, include debug and release versions.
The storage path is: oss://antsys-fhe/daily/***"DATE"***/***air-infra_deb/rel.tar.gz***
Example: Get the dependency library and install, such as:
```
ossutil64 cp oss://antsys-fhe/daily/20230718/air-infra_deb.tar.gz .
tar xf air-infra_deb.tar.gz -C /usr/local
ossutil64 cp oss://antsys-fhe/daily/20230718/nn-addon_deb.tar.gz .
tar xf nn-addon_deb.tar.gz -C /usr/local
```