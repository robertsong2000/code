const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// 游戏状态
let score = 0;
let lives = 3;
let isPlaying = false;
let gameLoop;

// 玩家飞机
const player = {
    x: canvas.width / 2,
    y: canvas.height - 50,
    width: 40,
    height: 40,
    speed: 5,
    color: '#4CAF50'
};

// 子弹数组
let bullets = [];
const bulletSpeed = 7;

// 敌机数组
let enemies = [];
const enemySpeed = 3;

// 按键状态
const keys = {
    left: false,
    right: false,
    up: false,
    down: false
};

function drawPlayer() {
    ctx.fillStyle = player.color;
    ctx.beginPath();
    ctx.moveTo(player.x, player.y);
    ctx.lineTo(player.x - player.width/2, player.y + player.height);
    ctx.lineTo(player.x + player.width/2, player.y + player.height);
    ctx.closePath();
    ctx.fill();
}

function drawBullets() {
    ctx.fillStyle = '#FFF';
    bullets.forEach(bullet => {
        ctx.fillRect(bullet.x - 2, bullet.y - 8, 4, 8);
    });
}

function drawEnemies() {
    ctx.fillStyle = '#FF5722';
    enemies.forEach(enemy => {
        ctx.beginPath();
        ctx.moveTo(enemy.x, enemy.y + enemy.height);
        ctx.lineTo(enemy.x - enemy.width/2, enemy.y);
        ctx.lineTo(enemy.x + enemy.width/2, enemy.y);
        ctx.closePath();
        ctx.fill();
    });
}

function movePlayer() {
    if (keys.left && player.x > player.width/2) player.x -= player.speed;
    if (keys.right && player.x < canvas.width - player.width/2) player.x += player.speed;
    if (keys.up && player.y > 0) player.y -= player.speed;
    if (keys.down && player.y < canvas.height - player.height) player.y += player.speed;
}

function moveBullets() {
    bullets = bullets.filter(bullet => {
        bullet.y -= bulletSpeed;
        return bullet.y > 0;
    });
}

function moveEnemies() {
    enemies = enemies.filter(enemy => {
        enemy.y += enemySpeed;
        return enemy.y < canvas.height;
    });
}

function spawnEnemy() {
    if (Math.random() < 0.03) {
        enemies.push({
            x: Math.random() * (canvas.width - 30) + 15,
            y: -20,
            width: 30,
            height: 30
        });
    }
}

function checkCollisions() {
    // 子弹击中敌机
    bullets.forEach((bullet, bulletIndex) => {
        enemies.forEach((enemy, enemyIndex) => {
            if (bullet.y <= enemy.y + enemy.height &&
                bullet.x >= enemy.x - enemy.width/2 &&
                bullet.x <= enemy.x + enemy.width/2) {
                bullets.splice(bulletIndex, 1);
                enemies.splice(enemyIndex, 1);
                score += 10;
                document.querySelector('.score').textContent = `得分: ${score}`;
            }
        });
    });

    // 敌机撞到玩家
    enemies.forEach((enemy, index) => {
        if (enemy.y + enemy.height >= player.y &&
            enemy.x >= player.x - player.width/2 &&
            enemy.x <= player.x + player.width/2) {
            enemies.splice(index, 1);
            lives--;
            document.querySelector('.lives').textContent = `生命: ${lives}`;
            if (lives <= 0) {
                gameOver();
            }
        }
    });
}

function gameOver() {
    isPlaying = false;
    clearInterval(gameLoop);
    alert(`游戏结束！得分：${score}`);
}

function update() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    movePlayer();
    moveBullets();
    moveEnemies();
    spawnEnemy();
    checkCollisions();
    
    drawPlayer();
    drawBullets();
    drawEnemies();
}

function startGame() {
    if (isPlaying) return;
    
    score = 0;
    lives = 3;
    bullets = [];
    enemies = [];
    player.x = canvas.width / 2;
    player.y = canvas.height - 50;
    
    document.querySelector('.score').textContent = `得分: ${score}`;
    document.querySelector('.lives').textContent = `生命: ${lives}`;
    
    isPlaying = true;
    gameLoop = setInterval(update, 1000/60);
}

// 键盘控制
document.addEventListener('keydown', (event) => {
    switch (event.key) {
        case 'ArrowLeft':
        case 'a':
        case 'A':
            keys.left = true;
            break;
        case 'ArrowRight':
        case 'd':
        case 'D':
            keys.right = true;
            break;
        case 'ArrowUp':
        case 'w':
        case 'W':
            keys.up = true;
            break;
        case 'ArrowDown':
        case 's':
        case 'S':
            keys.down = true;
            break;
        case ' ':
            if (isPlaying) {
                bullets.push({
                    x: player.x,
                    y: player.y
                });
            }
            break;
    }
});

document.addEventListener('keyup', (event) => {
    switch (event.key) {
        case 'ArrowLeft':
        case 'a':
        case 'A':
            keys.left = false;
            break;
        case 'ArrowRight':
        case 'd':
        case 'D':
            keys.right = false;
            break;
        case 'ArrowUp':
        case 'w':
        case 'W':
            keys.up = false;
            break;
        case 'ArrowDown':
        case 's':
        case 'S':
            keys.down = false;
            break;
    }
}); 