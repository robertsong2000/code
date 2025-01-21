# 个人简历网站

这是一个使用现代前端技术栈开发的响应式个人简历网站，具有清晰的界面设计和流畅的交互体验。

## 技术栈

- React
- TypeScript
- Tailwind CSS
- Framer Motion
- Vite

## 主要功能

### 1. 响应式设计
- 适配移动端和桌面端
- 流畅的页面过渡效果
- 优雅的滚动交互

### 2. 主要模块

- **头部导航**：固定导航栏，支持平滑滚动到对应区域
- **个人介绍**：展示个人基本信息和简介
- **工作经历**：时间轴形式展示工作经验
- **技能专长**：可视化展示各项技能水平
- **项目经验**：展示个人项目案例

### 3. 动画效果

- 使用Framer Motion实现流畅的页面动画
- 滚动触发的渐入效果
- 交互反馈动画

## 本地开发

1. 克隆项目
```bash
git clone [项目地址]
cd [项目目录]
```

2. 安装依赖
```bash
npm install
```

3. 启动开发服务器
```bash
npm run dev
```

4. 构建项目
```bash
npm run build
```

## 自定义内容

1. 个人信息修改
   - 编辑 `src/components/Hero.tsx` 更新个人简介
   - 编辑 `src/components/Experience.tsx` 更新工作经历
   - 编辑 `src/components/Skills.tsx` 更新技能信息

2. 样式定制
   - 在 `tailwind.config.js` 中修改主题颜色和其他样式配置
   - 在 `src/index.css` 中添加自定义样式

## 部署

项目使用Vite构建，可以轻松部署到任何静态网站托管服务：

1. 执行构建命令
```bash
npm run build
```

2. 将 `dist` 目录部署到你的托管服务

## 贡献

欢迎提交Issue和Pull Request来改进这个项目。

## 许可

MIT License
