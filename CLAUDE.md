## Python Environment

- Venv must exist at `.venv`. Create with `uv venv` if missing.
- All packages installed with `uv pip install`.
- Do not lint or type-check generated code (gRPC/protobuf bindings).
- Pyright runs in strict mode (`pyrightconfig.json`).