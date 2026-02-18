.PHONY: generate postgen build install lint typecheck check all clean generate-csharp build-csharp pack-csharp clean-csharp all-csharp generate-go build-go clean-go all-go generate-rust build-rust clean-rust all-rust generate-cpp build-cpp clean-cpp all-cpp

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

# --- Go targets ---
generate-go:
	rm -rf go_package/demo/
	PATH="$$(go env GOPATH)/bin:$$PATH" buf generate --template buf.gen.go.yaml

build-go: generate-go
	cd go_package && go mod tidy
	cd echo_server_go && go mod tidy && go build -o ../bin/echo_server_go .
	cd echo_client_go && go mod tidy && go build -o ../bin/echo_client_go .

clean-go:
	rm -rf go_package/demo/ bin/echo_server_go bin/echo_client_go

all-go: generate-go build-go

# --- Rust targets ---
generate-rust:
	buf generate --template buf.gen.rust.yaml

build-rust: generate-rust
	cargo build --release

clean-rust:
	rm -rf rust_package/src/gen/
	cargo clean

all-rust: generate-rust build-rust

# --- C++ targets ---
generate-cpp:
	buf generate --template buf.gen.cpp.yaml

build-cpp:
	cmake -B echo_server_cpp/build -S echo_server_cpp
	cmake --build echo_server_cpp/build
	cmake -B echo_client_cpp/build -S echo_client_cpp
	cmake --build echo_client_cpp/build
	mkdir -p bin
	cp echo_server_cpp/build/echo_server_cpp bin/
	cp echo_client_cpp/build/echo_client_cpp bin/

clean-cpp:
	rm -rf cpp_package/gen/ echo_server_cpp/build/ echo_client_cpp/build/ bin/echo_server_cpp bin/echo_client_cpp

all-cpp: generate-cpp build-cpp
