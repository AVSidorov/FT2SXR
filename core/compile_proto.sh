#!/bin/bash

start_dir=$(pwd)
script=$0
script_dir=${script%/*}

cd $script_dir

script_dir=$(pwd)

protoc --proto_path=$script_dir --python_out=$script_dir $script_dir/sxr_protocol.proto

cd $start_dir
