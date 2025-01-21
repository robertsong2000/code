import { motion } from 'framer-motion';

const Projects = () => {
  const projects = [
    {
      title: '企业级电商平台',
      description: '基于React + TypeScript开发的大型电商系统',
      role: '前端技术负责人',
      technologies: ['React', 'TypeScript', 'Redux', 'Ant Design'],
      achievements: [
        '主导前端架构设计，建立了完整的开发规范',
        '实现了高性能的商品列表和购物车功能',
        '开发了可复用的业务组件库',
        '优化首屏加载时间，提升用户体验'
      ]
    },
    {
      title: '数据可视化平台',
      description: '企业级数据分析和可视化系统',
      role: '核心开发者',
      technologies: ['Vue.js', 'ECharts', 'TypeScript', 'Element Plus'],
      achievements: [
        '设计并实现了多种数据图表组件',
        '优化大数据渲染性能',
        '实现了图表数据的实时更新功能',
        '开发了图表配置的可视化编辑器'
      ]
    },
    {
      title: '在线教育平台',
      description: '面向K12教育的在线学习系统',
      role: '前端开发工程师',
      technologies: ['Next.js', 'TailwindCSS', 'WebRTC', 'Socket.IO'],
      achievements: [
        '实现了实时在线课堂功能',
        '开发了响应式的课程内容展示系统',
        '优化了音视频通讯质量',
        '实现了离线缓存功能'
      ]
    }
  ];

  return (
    <section id="projects" className="min-h-screen pt-20 bg-gray-50">
      <div className="container mx-auto px-4 py-16">
        <motion.h2
          className="text-3xl md:text-4xl font-bold text-dark mb-12 text-center"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
        >
          项目经验
        </motion.h2>
        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-8 max-w-7xl mx-auto">
          {projects.map((project, index) => (
            <motion.div
              key={index}
              className="bg-white rounded-lg shadow-lg overflow-hidden"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: index * 0.1 }}
              viewport={{ once: true }}
            >
              <div className="p-6">
                <h3 className="text-xl font-bold text-primary mb-2">{project.title}</h3>
                <p className="text-gray-600 mb-4">{project.description}</p>
                <p className="text-gray-700 font-semibold mb-2">担任角色：{project.role}</p>
                <div className="mb-4">
                  <h4 className="text-gray-700 font-semibold mb-2">技术栈：</h4>
                  <div className="flex flex-wrap gap-2">
                    {project.technologies.map((tech, idx) => (
                      <span
                        key={idx}
                        className="px-3 py-1 bg-primary/10 text-primary rounded-full text-sm"
                      >
                        {tech}
                      </span>
                    ))}
                  </div>
                </div>
                <div>
                  <h4 className="text-gray-700 font-semibold mb-2">主要成就：</h4>
                  <ul className="list-disc list-inside space-y-1 text-gray-600">
                    {project.achievements.map((achievement, idx) => (
                      <li key={idx}>{achievement}</li>
                    ))}
                  </ul>
                </div>
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Projects;