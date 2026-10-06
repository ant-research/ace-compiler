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
                        pybind11-dev \
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
    pip3 install pybind11==2.13.1 && \
    pip3 install pandas==2.0.3 && \
    rm -rf /root/.cache/pip

# copy soucecode & scipt
WORKDIR /app

COPY ./ .
