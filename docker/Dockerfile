FROM ubuntu:16.04
MAINTAINER sangwan.kwon@samsung.com

RUN apt-get update && \
    apt-get install -qq build-essential git cmake automake

RUN cd /usr/src && \
    git clone https://github.com/google/googletest.git

RUN cd /usr/src/googletest && \
    cmake . && make && make install
