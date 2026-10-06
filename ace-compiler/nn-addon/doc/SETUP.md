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
docker run -it --name test -v ~/.ssh/:/root/.ssh/ -v "workspace":/app reg.docker.alibaba-inc.com/avh/avh:dev /bin/bash
```

## Clone SourceCode for AVHC repos

- Clone repos by script

After entering Docker, Run the ”clone_avhc.sh“ script to automatically clone AVHC repos.

```
clone_avhc.sh
```

- Clone repos by manually

```
mkdir workarea && cd workarea
git clone git@code.alipay.com:air-infra/nn-addon.git --recurse
or
git clone git@code.alipay.com:air-infra/nn-addon.git
cd nn-addon && git submodule update --init --recursive && cd ..
```

Note: Because ACI's git submodule can only support https, clone code to local, you need to enter the account password once