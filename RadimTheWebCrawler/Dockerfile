FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-openssl-dev \
    libpq-dev \
    libpqxx-dev \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

COPY ./src /app/src

COPY ./CMakeLists.txt /app

WORKDIR /app/build

RUN cmake ..

RUN cmake --build .

CMD ["./RadimWebCrawler"]
