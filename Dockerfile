FROM gcc:12

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y cmake libboost-filesystem-dev libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

RUN cmake .
RUN make

CMD ["./orderbook_collector"]
