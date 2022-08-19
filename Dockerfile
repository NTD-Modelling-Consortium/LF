FROM debian:bullseye-slim

# install prerequisite packages
RUN apt-get update -y && apt-get install -y build-essential vim curl git htop silversearcher-ag

# create ntd dir
RUN mkdir /ntd
WORKDIR /ntd

# install gsl library
RUN curl -OL https://ftp.gnu.org/gnu/gsl/gsl-latest.tar.gz \
	&& tar xzf gsl-latest.tar.gz \
	&& rm -f gsl-latest.tar.gz \
	&& cd gsl-* \
	&& ./configure --prefix=/ntd/gsl \
	&& make && make check && make install

# copy in the model code
COPY . /ntd/ntd-model-lf

# compile it
WORKDIR '/ntd/ntd-model-lf/C++ code'
RUN g++ *.cpp tinyxml/*.cpp -o transfil_N -g -I. -I./tinyxml $(/ntd/gsl/bin/gsl-config --libs) $(/ntd/gsl/bin/gsl-config --cflags) -lstdc++fs -Wall -O3 -std=c++11
RUN mkdir /ntd/bin
RUN cp transfil_N /ntd/bin
RUN rm -rf /ntd/ntd-model-lf

# set up library & binary paths so `transfil_N` is accessible & runnable in PATH
ENV LD_LIBRARY_PATH=/ntd/gsl/lib
ENV PATH /ntd/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# working dir inside container, create locally if not overridden with e.g. -v in `docker run`
WORKDIR /ntd/model
