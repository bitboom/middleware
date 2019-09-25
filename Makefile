all:
	mkdir -p build
	cd build && cmake .. && make --no-print-directory $(MAKEFLAGS)

clean:
	rm -rf build

docker_run:
	docker build --network=host --tag tizen-osquery ./.docker
	docker run --rm -it --net=host --privileged -v $(shell pwd):/usr/src tizen-osquery

gbs_run:
	gbs lb -A armv7l --include-all --define="build_type DEBUG" -P standard

%::
	mkdir -p build
	cd build && cmake .. && make --no-print-directory $@
