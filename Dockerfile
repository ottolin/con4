FROM alpine:latest as build
RUN apk update && apk add --no-cache build-base binutils cmake make musl-dev libgcc gcc g++

COPY ./src /src
WORKDIR /src
RUN mkdir out \
    && cd out \
    && gcc -o con4 /src/con4.cpp

FROM alpine:latest as runtime

RUN apk update && apk add --no-cache \
    libstdc++

RUN mkdir /usr/local/fireside
COPY --from=build /src/out/con4 /usr/local/fireside/con4

WORKDIR /usr/local/fireside
CMD ./con4

