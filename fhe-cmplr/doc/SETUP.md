# Local Development Environment

## Enviroment Requirement

The ACE compiler is developed on Ubuntu:20.04 with GCC version 9.4.0

```
uname -a
Linux dee31429054f 5.15.0-106-generic #116-Ubuntu SMP Wed Apr 17 09:17:56 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
```

```
cat /etc/os-release
NAME="Ubuntu"
VERSION="20.04.5 LTS (Focal Fossa)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 20.04.5 LTS"
VERSION_ID="20.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=focal
UBUNTU_CODENAME=focal
```

```
gcc --version
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

There are three ways to set up the development environment: 
- Pull a built docker image directly
- Build the docker image manually
- Install the development environment on the host manually

## Docker Development Enviroment

### docker/docker-install

The purpose of the install method is for a convenience for quickly installing the latest Docker-CE releases on the supported distros. For more thorough instructions for installing on the supported distros, see the install instructions.
- [Install Docker Engine](https://docs.docker.com/engine/install/)
- [Install Docker Desktop on Linux](https://docs.docker.com/desktop/install/linux-install/)
- [Install Docker Desktop on Mac](https://docs.docker.com/desktop/install/mac-install/)
- [Install Docker Desktop on Windows](https://docs.docker.com/desktop/install/windows-install/)

### Pull in the built Docker Image

- Pull Docker Image for local develop

```
# X86_64
docker pull reg.docker.alibaba-inc.com/ace/dev:latest
# AARCH64
docker pull reg.docker.alibaba-inc.com/ace/dev:latest_aarch64
```

- Run the Local Docker Develop Environment, Select the local development mount mode as required

```
# Run Docker Container
# Mount ssh key & workspace folder
docker run -it --name dev -v ~/.ssh/:/root/.ssh/ -v "workspace":/app reg.docker.alibaba-inc.com/ace/dev:latest /bin/bash
```

## Build a Docker Image manually

- Build Docker Image
```
cd fhe-cmplr/test/docker
docker build . -t ace:base
```

- Setup development Environment

```
docker run -it --name dev ace:latest bash
```

## Setup host Development Enviroment

- Install the apt-get package manually

```
apt-get update && \
apt-get install -y git vim wget time \
                    build-essential \
                    gcc \
                    g++ \
                    cmake \
                    ninja-build \
                    python3 \
                    python3-pip \
                    python3-dev \
                    pybind11-dev \
                    libprotobuf-dev  \
                    protobuf-compiler \
                    libssl-dev  \
                    libgmp3-dev  \
                    libtool \
                    libomp5 \
                    libomp-dev \
                    libntl-dev && \
```

- Install the pip package manually

```
pip3 install PyYAML==5.3.1
pip3 install torch==2.0.1
pip3 install onnx==1.14.1
pip3 install onnxruntime==1.15.1
pip3 install matplotlib==3.7.5
pip3 install numpy==1.24.4
pip3 install torchvision==0.15.2
pip3 install pybind11==2.13.1
pip3 install pandas==2.0.3
```