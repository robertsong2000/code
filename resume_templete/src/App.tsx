import './App.css'

import Header from './components/Header'
import Hero from './components/Hero'
import About from './components/About'
import Experience from './components/Experience'
import Skills from './components/Skills'
import Projects from './components/Projects'

function App() {
  return (
    <main className="relative">
      <Header />
      <Hero />
      <About />
      <Experience />
      <Skills />
      <Projects />
    </main>
  )
}

export default App
