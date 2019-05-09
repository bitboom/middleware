FROM ubuntu:16.04
MAINTAINER sangwan.kwon@samsung.com

RUN apt-get update && \
    apt-get install -qq build-essential git cmake automake

RUN cd /usr/src && \
    git clone https://github.com/google/glog.git && \
    git clone https://github.com/google/googletest.git && \
    git clone https://github.com/facebook/rocksdb.git

RUN cd /usr/src/glog && \
    cmake . && make && make install

RUN cd /usr/src/googletest && \
    cmake . && make && make install

RUN apt-get install -qq libsnappy-dev && \
    apt-get install -qq zlib1g-dev && \
    apt-get install -qq libbz2-dev && \
    apt-get install -qq liblz4-dev && \
    apt-get install -qq libzstd-dev

RUN cd /usr/src/rocksdb && \
    make static_lib && make install
