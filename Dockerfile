# ubuntu22.04 cuda:12.2.0
FROM nvidia/cuda:12.2.0-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y tzdata && \
    ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime && \
    echo "Asia/Shanghai" > /etc/timezone && \
    dpkg-reconfigure -f noninteractive tzdata

# install common tools for build ace
RUN apt-get install -y git git-lfs vim wget time \
                        cmake \
                        python3 \
                        python3-dev \
                        python3-pip \
                        libprotobuf-dev \
                        protobuf-compiler \
                        pybind11-dev \
                        libgmp-dev \
                        libntl-dev \
                        nlohmann-json3-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# add cifar dataset
WORKDIR /app/cifar
RUN wget -O - https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz | tar -xz -C . && \
    wget -O - https://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz | tar -xz -C . && \
    wget https://www.cs.toronto.edu/~kriz/cifar-10-python.tar.gz && \
    wget https://www.cs.toronto.edu/~kriz/cifar-100-python.tar.gz

# copy soucecode & scipt
WORKDIR /app

COPY ./ .

RUN pip3 install --no-cache-dir -r requirements.txt