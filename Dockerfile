# ubuntu:20.04
ARG IMAGE_BASE=ubuntu:20.04
FROM ${IMAGE_BASE}

ENV DEBIAN_FRONTEND=noninteractive

# install common tools
RUN apt-get update && \
    apt-get install -y git vim wget time \
                        build-essential \
                        gcc \
                        g++ \
                        cmake \
                        ninja-build \
                        python3 \
                        python3-pip \
                        python3-dev \
                        libprotobuf-dev  \
                        protobuf-compiler \
                        libssl-dev  \
                        libgmp3-dev  \
                        libtool \
                        libomp5 \
                        libomp-dev \
                        libntl-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN pip3 install PyYAML==5.3.1 &&\
    pip3 install torch==2.0.1 && \
    pip3 install onnx==1.14.1 && \
    pip3 install onnxruntime==1.15.1 && \
    pip3 install matplotlib==3.7.5 && \
    pip3 install numpy==1.24.4 && \
    pip3 install torchvision==0.15.2 && \
    rm -rf /root/.cache/pip

WORKDIR /app/cifar

RUN wget -qO- https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz | tar xzv --strip-components=1 && \
    wget -qO- https://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz | tar xzv --strip-components=1 && \
    wget https://www.cs.toronto.edu/~kriz/cifar-10-python.tar.gz && \
    wget https://www.cs.toronto.edu/~kriz/cifar-100-python.tar.gz

# copy soucecode & scipt
WORKDIR /app

COPY ./ .