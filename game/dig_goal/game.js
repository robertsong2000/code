class GoldMiner {
    constructor() {
        this.grid = document.getElementById('grid');
        this.scoreElement = document.getElementById('score');
        this.movesElement = document.getElementById('moves');
        this.restartBtn = document.getElementById('restartBtn');
        
        this.gridSize = 8;
        this.score = 0;
        this.moves = 30;
        this.cells = [];
        
        this.init();
    }

    init() {
        this.score = 0;
        this.moves = 30;
        this.updateScore();
        this.updateMoves();
        this.grid.innerHTML = '';
        this.cells = [];
        
        // 创建网格
        for (let i = 0; i < this.gridSize * this.gridSize; i++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            cell.dataset.index = i;
            cell.addEventListener('click', () => this.handleClick(i));
            this.grid.appendChild(cell);
            this.cells.push({
                element: cell,
                type: this.getRandomType()
            });
        }

        // 重新开始按钮
        this.restartBtn.addEventListener('click', () => this.init());
    }

    getRandomType() {
        const rand = Math.random();
        if (rand < 0.2) return 'gold';
        if (rand < 0.6) return 'stone';
        return 'empty';
    }

    handleClick(index) {
        if (this.moves <= 0) return;
        
        const cell = this.cells[index];
        if (cell.element.classList.contains('revealed')) return;

        this.moves--;
        this.updateMoves();

        cell.element.classList.add('revealed');
        cell.element.classList.add(cell.type);

        if (cell.type === 'gold') {
            this.score += 10;
            this.updateScore();
        }

        if (this.moves <= 0) {
            setTimeout(() => {
                alert(`游戏结束！你的得分是: ${this.score}`);
            }, 100);
        }
    }

    updateScore() {
        this.scoreElement.textContent = this.score;
    }

    updateMoves() {
        this.movesElement.textContent = this.moves;
    }
}

// 启动游戏
window.onload = () => {
    new GoldMiner();
}; 