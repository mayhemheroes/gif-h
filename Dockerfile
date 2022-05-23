# Build Stage
FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake clang git build-essential

## Add source code to the build stage.
WORKDIR /
RUN git clone https://github.com/capuanob/gif-h.git
WORKDIR /gif-h
RUN git checkout mayhem

## Build
RUN clang++ -DBUILD_FUZZER=1 -fsanitize=fuzzer fuzz.cc gif.h

# Package Stage
RUN mkdir /corpus
FROM --platform=linux/amd64 ubuntu:20.04
COPY --from=builder /gif-h/a.out /fuzz-gif
COPY --from=builder /gif-h/corpus /corpus


## Set up fuzzing!
ENTRYPOINT []
CMD /fuzz-gif /corpus -close_fd_mask=2
