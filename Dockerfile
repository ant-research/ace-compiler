# ubuntu:24.04
ARG IMAGE_BASE=ubuntu:24.04
FROM ${IMAGE_BASE}

ARG CI_TOKEN=""
ENV CI_TOKEN=${CI_TOKEN}
ENV DEBIAN_FRONTEND=noninteractive

# setting zone
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install the necessary packages and tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git curl wget emacs vim gdb time doxygen graphviz autoconf \
        build-essential gcc g++ clang-15 nlohmann-json3-dev \
        cmake make ninja-build \
        python3 python3-dev python3-pip python3-venv pylint \
        libprotobuf-dev protobuf-compiler libssl-dev libtool \
        libgmp3-dev libomp5 libomp-dev libntl-dev pybind11-dev fontconfig && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Update Font
RUN echo "ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true" | debconf-set-selections && \
    apt-get update && \
    apt-get install -y --reinstall ttf-mscorefonts-installer && \
    fc-cache -f -v && rm -rf ~/.cache/matplotlib && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# add scipt for clone avhc repos
WORKDIR /root
COPY requirements.txt requirements.txt
RUN python3 -m venv .pyenv && \
    /root/.pyenv/bin/pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/ && \
    /root/.pyenv/bin/pip install --no-cache-dir -r requirements.txt

ENV PATH="/root/.pyenv/bin:${PATH}"

WORKDIR /app

# Copy ace-compiler, scripts and set permissions
COPY README.md .
COPY air-infra ace-compiler/air-infra/
COPY nn-addon ace-compiler/nn-addon/
COPY fhe-cmplr ace-compiler/fhe-cmplr/
COPY proof ace-compiler/proof/
COPY test/* /app/test/
COPY model/* /app/model/
COPY scripts/* /app/scripts/
RUN chmod +x /app/scripts/build_cmplr.sh \
        /app/scripts/hpao.sh \
        /app/scripts/run_full.sh \
        /app/scripts/run_micro.sh \
        /app/scripts/run_perf.sh \
        /app/scripts/perf_common.sh \
        /app/scripts/micro_common.sh
