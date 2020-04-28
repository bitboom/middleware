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

# glog & gtest & gflag
RUN cd /usr/src && \
    git clone https://github.com/google/glog.git && \
    git clone https://github.com/google/googletest.git && \
    git clone https://github.com/gflags/gflags.git

RUN cd /usr/src/glog && \
    cmake . && make && make install

RUN cd /usr/src/googletest && \
    cmake . && make && make install

RUN cd /usr/src/gflags && \
    cmake . && make && make install

# build dependencies
RUN apt-get update --fix-missing
RUN apt-get install -qq libboost-all-dev libsqlite3-dev libsystemd-dev
