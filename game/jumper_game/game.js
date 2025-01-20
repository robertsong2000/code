let dino = document.getElementById('dino');
let cactus = document.getElementById('cactus');
let scoreElement = document.getElementById('score');
let gameOverElement = document.getElementById('game-over');
let gameContainer = document.querySelector('.game-container');

let isJumping = false;
let score = 0;
let gameOver = false;
const baseSpeed = 3;    // 基础移动速度
const maxSpeed = 7;     // 最大移动速度
let currentSpeed = baseSpeed;  // 当前速度
let distance = 0;      // 移动距离

// 创建多个仙人掌
function createCactus(position) {
    let newCactus = document.createElement('div');
    newCactus.className = 'cactus';
    newCactus.innerHTML = '│';
    newCactus.style.left = position + 'px';
    gameContainer.appendChild(newCactus);
}

// 初始化仙人掌
function initCactuses() {
    // 在不同位置创建多个仙人掌
    createCactus(600);
    createCactus(1000);
    createCactus(1400);
}

// 移动背景和检测碰撞
function moveBackground() {
    if (!gameOver) {
        distance += currentSpeed;
        
        // 移动所有仙人掌
        document.querySelectorAll('.cactus').forEach(cactus => {
            let currentLeft = parseInt(cactus.style.left) - currentSpeed;
            if (currentLeft < -50) {
                currentLeft = 1200;  // 重置到右侧
                score += 1;
                scoreElement.textContent = `得分: ${score}`;
            }
            cactus.style.left = currentLeft + 'px';

            // 碰撞检测
            let dinoRect = dino.getBoundingClientRect();
            let cactusRect = cactus.getBoundingClientRect();

            const horizontalTolerance = 15;
            const verticalTolerance = 10;

            if (
                dinoRect.right > cactusRect.left + horizontalTolerance &&
                dinoRect.left < cactusRect.right - horizontalTolerance &&
                dinoRect.bottom > cactusRect.top + verticalTolerance
            ) {
                gameOver = true;
                gameOverElement.classList.remove('hidden');
                return;
            }
        });

        requestAnimationFrame(moveBackground);
    }
}

// 调整速度
function adjustSpeed(direction) {
    if (!gameOver) {
        if (direction === 'right' && currentSpeed < maxSpeed) {
            currentSpeed = maxSpeed;
        } else if (direction === 'left' && currentSpeed > baseSpeed) {
            currentSpeed = baseSpeed;
        }
    }
}

// 重置游戏
function resetGame() {
    if (gameOver) {
        gameOver = false;
        score = 0;
        currentSpeed = baseSpeed;
        distance = 0;
        scoreElement.textContent = `得分: ${score}`;
        gameOverElement.classList.add('hidden');
        
        // 重置所有仙人掌位置
        document.querySelectorAll('.cactus').forEach(cactus => {
            gameContainer.removeChild(cactus);
        });
        initCactuses();
        requestAnimationFrame(moveBackground);
    }
}

// 跳跃功能
function jump() {
    if (!isJumping && !gameOver) {
        isJumping = true;
        let position = 0;
        let jumpInterval = setInterval(() => {
            // 上升阶段 - 速度适中
            if (position < 200) {
                position += 15;
                dino.style.bottom = position + 'px';
            }
            // 下降阶段 - 大幅降低下落速度
            else if (position < 400) {
                position += 4;
                dino.style.bottom = (400 - position) + 'px';
            }
            // 结束跳跃
            else {
                clearInterval(jumpInterval);
                isJumping = false;
                dino.style.bottom = '0px';
            }
        }, 30);
    }
}

// 监听键盘事件
document.addEventListener('keydown', (event) => {
    switch(event.code) {
        case 'Space':
            jump();
            break;
        case 'ArrowRight':
            adjustSpeed('right');
            break;
        case 'ArrowLeft':
            adjustSpeed('left');
            break;
    }
});

// 键盘松开时恢复基础速度
document.addEventListener('keyup', (event) => {
    if (event.code === 'ArrowRight' || event.code === 'ArrowLeft') {
        currentSpeed = baseSpeed;
    }
});

// 点击重新开始
document.addEventListener('click', resetGame);

// 初始化游戏
initCactuses();
moveBackground(); 