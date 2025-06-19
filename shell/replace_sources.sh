#!/bin/bash

# 检查是否以root权限运行
if [ "$(id -u)" -ne 0 ]; then
    echo "请使用sudo运行此脚本" >&2
    exit 1
fi

# 备份原始源列表
cp /etc/apt/sources.list /etc/apt/sources.list.bak

# 获取Ubuntu版本代号
CODENAME=$(lsb_release -cs)

# 定义清华源地址
TSINGHUA_MIRROR="https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports/"
UBUNTU_PORTS_MIRROR="http://ports.ubuntu.com/ubuntu-ports/"

# 生成新的sources.list内容
if [ "$CODENAME" = "noble" ]; then
    cat > /etc/apt/sources.list << EOF
# 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释 
deb $TSINGHUA_MIRROR  noble main restricted universe multiverse 
# deb-src $TSINGHUA_MIRROR  noble main restricted universe multiverse 
deb $TSINGHUA_MIRROR  noble-updates main restricted universe multiverse 
# deb-src $TSINGHUA_MIRROR  noble-updates main restricted universe multiverse 
deb $TSINGHUA_MIRROR  noble-backports main restricted universe multiverse 
# deb-src $TSINGHUA_MIRROR  noble-backports main restricted universe multiverse 

# 以下安全更新软件源包含了官方源与镜像站配置，如有需要可自行修改注释切换 
# deb $UBUNTU_PORTS_MIRROR  noble-security main restricted universe multiverse 
# deb-src $UBUNTU_PORTS_MIRROR  noble-security main restricted universe multiverse 

# 预发布软件源，不建议启用 
# deb $TSINGHUA_MIRROR  noble-proposed main restricted universe multiverse 
# # deb-src $TSINGHUA_MIRROR  noble-proposed main restricted universe multiverse
EOF
fi

# 更新软件包列表
apt update