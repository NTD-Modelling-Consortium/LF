FROM debian:bullseye-slim

# install prerequisite packages
RUN apt-get update -y && \
	apt-get install -y \
		build-essential \
		vim curl git htop \
		silversearcher-ag \
		gsl-bin libgsl-dev \
		libxml2-utils \
		parallel \
		python3 pip \
		time

# and python CSV package
RUN pip install pandas

# create ntd dir
RUN mkdir /ntd
WORKDIR /ntd

# copy in the model code
COPY ./src /ntd/ntd-model-lf

# compile it
WORKDIR '/ntd/ntd-model-lf'
RUN g++ *.cpp tinyxml/*.cpp -o transfil_N -g -I. -I./tinyxml $(gsl-config --libs) $(gsl-config --cflags) -lstdc++fs -Wall -O3 -std=c++11
RUN mkdir /ntd/bin
RUN cp transfil_N /ntd/bin
RUN rm -rf /ntd/ntd-model-lf

# set up library & binary paths so `transfil_N` is accessible & runnable in PATH
ENV LD_LIBRARY_PATH=/ntd/gsl/lib
ENV PATH /ntd/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# copy in runner scripts & parameters/scenarios
COPY ./run /ntd/run
COPY ./src/Pop_Distribution.csv /ntd/run/Pop_Distribution.csv
WORKDIR /ntd/run

# extract data files
RUN tar jxf parameters.tbz
RUN tar jxf scenarios.tbz

# convert runner script from macOS as plain 'sed' on debian is already GNU sed
RUN sed -i 's/\bgsed\b/sed/g' run-lf-model.bash

# working dir inside container
WORKDIR /ntd/run
