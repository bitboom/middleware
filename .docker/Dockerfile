FROM ubuntu:18.04
MAINTAINER sangwan.kwon@samsung.com

# basis toolchain
RUN apt-get update && \
    apt-get install -qq build-essential git cmake automake

# g++9
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y g++-9 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 20

# glog & gtest
RUN cd /usr/src && \
    git clone https://github.com/google/glog.git && \
    git clone https://github.com/google/googletest.git

RUN cd /usr/src/glog && \
    cmake . && make && make install

RUN cd /usr/src/googletest && \
    cmake . && make && make install

# boost
RUN apt-get install -qq libboost-all-dev

# sqlite3 - shell
RUN apt-get install -qq libsqlite3-dev libreadline-dev

# gflags
RUN cd /usr/src && \
    git clone https://github.com/gflags/gflags.git

RUN cd /usr/src/gflags && \
    git checkout v2.2.1 && \
    cmake . && make && make install

# table gen dependencies
RUN apt-get install -qq python-pip && \
    pip install Jinja2

# table dependencies
RUN apt-get update --fix-missing
RUN apt-get install -qq libprocps-dev libsystemd-dev \
                        libudev-dev iptables-dev uuid-dev libssl-dev

# linenoise
RUN cd /usr/src && \
    git clone https://github.com/arangodb/linenoise-ng.git && \
    cd /usr/src/linenoise-ng && \
    cmake -DCMAKE_BUILD_TYPE=Release . && make && make install
