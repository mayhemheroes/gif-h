# Build Stage
FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake clang build-essential

## Add source code to the build stage.
WORKDIR /
ADD . /gif-h
WORKDIR /gif-h

## Build
RUN clang++ -fsanitize=fuzzer fuzz_gif_creation.cc gif.h fuzz.h

# Package Stage
RUN mkdir /testsuite
FROM --platform=linux/amd64 ubuntu:20.04
COPY --from=builder /gif-h/a.out /fuzz-gif
COPY --from=builder /gif-h/testsuite /testsuite


## Set up fuzzing!
ENTRYPOINT []
CMD /fuzz-gif /testsuite
