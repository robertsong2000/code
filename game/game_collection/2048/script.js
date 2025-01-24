let board = [];
let score = 0;
let size = 4;
let gameOver = false;

function startGame() {
    board = Array(size).fill().map(() => Array(size).fill(0));
    score = 0;
    gameOver = false;
    document.querySelector('.score').textContent = `得分: ${score}`;
    
    // 初始化网格
    const grid = document.querySelector('.grid');
    grid.innerHTML = '';
    for (let i = 0; i < size * size; i++) {
        const cell = document.createElement('div');
        cell.className = 'grid-cell';
        grid.appendChild(cell);
    }
    
    // 添加两个初始数字
    addNewTile();
    addNewTile();
    updateDisplay();
}

function addNewTile() {
    const emptyCells = [];
    for (let i = 0; i < size; i++) {
        for (let j = 0; j < size; j++) {
            if (board[i][j] === 0) {
                emptyCells.push({x: i, y: j});
            }
        }
    }
    
    if (emptyCells.length > 0) {
        const {x, y} = emptyCells[Math.floor(Math.random() * emptyCells.length)];
        board[x][y] = Math.random() < 0.9 ? 2 : 4;
    }
}

function updateDisplay() {
    const tileContainer = document.querySelector('.tile');
    tileContainer.innerHTML = '';
    
    for (let i = 0; i < size; i++) {
        for (let j = 0; j < size; j++) {
            if (board[i][j] !== 0) {
                const tile = document.createElement('div');
                tile.className = `tile-inner tile-${board[i][j]}`;
                tile.textContent = board[i][j];
                tile.style.top = `${i * 25}%`;
                tile.style.left = `${j * 25}%`;
                tileContainer.appendChild(tile);
            }
        }
    }
}

function move(direction) {
    if (gameOver) return;
    
    let moved = false;
    const oldBoard = board.map(row => [...row]);
    
    // 合并相同数字
    if (direction === 'left' || direction === 'right') {
        for (let i = 0; i < size; i++) {
            let row = board[i].filter(cell => cell !== 0);
            if (direction === 'right') row.reverse();
            
            for (let j = 0; j < row.length - 1; j++) {
                if (row[j] === row[j + 1]) {
                    row[j] *= 2;
                    score += row[j];
                    row.splice(j + 1, 1);
                }
            }
            
            while (row.length < size) {
                direction === 'left' ? row.push(0) : row.unshift(0);
            }
            
            if (direction === 'right') row.reverse();
            board[i] = row;
        }
    } else {
        for (let j = 0; j < size; j++) {
            let column = board.map(row => row[j]).filter(cell => cell !== 0);
            if (direction === 'down') column.reverse();
            
            for (let i = 0; i < column.length - 1; i++) {
                if (column[i] === column[i + 1]) {
                    column[i] *= 2;
                    score += column[i];
                    column.splice(i + 1, 1);
                }
            }
            
            while (column.length < size) {
                direction === 'up' ? column.push(0) : column.unshift(0);
            }
            
            if (direction === 'down') column.reverse();
            for (let i = 0; i < size; i++) {
                board[i][j] = column[i];
            }
        }
    }
    
    // 检查是否有移动
    moved = !board.every((row, i) => 
        row.every((cell, j) => cell === oldBoard[i][j])
    );
    
    if (moved) {
        addNewTile();
        document.querySelector('.score').textContent = `得分: ${score}`;
        updateDisplay();
        
        // 检查游戏是否结束
        if (!canMove()) {
            gameOver = true;
            setTimeout(() => {
                alert(`游戏结束！最终得分：${score}`);
            }, 300);
        }
    }
}

function canMove() {
    // 检查是否有空格
    for (let i = 0; i < size; i++) {
        for (let j = 0; j < size; j++) {
            if (board[i][j] === 0) return true;
        }
    }
    
    // 检查是否有相邻的相同数字
    for (let i = 0; i < size; i++) {
        for (let j = 0; j < size; j++) {
            if (
                (i < size - 1 && board[i][j] === board[i + 1][j]) ||
                (j < size - 1 && board[i][j] === board[i][j + 1])
            ) {
                return true;
            }
        }
    }
    
    return false;
}

document.addEventListener('keydown', (event) => {
    switch (event.key) {
        case 'ArrowUp':
        case 'w':
        case 'W':
            move('up');
            break;
        case 'ArrowDown':
        case 's':
        case 'S':
            move('down');
            break;
        case 'ArrowLeft':
        case 'a':
        case 'A':
            move('left');
            break;
        case 'ArrowRight':
        case 'd':
        case 'D':
            move('right');
            break;
    }
});

// 初始化游戏
startGame(); 