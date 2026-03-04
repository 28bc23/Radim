FROM alpine:3.23.3

ENV DEBIAN_FRONTEND=noninteractive

RUN apk add --no-cache \
    build-base \
    cmake \
    curl-dev \
    curl \
    libpq-dev

RUN curl -L https://github.com/jtv/libpqxx/archive/refs/tags/7.10.0.tar.gz -o libpqxx.tar.gz && \
    tar -xzf libpqxx.tar.gz && \
    cd libpqxx-7.10.0 && \
    mkdir build && cd build && \
    cmake -D SKIP_BUILD_TEST=ON .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && rm -rf libpqxx.tar.gz libpqxx-7.10.0

COPY ./src /app/src

COPY ./CMakeLists.txt /app

WORKDIR /app/build

RUN cmake ..

RUN cmake --build .

CMD ["./Radim"]
