.PHONY: generate postgen build install lint typecheck check all clean

generate:
	buf generate

postgen:
	find proto_package/src/demo -type d -exec touch {}/__init__.py \;
	touch proto_package/src/demo/py.typed

build:
	uv build ./proto_package

install:
	uv pip install ./proto_package ./echo_client ./echo_server

lint:
	ruff check echo_client echo_server

typecheck:
	pyright

check: lint typecheck

all: generate postgen install check

clean:
	rm -rf proto_package/src/
