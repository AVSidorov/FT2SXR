SET SOURCE_PATH=%CD%

protoc --proto_path=%SOURCE_PATH% --python_out=%SOURCE_PATH% sxr_protocol.proto