# Build Stage
FROM fuzzers/aflplusplus:3.12c as builder

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
FROM fuzzers/aflplusplus:3.12c
COPY --from=builder /gif-h/a.out /fuzz-gif

## Debugging corpus
RUN mkdir /tests && echo seed > /tests/seed

## Set up fuzzing!
ENTRYPOINT ["afl-fuzz", "-i", "/tests", "-o", "/out"]
CMD ["/packcc"]
