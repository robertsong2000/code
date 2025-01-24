const emojis = ['❤️', '🎀', '🌹', '🍫', '🎵', '🎬', '🎮', '🌈'];
let cards = [];
let flippedCards = [];
let moves = 0;
let matchedPairs = 0;

function createBoard() {
    const gameBoard = document.querySelector('.game-board');
    const doubledEmojis = [...emojis, ...emojis];
    cards = doubledEmojis.sort(() => Math.random() - 0.5);
    
    gameBoard.innerHTML = '';
    cards.forEach((emoji, index) => {
        const card = document.createElement('div');
        card.classList.add('card');
        card.dataset.index = index;
        
        card.innerHTML = `
            <div class="card-front"></div>
            <div class="card-back">${emoji}</div>
        `;
        
        card.addEventListener('click', flipCard);
        gameBoard.appendChild(card);
    });
}

function flipCard() {
    if (flippedCards.length === 2) return;
    if (this.classList.contains('flipped')) return;
    
    this.classList.add('flipped');
    flippedCards.push(this);
    
    if (flippedCards.length === 2) {
        moves++;
        document.querySelector('.moves').textContent = `步数: ${moves}`;
        checkMatch();
    }
}

function checkMatch() {
    const [card1, card2] = flippedCards;
    const match = cards[card1.dataset.index] === cards[card2.dataset.index];
    
    if (match) {
        matchedPairs++;
        flippedCards = [];
        if (matchedPairs === emojis.length) {
            setTimeout(() => {
                alert(`恭喜你赢了！总共用了 ${moves} 步`);
            }, 500);
        }
    } else {
        setTimeout(() => {
            card1.classList.remove('flipped');
            card2.classList.remove('flipped');
            flippedCards = [];
        }, 1000);
    }
}

function resetGame() {
    moves = 0;
    matchedPairs = 0;
    flippedCards = [];
    document.querySelector('.moves').textContent = `步数: ${moves}`;
    document.querySelectorAll('.card').forEach(card => {
        card.classList.remove('flipped');
    });
    setTimeout(createBoard, 500);
}

function toggleInstructions() {
    const modal = document.getElementById('instructionsModal');
    modal.classList.toggle('show');
}

// 点击模态框外部时关闭
window.onclick = function(event) {
    const modal = document.getElementById('instructionsModal');
    if (event.target === modal) {
        modal.classList.remove('show');
    }
}

// 初始化游戏
createBoard(); 