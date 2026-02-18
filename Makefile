.PHONY: generate postgen build install lint typecheck check all clean generate-csharp build-csharp pack-csharp clean-csharp all-csharp

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

generate-csharp:
	buf generate --template buf.gen.csharp.yaml

build-csharp:
	dotnet build csharp_package/Demo.Proto.Bindings.csproj --configuration Release

pack-csharp:
	dotnet pack csharp_package/Demo.Proto.Bindings.csproj --configuration Release --output csharp_package/dist

clean-csharp:
	rm -rf csharp_package/Generated/ csharp_package/bin/ csharp_package/obj/ csharp_package/dist/

all-csharp: generate-csharp build-csharp pack-csharp
