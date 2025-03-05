#!/bin/bash

# 设置错误处理
set -e

# 日志函数
log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $1"
}

# 检查是否为root用户
if [ "$(id -u)" != "0" ]; then
    log "错误: 此脚本需要root权限运行"
    log "请使用 sudo ./setup_dev_env.sh 运行此脚本"
    exit 1
fi

# 更新包列表
log "更新包列表..."
apt update

# 安装中文语言支持
log "安装中文语言支持..."
apt install -y language-pack-zh-hans fonts-droid-fallback fonts-wqy-zenhei fonts-wqy-microhei fonts-arphic-ukai fonts-arphic-uming

# 设置系统默认语言环境
log "配置中文语言环境..."
locale-gen zh_CN.UTF-8
update-locale LANG=zh_CN.UTF-8 LANGUAGE=zh_CN:zh LC_ALL=zh_CN.UTF-8

# 在 ~/.bashrc 中追加语言环境变量
log "在 ~/.bashrc 中追加语言环境变量..."

# 添加环境变量的函数
add_lang_env() {
    local file=$1
    if [ -f "$file" ] && ! grep -q "export LANG=zh_CN.UTF-8" "$file"; then
        echo 'export LANG=zh_CN.UTF-8' >> "$file"
        log "已添加语言环境变量到 $file"
    else
        log "跳过 $file，环境变量已存在"
    fi
}

# 为 root 和新用户配置文件添加环境变量
add_lang_env "/root/.bashrc"
add_lang_env "/etc/skel/.bashrc"

# 为现有用户添加环境变量
if [ -d /home ]; then
    for user_home in /home/*; do
        if [ -d "$user_home" ]; then
            add_lang_env "$user_home/.bashrc"
            chown $(stat -c "%U:%G" "$user_home") "$user_home/.bashrc"
        fi
    done
fi

# 设置时区
log "设置时区为Asia/Shanghai..."
ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

# 安装基础开发工具
log "安装基础开发工具..."
apt install -y \
    build-essential \
    binutils \
    gcc \
    g++ \
    gdb \
    git \
    curl \
    wget \
    vim \
    nano \
    python3 \
    python3-pip \
    nodejs \
    npm \
    default-jdk \
    maven \
    coreutils \
    w3m \
    tmux \
    ctags \
    cscope \
    htop \
    lsof \
    strace \
    apt-file \
    bsdmainutils

# 安装额外的开发库
log "安装开发库..."
apt install -y \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    libncurses5-dev \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    libffi-dev \
    liblzma-dev

apt-file update

# 检查是否已安装 Miniconda3
if [ -d "/opt/miniconda3" ] || command -v conda &> /dev/null; then
    log "检测到 Conda 已安装，跳过 Miniconda3 安装步骤..."
else
    # 安装 Miniconda3
    log "安装 Miniconda3..."
    MINICONDA_VERSION="latest"
    
    # 检测系统架构并选择对应的安装包
    if [ "$(uname -m)" = "arm64" ] || [ "$(uname -m)" = "aarch64" ]; then
        MINICONDA_INSTALLER="Miniconda3-${MINICONDA_VERSION}-Linux-aarch64.sh"
    else
        MINICONDA_INSTALLER="Miniconda3-${MINICONDA_VERSION}-Linux-x86_64.sh"
    fi
    
    wget "https://repo.anaconda.com/miniconda/${MINICONDA_INSTALLER}"
    bash "${MINICONDA_INSTALLER}" -b -p /opt/miniconda3
    rm "${MINICONDA_INSTALLER}"

    # 配置 Miniconda3 环境
    log "配置 Miniconda3 环境..."
    echo 'export PATH="/opt/miniconda3/bin:$PATH"' >> /etc/profile.d/conda.sh
    echo 'export PATH="/opt/miniconda3/bin:$PATH"' >> /root/.bashrc
    if [ -d /home ]; then
        for user_home in /home/*; do
            if [ -d "$user_home" ]; then
                echo 'export PATH="/opt/miniconda3/bin:$PATH"' >> "$user_home/.bashrc"
                chown $(stat -c "%U:%G" "$user_home") "$user_home/.bashrc"
            fi
        done
    fi

    # 初始化 conda
    /opt/miniconda3/bin/conda init bash
    /opt/miniconda3/bin/conda config --set auto_activate_base false
fi

# 验证安装
log "验证安装..."
log "Git 版本: $(git --version)"
log "Python 版本: $(python3 --version)"
log "Node.js 版本: $(node --version)"
log "npm 版本: $(npm --version)"
log "Java 版本: $(java -version 2>&1 | head -n 1)"
log "Conda 版本: $(/opt/miniconda3/bin/conda --version)"

log "基础开发环境设置完成！"
log "中文环境配置完成！"
log "请注意：语言环境的更改需要重新登录才能生效"
log "建议您重新启动系统以确保所有更改生效"
