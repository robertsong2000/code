import { motion } from 'framer-motion';

const Experience = () => {
  const experiences = [
    {
      company: '某科技有限公司',
      position: '高级前端开发工程师',
      period: '2021 - 至今',
      description: '负责公司核心产品的前端开发和架构设计，带领团队完成多个重要项目。主要工作包括：',
      achievements: [
        '优化前端性能，提升应用加载速度30%',
        '设计并实现组件库，提高开发效率',
        '指导初级开发者，促进团队技术成长'
      ]
    },
    {
      company: '某网络科技公司',
      position: '前端开发工程师',
      period: '2019 - 2021',
      description: '参与电商平台的开发和维护工作。主要成就：',
      achievements: [
        '实现复杂的商品展示和购物车功能',
        '开发响应式布局，支持多端适配',
        '优化用户体验，提升用户转化率15%'
      ]
    },
    {
      company: '某软件公司',
      position: '初级前端开发工程师',
      period: '2018 - 2019',
      description: '参与企业管理系统的开发。主要工作：',
      achievements: [
        '实现数据可视化功能',
        '开发表单验证组件',
        '协助后端开发接口设计'
      ]
    }
  ];

  return (
    <section id="experience" className="min-h-screen pt-20 bg-gray-50">
      <div className="container mx-auto px-4 py-16">
        <motion.h2
          className="text-3xl md:text-4xl font-bold text-dark mb-12 text-center"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
        >
          工作经历
        </motion.h2>
        <div className="max-w-4xl mx-auto">
          {experiences.map((exp, index) => (
            <motion.div
              key={index}
              className="mb-12 relative pl-8 border-l-2 border-primary"
              initial={{ opacity: 0, x: -20 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.5, delay: index * 0.2 }}
              viewport={{ once: true }}
            >
              <div className="absolute left-[-9px] top-0 w-4 h-4 bg-primary rounded-full" />
              <h3 className="text-xl font-bold text-primary mb-2">{exp.company}</h3>
              <h4 className="text-lg font-semibold text-gray-700 mb-2">{exp.position}</h4>
              <p className="text-gray-500 mb-3">{exp.period}</p>
              <p className="text-gray-600 mb-3">{exp.description}</p>
              <ul className="list-disc list-inside space-y-2">
                {exp.achievements.map((achievement, idx) => (
                  <li key={idx} className="text-gray-600">{achievement}</li>
                ))}
              </ul>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Experience;