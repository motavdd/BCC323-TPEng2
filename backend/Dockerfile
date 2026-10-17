FROM debian:trixie-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates cmake g++ git libasio-dev ninja-build python3 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY CMakeLists.txt ./
COPY src ./src
COPY test ./test
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel 2

FROM build AS test
CMD ["ctest", "--test-dir", "build", "--output-on-failure"]

FROM debian:trixie-slim AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --uid 10001 --create-home app
COPY --from=build /app/build/bcc323-api /usr/local/bin/bcc323-api
USER app
ENV PORT=8081
EXPOSE 8081
CMD ["bcc323-api"]
