import { motion } from 'framer-motion';

const Skills = () => {
  const skillCategories = [
    {
      title: '前端开发',
      skills: [
        { name: 'React', level: 90 },
        { name: 'TypeScript', level: 85 },
        { name: 'Vue.js', level: 80 },
        { name: 'Next.js', level: 75 }
      ]
    },
    {
      title: 'UI/UX',
      skills: [
        { name: 'Tailwind CSS', level: 85 },
        { name: 'Sass/SCSS', level: 80 },
        { name: 'Responsive Design', level: 90 },
        { name: 'UI Libraries', level: 85 }
      ]
    },
    {
      title: '开发工具',
      skills: [
        { name: 'Git', level: 90 },
        { name: 'Webpack', level: 80 },
        { name: 'Vite', level: 85 },
        { name: 'Docker', level: 70 }
      ]
    },
    {
      title: '后端技术',
      skills: [
        { name: 'Node.js', level: 75 },
        { name: 'Express', level: 70 },
        { name: 'MongoDB', level: 65 },
        { name: 'RESTful API', level: 80 }
      ]
    }
  ];

  return (
    <section id="skills" className="min-h-screen pt-20 bg-white">
      <div className="container mx-auto px-4 py-16">
        <motion.h2
          className="text-3xl md:text-4xl font-bold text-dark mb-12 text-center"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
        >
          技能专长
        </motion.h2>
        <div className="grid md:grid-cols-2 gap-8 max-w-6xl mx-auto">
          {skillCategories.map((category, index) => (
            <motion.div
              key={index}
              className="bg-gray-50 rounded-lg p-6 shadow-lg"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: index * 0.1 }}
              viewport={{ once: true }}
            >
              <h3 className="text-xl font-bold text-primary mb-4">{category.title}</h3>
              <div className="space-y-4">
                {category.skills.map((skill, idx) => (
                  <div key={idx} className="space-y-2">
                    <div className="flex justify-between items-center">
                      <span className="text-gray-700">{skill.name}</span>
                      <span className="text-gray-500">{skill.level}%</span>
                    </div>
                    <div className="h-2 bg-gray-200 rounded-full overflow-hidden">
                      <motion.div
                        className="h-full bg-primary"
                        initial={{ width: 0 }}
                        whileInView={{ width: `${skill.level}%` }}
                        transition={{ duration: 1, ease: "easeOut" }}
                        viewport={{ once: true }}
                      />
                    </div>
                  </div>
                ))}
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Skills;