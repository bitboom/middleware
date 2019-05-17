FROM ubuntu:16.04
MAINTAINER sangwan.kwon@samsung.com

# basis toolchain
RUN apt-get update && \
    apt-get install -qq build-essential git cmake automake

# glog & gtest
RUN cd /usr/src && \
    git clone https://github.com/google/glog.git && \
    git clone https://github.com/google/googletest.git

RUN cd /usr/src/glog && \
    cmake . && make && make install

RUN cd /usr/src/googletest && \
    cmake . && make && make install

# rocksdb
RUN cd /usr/src && \
    git clone https://github.com/facebook/rocksdb.git

# rocksdb - dependencies
RUN apt-get install -qq libsnappy-dev && \
    apt-get install -qq zlib1g-dev && \
    apt-get install -qq libbz2-dev && \
    apt-get install -qq liblz4-dev && \
    apt-get install -qq libzstd-dev

RUN cd /usr/src/rocksdb && \
    make static_lib && make install

# boost
RUN apt-get install -qq libboost-all-dev

# sqlite3
RUN apt-get install -qq libsqlite3-dev

# gflags
RUN apt-get install -qq libgflags-dev

# thrift
RUN apt-get install -qq libevent-dev libtool flex bison pkg-config g++ libssl-dev

RUN cd /usr/src && \
    git clone https://github.com/apache/thrift.git

RUN cd /usr/src/thrift && \
    git checkout 0.9.3 && \
    ./bootstrap.sh && \
    ./configure && \
    make && make install
