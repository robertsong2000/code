const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const scoreElement = document.getElementById('score');
const gameOverElement = document.getElementById('game-over');

const BLOCK_SIZE = 20;
const GAME_SPEED = 100;
const CANVAS_WIDTH = canvas.width;
const CANVAS_HEIGHT = canvas.height;

class Snake {
    constructor() {
        this.reset();
    }

    reset() {
        this.x = Math.floor(CANVAS_WIDTH / 2 / BLOCK_SIZE) * BLOCK_SIZE;
        this.y = Math.floor(CANVAS_HEIGHT / 2 / BLOCK_SIZE) * BLOCK_SIZE;
        this.body = [{x: this.x, y: this.y}];
        this.dx = BLOCK_SIZE;
        this.dy = 0;
        this.score = 0;
        scoreElement.textContent = `Score: ${this.score}`;
    }

    update() {
        const newHead = {
            x: (this.body[0].x + this.dx + CANVAS_WIDTH) % CANVAS_WIDTH,
            y: (this.body[0].y + this.dy + CANVAS_HEIGHT) % CANVAS_HEIGHT
        };

        // Check collision with self
        for (let i = 1; i < this.body.length; i++) {
            if (this.body[i].x === newHead.x && this.body[i].y === newHead.y) {
                return false;
            }
        }

        this.body.unshift(newHead);
        
        // Check if eating food
        if (newHead.x === food.x && newHead.y === food.y) {
            this.score += 1;
            scoreElement.textContent = `Score: ${this.score}`;
            food.randomize();
        } else {
            this.body.pop();
        }

        return true;
    }

    draw() {
        ctx.fillStyle = '#00ff00';
        this.body.forEach(segment => {
            ctx.fillRect(segment.x, segment.y, BLOCK_SIZE - 1, BLOCK_SIZE - 1);
        });
    }
}

class Food {
    constructor() {
        this.randomize();
    }

    randomize() {
        this.x = Math.floor(Math.random() * (CANVAS_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        this.y = Math.floor(Math.random() * (CANVAS_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
    }

    draw() {
        ctx.fillStyle = '#ff0000';
        ctx.fillRect(this.x, this.y, BLOCK_SIZE - 1, BLOCK_SIZE - 1);
    }
}

const snake = new Snake();
const food = new Food();
let gameOver = false;

document.addEventListener('keydown', (event) => {
    if (gameOver) {
        gameOver = false;
        snake.reset();
        food.randomize();
        gameOverElement.style.display = 'none';
        return;
    }

    switch (event.key) {
        case 'ArrowUp':
            if (snake.dy === 0) {
                snake.dx = 0;
                snake.dy = -BLOCK_SIZE;
            }
            break;
        case 'ArrowDown':
            if (snake.dy === 0) {
                snake.dx = 0;
                snake.dy = BLOCK_SIZE;
            }
            break;
        case 'ArrowLeft':
            if (snake.dx === 0) {
                snake.dx = -BLOCK_SIZE;
                snake.dy = 0;
            }
            break;
        case 'ArrowRight':
            if (snake.dx === 0) {
                snake.dx = BLOCK_SIZE;
                snake.dy = 0;
            }
            break;
    }
});

function gameLoop() {
    if (!gameOver) {
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

        if (!snake.update()) {
            gameOver = true;
            gameOverElement.style.display = 'block';
        }

        snake.draw();
        food.draw();
    }
    setTimeout(gameLoop, GAME_SPEED);
}

gameLoop(); 