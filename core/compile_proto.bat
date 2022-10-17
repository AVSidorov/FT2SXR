SET SOURCE_PATH=%CD%

%protoc --proto_path=%SOURCE_PATH% --cpp_out=%SOURCE_PATH% --python_out=%SOURCE_PATH% --rust_out=%SOURCE_PATH% %1

protoc --proto_path=./core --python_out=./core core/sxr_protocol.proto
