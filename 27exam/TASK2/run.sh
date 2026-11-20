#!/usr/bin/env zsh
if [[ -z "$1" || "$1" == "help" ]]; then
        echo "用法：./run.sh <命令> [文件]"
        exit 0
fi

Command=$1

case "$Command" in
    build)
        if [[ -f "build/myapp" && "build/myapp" -nt "src/main.cpp" ]]; then
            echo "[提示]：可执行文件已是最新，跳过编译"
            exit 0
        fi
            cmake -B build
            cmake --build build
            if [[ $? -ne 0 ]]; then
                echo "[错误]：编译失败" >&2
                exit 1
            fi
                echo "编译成功！"
                exit 0
        ;;
    run)


        if [[ ! -f "build/myapp" || "build/myapp" -ot "src/main.cpp" ]]; then
            cmake -B build
            cmake --build build
            if [[ $? -ne 0 ]]; then
                echo "[错误]：编译失败，无法运行" >&2
                exit 1
            fi
        fi
            if [[ "$2" == "--" ]]; then
                shift 2
                ./build/myapp  "$@"
            else
                ./build/myapp
                exit 0
            fi
        ;;
    clean)
        if [[ -d "build" ]]; then
            rm -rf build
            echo "清理完成"
            exit 0
        fi
        ;;
    esac
    
        


