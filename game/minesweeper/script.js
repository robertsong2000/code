class Minesweeper {
    constructor(width = 9, height = 9, mines = 10) {
        this.width = width;
        this.height = height;
        this.mines = mines;
        this.board = [];
        this.minePositions = new Set();
        this.revealed = new Set();
        this.flagged = new Set();
        this.gameOver = false;
        this.startTime = null;
        this.timerInterval = null;
        this.longPressTimer = null;
        this.longPressDelay = 500; // 长按判定时间（毫秒）
        
        this.init();
        this.setupEventListeners();
    }

    init() {
        // 重置游戏状态
        this.minePositions.clear();
        this.revealed.clear();
        this.flagged.clear();
        this.gameOver = false;
        this.startTime = null;
        if (this.timerInterval) clearInterval(this.timerInterval);
        document.getElementById('timer').textContent = '0';
        document.getElementById('mineCount').textContent = this.mines;

        // 创建游戏板
        const board = document.getElementById('board');
        board.innerHTML = '';
        this.board = [];

        // 生成单元格
        for (let y = 0; y < this.height; y++) {
            const row = [];
            for (let x = 0; x < this.width; x++) {
                const cell = document.createElement('div');
                cell.className = 'cell';
                cell.dataset.x = x;
                cell.dataset.y = y;
                board.appendChild(cell);
                row.push(cell);
            }
            this.board.push(row);
        }

        // 随机放置地雷
        while (this.minePositions.size < this.mines) {
            const x = Math.floor(Math.random() * this.width);
            const y = Math.floor(Math.random() * this.height);
            this.minePositions.add(`${x},${y}`);
        }
    }

    setupEventListeners() {
        const board = document.getElementById('board');
        
        // 移除原有的点击和右键事件
        // board.addEventListener('click', (e) => this.handleClick(e));
        // board.addEventListener('contextmenu', (e) => this.handleRightClick(e));
        
        // 添加触摸事件
        board.addEventListener('touchstart', (e) => this.handleTouchStart(e));
        board.addEventListener('touchend', (e) => this.handleTouchEnd(e));
        board.addEventListener('touchmove', (e) => this.handleTouchMove(e));
        
        // 保留鼠标事件以支持同时使用鼠标的情况
        board.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        board.addEventListener('mouseup', (e) => this.handleMouseUp(e));
        board.addEventListener('contextmenu', (e) => e.preventDefault());
        
        document.getElementById('newGame').addEventListener('click', () => this.init());
    }

    handleTouchStart(e) {
        e.preventDefault();
        if (this.gameOver) return;
        
        const touch = e.touches[0];
        const cell = document.elementFromPoint(touch.clientX, touch.clientY);
        if (!cell?.classList.contains('cell')) return;

        this.longPressTimer = setTimeout(() => {
            this.handleFlag(cell);
        }, this.longPressDelay);
    }

    handleTouchMove(e) {
        // 如果手指移动，取消长按计时
        if (this.longPressTimer) {
            clearTimeout(this.longPressTimer);
            this.longPressTimer = null;
        }
    }

    handleTouchEnd(e) {
        e.preventDefault();
        if (this.gameOver) return;

        const touch = e.changedTouches[0];
        const cell = document.elementFromPoint(touch.clientX, touch.clientY);
        
        // 如果存在长按计时器，说明不是长按，执行普通点击
        if (this.longPressTimer) {
            clearTimeout(this.longPressTimer);
            this.longPressTimer = null;
            
            if (cell?.classList.contains('cell')) {
                this.handleClick(cell);
            }
        }
    }

    handleMouseDown(e) {
        if (this.gameOver) return;
        const cell = e.target;
        if (!cell.classList.contains('cell')) return;

        if (e.button === 0) { // 左键
            this.longPressTimer = setTimeout(() => {
                this.handleFlag(cell);
            }, this.longPressDelay);
        }
    }

    handleMouseUp(e) {
        if (this.gameOver) return;
        const cell = e.target;
        if (!cell.classList.contains('cell')) return;

        if (e.button === 0) { // 左键
            if (this.longPressTimer) {
                clearTimeout(this.longPressTimer);
                this.longPressTimer = null;
                this.handleClick(cell);
            }
        } else if (e.button === 2) { // 右键
            e.preventDefault();
            this.handleFlag(cell);
        }
    }

    // 新增处理点击的方法
    handleClick(cell) {
        if (!cell) return;
        this.startTimer();
        const x = parseInt(cell.dataset.x);
        const y = parseInt(cell.dataset.y);
        if (this.flagged.has(`${x},${y}`)) return;
        this.reveal(x, y);
    }

    // 新增处理插旗的方法
    handleFlag(cell) {
        if (!cell) return;
        const x = parseInt(cell.dataset.x);
        const y = parseInt(cell.dataset.y);
        const pos = `${x},${y}`;

        if (this.revealed.has(pos)) return;

        if (this.flagged.has(pos)) {
            this.flagged.delete(pos);
            cell.textContent = '';
        } else {
            this.flagged.add(pos);
            cell.textContent = '🚩';
        }

        document.getElementById('mineCount').textContent = 
            this.mines - this.flagged.size;
    }

    startTimer() {
        if (!this.startTime) {
            this.startTime = Date.now();
            this.timerInterval = setInterval(() => {
                const seconds = Math.floor((Date.now() - this.startTime) / 1000);
                document.getElementById('timer').textContent = seconds;
            }, 1000);
        }
    }

    reveal(x, y) {
        const pos = `${x},${y}`;
        if (this.revealed.has(pos)) return;
        
        this.revealed.add(pos);
        const cell = this.board[y][x];
        cell.classList.add('revealed');

        if (this.minePositions.has(pos)) {
            this.gameOver = true;
            cell.classList.add('mine');
            cell.textContent = '💣';
            this.revealAllMines();
            clearInterval(this.timerInterval);
            alert('游戏结束！');
            return;
        }

        const adjacentMines = this.countAdjacentMines(x, y);
        if (adjacentMines > 0) {
            cell.textContent = adjacentMines;
            cell.style.color = this.getNumberColor(adjacentMines);
        } else {
            // 如果是空白格，递归显示周围的格子
            for (let dx = -1; dx <= 1; dx++) {
                for (let dy = -1; dy <= 1; dy++) {
                    const newX = x + dx;
                    const newY = y + dy;
                    if (this.isValidPosition(newX, newY)) {
                        this.reveal(newX, newY);
                    }
                }
            }
        }

        if (this.checkWin()) {
            this.gameOver = true;
            clearInterval(this.timerInterval);
            alert('恭喜你赢了！');
        }
    }

    isValidPosition(x, y) {
        return x >= 0 && x < this.width && y >= 0 && y < this.height;
    }

    countAdjacentMines(x, y) {
        let count = 0;
        for (let dx = -1; dx <= 1; dx++) {
            for (let dy = -1; dy <= 1; dy++) {
                const newX = x + dx;
                const newY = y + dy;
                if (this.isValidPosition(newX, newY) && 
                    this.minePositions.has(`${newX},${newY}`)) {
                    count++;
                }
            }
        }
        return count;
    }

    revealAllMines() {
        this.minePositions.forEach(pos => {
            const [x, y] = pos.split(',').map(Number);
            const cell = this.board[y][x];
            cell.classList.add('revealed', 'mine');
            cell.textContent = '💣';
        });
    }

    checkWin() {
        return this.revealed.size + this.minePositions.size === 
               this.width * this.height;
    }

    getNumberColor(num) {
        const colors = [
            '#0000FF', '#008000', '#FF0000', '#000080',
            '#800000', '#008080', '#000000', '#808080'
        ];
        return colors[num - 1] || '#000000';
    }
}

// 初始化游戏
new Minesweeper(); 