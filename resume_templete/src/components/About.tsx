import { motion } from 'framer-motion';

const About = () => {
  return (
    <section id="about" className="min-h-screen pt-20 bg-white">
      <div className="container mx-auto px-4 py-16">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="max-w-4xl mx-auto"
        >
          <h2 className="text-3xl md:text-4xl font-bold text-dark mb-8 text-center">关于我</h2>
          <div className="bg-gray-50 rounded-lg p-8 shadow-lg">
            <p className="text-lg text-gray-700 mb-6 leading-relaxed">
              我是一名充满热情的前端开发工程师，拥有扎实的编程功底和丰富的项目经验。我热衷于创造直观、高效且用户友好的web应用，并且始终保持对新技术的学习热情。
            </p>
            <div className="grid md:grid-cols-2 gap-6">
              <div>
                <h3 className="text-xl font-semibold text-primary mb-4">个人特点</h3>
                <ul className="space-y-2 text-gray-600">
                  <li>• 积极主动的学习态度</li>
                  <li>• 优秀的团队协作能力</li>
                  <li>• 强大的问题解决能力</li>
                  <li>• 注重代码质量和性能优化</li>
                </ul>
              </div>
              <div>
                <h3 className="text-xl font-semibold text-primary mb-4">兴趣爱好</h3>
                <ul className="space-y-2 text-gray-600">
                  <li>• 技术博客写作</li>
                  <li>• 开源项目贡献</li>
                  <li>• 参与技术社区</li>
                  <li>• 持续学习新技术</li>
                </ul>
              </div>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default About;