let score = 0;
let timeLeft = 30;
let gameTimer;
let moleTimer;
let isPlaying = false;

const holes = document.querySelectorAll('.hole');
const moles = document.querySelectorAll('.mole');
const scoreDisplay = document.querySelector('.score');
const timeDisplay = document.querySelector('.time');

function randomTime(min, max) {
    return Math.round(Math.random() * (max - min) + min);
}

function randomHole(holes) {
    const index = Math.floor(Math.random() * holes.length);
    const hole = holes[index];
    return hole;
}

function peep() {
    if (!isPlaying) return;
    
    const time = randomTime(500, 1000);
    const hole = randomHole(holes);
    const mole = hole.querySelector('.mole');
    
    mole.classList.add('up');
    
    setTimeout(() => {
        mole.classList.remove('up');
        if (isPlaying) peep();
    }, time);
}

function startGame() {
    if (isPlaying) return;
    
    score = 0;
    timeLeft = 30;
    isPlaying = true;
    scoreDisplay.textContent = `得分: ${score}`;
    timeDisplay.textContent = `时间: ${timeLeft}s`;
    
    peep();
    
    gameTimer = setInterval(() => {
        timeLeft--;
        timeDisplay.textContent = `时间: ${timeLeft}s`;
        
        if (timeLeft <= 0) {
            clearInterval(gameTimer);
            isPlaying = false;
            alert(`游戏结束！你的得分是: ${score}`);
        }
    }, 1000);
}

moles.forEach(mole => {
    mole.addEventListener('click', () => {
        if (!isPlaying) return;
        if (!mole.classList.contains('up')) return;
        
        score++;
        scoreDisplay.textContent = `得分: ${score}`;
        mole.classList.remove('up');
    });
}); 