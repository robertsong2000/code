import { motion } from 'framer-motion';

const Hero = () => {
  return (
    <section className="min-h-screen pt-20 flex items-center bg-gradient-to-b from-white to-gray-50">
      <div className="container mx-auto px-4 py-12">
        <div className="flex flex-col md:flex-row items-center justify-between gap-8">
          <motion.div
            className="flex-1 text-center md:text-left"
            initial={{ opacity: 0, x: -20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.5 }}
          >
            <motion.h1 
              className="text-4xl md:text-6xl font-bold text-dark mb-4"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: 0.2, duration: 0.5 }}
            >
              你的名字
            </motion.h1>
            <motion.h2 
              className="text-2xl md:text-3xl text-gray-600 mb-6"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: 0.3, duration: 0.5 }}
            >
              前端开发工程师
            </motion.h2>
            <motion.p 
              className="text-lg text-gray-600 mb-8 max-w-2xl"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: 0.4, duration: 0.5 }}
            >
              热爱编程、善于解决问题的开发者。擅长React、TypeScript和现代化前端开发技术，致力于创造优秀的用户体验。
            </motion.p>
            <motion.div
              className="flex gap-4 justify-center md:justify-start"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: 0.5, duration: 0.5 }}
            >
              <button className="bg-primary text-white px-8 py-3 rounded-lg hover:bg-secondary transition-colors">
                联系我
              </button>
              <button className="border-2 border-primary text-primary px-8 py-3 rounded-lg hover:bg-primary hover:text-white transition-colors">
                下载简历
              </button>
            </motion.div>
          </motion.div>
          <motion.div
            className="flex-1 flex justify-center"
            initial={{ opacity: 0, x: 20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.5 }}
          >
            <div className="w-64 h-64 md:w-96 md:h-96 rounded-full bg-gradient-to-r from-primary to-secondary opacity-90 flex items-center justify-center">
              <span className="text-white text-lg">添加你的照片</span>
            </div>
          </motion.div>
        </div>
      </div>
    </section>
  );
};

export default Hero;