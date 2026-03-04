FROM alpine:3.23.3

ENV DEBIAN_FRONTEND=noninteractive

RUN apk add --no-cache \
    build-base \
    cmake \
    curl-dev \
    libpqxx

COPY ./src /app/src

COPY ./CMakeLists.txt /app

WORKDIR /app/build

RUN cmake ..

RUN cmake --build .

CMD ["./Radim"]
