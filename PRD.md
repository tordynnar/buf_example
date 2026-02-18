Use the Buf CLI to create a Python package (wheel) from the proto folder. The package must include .pyi stubs for both the protobuf and gRPC definitions. Use protoc-gen-mypy and protoc-gen-mypy_grpc as plugins in buf.gen.yaml to achieve that.

Confirm proto bindings Python package works by using it with the echo client/server.

Also use pyright and ruff to type check and lint the echo client/server.

IMPORTANT: Avoid changing the proto files or the echo client/server code.